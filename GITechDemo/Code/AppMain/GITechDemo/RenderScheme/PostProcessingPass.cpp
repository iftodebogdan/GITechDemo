/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   PostProcessingPass.cpp
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

#include "Renderer.h"
#include "ResourceManager.h"
#include "IndexBuffer.h"
#include "VertexFormat.h"
#include "VertexBuffer.h"
using namespace Synesthesia3D;

#include "AppResources.h"
#include "PostProcessingPass.h"
using namespace GITechDemoApp;

PostProcessingPass::PostProcessingPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{

}

PostProcessingPass::~PostProcessingPass()
{

}

void PostProcessingPass::Update(const float fDeltaTime)
{

}

void PostProcessingPass::Draw()
{
    if (POST_PROCESSING_ENABLED)
        DrawChildren();
}

void PostProcessingPass::AllocateResources()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    // Create a full screen quad (it's actually an over-sized triangle) for fullscreen effects and processing
    unsigned int vfIdx = ResourceMgr->CreateVertexFormat(1, VAS_POSITION, VAT_FLOAT4, 0);
    VertexFormat* vf = ResourceMgr->GetVertexFormat(vfIdx);

    unsigned int ibIdx = ResourceMgr->CreateIndexBuffer(3);
    IndexBuffer* ib = ResourceMgr->GetIndexBuffer(ibIdx);
    const unsigned short fsqIndices[] = { 0, 1, 2 };
    ib->SetIndices(fsqIndices, 3);

    unsigned int vbIdx = ResourceMgr->CreateVertexBuffer(vf, 3, ib);
    FullScreenTri = ResourceMgr->GetVertexBuffer(vbIdx);

    FullScreenTri->Lock(BL_WRITE_ONLY);
    FullScreenTri->Position<Vec4f>(0) = Vec4f(-1.f, 1.f, 1.f, 1.f);
    FullScreenTri->Position<Vec4f>(1) = Vec4f(3.f, 1.f, 1.f, 1.f);
    FullScreenTri->Position<Vec4f>(2) = Vec4f(-1.f, -3.f, 1.f, 1.f);
    FullScreenTri->Update();
    FullScreenTri->Unlock();
}

void PostProcessingPass::ReleaseResources()
{
    FullScreenTri = nullptr;
}
