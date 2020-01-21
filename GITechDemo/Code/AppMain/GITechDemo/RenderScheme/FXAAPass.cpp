/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   FXAAPass.cpp
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
using namespace Synesthesia3D;

#include "FXAAPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

FXAAPass::FXAAPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

FXAAPass::~FXAAPass()
{}

void FXAAPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    ResourceMgr->GetTexture(LDRFxaaImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(LDRFxaaImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetSRGBEnabled(true);

    const Synesthesia3D::RenderTarget* const srcRT = RenderConfig::PostProcessing::ToneMapping::Enabled ? LDRToneMappedImageBuffer.GetRenderTarget() : LightAccumulationBuffer.GetRenderTarget();
    HLSL::FXAAParams->HalfTexelOffset = Vec2f(0.5f / srcRT->GetWidth(), 0.5f / srcRT->GetHeight());
    HLSL::FXAAParams->TextureSize = Vec4f(
        (float)srcRT->GetWidth(),
        (float)srcRT->GetHeight(),
        1.f / (float)srcRT->GetWidth(),
        1.f / (float)srcRT->GetHeight()
    );
    HLSL::FXAA_SourceTexture = srcRT->GetColorBuffer(0);
    ResourceMgr->GetTexture(srcRT->GetColorBuffer())->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ResourceMgr->GetTexture(srcRT->GetColorBuffer())->SetAddressingMode(SAM_CLAMP);

    HLSL::FXAA_DepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
    //HLSL::FXAADepthBuffer = LinearFullDepthBuffer.GetRenderTarget()->GetColorBuffer();
}

void FXAAPass::Draw()
{
    if (!RenderConfig::PostProcessing::FastApproximateAntiAliasing::Enabled)
        return;

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    LDRFxaaImageBuffer.Enable();

    const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
    RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);

    // Not necesarry
    //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    FxaaShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    FxaaShader.Disable();

    RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);

    LDRFxaaImageBuffer.Disable();
}

void FXAAPass::AllocateResources()
{

}

void FXAAPass::ReleaseResources()
{

}
