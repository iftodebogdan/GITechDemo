/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   DepthDownsamplePass.cpp
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
using namespace Synesthesia3D;

#include "DepthDownsamplePass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

DepthDownsamplePass::DepthDownsamplePass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

DepthDownsamplePass::~DepthDownsamplePass()
{}

void DepthDownsamplePass::Update(const float fDeltaTime)
{}

void DepthDownsamplePass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    // Generate linear depth buffer and quarter-resolution versions of the hyperbolic and linear depth buffers
    const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

    bReconstructDepth = false;
    nDownsampleFactor = 4;
    DownsampleShader.Enable();
    HyperbolicQuarterDepthBuffer.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    HyperbolicQuarterDepthBuffer.Disable();
    DownsampleShader.Disable();

    bReconstructDepth = true;
    nDownsampleFactor = 1;
    DownsampleShader.Enable();
    LinearFullDepthBuffer.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    LinearFullDepthBuffer.Disable();
    DownsampleShader.Disable();

    bReconstructDepth = true;
    nDownsampleFactor = 4;
    DownsampleShader.Enable();
    LinearQuarterDepthBuffer.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    LinearQuarterDepthBuffer.Disable();
    DownsampleShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
}

void DepthDownsamplePass::AllocateResources()
{

}

void DepthDownsamplePass::ReleaseResources()
{

}
