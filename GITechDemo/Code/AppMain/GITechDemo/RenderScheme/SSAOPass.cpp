/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   SSAOPass.cpp
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
#include <RenderState.h>
#include <Texture.h>
#include <RenderTarget.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "SSAOPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

SSAOPass::SSAOPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
    , SSAOBuffer(SSAOFullBuffer)
    , BlurKernelCount(SSAO_BLUR_KERNEL_COUNT)
{}

SSAOPass::~SSAOPass()
{}

void SSAOPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    HLSL::SSAO_NormalBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(1);
    HLSL::SSAO_DepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();

    if (SSAO_USE_QUARTER_RESOLUTION_BUFFER)
    {
        SSAOBuffer = SSAOQuarterBuffer;
        BlurKernelCount = SSAO_BLUR_KERNEL_COUNT - 1;
    }
    else
    {
        SSAOBuffer = SSAOFullBuffer;
        BlurKernelCount = SSAO_BLUR_KERNEL_COUNT;
    }

    HLSL::ColorCopyParams->SingleChannelCopy = true;
    HLSL::ColorCopyParams->CustomColorModulator = Vec4f(1.f, 1.f, 1.f, 1.f);
    HLSL::ColorCopyParams->ApplyTonemap = false;
}

// Generate ambient occlusion buffer
void SSAOPass::CalculateSSAO()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    PUSH_PROFILE_MARKER("Calculate");

    SSAOBuffer[0]->Enable();

    const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

    // Not necesarry
    //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    HLSL::SSAOParams->HalfTexelOffset = Vec2f(
        0.5f / GBuffer.GetRenderTarget()->GetWidth(),
        0.5f / GBuffer.GetRenderTarget()->GetHeight()
        );

    SsaoShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    SsaoShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);

    SSAOBuffer[0]->Disable();

    POP_PROFILE_MARKER();
}

// Blur ambient occlusion buffer
void SSAOPass::BlurSSAO()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Blur");

    for (unsigned int i = 0; i < BlurKernelCount; i++)
    {
#if ENABLE_PROFILE_MARKERS
        char label[10];
        sprintf_s(label, "Kernel %d", SSAO_BLUR_KERNEL[i]);
#endif
        PUSH_PROFILE_MARKER(label);

        SSAOBuffer[(i + 1) % 2]->Enable();

        const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
        RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

        // Not necesarry
        //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

        HLSL::BloomParams->HalfTexelOffset = Vec2f(
            0.5f / SSAOBuffer[i % 2]->GetRenderTarget()->GetWidth(),
            0.5f / SSAOBuffer[i % 2]->GetRenderTarget()->GetHeight()
            );
        ResourceMgr->GetTexture(
            SSAOBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0)
            )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
        HLSL::Bloom_Source = SSAOBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0);
        HLSL::BloomParams->TexSize = Vec4f(
            (float)SSAOBuffer[i % 2]->GetRenderTarget()->GetWidth(),
            (float)SSAOBuffer[i % 2]->GetRenderTarget()->GetHeight(),
            1.f / (float)SSAOBuffer[i % 2]->GetRenderTarget()->GetWidth(),
            1.f / (float)SSAOBuffer[i % 2]->GetRenderTarget()->GetHeight()
            );
        HLSL::BloomParams->Kernel = SSAO_BLUR_KERNEL[i];
        HLSL::BloomParams->AdjustIntensity = false;

        // Reuse the bloom shader for blurring the ambient occlusion render target
        BloomShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        BloomShader.Disable();

        RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);

        SSAOBuffer[(i + 1) % 2]->Disable();

        POP_PROFILE_MARKER();
    }

    POP_PROFILE_MARKER();
}

// Apply ambient occlusion to the light accumulation buffer
void SSAOPass::ApplySSAO()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Apply");

    //LightAccumulationBuffer.Enable();

    const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    const Blend dstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
    const Blend srcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();
    const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
    RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_INVSRCCOLOR);
    RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ZERO);
    RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

    HLSL::ColorCopyParams->HalfTexelOffset = Vec2f(
        0.5f / SSAOBuffer[BlurKernelCount % 2]->GetRenderTarget()->GetWidth(),
        0.5f / SSAOBuffer[BlurKernelCount % 2]->GetRenderTarget()->GetHeight()
        );
    ResourceMgr->GetTexture(
        SSAOBuffer[BlurKernelCount % 2]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    HLSL::ColorCopy_SourceTexture = SSAOBuffer[BlurKernelCount % 2]->GetRenderTarget()->GetColorBuffer(0);

    ColorCopyShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    ColorCopyShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
    RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
    RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);
    RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

    //LightAccumulationBuffer.Disable();

    POP_PROFILE_MARKER();
}

void SSAOPass::Draw()
{
    if (!SSAO_ENABLED)
        return;

    //Synesthesia3D::RenderTarget* pCurrRT = Synesthesia3D::RenderTarget::GetActiveRenderTarget();
    //if (pCurrRT)
    //  pCurrRT->Disable();

    CalculateSSAO();
    BlurSSAO();
    ApplySSAO();

    //pCurrRT->Enable();
}

void SSAOPass::AllocateResources()
{

}

void SSAOPass::ReleaseResources()
{

}
