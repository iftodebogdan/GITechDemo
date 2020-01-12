/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   RSMDirectionalLightPass.cpp
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
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderTarget.h>
using namespace Synesthesia3D;

#include "RSMDirectionalLightPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

namespace GITechDemoApp
{
    extern bool INDIRECT_LIGHT_ENABLED;
    extern AABoxf SceneLightSpaceAABB;
}

RSMDirectionalLightPass::RSMDirectionalLightPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

RSMDirectionalLightPass::~RSMDirectionalLightPass()
{}

void RSMDirectionalLightPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_BORDER);
    ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(0))->SetBorderColor(Vec4f(0.f, 0.f, 0.f, 0.f));
    ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(1))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetDepthBuffer())->SetAddressingMode(SAM_BORDER);
    ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetDepthBuffer())->SetBorderColor(Vec4f(0.f, 0.f, 0.f, 0.f));

    // RSM matrices
    RenderContext->CreateOrthographicMatrix(HLSL::RSMCommonParams->RSMProjMat,
        SceneLightSpaceAABB.mMin[0],
        SceneLightSpaceAABB.mMax[1],
        SceneLightSpaceAABB.mMax[0],
        SceneLightSpaceAABB.mMin[1],
        SceneLightSpaceAABB.mMin[2], SceneLightSpaceAABB.mMax[2]);
    HLSL::RSMCaptureParams->RSMWorldViewProjMat = HLSL::RSMCommonParams->RSMProjMat * HLSL::RSMCaptureParams->LightWorldViewMat;
    invertFull(HLSL::RSMCommonParams->RSMInvProjMat, HLSL::RSMCommonParams->RSMProjMat);
    HLSL::RSMCommonParams->ViewToRSMViewMat = HLSL::FrameParams->DirectionalLightViewMat * HLSL::BRDFParams->InvViewMat;
}

void RSMDirectionalLightPass::Draw()
{
    if (!INDIRECT_LIGHT_ENABLED)
        return;

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    RSMBuffer.Enable();

    RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
    {
        HLSL::RSMCapture_Diffuse = SponzaScene.GetTexture(Synesthesia3D::Model::TextureDesc::TT_DIFFUSE, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);

        RSMCaptureShader.Enable();
        RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
        RSMCaptureShader.Disable();
    }

    RSMBuffer.Disable();
}

void RSMDirectionalLightPass::AllocateResources()
{

}

void RSMDirectionalLightPass::ReleaseResources()
{

}
