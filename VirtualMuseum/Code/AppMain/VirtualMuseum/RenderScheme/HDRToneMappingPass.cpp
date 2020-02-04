/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   HDRToneMappingPass.cpp
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

#include "HDRToneMappingPass.h"
using namespace VirtualMuseumApp;

#include "AppResources.h"

HDRToneMappingPass::HDRToneMappingPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

HDRToneMappingPass::~HDRToneMappingPass()
{}

void HDRToneMappingPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    ResourceMgr->GetTexture(AverageLuminanceBuffer[0]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(AverageLuminanceBuffer[1]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(AverageLuminanceBuffer[2]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(AverageLuminanceBuffer[3]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

    ResourceMgr->GetTexture(AdaptedLuminance[0]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(AdaptedLuminance[1]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

    ResourceMgr->GetTexture(LDRToneMappedImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ResourceMgr->GetTexture(LDRToneMappedImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetSRGBEnabled(true);

    ColorCorrectionTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ColorCorrectionTexture.GetTexture()->SetAddressingMode(SAM_CLAMP);
    ColorCorrectionTexture.GetTexture()->SetSRGBEnabled(RenderConfig::PostProcessing::ColorCorrection::sRGBColorCorrectionTexture);

    SWAP_RENDER_TARGET_HANDLES(AdaptedLuminance[0], AdaptedLuminance[1]);

    HLSL::LumaAdaptParams->LumaAdaptSpeed = RenderConfig::PostProcessing::ToneMapping::AutoExposureSpeed;
    HLSL::HDRToneMappingParams->FrameTime = gmtl::Math::clamp(fDeltaTime, 0.f, 1.f / RenderConfig::PostProcessing::ToneMapping::AutoExposureSpeed);

    HLSL::LumaAdapt_LumaTarget = AverageLuminanceBuffer[3]->GetRenderTarget()->GetColorBuffer(0);
    HLSL::HDRToneMapping_ColorCorrectionTexture = ColorCorrectionTexture.GetTextureIndex();

    HLSL::LumaCaptureParams->AvgLumaClamp = RenderConfig::PostProcessing::ToneMapping::AvgLumaClamp;

    HLSL::HDRToneMappingParams->ExposureBias = RenderConfig::PostProcessing::ToneMapping::ExposureBias;
    HLSL::HDRToneMappingParams->ShoulderStrength = RenderConfig::PostProcessing::ToneMapping::ShoulderStrength;
    HLSL::HDRToneMappingParams->LinearStrength = RenderConfig::PostProcessing::ToneMapping::LinearStrength;
    HLSL::HDRToneMappingParams->LinearAngle = RenderConfig::PostProcessing::ToneMapping::LinearAngle;
    HLSL::HDRToneMappingParams->ToeStrength = RenderConfig::PostProcessing::ToneMapping::ToeStrength;
    HLSL::HDRToneMappingParams->ToeNumerator = RenderConfig::PostProcessing::ToneMapping::ToeNumerator;
    HLSL::HDRToneMappingParams->ToeDenominator = RenderConfig::PostProcessing::ToneMapping::ToeDenominator;
    HLSL::HDRToneMappingParams->LinearWhite = RenderConfig::PostProcessing::ToneMapping::LinearWhite;

    HLSL::HDRToneMappingParams->ApplyColorCorrection = RenderConfig::PostProcessing::ColorCorrection::ApplyColorCorrection;

    HLSL::HDRToneMappingParams->FilmGrainAmount = RenderConfig::PostProcessing::FilmGrain::FilmGrainAmount;
}

// Measure average luminance level of scene
void HDRToneMappingPass::LuminanceMeasurementPass()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    PUSH_PROFILE_MARKER("Luminance measurement");

    for (unsigned int i = 0; i < 4; i++)
    {
        switch (i)
        {
        case 0:
            PUSH_PROFILE_MARKER("64x64");
            break;
        case 1:
            PUSH_PROFILE_MARKER("16x16");
            break;
        case 2:
            PUSH_PROFILE_MARKER("4x4");
            break;
        case 3:
            PUSH_PROFILE_MARKER("1x1");
        }

        AverageLuminanceBuffer[i]->Enable();

        // Not necesarry
        //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

        HLSL::HDRToneMappingParams->HalfTexelOffset = Vec2f(
            0.5f / AverageLuminanceBuffer[i]->GetRenderTarget()->GetWidth(),
            0.5f / AverageLuminanceBuffer[i]->GetRenderTarget()->GetHeight()
            );

        if (i == 0)
        {
            HLSL::LumaCaptureParams->TexSize = Vec4f(
                (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetWidth(),
                (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetHeight(),
                1.f / (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetWidth(),
                1.f / (float)HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetHeight()
            );
            HLSL::LumaCapture_LumaInput = HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetColorBuffer(0);
            HLSL::LumaCaptureParams->InitialLumaPass = true;
            HLSL::LumaCaptureParams->FinalLumaPass = false;
        }
        else
        {
            HLSL::LumaCaptureParams->TexSize = Vec4f(
                (float)AverageLuminanceBuffer[i - 1]->GetRenderTarget()->GetWidth(),
                (float)AverageLuminanceBuffer[i - 1]->GetRenderTarget()->GetHeight(),
                1.f / (float)AverageLuminanceBuffer[i - 1]->GetRenderTarget()->GetWidth(),
                1.f / (float)AverageLuminanceBuffer[i - 1]->GetRenderTarget()->GetHeight()
            );
            HLSL::LumaCapture_LumaInput = AverageLuminanceBuffer[i - 1]->GetRenderTarget()->GetColorBuffer(0);
            HLSL::LumaCaptureParams->InitialLumaPass = false;

            if (i == 3)
                HLSL::LumaCaptureParams->FinalLumaPass = true;
            else
                HLSL::LumaCaptureParams->FinalLumaPass = false;
        }

        LumaCaptureShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        LumaCaptureShader.Disable();

        AverageLuminanceBuffer[i]->Disable();

        POP_PROFILE_MARKER();
    }

    POP_PROFILE_MARKER();
}

void HDRToneMappingPass::LuminanceAdaptationPass()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    PUSH_PROFILE_MARKER("Luminance adaptation");

    AdaptedLuminance[0]->Enable();

    HLSL::HDRToneMappingParams->HalfTexelOffset = Vec2f(0.5f / AdaptedLuminance[1]->GetRenderTarget()->GetWidth(), 0.5f / AdaptedLuminance[1]->GetRenderTarget()->GetHeight());
    HLSL::LumaAdapt_LumaInput = AdaptedLuminance[1]->GetRenderTarget()->GetColorBuffer(0);

    LumaAdaptShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    LumaAdaptShader.Disable();

    AdaptedLuminance[0]->Disable();

    POP_PROFILE_MARKER();
}

void HDRToneMappingPass::ToneMappingPass()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    PUSH_PROFILE_MARKER("Tone mapping");

    LDRToneMappedImageBuffer.Enable();

    const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
    RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);

    // Not necesarry
    //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    HLSL::HDRToneMappingParams->HalfTexelOffset = Vec2f(0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(), 0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight());
    HLSL::HDRToneMapping_SourceTexture = LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0);
    HLSL::HDRToneMapping_AvgLumaTexture = AdaptedLuminance[0]->GetRenderTarget()->GetColorBuffer(0);

    HDRToneMappingShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    HDRToneMappingShader.Disable();

    RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);

    LDRToneMappedImageBuffer.Disable();

    POP_PROFILE_MARKER();
}

void HDRToneMappingPass::Draw()
{
    if (!RenderConfig::PostProcessing::ToneMapping::Enabled)
        return;

    LuminanceMeasurementPass();
    LuminanceAdaptationPass();
    ToneMappingPass();
}

void HDRToneMappingPass::AllocateResources()
{

}

void HDRToneMappingPass::ReleaseResources()
{

}
