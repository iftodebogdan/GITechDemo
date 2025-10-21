/**
 * @file        RendererNVRHID3D12.h
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

#ifndef RENDERERNVRHID3D12_H
#define RENDERERNVRHID3D12_H

#include "ResourceData.h"

#if ENABLE_NVRHI_D3D12

#include <dxgi1_5.h>

#include <nvrhi/d3d12.h>

#include "RendererNVRHI.h"

namespace Synesthesia3D
{
    class RendererNVRHID3D12 : public RendererNVRHI
    {
    protected:
        RendererNVRHID3D12();
        virtual ~RendererNVRHID3D12();

    public:
        void Initialize(void* hWnd) override;

    private:
        void DestroyDevice();

        nvrhi::RefCountPtr<IDXGIFactory2> m_DxgiFactory2;
        nvrhi::RefCountPtr<IDXGIAdapter> m_DxgiAdapter;
        nvrhi::RefCountPtr<ID3D12Device> m_Device12;
        nvrhi::RefCountPtr<ID3D12CommandQueue> m_GraphicsQueue;
        nvrhi::RefCountPtr<ID3D12CommandQueue> m_ComputeQueue;
        nvrhi::RefCountPtr<ID3D12CommandQueue> m_CopyQueue;

        friend class Renderer;
    };
}

#else // ENABLE_NVRHI_D3D12

#include "RendererNULL.h"

namespace Synesthesia3D
{
    typedef RendererNULL RendererNVRHID3D12;
}

#endif // ENABLE_NVRHI_D3D11

#endif // RENDERERNVRHID3D11_H
