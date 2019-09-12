/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   LensFlarePass.cpp
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
#include <RenderTarget.h>
#include <Texture.h>
#include <RenderState.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "LensFlarePass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

namespace GITechDemoApp
{
    bool LENS_FLARE_ENABLED = true;
    float LENS_FLARE_BRIGHTNESS_THRESHOLD = 3.5f;
    bool LENS_FLARE_ANAMORPHIC = true;

    const unsigned int LENS_FLARE_BLUR_KERNEL_COUNT = 3;
    const unsigned int LENS_FLARE_BLUR_KERNEL[LENS_FLARE_BLUR_KERNEL_COUNT] = { 0, 1, 2 };

    const unsigned int LENS_FLARE_ANAMORPHIC_BLUR_PASSES = 6;
}

LensFlarePass::LensFlarePass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{
    DirtIntensity = 0.3f;
    StarBurstIntensity = 0.5f;
}

LensFlarePass::~LensFlarePass()
{}

void LensFlarePass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    LensFlareGhostColorLUT.GetTexture()->SetAddressingMode(SAM_MIRROR);
    LensFlareGhostColorLUT.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    texGhostColorLUT = LensFlareGhostColorLUT.GetTextureIndex();

    LensFlareDirtTexture.GetTexture()->SetAddressingMode(SAM_CLAMP);
    LensFlareDirtTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
    LensFlareDirt = LensFlareDirtTexture.GetTextureIndex();

    LensFlareStarBurstTexture.GetTexture()->SetAddressingMode(SAM_CLAMP);
    LensFlareStarBurstTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
    LensFlareStarBurst = LensFlareStarBurstTexture.GetTextureIndex();

    nDownsampleFactor = 1;
    bDepthDownsample = false;
    bApplyBrightnessFilter = true;
    bAdjustIntensity = false;

    // Calculate star burst matrix
    const Vec3f camX = Vec3f(
        f44ViewMat.GetCurrentValue()[0][0],
        f44ViewMat.GetCurrentValue()[0][1],
        f44ViewMat.GetCurrentValue()[0][2]);
    
    const Vec3f camZ = Vec3f(
        f44ViewMat.GetCurrentValue()[1][0],
        f44ViewMat.GetCurrentValue()[1][1],
        f44ViewMat.GetCurrentValue()[1][2]);
    
    const float camRot = dot(camX, Vec3f(0.f, 0.f, 1.f)) + dot(camZ, Vec3f(0.f, 1.f, 0.f));

    Matrix33f scaleBias1;
    scaleBias1.set(
        2.f,    0.f,    -1.f,
        0.f,    2.f,    -1.f,
        0.f,    0.f,    1.f);

    Matrix33f rotMat;
    rotMat.set(
        cosf(camRot),   -sinf(camRot),  0.f,
        sinf(camRot),   cosf(camRot),   0.f,
            0.f,            0.f,        1.f);

    Matrix33f scaleBias2;
    scaleBias2.set(
        0.5f,   0.f,    0.5f,
        0.f,    0.5f,   0.5f,
        0.f,    0.f,    1.f);

    HLSL::LensFlareApplyParams->StarBurstMat = scaleBias2 * rotMat * scaleBias1;

    HLSL::LensFlareApplyParams->DirtIntensity = DirtIntensity;
    HLSL::LensFlareApplyParams->StarBurstIntensity = StarBurstIntensity;

    if (LENS_FLARE_ANAMORPHIC)
        CurrentLensFlareBuffer = AnamorphicLensFlareBuffer;
    else
        CurrentLensFlareBuffer = SphericalLensFlareBuffer;
}

void LensFlarePass::Draw()
{
    if (!LENS_FLARE_ENABLED)
        return;

    ApplyBrightnessFilter();
    GenerateFeatures();
    if (LENS_FLARE_ANAMORPHIC)
        AnamorphicBlur();
    else
        Blur();
    UpscaleAndBlend();
}

void LensFlarePass::ApplyBrightnessFilter()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    PUSH_PROFILE_MARKER("Brightness filter");

    if (!LENS_FLARE_ANAMORPHIC)
        CurrentLensFlareBuffer[0]->Enable();
    else
        CurrentLensFlareBuffer[2]->Enable();

    // Not necesarry
    //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    if (!LENS_FLARE_ANAMORPHIC)
    {
        f2HalfTexelOffset = Vec2f(
            0.5f / HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetWidth(),
            0.5f / HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetHeight()
            );
        f4TexSize = Vec4f(
            (float)HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetWidth(),
            (float)HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetHeight(),
            1.f / (float)HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetWidth(),
            1.f / (float)HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetHeight()
            );
        texSource = HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetColorBuffer(0);
    }
    else
    {
        f2HalfTexelOffset = Vec2f(
            0.5f / HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetWidth(),
            0.5f / HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetHeight()
            );
        f4TexSize = Vec4f(
            (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetWidth(),
            (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetHeight(),
            1.f / (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetWidth(),
            1.f / (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetHeight()
            );
        texSource = HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetColorBuffer(0);
    }

    const float bkp = fBrightnessThreshold;
    fBrightnessThreshold = LENS_FLARE_BRIGHTNESS_THRESHOLD;

    DownsampleShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    DownsampleShader.Disable();

    fBrightnessThreshold = bkp;

    if (!LENS_FLARE_ANAMORPHIC)
        CurrentLensFlareBuffer[0]->Disable();
    else
        CurrentLensFlareBuffer[2]->Disable();

    POP_PROFILE_MARKER();
}

void LensFlarePass::GenerateFeatures()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Feature generation");

    if (!LENS_FLARE_ANAMORPHIC)
    {
        ResourceMgr->GetTexture(
            CurrentLensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
            )->SetAddressingMode(SAM_CLAMP);

        ResourceMgr->GetTexture(
            CurrentLensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
            )->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    }
    else
    {
        ResourceMgr->GetTexture(
            CurrentLensFlareBuffer[2]->GetRenderTarget()->GetColorBuffer(0)
            )->SetAddressingMode(SAM_CLAMP);

        ResourceMgr->GetTexture(
            CurrentLensFlareBuffer[2]->GetRenderTarget()->GetColorBuffer(0)
            )->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    }

    CurrentLensFlareBuffer[1]->Enable();

    // Not necesarry
    //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    if (!LENS_FLARE_ANAMORPHIC)
    {
        f2HalfTexelOffset = Vec2f(
            0.5f / CurrentLensFlareBuffer[0]->GetRenderTarget()->GetWidth(),
            0.5f / CurrentLensFlareBuffer[0]->GetRenderTarget()->GetHeight()
            );
        f4TexSize = Vec4f(
            (float)CurrentLensFlareBuffer[0]->GetRenderTarget()->GetWidth(),
            (float)CurrentLensFlareBuffer[0]->GetRenderTarget()->GetHeight(),
            1.f / (float)CurrentLensFlareBuffer[0]->GetRenderTarget()->GetWidth(),
            1.f / (float)CurrentLensFlareBuffer[0]->GetRenderTarget()->GetHeight()
            );
        texSource = CurrentLensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer();

        SphericalLensFlareFeaturesShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        SphericalLensFlareFeaturesShader.Disable();
    }
    else
    {
        f2HalfTexelOffset = Vec2f(
            0.5f / CurrentLensFlareBuffer[2]->GetRenderTarget()->GetWidth(),
            0.5f / CurrentLensFlareBuffer[2]->GetRenderTarget()->GetHeight()
            );
        f4TexSize = Vec4f(
            (float)CurrentLensFlareBuffer[2]->GetRenderTarget()->GetWidth(),
            (float)CurrentLensFlareBuffer[2]->GetRenderTarget()->GetHeight(),
            1.f / (float)CurrentLensFlareBuffer[2]->GetRenderTarget()->GetWidth(),
            1.f / (float)CurrentLensFlareBuffer[2]->GetRenderTarget()->GetHeight()
            );
        texSource = CurrentLensFlareBuffer[2]->GetRenderTarget()->GetColorBuffer();

        AnamorphicLensFlareFeaturesShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        AnamorphicLensFlareFeaturesShader.Disable();
    }

    CurrentLensFlareBuffer[1]->Disable();

    POP_PROFILE_MARKER();
}

void LensFlarePass::Blur()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Blur");

    ResourceMgr->GetTexture(
        CurrentLensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(
        CurrentLensFlareBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);

    ResourceMgr->GetTexture(
        CurrentLensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(
        CurrentLensFlareBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

    for (unsigned int i = 0; i < LENS_FLARE_BLUR_KERNEL_COUNT; i++)
    {
#if ENABLE_PROFILE_MARKERS
        char label[10];
        sprintf_s(label, "Kernel %d", LENS_FLARE_BLUR_KERNEL[i]);
#endif
        PUSH_PROFILE_MARKER(label);

        CurrentLensFlareBuffer[i % 2]->Enable();

        // Not necesarry
        //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

        f2HalfTexelOffset = Vec2f(
            0.5f / CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            0.5f / CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight()
            );
        f4TexSize = Vec4f(
            (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight(),
            1.f / (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            1.f / (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight()
            );
        texSource = CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetColorBuffer(0);
        nKernel = LENS_FLARE_BLUR_KERNEL[i];

        // Reuse bloom shader for Kawase blur
        BloomShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        BloomShader.Disable();

        CurrentLensFlareBuffer[i % 2]->Disable();

        POP_PROFILE_MARKER();
    }

    POP_PROFILE_MARKER();
}

void LensFlarePass::AnamorphicBlur()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Blur");

    ResourceMgr->GetTexture(
        CurrentLensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);
    ResourceMgr->GetTexture(
        CurrentLensFlareBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);

    ResourceMgr->GetTexture(
        CurrentLensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(
        CurrentLensFlareBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

    for (unsigned int i = 0; i < LENS_FLARE_ANAMORPHIC_BLUR_PASSES; i++)
    {
#if ENABLE_PROFILE_MARKERS
        char label[10];
        sprintf_s(label, "Kernel %d", i);
#endif
        PUSH_PROFILE_MARKER(label);

        CurrentLensFlareBuffer[i % 2]->Enable();

        // Not necesarry
        //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

        f2HalfTexelOffset = Vec2f(
            0.5f / CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            0.5f / CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight()
            );
        f4TexSize = Vec4f(
            (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight(),
            1.f / (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            1.f / (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight()
            );
        texSource = CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetColorBuffer(0);
        nKernel = i;

        AnamorphicLensFlareBlurShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        AnamorphicLensFlareBlurShader.Disable();

        CurrentLensFlareBuffer[i % 2]->Disable();

        POP_PROFILE_MARKER();
    }

    POP_PROFILE_MARKER();
}

void LensFlarePass::UpscaleAndBlend()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Upscale and blend + full-resolution features");

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

    HLSL::LensFlareApplyParams->HalfTexelOffset = Vec2f(0.5f / CurrentLensFlareBuffer[(LENS_FLARE_BLUR_KERNEL_COUNT + 1) % 2]->GetRenderTarget()->GetWidth(), 0.5f / CurrentLensFlareBuffer[(LENS_FLARE_BLUR_KERNEL_COUNT + 1) % 2]->GetRenderTarget()->GetHeight());
    ResourceMgr->GetTexture(
        CurrentLensFlareBuffer[(LENS_FLARE_BLUR_KERNEL_COUNT + 1) % 2]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    LensFlareFeatures = CurrentLensFlareBuffer[(LENS_FLARE_BLUR_KERNEL_COUNT + 1) % 2]->GetRenderTarget()->GetColorBuffer();

    LensFlareApplyShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    LensFlareApplyShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
    RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
    RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
    RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

    LightAccumulationBuffer.Disable();

    POP_PROFILE_MARKER();
}

void LensFlarePass::AllocateResources()
{

}

void LensFlarePass::ReleaseResources()
{

}
