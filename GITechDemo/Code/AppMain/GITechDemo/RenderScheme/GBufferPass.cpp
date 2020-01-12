/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   GBufferPass.cpp
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
#include <RenderState.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderTarget.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "App.h"

#include "GBufferPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

namespace GITechDemoApp
{
    /* G-Buffer generation */
    bool GBUFFER_Z_PREPASS = true;
    int DIFFUSE_ANISOTROPY = MAX_ANISOTROPY;
    bool GBUFFER_USE_NORMAL_MAPS = true;
    bool DRAW_ALPHA_TEST_GEOMETRY = true;
}

GBufferPass::GBufferPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

GBufferPass::~GBufferPass()
{}

void GBufferPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!RenderContext || !ResourceMgr)
        return;

    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(1))->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(2))->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(3))->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetDepthBuffer())->SetAddressingMode(SAM_CLAMP);

    // Update matrices
    HLSL::FrameParams->WorldMat = makeTrans(Vec3f(0, 0, 0), Type2Type<Matrix44f>());
    HLSL::GBufferGenerationParams->WorldViewMat = HLSL::BRDFParams->ViewMat * HLSL::FrameParams->WorldMat;
    HLSL::GBufferGenerationParams->WorldViewProjMat = HLSL::FrameParams->ProjMat * HLSL::GBufferGenerationParams->WorldViewMat;
    HLSL::DepthPassAlphaTestParams->WorldViewProjMat = HLSL::FrameParams->ProjMat * HLSL::GBufferGenerationParams->WorldViewMat;
    HLSL::DepthPassParams->WorldViewProjMat = HLSL::FrameParams->ProjMat * HLSL::GBufferGenerationParams->WorldViewMat;

    // In order to have better low-intensity precision in the G-Buffer (which is in LDR format, obviously)
    // we need to delay the gamma correction of input textures right up until we need them to be linear,
    // which is at the lighting stage. To achieve this, we need to keep the G-buffer in sRGB format.
    // The rest of the rendering pipeline supports HDR, so we keep color information in linear format
    // up until the tone mapping pass, where we output a LDR sRGB texture.
    // Kudos to VladC of FUN labs for pointing this out!
    ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(0))->SetSRGBEnabled(true);

    HLSL::DepthCopy_Source = GBuffer.GetRenderTarget()->GetDepthBuffer();
    HLSL::DownsampleParams->TexSize = Vec4f(
        (float)GBuffer.GetRenderTarget()->GetWidth(),
        (float)GBuffer.GetRenderTarget()->GetHeight(),
        1.f / (float)GBuffer.GetRenderTarget()->GetWidth(),
        1.f / (float)GBuffer.GetRenderTarget()->GetHeight()
    );
    HLSL::DownsampleParams->DepthDownsample = true;
    HLSL::PostProcessingParams->DepthHalfTexelOffset = Vec2f(
        0.5f / (float)GBuffer.GetRenderTarget()->GetWidth(),
        0.5f / (float)GBuffer.GetRenderTarget()->GetHeight()
    );

    ResourceMgr->GetTexture(LinearFullDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(LinearQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(HyperbolicQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(LinearFullDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(LinearQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(HyperbolicQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer())->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

    DIFFUSE_ANISOTROPY = Math::clamp(DIFFUSE_ANISOTROPY, 1, (int)MAX_ANISOTROPY);
    HLSL::BRDFParams->BRDFModel = gmtl::Math::clamp((int)HLSL::BRDFParams->BRDFModel, (int)HLSL::BRDF::BlinnPhong, (int)HLSL::BRDF::BRDFModelCount - 1);
}

void GBufferPass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    GBuffer.Enable();
    RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);
    DrawChildren();
    GBuffer.Disable();
}

void GBufferPass::AllocateResources()
{

}

void GBufferPass::ReleaseResources()
{

}
