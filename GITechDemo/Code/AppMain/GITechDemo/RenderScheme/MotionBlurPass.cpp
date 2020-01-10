/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   MotionBlurPass.cpp
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
#include <RenderTarget.h>
#include <RenderState.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "MotionBlurPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

namespace GITechDemoApp
{
    bool MOTION_BLUR_ENABLED = true;
}

MotionBlurPass::MotionBlurPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

MotionBlurPass::~MotionBlurPass()
{}

void MotionBlurPass::Update(const float fDeltaTime)
{
    HLSL::ColorCopyParams->SingleChannelCopy = false;
    HLSL::ColorCopyParams->CustomColorModulator = Vec4f(1.f, 1.f, 1.f, 1.f);
    HLSL::ColorCopyParams->ApplyTonemap = false;
}

void MotionBlurPass::Draw()
{
    if(!MOTION_BLUR_ENABLED)
        return;

    CalculateMotionBlur();
    ApplyMotionBlur();
}

void MotionBlurPass::CalculateMotionBlur()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Calculate");

    MotionBlurBuffer.Enable();

    const bool colorBlendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

    f2HalfTexelOffset = Vec2f(
        0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
        0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight()
        );
    ResourceMgr->GetTexture(LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer())->SetAddressingMode(SAM_MIRROR);
    texSource = LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer();
    texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();

    MotionBlurShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    MotionBlurShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(colorBlendEnabled);

    MotionBlurBuffer.Disable();

    POP_PROFILE_MARKER();
}

void MotionBlurPass::ApplyMotionBlur()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    PUSH_PROFILE_MARKER("Apply");

    LightAccumulationBuffer.Enable();

    const bool blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
    Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
    RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

    HLSL::ColorCopyParams->HalfTexelOffset = Vec2f(
        0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
        0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight()
        );

    HLSL::ColorCopy_SourceTexture = MotionBlurBuffer.GetRenderTarget()->GetColorBuffer();

    ColorCopyShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    ColorCopyShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
    RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

    LightAccumulationBuffer.Disable();

    POP_PROFILE_MARKER();
}

void MotionBlurPass::AllocateResources()
{

}

void MotionBlurPass::ReleaseResources()
{

}
