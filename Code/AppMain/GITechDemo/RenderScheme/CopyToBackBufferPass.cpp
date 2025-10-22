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

CopyToBackBufferPass::CopyToBackBufferPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
    , m_pFinalImageBuffer(nullptr)
{}

CopyToBackBufferPass::~CopyToBackBufferPass()
{}

void CopyToBackBufferPass::Update(const float fDeltaTime)
{
    m_pFinalImageBuffer = nullptr;

    if (RenderConfig::PostProcessing::Enabled)
    {
        m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

        if (RenderConfig::PostProcessing::DepthOfField::Enabled)
            m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

        if (RenderConfig::PostProcessing::MotionBlur::Enabled)
            m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

        if (RenderConfig::PostProcessing::Bloom::Enabled)
            m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

        if (RenderConfig::PostProcessing::LensFlare::Enabled)
            m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

        if (RenderConfig::PostProcessing::ToneMapping::Enabled)
            m_pFinalImageBuffer = LDRToneMappedImageBuffer.GetRenderTarget();

        if (RenderConfig::PostProcessing::FastApproximateAntiAliasing::Enabled)
            m_pFinalImageBuffer = LDRFxaaImageBuffer.GetRenderTarget();

        if (RenderConfig::PostProcessing::ASCIIEffect::Enabled)
            m_pFinalImageBuffer = ASCIIEffectBuffer.GetRenderTarget();
    }
    else
        m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

    HLSL::ColorCopy_SourceTexture = m_pFinalImageBuffer->GetColorBuffer(0);
    HLSL::ColorCopyParams->SingleChannelCopy = false;
    HLSL::ColorCopyParams->CustomColorModulator = Vec4f(1.f, 1.f, 1.f, 1.f);
    HLSL::ColorCopyParams->ApplyTonemap = false;

    // G-Buffer debugging
    RenderConfig::GBuffer::DebugViewColor = Math::clamp(RenderConfig::GBuffer::DebugViewColor, -1, (int)GBuffer.GetRenderTarget()->GetTargetCount() - 1);
    if (RenderConfig::GBuffer::DebugViewColor != -1)
    {
        HLSL::ColorCopy_SourceTexture = GBuffer.GetRenderTarget()->GetColorBuffer(RenderConfig::GBuffer::DebugViewColor);
    }
    else if (RenderConfig::GBuffer::DebugViewDepth)
    {
        HLSL::ColorCopy_SourceTexture = LinearFullDepthBuffer.GetRenderTarget()->GetColorBuffer();
        HLSL::ColorCopyParams->SingleChannelCopy = true;
        HLSL::ColorCopyParams->CustomColorModulator = Vec4f(1.f, 1.f, 1.f, 1.f) / HLSL::PostProcessingParams->ZFar;
    }

    if (RenderConfig::CascadedShadowMaps::DebugCameraView)
    {
        HLSL::ColorCopy_SourceTexture = ShadowMapDir.GetRenderTarget()->GetDepthBuffer();
        HLSL::ColorCopyParams->SingleChannelCopy = true;
    }
    else if (RenderConfig::ReflectiveShadowMap::DebugCameraView)
    {
        HLSL::ColorCopy_SourceTexture = RSMBuffer.GetRenderTarget()->GetColorBuffer();
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
