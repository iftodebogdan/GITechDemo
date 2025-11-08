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

#if ENABLE_NVRHI

//#define LOAD_RENDERDOC_DLL (_PROFILE || _DEBUG)

#if LOAD_RENDERDOC_DLL
#include "renderdoc_app.h"
#endif

#include "ResourceManagerNVRHI.h"
#include "RenderStateNVRHI.h"
#include "SamplerStateNVRHI.h"
#include "ProfilerNVRHI.h"

#include "MappingsNVRHI.h"

#if _DEBUG
#pragma comment(lib, "NVRHI_x64_Debug.lib")
#elif _PROFILE
#pragma comment(lib, "NVRHI_x64_Profile.lib")
#elif NDEBUG
#pragma comment(lib, "NVRHI_x64_Release.lib")
#endif

RendererNVRHI::RendererNVRHI()
{
    InitializeRenderDoc();
}

RendererNVRHI::~RendererNVRHI()
{

}

RendererNVRHI* const RendererNVRHI::GetInstance()
{
    assert(!ms_pInstance || ms_pInstance->GetAPI() == API_NVRHI_D3D11 || ms_pInstance->GetAPI() == API_NVRHI_D3D12 || ms_pInstance->GetAPI() == API_NVRHI_VULKAN);
    return static_cast<RendererNVRHI*>(Renderer::GetInstance());
}

void RendererNVRHI::Initialize(void* hWnd)
{
    CheckDeviceCaps();

    m_pResourceManager = new ResourceManagerNVRHI();
    m_pRenderStateManager = new RenderStateNVRHI();
    m_pSamplerStateManager = new SamplerStateNVRHI();

    m_pProfiler = new ProfilerNVRHI();

    m_pImmediateGraphicsCommandList = m_pDevice->createCommandList();

    SetDeviceState(DS_READY);

    // TODO: find alternative to this
    if (!BeginFrame())
        assert(false);
}

const bool RendererNVRHI::SetDisplayResolution(const Vec2i size, const Vec2i offset, const bool fullscreen, const unsigned int refreshRate, const bool vsync)
{
    return false;
}

const Vec2i RendererNVRHI::GetDisplayResolution() const
{
    Vec2i swapchainSize;

    if (m_RhiSwapChainBuffers.size() > 0 && m_RhiSwapChainBuffers[0].Get())
    {
        swapchainSize[0] = m_RhiSwapChainBuffers[0]->getDesc().width;
        swapchainSize[1] = m_RhiSwapChainBuffers[0]->getDesc().height;
    }

    return swapchainSize;
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

const bool RendererNVRHI::BeginFrame()
{
    if (GetDeviceState() == DS_RENDERING)
        return true;

    assert(GetDeviceState() == DS_READY);
    m_pImmediateGraphicsCommandList->open();

    return Renderer::BeginFrame();
}

void RendererNVRHI::EndFrame()
{
    assert(GetDeviceState() == DS_RENDERING);
    if (GetDeviceState() != DS_RENDERING)
        return;

    m_pImmediateGraphicsCommandList->close();
    m_pDevice->executeCommandList(m_pImmediateGraphicsCommandList);

    Renderer::EndFrame();
}

void RendererNVRHI::Clear(const Vec4f rgba, const float z, const unsigned int stencil)
{

}

void RendererNVRHI::CheckDeviceCaps()
{

}

void Synesthesia3D::RendererNVRHI::InitializeRenderDoc()
{
#ifdef LOAD_RENDERDOC_DLL
    RENDERDOC_API_1_1_2* rdoc_api = NULL;

#if _DEBUG
    VLDDisable();
#endif

    HINSTANCE hLib = LoadLibraryA("renderdoc.dll");

#if _DEBUG
    VLDEnable();
#endif

    if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI =
            (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
        int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
        assert(ret == 1);
    }
#endif
}

#endif // ENABLE_NVRHI
