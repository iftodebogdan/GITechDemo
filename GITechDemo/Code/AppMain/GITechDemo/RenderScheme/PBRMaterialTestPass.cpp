/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   PBRMaterialTestPass.cpp
 *      Author: Bogdan Iftode
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "stdafx.h"

#include <Renderer.h>
#include <Texture.h>
#include <Profiler.h>

#include "PBRMaterialTestPass.h"
#include "AppResources.h"
using namespace GITechDemoApp;

namespace GITechDemoApp
{
    extern PBRMaterialDescription g_arrPBRMaterialDescription[];
    extern unsigned int g_nPBRMaterialDescriptionCount;
    extern int DIFFUSE_ANISOTROPY;
    extern AABoxf SceneAABB;
}

PBRMaterialDescription::PBRMaterialDescription(const char* const materialName, const char* const folderName)
    : szMaterialName(materialName)
    , szFolderName(folderName)
{
    g_nPBRMaterialDescriptionCount++;
    arrTextures[PBRTT_ALBEDO] = new Texture(("models/pbr-test/textures/" + szFolderName + "/albedo.s3dtex").c_str());
    arrTextures[PBRTT_NORMAL] = new Texture(("models/pbr-test/textures/" + szFolderName + "/normal.s3dtex").c_str());
    arrTextures[PBRTT_ROUGHNESS] = new Texture(("models/pbr-test/textures/" + szFolderName + "/roughness.s3dtex").c_str());
    arrTextures[PBRTT_MATERIAL] = new Texture(("models/pbr-test/textures/" + szFolderName + "/metallic.s3dtex").c_str());
}

PBRMaterialDescription::~PBRMaterialDescription()
{
    assert(g_nPBRMaterialDescriptionCount > 0);
    g_nPBRMaterialDescriptionCount--;
    for (unsigned int i = 0; i < PBRTT_MAX; i++)
    {
        if (arrTextures[i] != nullptr)
        {
            delete arrTextures[i];
            arrTextures[i] = nullptr;
        }
    }

}

PBRMaterialTestPass::PBRMaterialTestPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

PBRMaterialTestPass::~PBRMaterialTestPass()
{}

void PBRMaterialTestPass::Update(const float fDeltaTime)
{

}

void PBRMaterialTestPass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    GBuffer.Enable();

    for (unsigned int matIdx = 0; matIdx < g_nPBRMaterialDescriptionCount; matIdx++)
    {
        PUSH_PROFILE_MARKER(g_arrPBRMaterialDescription[matIdx].szMaterialName.c_str());

        // Update matrices
        f44WorldMat = CalculateWorldMatrixForSphereIdx(matIdx);
        f44WorldViewMat = f44ViewMat * f44WorldMat;
        f44WorldViewProjMat = f44ProjMat * f44WorldViewMat;

        const unsigned int diffuseTexIdx = g_arrPBRMaterialDescription[matIdx].arrTextures[PBRMaterialDescription::PBRTT_ALBEDO]->GetTextureIndex();
        const unsigned int normalTexIdx = g_arrPBRMaterialDescription[matIdx].arrTextures[PBRMaterialDescription::PBRTT_NORMAL]->GetTextureIndex();
        const unsigned int matTexIdx = g_arrPBRMaterialDescription[matIdx].arrTextures[PBRMaterialDescription::PBRTT_MATERIAL]->GetTextureIndex();
        const unsigned int roughnessTexIdx = g_arrPBRMaterialDescription[matIdx].arrTextures[PBRMaterialDescription::PBRTT_ROUGHNESS]->GetTextureIndex();

        if (diffuseTexIdx != ~0u && normalTexIdx != ~0u && matTexIdx != ~0u && roughnessTexIdx != ~0u)
        {
            // Reset texture states
            for (unsigned int i = 0; i < PBRMaterialDescription::PBRTT_MAX; i++)
            {
                g_arrPBRMaterialDescription[matIdx].arrTextures[i]->GetTexture()->SetAnisotropy(1u);
                g_arrPBRMaterialDescription[matIdx].arrTextures[i]->GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
            }
            g_arrPBRMaterialDescription[matIdx].arrTextures[PBRMaterialDescription::PBRTT_ALBEDO]->GetTexture()->SetAnisotropy((unsigned int)DIFFUSE_ANISOTROPY);

            texDiffuse = diffuseTexIdx;
            texNormal = normalTexIdx;
            bHasNormalMap = true;

            // For Blinn-Phong BRDF
            texSpec = roughnessTexIdx;
            bHasSpecMap = true;

            // For Cook-Torrance BRDF
            texMatType = matTexIdx;
            texRoughness = roughnessTexIdx;

            GBufferGenerationShader.Enable();

            // It should have only one mesh, but in case we ever change that...
            for (unsigned int mesh = 0; mesh < SphereModel.GetModel()->arrMesh.size(); mesh++)
                RenderContext->DrawVertexBuffer(SphereModel.GetModel()->arrMesh[mesh]->pVertexBuffer);

            GBufferGenerationShader.Disable();
        }

        POP_PROFILE_MARKER();
    }

    GBuffer.Disable();
}

Matrix44f PBRMaterialTestPass::CalculateWorldMatrixForSphereIdx(const unsigned int idx)
{
    // Do some prerequisite calculations for araging the spheres on a grid
    const unsigned int sphereRowCount = g_nPBRMaterialDescriptionCount / 10; // Math::sqrt(g_nPBRMaterialDescriptionCount);
    const unsigned int sphereColCount = (unsigned int)Math::ceil((float)g_nPBRMaterialDescriptionCount / (float)sphereRowCount);

    // Tuned for exactly 20 spheres
    const float sphereSize = 50.f;
    const float sphereSpacingX = 185.f;
    const float sphereSpacingY = 485.f;
    const float sphereOffsetX = 165.f;
    const float sphereOffsetY = 200.f;
    const float sphereOffsetOddX = -15.f;

    // Calculate a position for the current sphere
    const unsigned int sphereRowIdx = idx / sphereColCount;
    const unsigned int sphereColIdx = idx % sphereColCount;
    const Vec3f sphereWorldPos = Vec3f(
        ((float)sphereColIdx - (float)sphereColCount * 0.5f) * sphereSpacingX + sphereOffsetX + (idx % 2 ? sphereOffsetOddX : 0.f),
        600.f,// SceneAABB.getMax()[1] + 100.f,
        ((float)sphereRowIdx - (float)sphereRowCount * 0.5f) * sphereSpacingY + sphereOffsetY
    );

    return makeTrans(sphereWorldPos, Type2Type<Matrix44f>()) * makeScale(Vec3f((float)sphereSize, (float)sphereSize, (float)sphereSize), Type2Type<Matrix44f>());
}
