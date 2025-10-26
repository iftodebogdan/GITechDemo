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

        virtual void CheckDeviceCaps();

        void InitializeRenderDoc();

    public:
        static RendererNVRHI* const GetInstance();

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

        const bool BeginFrame() override;
        void EndFrame() override;

        void Clear(const Vec4f rgba, const float z, const unsigned int stencil) override;

        nvrhi::DeviceHandle GetDevice() const { return m_pDevice; }
        nvrhi::CommandListHandle GetCommandList() const { return m_pImmediateGraphicsCommandList; }

    protected:
        nvrhi::DeviceHandle m_pDevice;
        nvrhi::CommandListHandle m_pImmediateGraphicsCommandList;

        std::vector<nvrhi::TextureHandle> m_RhiSwapChainBuffers;
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
