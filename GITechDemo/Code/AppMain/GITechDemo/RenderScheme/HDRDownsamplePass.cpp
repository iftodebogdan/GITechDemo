/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   HDRDownsamplePass.cpp
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

#include "HDRDownsamplePass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

HDRDownsamplePass::HDRDownsamplePass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

HDRDownsamplePass::~HDRDownsamplePass()
{}

void HDRDownsamplePass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    ResourceMgr->GetTexture(HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

    HLSL::DownsampleParams->DownsampleFactor = 4;
    HLSL::DownsampleParams->ApplyBrightnessFilter = false;
    HLSL::DownsampleParams->DepthDownsample = false;
}

void HDRDownsamplePass::DownsamplePass(GITechDemoApp::RenderTarget* const pSource, GITechDemoApp::RenderTarget* const pDest)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    pDest->Enable();

    // Not necessary
    //RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

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
}

void HDRDownsamplePass::Draw()
{
    if (!RenderConfig::PostProcessing::ToneMapping::Enabled)
        return;

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    DownsamplePass(&LightAccumulationBuffer, HDRDownsampleBuffer[QUARTER]);
    DownsamplePass(HDRDownsampleBuffer[QUARTER], HDRDownsampleBuffer[SIXTEENTH]);
}

void HDRDownsamplePass::AllocateResources()
{

}

void HDRDownsamplePass::ReleaseResources()
{

}
