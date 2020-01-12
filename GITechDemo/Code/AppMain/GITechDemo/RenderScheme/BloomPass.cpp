/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   BloomPass.cpp
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
#include <RenderState.h>
#include <RenderTarget.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "BloomPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

namespace GITechDemoApp
{
    bool BLOOM_ENABLED = true;
    const unsigned int BLOOM_BLUR_KERNEL_COUNT = 9;
    const unsigned int BLOOM_BLUR_KERNEL[BLOOM_BLUR_KERNEL_COUNT] = { 0, 1, 2, 3, 4, 4, 5, 6, 7 };
}

BloomPass::BloomPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

BloomPass::~BloomPass()
{}

void BloomPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    ResourceMgr->GetTexture(
        BloomBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(
        BloomBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);

    HLSL::DownsampleParams->DownsampleFactor = 1;
    HLSL::DownsampleParams->ApplyBrightnessFilter = true;
    HLSL::DownsampleParams->DepthDownsample = false;

    HLSL::ColorCopyParams->SingleChannelCopy = false;
    HLSL::ColorCopyParams->CustomColorModulator = Vec4f(1.f, 1.f, 1.f, 1.f);
    HLSL::ColorCopyParams->ApplyTonemap = false;
}

// Apply a brightness filter to the downsampled HDR scene color buffer
void BloomPass::BloomBrightnessFilter()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Brightness filter");

    BloomBuffer[0]->Enable();

    // Not necesarry
    //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    const SamplerFilter samplerFilter = ResourceMgr->GetTexture(HDRDownsampleForBloomBuffer.GetRenderTarget()->GetColorBuffer(0))->GetFilter();
    ResourceMgr->GetTexture(HDRDownsampleForBloomBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);

    HLSL::DownsampleParams->HalfTexelOffset = Vec2f(
        0.5f / (float)BloomBuffer[0]->GetRenderTarget()->GetWidth(),
        0.5f / (float)BloomBuffer[0]->GetRenderTarget()->GetHeight()
        );
    HLSL::DownsampleParams->TexSize = Vec4f(
        (float)HDRDownsampleForBloomBuffer.GetRenderTarget()->GetWidth(),
        (float)HDRDownsampleForBloomBuffer.GetRenderTarget()->GetHeight(),
        1.f / (float)HDRDownsampleForBloomBuffer.GetRenderTarget()->GetWidth(),
        1.f / (float)HDRDownsampleForBloomBuffer.GetRenderTarget()->GetHeight()
        );
    HLSL::Downsample_Source = HDRDownsampleForBloomBuffer.GetRenderTarget()->GetColorBuffer(0);

    DownsampleShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    DownsampleShader.Disable();

    ResourceMgr->GetTexture(HDRDownsampleForBloomBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(samplerFilter);

    BloomBuffer[0]->Disable();

    POP_PROFILE_MARKER();
}

// Blur the bloom buffer
void BloomPass::BloomBlur()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Blur");

    for (unsigned int i = 0; i < BLOOM_BLUR_KERNEL_COUNT; i++)
    {
#if ENABLE_PROFILE_MARKERS
        char label[10];
        sprintf_s(label, "Kernel %d", BLOOM_BLUR_KERNEL[i]);
#endif
        PUSH_PROFILE_MARKER(label);

        BloomBuffer[(i + 1) % 2]->Enable();

        // Not necesarry
        //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

        HLSL::BloomParams->HalfTexelOffset = Vec2f(
            0.5f / (float)BloomBuffer[i % 2]->GetRenderTarget()->GetWidth(),
            0.5f / (float)BloomBuffer[i % 2]->GetRenderTarget()->GetHeight()
            );
        ResourceMgr->GetTexture(
            BloomBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0)
            )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
        HLSL::Bloom_Source = BloomBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0);
        HLSL::BloomParams->TexSize = Vec4f(
            (float)BloomBuffer[i % 2]->GetRenderTarget()->GetWidth(),
            (float)BloomBuffer[i % 2]->GetRenderTarget()->GetHeight(),
            1.f / (float)BloomBuffer[i % 2]->GetRenderTarget()->GetWidth(),
            1.f / (float)BloomBuffer[i % 2]->GetRenderTarget()->GetHeight()
            );
        HLSL::BloomParams->Kernel = BLOOM_BLUR_KERNEL[i];
        if (i == BLOOM_BLUR_KERNEL_COUNT - 1)
            HLSL::BloomParams->AdjustIntensity = true;
        else
            HLSL::BloomParams->AdjustIntensity = false;

        BloomShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        BloomShader.Disable();

        BloomBuffer[(i + 1) % 2]->Disable();

        POP_PROFILE_MARKER();
    }

    POP_PROFILE_MARKER();
}

// Apply the bloom buffer to the light accumulation buffer
void BloomPass::BloomApply()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Apply");

    LightAccumulationBuffer.Enable();

    const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    const Blend dstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
    const Blend srcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();
    bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
    Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
    RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ONE);
    RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
    RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

    HLSL::ColorCopyParams->HalfTexelOffset = Vec2f(0.5f / BloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetWidth(), 0.5f / BloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetHeight());
    ResourceMgr->GetTexture(
        BloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    HLSL::ColorCopy_SourceTexture = BloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetColorBuffer(0);

    ColorCopyShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    ColorCopyShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
    RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
    RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
    RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

    LightAccumulationBuffer.Disable();

    POP_PROFILE_MARKER();
}

void BloomPass::Draw()
{
    if (!BLOOM_ENABLED)
        return;

    BloomBrightnessFilter();
    BloomBlur();
    BloomApply();
}

void BloomPass::AllocateResources()
{

}

void BloomPass::ReleaseResources()
{

}
