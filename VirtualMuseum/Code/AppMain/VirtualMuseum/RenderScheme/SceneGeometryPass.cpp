/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   SceneGeometryPass.cpp
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
#include <Profiler.h>
using namespace Synesthesia3D;

#include "SceneGeometryPass.h"
#include "Scene.h"
#include "VirtualMuseum.h"
using namespace VirtualMuseumApp;

#include "AppResources.h"

SceneGeometryPass::SceneGeometryPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

SceneGeometryPass::~SceneGeometryPass()
{}

void SceneGeometryPass::Update(const float fDeltaTime)
{}

void SceneGeometryPass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    ResourceManager* const ResMgr = RenderContext ? RenderContext->GetResourceManager() : nullptr;
    if (!RenderContext || !ResMgr)
        return;

    const bool zWriteEnabled = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
    const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
    bool red, blue, green, alpha;
    RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);

    // A depth prepass is useful if we have expensive pixel shaders in our
    // G-Buffer generation process. It allows us to avoid shading pixels
    // that eventually get overwritten by other pixels that have smaller
    // depth values. We are not fill-rate bound, so the depth prepass is disabled.
    if (RenderConfig::GBuffer::ZPrepass)
    {
        PUSH_PROFILE_MARKER("Z prepass");

        RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);

        ((VirtualMuseum*)AppMain)->GetScene()->Draw(Scene::Actor::DEPTH_ONLY_ALPHA_TEST);

        RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
        RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
        RenderContext->GetRenderStateManager()->SetZFunc(CMP_EQUAL);

        POP_PROFILE_MARKER();

        PUSH_PROFILE_MARKER("Capture");
    }

    // A visibility test would be useful if we were CPU bound (or vertex bound).
    // However, there isn't a reason to do such an optimization for now, since the
    // scene isn't very big and we are mostly pixel bound.
    ((VirtualMuseum*)AppMain)->GetScene()->Draw(Scene::Actor::COLOR);

    if (RenderConfig::GBuffer::ZPrepass)
    {
        RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWriteEnabled);
        RenderContext->GetRenderStateManager()->SetZFunc(zFunc);
        POP_PROFILE_MARKER();
    }
}

void SceneGeometryPass::AllocateResources()
{

}

void SceneGeometryPass::ReleaseResources()
{

}
