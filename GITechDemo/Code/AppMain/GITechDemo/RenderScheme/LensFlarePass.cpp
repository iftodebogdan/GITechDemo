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

LensFlarePass::LensFlarePass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

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
    HLSL::SphericalLensFlareFeatures_GhostColorLUT = LensFlareGhostColorLUT.GetTextureIndex();

    LensFlareDirtTexture.GetTexture()->SetAddressingMode(SAM_CLAMP);
    LensFlareDirtTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
    HLSL::LensFlareApply_Dirt = LensFlareDirtTexture.GetTextureIndex();

    LensFlareStarBurstTexture.GetTexture()->SetAddressingMode(SAM_CLAMP);
    LensFlareStarBurstTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
    HLSL::LensFlareApply_StarBurst = LensFlareStarBurstTexture.GetTextureIndex();

    HLSL::DownsampleParams->DownsampleFactor = 1;
    HLSL::DownsampleParams->DepthDownsample = false;
    HLSL::DownsampleParams->ApplyBrightnessFilter = true;
    HLSL::BloomParams->AdjustIntensity = false;

    // Calculate star burst matrix
    const Vec3f camX = Vec3f(
        HLSL::BRDFParams->ViewMat[0][0],
        HLSL::BRDFParams->ViewMat[0][1],
        HLSL::BRDFParams->ViewMat[0][2]);
    
    const Vec3f camZ = Vec3f(
        HLSL::BRDFParams->ViewMat[1][0],
        HLSL::BRDFParams->ViewMat[1][1],
        HLSL::BRDFParams->ViewMat[1][2]);
    
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
        HLSL::DownsampleParams->HalfTexelOffset = Vec2f(
            0.5f / HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetWidth(),
            0.5f / HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetHeight()
            );
        HLSL::DownsampleParams->TexSize = Vec4f(
            (float)HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetWidth(),
            (float)HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetHeight(),
            1.f / (float)HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetWidth(),
            1.f / (float)HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetHeight()
            );
        HLSL::Downsample_Source = HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetColorBuffer(0);
    }
    else
    {
        HLSL::DownsampleParams->HalfTexelOffset = Vec2f(
            0.5f / HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetWidth(),
            0.5f / HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetHeight()
            );
        HLSL::DownsampleParams->TexSize = Vec4f(
            (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetWidth(),
            (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetHeight(),
            1.f / (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetWidth(),
            1.f / (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetHeight()
            );
        HLSL::Downsample_Source = HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetColorBuffer(0);
    }

    const float bkp = HLSL::DownsampleParams->BrightnessThreshold;
    HLSL::DownsampleParams->BrightnessThreshold = LENS_FLARE_BRIGHTNESS_THRESHOLD;

    DownsampleShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    DownsampleShader.Disable();

    HLSL::DownsampleParams->BrightnessThreshold = bkp;

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
        HLSL::SphericalLensFlareFeaturesParams->HalfTexelOffset = Vec2f(
            0.5f / CurrentLensFlareBuffer[0]->GetRenderTarget()->GetWidth(),
            0.5f / CurrentLensFlareBuffer[0]->GetRenderTarget()->GetHeight()
            );
        HLSL::SphericalLensFlareFeaturesParams->TexSize = Vec4f(
            (float)CurrentLensFlareBuffer[0]->GetRenderTarget()->GetWidth(),
            (float)CurrentLensFlareBuffer[0]->GetRenderTarget()->GetHeight(),
            1.f / (float)CurrentLensFlareBuffer[0]->GetRenderTarget()->GetWidth(),
            1.f / (float)CurrentLensFlareBuffer[0]->GetRenderTarget()->GetHeight()
            );
        HLSL::SphericalLensFlareFeatures_Source = CurrentLensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer();

        SphericalLensFlareFeaturesShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        SphericalLensFlareFeaturesShader.Disable();
    }
    else
    {
        HLSL::AnamorphicLensFlareFeaturesParams->HalfTexelOffset = Vec2f(
            0.5f / CurrentLensFlareBuffer[2]->GetRenderTarget()->GetWidth(),
            0.5f / CurrentLensFlareBuffer[2]->GetRenderTarget()->GetHeight()
            );
        HLSL::AnamorphicLensFlareFeaturesParams->TexSize = Vec4f(
            (float)CurrentLensFlareBuffer[2]->GetRenderTarget()->GetWidth(),
            (float)CurrentLensFlareBuffer[2]->GetRenderTarget()->GetHeight(),
            1.f / (float)CurrentLensFlareBuffer[2]->GetRenderTarget()->GetWidth(),
            1.f / (float)CurrentLensFlareBuffer[2]->GetRenderTarget()->GetHeight()
            );
        HLSL::AnamorphicLensFlareFeatures_Source = CurrentLensFlareBuffer[2]->GetRenderTarget()->GetColorBuffer();

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

        HLSL::BloomParams->HalfTexelOffset = Vec2f(
            0.5f / CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            0.5f / CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight()
            );
        HLSL::BloomParams->TexSize = Vec4f(
            (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight(),
            1.f / (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            1.f / (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight()
            );
        HLSL::Bloom_Source = CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetColorBuffer(0);
        HLSL::BloomParams->Kernel = LENS_FLARE_BLUR_KERNEL[i];

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

        HLSL::AnamorphicLensFlareBlurParams->HalfTexelOffset = Vec2f(
            0.5f / CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            0.5f / CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight()
            );
        HLSL::AnamorphicLensFlareBlurParams->TexSize = Vec4f(
            (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight(),
            1.f / (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
            1.f / (float)CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight()
            );
        HLSL::AnamorphicLensFlareBlur_Source = CurrentLensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetColorBuffer(0);
        HLSL::AnamorphicLensFlareBlurParams->Kernel = i;

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

    PUSH_PROFILE_MARKER("Upscale and blend");

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
    HLSL::LensFlareApply_Features = CurrentLensFlareBuffer[(LENS_FLARE_BLUR_KERNEL_COUNT + 1) % 2]->GetRenderTarget()->GetColorBuffer();

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
