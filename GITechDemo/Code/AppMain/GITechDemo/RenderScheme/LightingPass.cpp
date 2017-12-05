/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   LightingPass.cpp
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

#include "LightingPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

LightingPass::LightingPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

LightingPass::~LightingPass()
{}

void LightingPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    ResourceMgr->GetTexture(
        LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0)
        )->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
    ResourceMgr->GetTexture(
        LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0)
        )->SetAddressingMode(SAM_CLAMP);

    f2HalfTexelOffset = Vec2f(
        0.5f / GBuffer.GetRenderTarget()->GetWidth(),
        0.5f / GBuffer.GetRenderTarget()->GetHeight()
        );
}

void LightingPass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    LightAccumulationBuffer.Enable();

    RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

    const bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
    const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
    const bool blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
    const Blend DstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
    const Blend SrcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

    // Disable Z writes, since we already have the correct depth buffer
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
    RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

    // Additive color blending is required for accumulating light
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
    RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ONE);
    RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);

    DrawChildren();

    // Reset the render states
    RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
    RenderContext->GetRenderStateManager()->SetZFunc(zFunc);
    RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
    RenderContext->GetRenderStateManager()->SetColorDstBlend(DstBlend);
    RenderContext->GetRenderStateManager()->SetColorSrcBlend(SrcBlend);

    LightAccumulationBuffer.Disable();
}
