/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   DirectionalLightPass.cpp
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
#include <Texture.h>
#include <RenderTarget.h>
using namespace Synesthesia3D;

#include "Poisson.h"

#include "DirectionalLightPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

DirectionalLightPass::DirectionalLightPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{
}

DirectionalLightPass::~DirectionalLightPass()
{
}

void DirectionalLightPass::AllocateResources()
{
    // Generate Poisson-disk sampling pattern
    std::vector<sPoint> poisson;
    float minDist = sqrt((float)HLSL::Utils::PoissonDiskSampleCount) / (float)HLSL::Utils::PoissonDiskSampleCount * 0.8f;
    float oneOverMinDist = 1.f / minDist;
    const float sqrt2 = sqrt(2.f);
    do
    {
        poisson =
            GeneratePoissonPoints(
                minDist,
                30,
                HLSL::Utils::PoissonDiskSampleCount
            );
    } while (poisson.size() != HLSL::Utils::PoissonDiskSampleCount);

    // Normalize the kernel
    for (unsigned int i = 0; i < HLSL::Utils::PoissonDiskSampleCount; i++)
    {
        HLSL::UtilsParams->PoissonDisk[i][0] = poisson[i].x * oneOverMinDist * sqrt2;
        HLSL::UtilsParams->PoissonDisk[i][1] = poisson[i].y * oneOverMinDist * sqrt2;
    }
}

void DirectionalLightPass::ReleaseResources()
{
}

void DirectionalLightPass::Update(const float fDeltaTime)
{
    HLSL::DirectionalLightParams->DebugCascades = RenderConfig::CascadedShadowMaps::DebugCascades;
    HLSL::DirectionalLightParams->HalfTexelOffset = Vec2f(0.5f / GBuffer.GetRenderTarget()->GetWidth(), 0.5f / GBuffer.GetRenderTarget()->GetHeight());
    HLSL::DirectionalLight_DiffuseBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(0);
    HLSL::DirectionalLight_NormalBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(1);
    HLSL::DirectionalLight_DepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
    HLSL::DirectionalLight_MaterialBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(2);
    HLSL::DirectionalLight_ShadowMap = ShadowMapDir.GetRenderTarget()->GetDepthBuffer();
    HLSL::DirectionalLightParams->OneOverShadowMapSize = Vec2f(1.f / (float)RenderConfig::CascadedShadowMaps::ShadowMapSize[0], 1.f / (float)RenderConfig::CascadedShadowMaps::ShadowMapSize[1]);

    IrradianceTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
    IrradianceTexture.GetTexture()->SetSRGBEnabled(true);
    EnvironmentTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
    EnvironmentTexture.GetTexture()->SetSRGBEnabled(true);

    HLSL::BRDF_IrradianceMap = IrradianceTexture;
    HLSL::BRDF_EnvMap = EnvironmentTexture;

    HLSL::BRDFParams->DiffuseFactor = RenderConfig::DirectionalLight::DiffuseFactor;
    HLSL::BRDFParams->SpecFactor = RenderConfig::DirectionalLight::SpecFactor;
    HLSL::BRDFParams->AmbientFactor = RenderConfig::DirectionalLight::AmbientFactor;
    HLSL::BRDFParams->IrradianceFactor = RenderConfig::DirectionalLight::IrradianceFactor;

    // Disable environment map reflections if
    // screen space reflections are active.
    if (RenderConfig::PostProcessing::ScreenSpaceReflections::Enabled)
        HLSL::BRDFParams->ReflectionFactor = 0.f;
    else
        HLSL::BRDFParams->ReflectionFactor = RenderConfig::DirectionalLight::ReflectionFactor;
}

void DirectionalLightPass::Draw()
{
    if (!RenderConfig::DirectionalLight::Enabled)
        return;

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    DirectionalLightShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    DirectionalLightShader.Disable();
}