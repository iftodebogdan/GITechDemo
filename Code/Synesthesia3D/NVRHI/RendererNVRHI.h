/**
 * @file        RendererNVRHI.h
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

#ifndef RENDERERNVRHI_H
#define RENDERERNVRHI_H

#include "ResourceData.h"

#if ENABLE_NVRHI

#include <nvrhi/nvrhi.h>

#include "Renderer.h"

namespace Synesthesia3D
{
    class RendererNVRHI : public Renderer
    {
    protected:
        RendererNVRHI();
        virtual ~RendererNVRHI();

    public:
        virtual void Initialize(void* hWnd) override;
        const bool SetDisplayResolution(
            const Vec2i size,
            const Vec2i offset = Vec2i(0, 0),
            const bool fullscreen = false,
            const unsigned int refreshRate = 0,
            const bool vsync = true) override;
        const Vec2i GetDisplayResolution() const override;
        const bool GetVSyncStatus() const override;
        const unsigned int GetDisplayRefreshRate() const override;
        const PixelFormat GetBackBufferFormat() const override;
        const bool IsFullscreen() const override;
        void SetViewport(const Vec2i size, const Vec2i offset = Vec2i(0, 0)) override;
        void CreatePerspectiveMatrix(Matrix44f& matProj, const float fovYRad, const float aspectRatio, const float zNear, const float zFar) const override;
        void CreateInfinitePerspectiveMatrix(Matrix44f& matProj, const float fovYRad, const float aspectRatio, const float zNear) const override;
        void CreateOrthographicMatrix(Matrix44f& matProj, const float left, const float top, const float right, const float bottom, const float zNear, const float zFar) const override;
        void Clear(const Vec4f rgba, const float z, const unsigned int stencil) override;

    protected:
        nvrhi::DeviceHandle m_NvrhiDevice;
    };
}

#else // ENABLE_NVRHI

#include "RendererNULL.h"

namespace Synesthesia3D
{
    typedef RendererNULL RendererNVRHI;
}

#endif // ENABLE_NVRHI

#endif // RENDERERNVRHI_H
