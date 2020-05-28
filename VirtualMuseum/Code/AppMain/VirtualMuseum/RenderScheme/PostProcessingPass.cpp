/*=============================================================================
 * This file is part of the "VirtualMuseum" application
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
using namespace VirtualMuseumApp;

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
    if (RenderConfig::PostProcessing::Enabled)
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
    {
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

    {
        // Create a quad
        unsigned int vfIdx = ResourceMgr->CreateVertexFormat(
            6,
            VAS_POSITION,   VAT_FLOAT4, 0,
            VAS_TEXCOORD,   VAT_FLOAT2, 0,
            VAS_NORMAL,     VAT_FLOAT3, 0,
            VAS_TANGENT,    VAT_FLOAT3, 0,
            VAS_BINORMAL,   VAT_FLOAT3, 0,
            VAS_COLOR,      VAT_FLOAT4, 0
        );
        VertexFormat* vf = ResourceMgr->GetVertexFormat(vfIdx);

        unsigned int ibIdx = ResourceMgr->CreateIndexBuffer(6);
        IndexBuffer* ib = ResourceMgr->GetIndexBuffer(ibIdx);
        const unsigned short fsqIndices[] = { 0, 1, 2, 2, 1, 3 };
        ib->SetIndices(fsqIndices, 6);

        unsigned int vbIdx = ResourceMgr->CreateVertexBuffer(vf, 4, ib);
        SimpleQuad = ResourceMgr->GetVertexBuffer(vbIdx);

        SimpleQuad->Lock(BL_WRITE_ONLY);

        SimpleQuad->Position<Vec4f>(0) = Vec4f(-1.f, 1.f, 0.f, 1.f);
        SimpleQuad->Position<Vec4f>(1) = Vec4f(1.f, 1.f, 0.f, 1.f);
        SimpleQuad->Position<Vec4f>(2) = Vec4f(-1.f, -1.f, 0.f, 1.f);
        SimpleQuad->Position<Vec4f>(3) = Vec4f(1.f, -1.f, 0.f, 1.f);

        SimpleQuad->TexCoord<Vec2f>(0, 0) = Vec2f(0.f, 0.f);
        SimpleQuad->TexCoord<Vec2f>(1, 0) = Vec2f(1.f, 0.f);
        SimpleQuad->TexCoord<Vec2f>(2, 0) = Vec2f(0.f, 1.f);
        SimpleQuad->TexCoord<Vec2f>(3, 0) = Vec2f(1.f, 1.f);

        SimpleQuad->Normal<Vec3f>(0) = Vec3f(0.f, 0.f, -1.f);
        SimpleQuad->Normal<Vec3f>(1) = Vec3f(0.f, 0.f, -1.f);
        SimpleQuad->Normal<Vec3f>(2) = Vec3f(0.f, 0.f, -1.f);
        SimpleQuad->Normal<Vec3f>(3) = Vec3f(0.f, 0.f, -1.f);

        SimpleQuad->Tangent<Vec3f>(0) = Vec3f(-1.f, 0.f, 0.f);
        SimpleQuad->Tangent<Vec3f>(1) = Vec3f(-1.f, 0.f, 0.f);
        SimpleQuad->Tangent<Vec3f>(2) = Vec3f(-1.f, 0.f, 0.f);
        SimpleQuad->Tangent<Vec3f>(3) = Vec3f(-1.f, 0.f, 0.f);

        SimpleQuad->Binormal<Vec3f>(0) = Vec3f(0.f, 1.f, 0.f);
        SimpleQuad->Binormal<Vec3f>(1) = Vec3f(0.f, 1.f, 0.f);
        SimpleQuad->Binormal<Vec3f>(2) = Vec3f(0.f, 1.f, 0.f);
        SimpleQuad->Binormal<Vec3f>(3) = Vec3f(0.f, 1.f, 0.f);

        SimpleQuad->Color<Vec4f>(0, 0) = Vec4f(1.f, 1.f, 1.f, 1.f);
        SimpleQuad->Color<Vec4f>(1, 0) = Vec4f(1.f, 1.f, 1.f, 1.f);
        SimpleQuad->Color<Vec4f>(2, 0) = Vec4f(1.f, 1.f, 1.f, 1.f);
        SimpleQuad->Color<Vec4f>(3, 0) = Vec4f(1.f, 1.f, 1.f, 1.f);

        SimpleQuad->Update();
        SimpleQuad->Unlock();
    }
}

void PostProcessingPass::ReleaseResources()
{
    FullScreenTri = nullptr;
}
