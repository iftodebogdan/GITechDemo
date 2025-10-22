/**
 * @file        RendererNVRHI.cpp
 *
 * @note        This file is part of the "Synesthesia3D" graphics engine
 *
 * @copyright   Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * @copyright
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * @copyright
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"

#include "RendererNVRHI.h"
using namespace Synesthesia3D;

#include "ResourceManagerNVRHI.h"
#include "MappingsNVRHI.h"

#pragma comment(lib, "NVRHI.lib")

RendererNVRHI::RendererNVRHI()
{

}

RendererNVRHI::~RendererNVRHI()
{

}

void RendererNVRHI::Initialize(void* hWnd)
{
    CheckDeviceCaps();

    m_pResourceManager = new ResourceManagerNVRHI();
    //m_pRenderStateManager = new RenderStateNVRHI();
    //m_pSamplerStateManager = new SamplerStateNVRHI();
    //
    //m_pProfiler = new ProfilerNVRHI();
}

const bool RendererNVRHI::SetDisplayResolution(const Vec2i size, const Vec2i offset, const bool fullscreen, const unsigned int refreshRate, const bool vsync)
{
    return false;
}

const Vec2i RendererNVRHI::GetDisplayResolution() const
{
    return Vec2i();
}

const bool RendererNVRHI::GetVSyncStatus() const
{
    return false;
}

const unsigned int RendererNVRHI::GetDisplayRefreshRate() const
{
    return 0;
}

const PixelFormat RendererNVRHI::GetBackBufferFormat() const
{
    return PixelFormat();
}

const bool RendererNVRHI::IsFullscreen() const
{
    return false;
}

void RendererNVRHI::SetViewport(const Vec2i size, const Vec2i offset)
{
}

void RendererNVRHI::CreatePerspectiveMatrix(Matrix44f& matProj, const float fovYRad, const float aspectRatio, const float zNear, const float zFar) const
{
}

void RendererNVRHI::CreateInfinitePerspectiveMatrix(Matrix44f& matProj, const float fovYRad, const float aspectRatio, const float zNear) const
{
}

void RendererNVRHI::CreateOrthographicMatrix(Matrix44f& matProj, const float left, const float top, const float right, const float bottom, const float zNear, const float zFar) const
{
}

void RendererNVRHI::Clear(const Vec4f rgba, const float z, const unsigned int stencil)
{
}

void RendererNVRHI::CheckDeviceCaps()
{

}
