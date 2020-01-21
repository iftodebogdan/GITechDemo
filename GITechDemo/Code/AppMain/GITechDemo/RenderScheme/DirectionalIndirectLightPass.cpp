/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   DirectionalIndirectLightPass.cpp
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

#include "Poisson.h"

#include "DirectionalIndirectLightPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

DirectionalIndirectLightPass::DirectionalIndirectLightPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{
}

DirectionalIndirectLightPass::~DirectionalIndirectLightPass()
{
}

void DirectionalIndirectLightPass::AllocateResources()
{
    // Generate Poisson-disk sampling pattern
    std::vector<sPoint> poisson;
    float minDist = sqrt((float)HLSL::RSM::SampleCount) / (float)HLSL::RSM::SampleCount * 0.7f;
    do
    {
        poisson =
            GeneratePoissonPoints(
                minDist,
                30,
                HLSL::RSM::SampleCount
            );
    } while (poisson.size() != HLSL::RSM::SampleCount);

    // Warp kernel so as to distribute more samples towards the exterior
    for (unsigned int i = 0; i < HLSL::RSM::SampleCount; i++)
    {
        // Increase sample density towards the outside of the kernel
        HLSL::RSMCommonParams->KernelUpscalePass[i][0] = sqrt(abs(poisson[i].x - 0.5f) * 2.f) * ((poisson[i].x < 0.5f) ? -1.f : 1.f);
        HLSL::RSMCommonParams->KernelUpscalePass[i][1] = sqrt(abs(poisson[i].y - 0.5f) * 2.f) * ((poisson[i].y < 0.5f) ? -1.f : 1.f);

        // Linear weights combined with non-linear sample density has proven
        // to provide very good quality with very little jitter / noise
        HLSL::RSMCommonParams->KernelUpscalePass[i][2] = length(Vec2f(poisson[i].x - 0.5f, poisson[i].y - 0.5f)) * 2.f;
    }
}

void DirectionalIndirectLightPass::ReleaseResources()
{
}

void DirectionalIndirectLightPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    HLSL::RSMCommon_RSMFluxBuffer = RSMBuffer.GetRenderTarget()->GetColorBuffer(0);
    HLSL::RSMCommon_RSMNormalBuffer = RSMBuffer.GetRenderTarget()->GetColorBuffer(1);
    HLSL::RSMCommon_RSMDepthBuffer = RSMBuffer.GetRenderTarget()->GetDepthBuffer();
    HLSL::RSMCommon_NormalBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(3); // use vertex normals to reduce noise
}

void DirectionalIndirectLightPass::Draw()
{
    if (!RenderConfig::ReflectiveShadowMap::Enabled)
        return;

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Apply");

    if (RenderConfig::ReflectiveShadowMap::QuarterResolution)
    {
        IndirectLightAccumulationBuffer[0]->Enable();
        RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);
    }

    HLSL::RSMApplyParams->HalfTexelOffset = Vec2f(
        0.5f / GBuffer.GetRenderTarget()->GetWidth(),
        0.5f / GBuffer.GetRenderTarget()->GetHeight()
    );

    for (unsigned int i = 0; i < HLSL::RSM::PassCount; i++)
    {
        for (unsigned int j = 0; j < HLSL::RSM::SamplesPerPass; j++)
        {
            HLSL::RSMCommonParams->KernelApplyPass[j] = HLSL::RSMCommonParams->KernelUpscalePass[j + i * HLSL::RSM::SamplesPerPass];
        }

#if ENABLE_PROFILE_MARKERS
        char marker[16];
        sprintf_s(marker, "Pass %d", i);
#endif
        PUSH_PROFILE_MARKER(marker);

        RSMApplyShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        RSMApplyShader.Disable();

        POP_PROFILE_MARKER();
    }

    POP_PROFILE_MARKER();

    if (RenderConfig::ReflectiveShadowMap::QuarterResolution)
    {
        IndirectLightAccumulationBuffer[0]->Disable();

        Blur();

        Upscale();
    }
}

void DirectionalIndirectLightPass::Blur()
{
    if (!RenderConfig::ReflectiveShadowMap::BilateralBlur)
        return;

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Bilateral blur");

    const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

    HLSL::BilateralBlur_DepthBuffer = LinearQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer();

    PUSH_PROFILE_MARKER("Horizontal");

    IndirectLightAccumulationBuffer[1]->Enable();

    RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    HLSL::BilateralBlurParams->HalfTexelOffset = Vec2f(0.5f / IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(), 0.5f / IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight());
    HLSL::BilateralBlurParams->TexSize = Vec4f(
        (float)IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
        (float)IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight(),
        1.f / (float)IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
        1.f / (float)IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight()
    );

    ResourceMgr->GetTexture(
        IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
    )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(
        IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
    )->SetAddressingMode(SAM_CLAMP);

    HLSL::BilateralBlur_Source = IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0);
    HLSL::BilateralBlurParams->BlurDir = Vec2f(1.f, 0.f);

    BilateralBlurShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    BilateralBlurShader.Disable();

    IndirectLightAccumulationBuffer[1]->Disable();

    POP_PROFILE_MARKER();

    PUSH_PROFILE_MARKER("Vertical");

    IndirectLightAccumulationBuffer[0]->Enable();

    HLSL::BilateralBlurParams->HalfTexelOffset = Vec2f(0.5f / IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetWidth(), 0.5f / IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetHeight());
    HLSL::BilateralBlurParams->TexSize = Vec4f(
        (float)IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetWidth(),
        (float)IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetHeight(),
        1.f / (float)IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetWidth(),
        1.f / (float)IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetHeight()
    );

    ResourceMgr->GetTexture(
        IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
    )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(
        IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
    )->SetAddressingMode(SAM_CLAMP);

    HLSL::BilateralBlur_Source = IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetColorBuffer(0);
    HLSL::BilateralBlurParams->BlurDir = Vec2f(0.f, 1.f);

    BilateralBlurShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    BilateralBlurShader.Disable();

    IndirectLightAccumulationBuffer[0]->Disable();

    POP_PROFILE_MARKER();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);

    POP_PROFILE_MARKER();
}

void DirectionalIndirectLightPass::Upscale()
{
    if (!RenderConfig::ReflectiveShadowMap::QuarterResolution)
        return;

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Upscale");

    HLSL::RSMUpscaleParams->HalfTexelOffset = Vec2f(
        0.5f / IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
        0.5f / IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight()
    );

    assert(unsigned long long(&HLSL::RSMUpscaleParams.GetCurrentValue()) % 16 == 0);
    assert(unsigned long long(&HLSL::RSMUpscaleParams->HalfTexelOffset) % 16 == 0);
    assert(unsigned long long(&HLSL::RSMUpscaleParams->WeightThreshold) % 16 == 0);
    assert(unsigned long long(&HLSL::RSMUpscaleParams->DebugUpscalePass) % 16 == 0);

    ResourceMgr->GetTexture(
        IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
    )->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ResourceMgr->GetTexture(
        IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
    )->SetAddressingMode(SAM_CLAMP);

    HLSL::RSMUpscale_DepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
    HLSL::RSMUpscale_Source = IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0);

    RSMUpscaleShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    RSMUpscaleShader.Disable();

    POP_PROFILE_MARKER();
}