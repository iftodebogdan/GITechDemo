/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   TerrainPass.cpp
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

#include <RenderState.h>
#include <RenderTarget.h>
using namespace Synesthesia3D;

#include "VirtualMuseum.h"

#include "TerrainPass.h"
using namespace VirtualMuseumApp;

#include "AppResources.h"

TerrainPass::TerrainPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

TerrainPass::~TerrainPass()
{}

void TerrainPass::AllocateResources()
{
}

void TerrainPass::ReleaseResources()
{
}

void TerrainPass::Update(const float fDeltaTime)
{
    HLSL::TerrainParams->HalfTexelOffset = Vec2f(
        0.5f / (float)GBuffer.GetRenderTarget()->GetWidth(),
        0.5f / (float)GBuffer.GetRenderTarget()->GetHeight()
    );

    HLSL::TerrainParams->TerrainPlanePoint = Vec3f(0.f, 0.f, 0.f);
    HLSL::TerrainParams->TerrainPlaneNormal = Vec3f(0.f, 1.f, 0.f);
    HLSL::TerrainParams->CameraWorldPos = -((VirtualMuseum*)AppMain)->GetCamera().vPos;
    HLSL::TerrainParams->TextureMappingScale = RenderConfig::Terrain::TextureMappingScale;

    const vector<RenderResource*>& arrRenderResourceList = RenderResource::GetResourceList();
    const unsigned int pbrMaterialCount = RenderResource::GetResourceCountByType(RenderResource::RES_PBR_MATERIAL);
    RenderConfig::Terrain::MaterialIndex = gmtl::Math::Min(RenderConfig::Terrain::MaterialIndex, (int)pbrMaterialCount - 1);

    for (unsigned int resIdx = 0, pbrMatIdx = 0; resIdx < arrRenderResourceList.size(); resIdx++)
    {
        if (arrRenderResourceList[resIdx] &&
            arrRenderResourceList[resIdx]->GetResourceType() == RenderResource::RES_PBR_MATERIAL &&
            pbrMatIdx++ == RenderConfig::Terrain::MaterialIndex)
        {
            const PBRMaterial* const pbrMaterial = (PBRMaterial*)arrRenderResourceList[resIdx];

            // Reset texture states
            for (unsigned int i = 0; i < PBRMaterial::PBRTT_MAX; i++)
            {
                pbrMaterial->GetTexture((PBRMaterial::PBRTextureType)i)->SetAnisotropy(1u);
                pbrMaterial->GetTexture((PBRMaterial::PBRTextureType)i)->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
            }
            pbrMaterial->GetTexture(PBRMaterial::PBRTT_ALBEDO)->SetAnisotropy((unsigned int)RenderConfig::GBuffer::DiffuseAnisotropy);

            HLSL::Terrain_Diffuse = pbrMaterial->GetTexture(PBRMaterial::PBRTT_ALBEDO);
            HLSL::Terrain_Normal = pbrMaterial->GetTexture(PBRMaterial::PBRTT_NORMAL);
            HLSL::Terrain_MatType = pbrMaterial->GetTexture(PBRMaterial::PBRTT_MATERIAL);
            HLSL::Terrain_Roughness = pbrMaterial->GetTexture(PBRMaterial::PBRTT_ROUGHNESS);

            break;
        }
    }
}

void TerrainPass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    RenderState* RenderStateMgr = RenderContext->GetRenderStateManager();
    if (!RenderStateMgr)
        return;

    const ZBuffer zEnabled = RenderStateMgr->GetZEnabled();
    const bool zWriteEnabled = RenderStateMgr->GetZWriteEnabled();
    const Cmp cmpFunc = RenderStateMgr->GetZFunc();

    RenderStateMgr->SetZEnabled(ZB_ENABLED);
    RenderStateMgr->SetZWriteEnabled(true);
    RenderStateMgr->SetZFunc(CMP_ALWAYS);

    TerrainShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    TerrainShader.Disable();

    RenderStateMgr->SetZEnabled(zEnabled);
    RenderStateMgr->SetZWriteEnabled(zWriteEnabled);
    RenderStateMgr->SetZFunc(cmpFunc);
}
