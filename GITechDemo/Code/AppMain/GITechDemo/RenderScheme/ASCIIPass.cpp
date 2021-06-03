/*=============================================================================
 * This file is part of the "GITechDemo_Shared" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   ASCIIPass.cpp
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

#include "ASCIIPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

ASCIIPass::ASCIIPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
    , m_pSourceImageMipChain(nullptr)
    , m_nSourceImageMipChainIdx(~0u)
{}

ASCIIPass::~ASCIIPass()
{}

void ASCIIPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    Synesthesia3D::RenderTarget* sourceImage = LightAccumulationBuffer.GetRenderTarget();
    if (RenderConfig::PostProcessing::ToneMapping::Enabled)
        sourceImage = LDRToneMappedImageBuffer.GetRenderTarget();
    if (RenderConfig::PostProcessing::FastApproximateAntiAliasing::Enabled)
        sourceImage = LDRFxaaImageBuffer.GetRenderTarget();

    if (!m_pSourceImageMipChain ||
        sourceImage->GetWidth() != m_pSourceImageMipChain->GetWidth() ||
        sourceImage->GetHeight() != m_pSourceImageMipChain->GetHeight())
    {
        if (m_pSourceImageMipChain)
        {
            ResourceMgr->ReleaseRenderTarget(m_nSourceImageMipChainIdx);
            m_pSourceImageMipChain = nullptr;
        }

        m_nSourceImageMipChainIdx = ResourceMgr->CreateRenderTarget(
            sourceImage->GetTargetCount(),
            sourceImage->GetPixelFormat(),
            sourceImage->GetWidth(),
            sourceImage->GetHeight(),
            true,
            false,
            PF_NONE);
        m_pSourceImageMipChain = ResourceMgr->GetRenderTarget(m_nSourceImageMipChainIdx);
    }

    Synesthesia3D::Texture* const sourceImageMipChain = ResourceMgr->GetTexture(m_pSourceImageMipChain->GetColorBuffer());
    sourceImageMipChain->SetAddressingMode(SAM_CLAMP);
    sourceImageMipChain->SetFilter(SF_MIN_MAG_POINT_MIP_POINT);
    sourceImageMipChain->SetSRGBEnabled(true);

    ResourceMgr->GetTexture(ASCIIEffectBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(ASCIIEffectBuffer.GetRenderTarget()->GetColorBuffer(0))->SetSRGBEnabled(true);

    HLSL::ColorCopy_SourceTexture = sourceImage->GetColorBuffer();
    HLSL::ColorCopyParams->SingleChannelCopy = false;
    HLSL::ColorCopyParams->CustomColorModulator = Vec4f(1.f, 1.f, 1.f, 1.f);
    HLSL::ColorCopyParams->ApplyTonemap = false;

    ASCIIMap.GetTexture()->SetAddressingMode(SAM_CLAMP);
    ASCIIMap.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ASCIIMap.GetTexture()->SetSRGBEnabled(true);

    HLSL::ASCIIEffect_ASCIIMap = ASCIIMap;
    HLSL::ASCIIEffect_Source = sourceImage->GetColorBuffer(0);
    HLSL::ASCIIEffect_SourceMipChain = m_pSourceImageMipChain->GetColorBuffer(0);

    HLSL::ASCIIEffectParams->SourceTexSize = Vec4f(
        (float)sourceImage->GetWidth(),
        (float)sourceImage->GetHeight(),
        1.f / (float)sourceImage->GetWidth(),
        1.f / (float)sourceImage->GetHeight()
    );
    HLSL::ASCIIEffectParams->OutputSize = Vec4f(
        (float)(sourceImage->GetWidth() >> RenderConfig::PostProcessing::ASCIIEffect::ResolutionDescaler),
        (float)(sourceImage->GetHeight() >> RenderConfig::PostProcessing::ASCIIEffect::ResolutionDescaler),
        1.f / (float)(sourceImage->GetWidth() >> RenderConfig::PostProcessing::ASCIIEffect::ResolutionDescaler),
        1.f / (float)(sourceImage->GetHeight() >> RenderConfig::PostProcessing::ASCIIEffect::ResolutionDescaler)
    );
    RenderConfig::PostProcessing::ASCIIEffect::ResolutionDescaler = gmtl::Math::clamp(RenderConfig::PostProcessing::ASCIIEffect::ResolutionDescaler, 0, (int)sourceImageMipChain->GetMipCount() - 1);
    HLSL::ASCIIEffectParams->ResolutionDescaler = RenderConfig::PostProcessing::ASCIIEffect::ResolutionDescaler;
    HLSL::ASCIIEffectParams->Gamma = RenderConfig::PostProcessing::ASCIIEffect::Gamma;
    HLSL::ASCIIEffectParams->UseColor = RenderConfig::PostProcessing::ASCIIEffect::UseColor;
}

void ASCIIPass::Draw()
{
    if (!RenderConfig::PostProcessing::ASCIIEffect::Enabled)
        return;

    CopySourceImageAndGenerateMips();
    ApplyASCIIEffect();
}

void ASCIIPass::CopySourceImageAndGenerateMips()
{
    Renderer* RenderContext = Renderer::GetInstance();
    RenderState* RenderStateManager = RenderContext->GetRenderStateManager();
    if (!RenderContext || !RenderStateManager)
        return;

    PUSH_PROFILE_MARKER("Source image copy and auto mip generation");

    const bool sRGBEnabled = RenderStateManager->GetSRGBWriteEnabled();
    const bool blendEnabled = RenderStateManager->GetColorBlendEnabled();

    RenderStateManager->SetSRGBWriteEnabled(true);
    RenderStateManager->SetColorBlendEnabled(false);

    m_pSourceImageMipChain->Enable();

    ColorCopyShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    ColorCopyShader.Disable();

    m_pSourceImageMipChain->Disable();

    RenderStateManager->SetSRGBWriteEnabled(sRGBEnabled);
    RenderStateManager->SetColorBlendEnabled(blendEnabled);

    POP_PROFILE_MARKER();
}

void ASCIIPass::ApplyASCIIEffect()
{
    Renderer* RenderContext = Renderer::GetInstance();
    RenderState* RenderStateManager = RenderContext->GetRenderStateManager();
    if (!RenderContext || !RenderStateManager)
        return;

    PUSH_PROFILE_MARKER("Apply ASCII effect");

    ASCIIEffectBuffer.Enable();

    const bool sRGBEnabled = RenderStateManager->GetSRGBWriteEnabled();
    const bool zWrite = RenderStateManager->GetZWriteEnabled();
    const Cmp zFunc = RenderStateManager->GetZFunc();

    RenderStateManager->SetSRGBWriteEnabled(true);
    RenderStateManager->SetZWriteEnabled(false);
    RenderStateManager->SetZFunc(CMP_ALWAYS);

    ASCIIEffect.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    ASCIIEffect.Disable();

    RenderStateManager->SetSRGBWriteEnabled(sRGBEnabled);
    RenderStateManager->SetZWriteEnabled(zWrite);
    RenderStateManager->SetZFunc(zFunc);

    ASCIIEffectBuffer.Disable();

    POP_PROFILE_MARKER();
}

void ASCIIPass::AllocateResources()
{}

void ASCIIPass::ReleaseResources()
{}
