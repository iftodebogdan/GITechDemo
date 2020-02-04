/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   HDRDownsampleForBloomPass.cpp
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
using namespace Synesthesia3D;

#include "HDRDownsampleForBloomPass.h"
using namespace VirtualMuseumApp;

#include "AppResources.h"

HDRDownsampleForBloomPass::HDRDownsampleForBloomPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

HDRDownsampleForBloomPass::~HDRDownsampleForBloomPass()
{}

void HDRDownsampleForBloomPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    ResourceMgr->GetTexture(HDRDownsampleForBloomBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

    HLSL::DownsampleParams->DownsampleFactor = 16;
    HLSL::DownsampleParams->ApplyBrightnessFilter = false;
    HLSL::DownsampleParams->DepthDownsample = false;
}

void HDRDownsampleForBloomPass::DownsampleForBloomPass(VirtualMuseumApp::RenderTarget* const pSource, VirtualMuseumApp::RenderTarget* const pDest)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    RenderState* RenderStateManager = RenderContext->GetRenderStateManager();
    const bool blendEnabled = RenderStateManager->GetColorBlendEnabled();
    RenderStateManager->SetColorBlendEnabled(false);

    pDest->Enable();

    // Not necessary
    //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    HLSL::DownsampleParams->HalfTexelOffset = Vec2f(
        0.5f / pDest->GetRenderTarget()->GetWidth(),
        0.5f / pDest->GetRenderTarget()->GetHeight()
        );
    HLSL::DownsampleParams->TexSize = Vec4f(
        (float)pSource->GetRenderTarget()->GetWidth(),
        (float)pSource->GetRenderTarget()->GetHeight(),
        1.f / (float)pSource->GetRenderTarget()->GetWidth(),
        1.f / (float)pSource->GetRenderTarget()->GetHeight()
        );
    HLSL::Downsample_Source = pSource->GetRenderTarget()->GetColorBuffer(0);

    DownsampleShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    DownsampleShader.Disable();

    pDest->Disable();

    RenderStateManager->SetColorBlendEnabled(blendEnabled);
}

void HDRDownsampleForBloomPass::Draw()
{
    if (!RenderConfig::PostProcessing::Bloom::Enabled)
        return;

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    DownsampleForBloomPass(&LightAccumulationBuffer, &HDRDownsampleForBloomBuffer);
}

void HDRDownsampleForBloomPass::AllocateResources()
{

}

void HDRDownsampleForBloomPass::ReleaseResources()
{

}
