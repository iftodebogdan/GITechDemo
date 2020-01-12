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
    extern int DIFFUSE_ANISOTROPY;
    extern AABoxf SceneAABB;
    extern bool GBUFFER_USE_NORMAL_MAPS;
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

    const vector<RenderResource*>& arrRenderResourceList = RenderResource::GetResourceList();
    const unsigned int pbrMaterialCount = RenderResource::GetResourceCountByType(RenderResource::RES_PBR_MATERIAL);

    for (unsigned int resIdx = 0, pbrMatIdx = 0; resIdx < arrRenderResourceList.size(); resIdx++)
    {
        if (arrRenderResourceList[resIdx] && arrRenderResourceList[resIdx]->GetResourceType() == RenderResource::RES_PBR_MATERIAL)
        {
            const PBRMaterial* const pbrMaterial = (PBRMaterial*)arrRenderResourceList[resIdx];

            PUSH_PROFILE_MARKER(pbrMaterial->GetDesc());

            // Update matrices
            f44WorldMat = CalculateWorldMatrixForSphereIdx(pbrMatIdx++, pbrMaterialCount);
            f44WorldViewMat = f44ViewMat * f44WorldMat;
            f44WorldViewProjMat = f44ProjMat * f44WorldViewMat;

            const unsigned int diffuseTexIdx = pbrMaterial->GetTextureIndex(PBRMaterial::PBRTT_ALBEDO);
            const unsigned int normalTexIdx = pbrMaterial->GetTextureIndex(PBRMaterial::PBRTT_NORMAL);
            const unsigned int matTexIdx = pbrMaterial->GetTextureIndex(PBRMaterial::PBRTT_MATERIAL);
            const unsigned int roughnessTexIdx = pbrMaterial->GetTextureIndex(PBRMaterial::PBRTT_ROUGHNESS);

            if (diffuseTexIdx != ~0u && normalTexIdx != ~0u && matTexIdx != ~0u && roughnessTexIdx != ~0u)
            {
                // Reset texture states
                for (unsigned int i = 0; i < PBRMaterial::PBRTT_MAX; i++)
                {
                    pbrMaterial->GetTexture((PBRMaterial::PBRTextureType)i)->SetAnisotropy(1u);
                    pbrMaterial->GetTexture((PBRMaterial::PBRTextureType)i)->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
                }
                pbrMaterial->GetTexture(PBRMaterial::PBRTT_ALBEDO)->SetAnisotropy((unsigned int)DIFFUSE_ANISOTROPY);

                HLSL::GBufferGeneration_Diffuse = diffuseTexIdx;
                HLSL::GBufferGeneration_Normal = normalTexIdx;
                HLSL::GBufferGenerationParams->HasNormalMap = (HLSL::GBufferGeneration_Normal != -1) && GBUFFER_USE_NORMAL_MAPS;

                // For Blinn-Phong BRDF
                HLSL::GBufferGeneration_Spec = roughnessTexIdx;
                HLSL::GBufferGenerationParams->HasSpecMap = true;

                // For Cook-Torrance BRDF
                HLSL::GBufferGeneration_MatType = matTexIdx;
                HLSL::GBufferGeneration_Roughness = roughnessTexIdx;

                GBufferGenerationShader.Enable();

                // It should have only one mesh, but in case we ever change that...
                for (unsigned int mesh = 0; mesh < SphereModel.GetModel()->arrMesh.size(); mesh++)
                    RenderContext->DrawVertexBuffer(SphereModel.GetModel()->arrMesh[mesh]->pVertexBuffer);

                GBufferGenerationShader.Disable();
            }

            POP_PROFILE_MARKER();
        }
    }

    GBuffer.Disable();
}

Matrix44f PBRMaterialTestPass::CalculateWorldMatrixForSphereIdx(const unsigned int idx, const unsigned int total)
{
    // Do some prerequisite calculations for araging the spheres on a grid
    const unsigned int sphereRowCount = total / 10; // Math::sqrt(g_nPBRMaterialDescriptionCount);
    const unsigned int sphereColCount = (unsigned int)Math::ceil((float)total / (float)sphereRowCount);

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

void PBRMaterialTestPass::AllocateResources()
{

}

void PBRMaterialTestPass::ReleaseResources()
{

}
