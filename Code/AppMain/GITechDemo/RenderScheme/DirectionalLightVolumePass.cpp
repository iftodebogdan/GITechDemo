/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   DirectionalLightVolumePass.cpp
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
#include <RenderTarget.h>
#include <Texture.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "GITechDemo.h"
#include "DirectionalLightVolumePass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

DirectionalLightVolumePass::DirectionalLightVolumePass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{
    HLSL::DirectionalLightVolumeParams->ElapsedTime = 0.f;
}

DirectionalLightVolumePass::~DirectionalLightVolumePass()
{}

void DirectionalLightVolumePass::Update(const float fDeltaTime)
{
    if (RenderConfig::DirectionalLightVolume::QuarterResolution)
        VolumetricLightAccumulationBuffer = VolumetricLightQuarterBuffer;
    else
        VolumetricLightAccumulationBuffer = VolumetricLightFullBuffer;

    Vec4f v4CamPosLightVS = HLSL::FrameParams->ScreenToLightViewMat * Vec4f(0.f, 0.f, 0.f, 1.f);
    v4CamPosLightVS /= v4CamPosLightVS[3];
    HLSL::DirectionalLightVolumeParams->CameraPositionLightVS = Vec3f(v4CamPosLightVS[0], v4CamPosLightVS[1], v4CamPosLightVS[2]);
    HLSL::DirectionalLightVolumeParams->RaymarchDistanceLimit = RenderConfig::CascadedShadowMaps::MaxViewDepth;
    HLSL::DirectionalLightVolumeParams->TexSize = Vec4f(
        (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
        (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight(),
        1.f / (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
        1.f / (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight()
    );
    RenderConfig::DirectionalLightVolume::LightColor[0] = Math::clamp(RenderConfig::DirectionalLightVolume::LightColor[0], 0.f, 1.f);
    RenderConfig::DirectionalLightVolume::LightColor[1] = Math::clamp(RenderConfig::DirectionalLightVolume::LightColor[1], 0.f, 1.f);
    RenderConfig::DirectionalLightVolume::LightColor[2] = Math::clamp(RenderConfig::DirectionalLightVolume::LightColor[2], 0.f, 1.f);
    RenderConfig::DirectionalLightVolume::LightColor[3] = 1.f;
    BayerMatrix.GetTexture()->SetAddressingMode(SAM_WRAP);
    BayerMatrix.GetTexture()->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    HLSL::PostProcessing_DitherMap = BayerMatrix.GetTextureIndex();
    NoiseTexture.GetTexture()->SetAddressingMode(SAM_WRAP);
    NoiseTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    HLSL::DirectionalLightVolume_Noise = NoiseTexture;
    HLSL::DirectionalLightVolumeParams->ElapsedTime += fDeltaTime;
    HLSL::DirectionalLightVolumeParams->FogBox = Vec3f(RenderConfig::CascadedShadowMaps::MaxViewDepth, RenderConfig::CascadedShadowMaps::MaxViewDepth, RenderConfig::CascadedShadowMaps::MaxViewDepth);

    HLSL::DirectionalLightVolume_ShadowMap = ShadowMapDir.GetRenderTarget()->GetDepthBuffer();
    HLSL::DirectionalLightVolume_DepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();

    HLSL::DirectionalLightVolumeParams->SampleCount = RenderConfig::DirectionalLightVolume::SampleCount;
    HLSL::DirectionalLightVolumeParams->SampleDistrib = RenderConfig::DirectionalLightVolume::SampleDistrib;
    HLSL::DirectionalLightVolumeParams->LightIntensity = RenderConfig::DirectionalLightVolume::LightIntensity;
    HLSL::DirectionalLightVolumeParams->MultScatterIntensity = RenderConfig::DirectionalLightVolume::MultScatterIntensity;
    HLSL::DirectionalLightVolumeParams->FogVerticalFalloff = RenderConfig::DirectionalLightVolume::FogVerticalFalloff;
    HLSL::DirectionalLightVolumeParams->FogSpeed = RenderConfig::DirectionalLightVolume::FogSpeed;
    
    HLSL::NearestDepthUpscaleParams->UpsampleDepthThreshold = RenderConfig::DirectionalLightVolume::UpsampleDepthThreshold;
    HLSL::NearestDepthUpscaleParams->SingleChannelCopy = true;
    HLSL::NearestDepthUpscaleParams->CustomColorModulator = RenderConfig::DirectionalLightVolume::LightColor;

    HLSL::NearestDepthUpscaleParams->DstTexSize = Vec2f(
        (float)LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
        (float)LightAccumulationBuffer.GetRenderTarget()->GetHeight(),
        1.f / (float)LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
        1.f / (float)LightAccumulationBuffer.GetRenderTarget()->GetHeight()
    );
    HLSL::NearestDepthUpscale_Source = VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0);

    HLSL::NearestDepthUpscaleParams->SrcTexSize = Vec4f(
        (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
        (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight(),
        1.f / (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
        1.f / (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight()
    );
    HLSL::NearestDepthUpscale_DepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
    HLSL::NearestDepthUpscale_QuarterDepthBuffer = HyperbolicQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer();

    HLSL::ColorCopy_SourceTexture = VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0);

    HLSL::ColorCopyParams->SingleChannelCopy = true;
    HLSL::ColorCopyParams->CustomColorModulator = RenderConfig::DirectionalLightVolume::LightColor;
    HLSL::ColorCopyParams->ApplyTonemap = false;
}

void DirectionalLightVolumePass::CalculateLightVolume()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    PUSH_PROFILE_MARKER("Raymarch (scatter)");

    VolumetricLightAccumulationBuffer[0]->Enable();

    const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

    DirectionalLightVolumeShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    DirectionalLightVolumeShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);

    VolumetricLightAccumulationBuffer[0]->Disable();

    POP_PROFILE_MARKER();
}

void DirectionalLightVolumePass::GatherSamples()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Bilateral blur (gather)");

    const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

    if (!RenderConfig::DirectionalLightVolume::DepthAwareBlur)
        HLSL::BilateralBlurParams->BlurDepthFalloff = 0.f;
    else
        HLSL::BilateralBlurParams->BlurDepthFalloff = RenderConfig::DirectionalLightVolume::BlurDepthFalloff;

    HLSL::BilateralBlur_DepthBuffer = LinearQuarterDepthBuffer.GetRenderTarget()->GetColorBuffer();

    PUSH_PROFILE_MARKER("Horizontal");

    VolumetricLightAccumulationBuffer[1]->Enable();

    HLSL::BilateralBlurParams->TexSize = Vec4f(
        (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
        (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight(),
        1.f / (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
        1.f / (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight()
        );

    ResourceMgr->GetTexture(
        VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(
        VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);

    HLSL::BilateralBlur_Source = VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0);
    HLSL::BilateralBlurParams->BlurDir = Vec2f(1.f, 0.f);

    BilateralBlurShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    BilateralBlurShader.Disable();

    VolumetricLightAccumulationBuffer[1]->Disable();

    POP_PROFILE_MARKER();

    PUSH_PROFILE_MARKER("Vertical");

    VolumetricLightAccumulationBuffer[0]->Enable();

    HLSL::BilateralBlurParams->TexSize = Vec4f(
        (float)VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetWidth(),
        (float)VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetHeight(),
        1.f / (float)VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetWidth(),
        1.f / (float)VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetHeight()
        );

    ResourceMgr->GetTexture(
        VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(
        VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);

    HLSL::BilateralBlur_Source = VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetColorBuffer(0);
    HLSL::BilateralBlurParams->BlurDir = Vec2f(0.f, 1.f);

    BilateralBlurShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    BilateralBlurShader.Disable();

    VolumetricLightAccumulationBuffer[0]->Disable();

    POP_PROFILE_MARKER();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);

    POP_PROFILE_MARKER();
}

void DirectionalLightVolumePass::ApplyLightVolume()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    PUSH_PROFILE_MARKER("Apply (bilateral upscale)");

    //LightAccumulationBuffer.Enable();

    ResourceMgr->GetTexture(
        VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
    ResourceMgr->GetTexture(
        VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);

    if (RenderConfig::DirectionalLightVolume::QuarterResolution && RenderConfig::DirectionalLightVolume::DepthAwareUpscale)
    {
        NearestDepthUpscaleShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        NearestDepthUpscaleShader.Disable();
    }
    else
    {
        ColorCopyShader.Enable();
        RenderContext->DrawVertexBuffer(FullScreenTri);
        ColorCopyShader.Disable();
    }

    //LightAccumulationBuffer.Disable();

    POP_PROFILE_MARKER();
}

void DirectionalLightVolumePass::Draw()
{
    if (RenderConfig::DirectionalLightVolume::Enabled)
    {
        //Synesthesia3D::RenderTarget* pCurrRT = Synesthesia3D::RenderTarget::GetActiveRenderTarget();
        //if (pCurrRT)
        //  pCurrRT->Disable();
        
        CalculateLightVolume();

        if (RenderConfig::DirectionalLightVolume::BlurSamples)
            GatherSamples();

        ApplyLightVolume();

        //pCurrRT->Enable();
    }
}

void DirectionalLightVolumePass::AllocateResources()
{

}

void DirectionalLightVolumePass::ReleaseResources()
{

}
