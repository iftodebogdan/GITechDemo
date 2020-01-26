/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   ResolveDepthBufferPass.cpp
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
#include <RenderTarget.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "ResolveDepthBufferPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

ResolveDepthBufferPass::ResolveDepthBufferPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

ResolveDepthBufferPass::~ResolveDepthBufferPass()
{}

void ResolveDepthBufferPass::Update(float fDeltaTime)
{
    HLSL::DepthCopy_Source = GBuffer.GetRenderTarget()->GetDepthBuffer();
    HLSL::DepthCopyParams->HalfTexelOffset = Vec2f(
        0.5f / (float)GBuffer.GetRenderTarget()->GetWidth(),
        0.5f / (float)GBuffer.GetRenderTarget()->GetHeight()
    );
}

void ResolveDepthBufferPass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    PUSH_PROFILE_MARKER("Copy-resolve Depth Buffer");

    bool red, blue, green, alpha;
    const bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
    const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
    RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);

    RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(true);
    RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

    // Copy-resolve the INTZ depth buffer of the G-Buffer to the D24S8 depth buffer of
    // the light accumulation buffer for depth testing (e.g. when rendering the sky).
    DepthCopyShader.Enable();
    RenderContext->DrawVertexBuffer(FullScreenTri);
    DepthCopyShader.Disable();

    RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
    RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

    POP_PROFILE_MARKER();
}

void ResolveDepthBufferPass::AllocateResources()
{

}

void ResolveDepthBufferPass::ReleaseResources()
{

}
