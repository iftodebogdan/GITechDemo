/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   DepthOfFieldPass.cpp
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

#include "DepthOfFieldPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

#include "GITechDemo.h"

DepthOfFieldPass::DepthOfFieldPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

DepthOfFieldPass::~DepthOfFieldPass()
{}

void DepthOfFieldPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    ResourceMgr->GetTexture(
        DepthOfFieldBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ResourceMgr->GetTexture(
        DepthOfFieldBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);

    ResourceMgr->GetTexture(
        DepthOfFieldBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
    )->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ResourceMgr->GetTexture(
        DepthOfFieldBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
    )->SetAddressingMode(SAM_CLAMP);

    HLSL::BokehDoF_DepthBuffer = LinearFullDepthBuffer.GetRenderTarget()->GetColorBuffer();

    ResourceMgr->GetTexture(AutofocusBuffer[0]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(AutofocusBuffer[1]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

    SWAP_RENDER_TARGET_HANDLES(AutofocusBuffer[0], AutofocusBuffer[1]);

    HLSL::BokehDoF_TargetFocus = AutofocusBuffer[0]->GetRenderTarget()->GetColorBuffer();

    HLSL::ColorCopyParams->SingleChannelCopy = false;
    HLSL::ColorCopyParams->CustomColorModulator = Vec4f(1.f, 1.f, 1.f, 1.f);
    HLSL::ColorCopyParams->ApplyTonemap = false;
}

void DepthOfFieldPass::AutofocusPass()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    if (!HLSL::BokehDoFParams->Autofocus[0])
        return;

    PUSH_PROFILE_MARKER("Autofocus pass");

    AutofocusBuffer[0]->Enable();

    HLSL::LumaAdaptParams->HalfTexelOffset = Vec2f(0.5f / AutofocusBuffer[1]->GetRenderTarget()->GetWidth(), 0.5f / AutofocusBuffer[1]->GetRenderTarget()->GetHeight());
    HLSL::LumaAdapt_LumaInput = AutofocusBuffer[1]->GetRenderTarget()->GetColorBuffer(0);
    HLSL::LumaAdapt_LumaTarget = LinearQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer(); // GBuffer.GetRenderTarget()->GetDepthBuffer();

    const float bkp = HLSL::LumaAdaptParams->LumaAdaptSpeed;
    HLSL::LumaAdaptParams->LumaAdaptSpeed = DOF_AUTOFOCUS_TIME;
    HLSL::LumaAdaptParams->FrameTime = gmtl::Math::clamp(((GITechDemo*)AppMain)->GetDeltaTime(), 0.f, 1.f / HLSL::LumaAdaptParams->LumaAdaptSpeed);

    // Reuse the luminance animation shader
    LumaAdaptShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    LumaAdaptShader.Disable();

    HLSL::LumaAdaptParams->LumaAdaptSpeed = bkp;

    AutofocusBuffer[0]->Disable();

    POP_PROFILE_MARKER();
}

// Apply DoF effect to a separate target with
// CoC value in the alpha channel for compositing
void DepthOfFieldPass::AccumulateDoFEffect()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    PUSH_PROFILE_MARKER("Accumulate");

    DepthOfFieldBuffer[0]->Enable();

    const bool colorBlendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

    HLSL::BokehDoFParams->HalfTexelOffset = Vec2f(
        0.5f / DepthOfFieldBuffer[1]->GetRenderTarget()->GetWidth(),
        0.5f / DepthOfFieldBuffer[1]->GetRenderTarget()->GetHeight()
        );
    HLSL::BokehDoFParams->TexSize = Vec4f(
        (float)DepthOfFieldBuffer[1]->GetRenderTarget()->GetWidth(),
        (float)DepthOfFieldBuffer[1]->GetRenderTarget()->GetHeight(),
        1.f / (float)DepthOfFieldBuffer[1]->GetRenderTarget()->GetWidth(),
        1.f / (float)DepthOfFieldBuffer[1]->GetRenderTarget()->GetHeight()
    );
    HLSL::BokehDoF_Source = DepthOfFieldBuffer[1]->GetRenderTarget()->GetColorBuffer(0);

    BokehDofShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    BokehDofShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(colorBlendEnabled);

    DepthOfFieldBuffer[0]->Disable();

    POP_PROFILE_MARKER();
}

void DepthOfFieldPass::CalculateBlurFactor()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    PUSH_PROFILE_MARKER("Calculate Blur");

    // Set aperture size to 0, so that the shader only calculates CoC values
    float bkp = HLSL::BokehDoFParams->ApertureSize;
    HLSL::BokehDoFParams->ApertureSize = 0.f;

    DepthOfFieldBuffer[0]->Enable();

    const bool colorBlendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

    HLSL::BokehDoFParams->HalfTexelOffset = Vec2f(
        0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
        0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight()
    );
    HLSL::BokehDoFParams->TexSize = Vec4f(
        (float)LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
        (float)LightAccumulationBuffer.GetRenderTarget()->GetHeight(),
        1.f / (float)LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
        1.f / (float)LightAccumulationBuffer.GetRenderTarget()->GetHeight()
    );
    HLSL::BokehDoF_Source = LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0);

    BokehDofShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    BokehDofShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(colorBlendEnabled);

    DepthOfFieldBuffer[0]->Disable();

    // Reset aperture size
    HLSL::BokehDoFParams->ApertureSize = bkp;

    POP_PROFILE_MARKER();
}

// Apply DoF buffer to the light accumulation buffer by blending
// using the information in the DoF buffer's alpha channel (CoC)
void DepthOfFieldPass::ApplyDoF()
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
    HLSL::ColorCopy_SourceTexture = (DepthOfFieldBuffer[0]->GetRenderTarget())->GetColorBuffer(0);

    ColorCopyShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    ColorCopyShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
    RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

    LightAccumulationBuffer.Disable();

    POP_PROFILE_MARKER();
}

void DepthOfFieldPass::Draw()
{
    if (!DOF_ENABLED)
        return;

    AutofocusPass();

    CalculateBlurFactor();

    float bkp = HLSL::BokehDoFParams->ApertureSize;
    for (int i = 0; i < DOF_NUM_PASSES; i++)
    {
        SWAP_RENDER_TARGET_HANDLES(DepthOfFieldBuffer[0], DepthOfFieldBuffer[1]);
        AccumulateDoFEffect();

        // Vary aperture size per pass so as to have a better sample distribution
        HLSL::BokehDoFParams->ApertureSize = HLSL::BokehDoFParams->ApertureSize - bkp / (float)DOF_NUM_PASSES;
    }
    HLSL::BokehDoFParams->ApertureSize = bkp;

    ApplyDoF();
}

void DepthOfFieldPass::AllocateResources()
{

}

void DepthOfFieldPass::ReleaseResources()
{

}
