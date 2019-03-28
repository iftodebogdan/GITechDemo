/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   CopyToBackBufferPass.cpp
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
#include <RenderTarget.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "CopyToBackBufferPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

namespace GITechDemoApp
{
    int GBUFFER_DEBUG_VIEW = -1;
    bool GBUFFER_DEBUG_VIEW_DEPTH = false;
    bool DEBUG_CSM_CAMERA = false;
    bool DEBUG_RSM_CAMERA = false;

    extern bool POST_PROCESSING_ENABLED;

    // Tone mapping
    extern bool HDR_TONE_MAPPING_ENABLED;
    // Bloom
    extern bool BLOOM_ENABLED;
    // FXAA
    extern bool FXAA_ENABLED;
    // DoF
    extern bool DOF_ENABLED;
    // Motion blur
    extern bool MOTION_BLUR_ENABLED;
    // Lens flare
    extern bool LENS_FLARE_ENABLED;
}

CopyToBackBufferPass::CopyToBackBufferPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
    , m_pFinalImageBuffer(nullptr)
{}

CopyToBackBufferPass::~CopyToBackBufferPass()
{}

void CopyToBackBufferPass::Update(const float fDeltaTime)
{
    m_pFinalImageBuffer = nullptr;

    if (POST_PROCESSING_ENABLED)
    {
        m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

        if (DOF_ENABLED)
            m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

        if (MOTION_BLUR_ENABLED)
            m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

        if (BLOOM_ENABLED)
            m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

        if (LENS_FLARE_ENABLED)
            m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

        if (HDR_TONE_MAPPING_ENABLED)
            m_pFinalImageBuffer = LDRToneMappedImageBuffer.GetRenderTarget();

        if (FXAA_ENABLED)
            m_pFinalImageBuffer = LDRFxaaImageBuffer.GetRenderTarget();
    }
    else
        m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

    HLSL::ColorCopyParams->HalfTexelOffset = Vec2f(0.5f / m_pFinalImageBuffer->GetWidth(), 0.5f / m_pFinalImageBuffer->GetHeight());
    HLSL::ColorCopySourceTexture = m_pFinalImageBuffer->GetColorBuffer(0);
    HLSL::ColorCopyParams->SingleChannelCopy = false;
    HLSL::ColorCopyParams->CustomColorModulator = Vec4f(1.f, 1.f, 1.f, 1.f);
    HLSL::ColorCopyParams->ApplyTonemap = false;

    // G-Buffer debugging
    GBUFFER_DEBUG_VIEW = Math::clamp(GBUFFER_DEBUG_VIEW, -1, (int)GBuffer.GetRenderTarget()->GetTargetCount() - 1);
    if (GBUFFER_DEBUG_VIEW != -1)
    {
        HLSL::ColorCopyParams->HalfTexelOffset = Vec2f(0.5f / GBuffer.GetRenderTarget()->GetWidth(), 0.5f / GBuffer.GetRenderTarget()->GetHeight());
        HLSL::ColorCopySourceTexture = GBuffer.GetRenderTarget()->GetColorBuffer(GBUFFER_DEBUG_VIEW);
    }
    else if (GBUFFER_DEBUG_VIEW_DEPTH)
    {
        HLSL::ColorCopyParams->HalfTexelOffset = Vec2f(0.5f / LinearFullDepthBuffer.GetRenderTarget()->GetWidth(), 0.5f / LinearFullDepthBuffer.GetRenderTarget()->GetHeight());
        HLSL::ColorCopySourceTexture = LinearFullDepthBuffer.GetRenderTarget()->GetColorBuffer();
        HLSL::ColorCopyParams->SingleChannelCopy = true;
        HLSL::ColorCopyParams->CustomColorModulator = Vec4f(1.f, 1.f, 1.f, 1.f) / fZFar.GetCurrentValue();
    }

    if (DEBUG_CSM_CAMERA)
    {
        HLSL::ColorCopyParams->HalfTexelOffset = Vec2f(0.5f / ShadowMapDir.GetRenderTarget()->GetWidth(), 0.5f / ShadowMapDir.GetRenderTarget()->GetHeight());
        HLSL::ColorCopySourceTexture = ShadowMapDir.GetRenderTarget()->GetDepthBuffer();
        HLSL::ColorCopyParams->SingleChannelCopy = true;
    }
    else if (DEBUG_RSM_CAMERA)
    {
        HLSL::ColorCopyParams->HalfTexelOffset = Vec2f(0.5f / RSMBuffer.GetRenderTarget()->GetWidth(), 0.5f / RSMBuffer.GetRenderTarget()->GetHeight());
        HLSL::ColorCopySourceTexture = RSMBuffer.GetRenderTarget()->GetColorBuffer();
    }
}

void CopyToBackBufferPass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext || !m_pFinalImageBuffer)
        return;

    PUSH_PROFILE_MARKER("Copy to back buffer");

    const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
    const bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
    const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();

    RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
    RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

    // Not necesarry
    //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    ColorCopyShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    ColorCopyShader.Disable();

    RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
    RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

    POP_PROFILE_MARKER();
}

void CopyToBackBufferPass::AllocateResources()
{

}

void CopyToBackBufferPass::ReleaseResources()
{

}
