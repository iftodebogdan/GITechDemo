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

namespace GITechDemoApp
{
    bool INDIRECT_LIGHT_ENABLED = true;
    bool RSM_USE_QUARTER_RESOLUTION_BUFFER = true;
    bool RSM_USE_BILATERAL_BLUR = true;

    extern const unsigned int RSM_SIZE = 1024;
    const unsigned int RSM_NUM_PASSES = 4;
    const unsigned int RSM_SAMPLES_PER_PASS = 16;
    const unsigned int RSM_NUM_SAMPLES = RSM_NUM_PASSES * RSM_SAMPLES_PER_PASS;
}

DirectionalIndirectLightPass::DirectionalIndirectLightPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{
}

DirectionalIndirectLightPass::~DirectionalIndirectLightPass()
{
}

void DirectionalIndirectLightPass::AllocateResources()
{
    f3RSMKernel = new Vec3f[RSM_NUM_SAMPLES];

    // Generate Poisson-disk sampling pattern
    std::vector<sPoint> poisson;
    float minDist = sqrt((float)RSM_NUM_SAMPLES) / (float)RSM_NUM_SAMPLES * 0.7f;
    do
    {
        poisson =
            GeneratePoissonPoints(
                minDist,
                30,
                RSM_NUM_SAMPLES
            );
    } while (poisson.size() != RSM_NUM_SAMPLES);

    // Warp kernel so as to distribute more samples towards the exterior
    for (unsigned int i = 0; i < RSM_NUM_SAMPLES; i++)
    {
        // Increase sample density towards the outside of the kernel
        f3RSMKernel[i][0] = sqrt(abs(poisson[i].x - 0.5f) * 2.f) * ((poisson[i].x < 0.5f) ? -1.f : 1.f);
        f3RSMKernel[i][1] = sqrt(abs(poisson[i].y - 0.5f) * 2.f) * ((poisson[i].y < 0.5f) ? -1.f : 1.f);

        // Linear weights combined with non-linear sample density has proven
        // to provide very good quality with very little jitter / noise
        f3RSMKernel[i][2] = length(Vec2f(poisson[i].x - 0.5f, poisson[i].y - 0.5f)) * 2.f;
    }
}

void DirectionalIndirectLightPass::ReleaseResources()
{
    if (f3RSMKernel)
        delete[] f3RSMKernel;
}

void DirectionalIndirectLightPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    texRSMFluxBuffer = RSMBuffer.GetRenderTarget()->GetColorBuffer(0);
    texRSMNormalBuffer = RSMBuffer.GetRenderTarget()->GetColorBuffer(1);
    texRSMDepthBuffer = RSMBuffer.GetRenderTarget()->GetDepthBuffer();
    texNormalBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(3); // use vertex normals to reduce noise
}

void DirectionalIndirectLightPass::Draw()
{
    if (!INDIRECT_LIGHT_ENABLED)
        return;

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Apply");

    if (RSM_USE_QUARTER_RESOLUTION_BUFFER)
    {
        IndirectLightAccumulationBuffer[0]->Enable();
        RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);
    }

    f2HalfTexelOffset = Vec2f(
        0.5f / GBuffer.GetRenderTarget()->GetWidth(),
        0.5f / GBuffer.GetRenderTarget()->GetHeight()
    );

    Vec3f* const f3RSMKernelBkp = f3RSMKernel;

    for (unsigned int i = 0; i < RSM_NUM_PASSES; i++, f3RSMKernel += RSM_SAMPLES_PER_PASS)
    {
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

    f3RSMKernel = f3RSMKernelBkp;

    POP_PROFILE_MARKER();

    if (RSM_USE_QUARTER_RESOLUTION_BUFFER)
    {
        IndirectLightAccumulationBuffer[0]->Disable();

        Blur();

        Upscale();
    }
}

void DirectionalIndirectLightPass::Blur()
{
    if (!RSM_USE_BILATERAL_BLUR)
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

    texDepthBuffer = LinearQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer();

    PUSH_PROFILE_MARKER("Horizontal");

    IndirectLightAccumulationBuffer[1]->Enable();

    RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    f2HalfTexelOffset = Vec2f(0.5f / IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(), 0.5f / IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight());
    f4TexSize = Vec4f(
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

    texSource = IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0);
    f2BlurDir = Vec2f(1.f, 0.f);

    BilateralBlurShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    BilateralBlurShader.Disable();

    IndirectLightAccumulationBuffer[1]->Disable();

    POP_PROFILE_MARKER();

    PUSH_PROFILE_MARKER("Vertical");

    IndirectLightAccumulationBuffer[0]->Enable();

    f2HalfTexelOffset = Vec2f(0.5f / IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetWidth(), 0.5f / IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetHeight());
    f4TexSize = Vec4f(
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

    texSource = IndirectLightAccumulationBuffer[1]->GetRenderTarget()->GetColorBuffer(0);
    f2BlurDir = Vec2f(0.f, 1.f);

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
    if (!RSM_USE_QUARTER_RESOLUTION_BUFFER)
        return;

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Upscale");

    f2HalfTexelOffset = Vec2f(
        0.5f / IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
        0.5f / IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight()
    );

    ResourceMgr->GetTexture(
        IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
    )->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ResourceMgr->GetTexture(
        IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
    )->SetAddressingMode(SAM_CLAMP);

    texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
    texSource = IndirectLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0);

    RSMUpscaleShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    RSMUpscaleShader.Disable();

    POP_PROFILE_MARKER();
}