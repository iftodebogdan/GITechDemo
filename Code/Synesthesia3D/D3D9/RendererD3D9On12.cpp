/**
 * @file        RendererD3D9On12.cpp
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

#include "RendererD3D9On12.h"
using namespace Synesthesia3D;

#if ENABLE_D3D9_ON_12

#include <dxgi1_4.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

RendererD3D9On12::RendererD3D9On12()
    : m_pfnDirect3DCreate9On12(nullptr)
{
    if (!LoadDLLFunctions())
    {
        S3D_DBGPRINT("Failed to load d3d9.dll and/or its functions for D3D9On12.\n");
        assert(false);
    }
}

RendererD3D9On12::~RendererD3D9On12()
{

}

IDXGIAdapter1* RendererD3D9On12::CreateD3D12Adapter()
{
    IDXGIFactory4* dxgiFactory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));

    if (FAILED(hr))
        return nullptr;

    IDXGIAdapter1* adapter;
    int adapterIndex = 0;

    while (dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            adapterIndex++;
            continue;
        }

        hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
        if (SUCCEEDED(hr))
        {
            S3D_DBGPRINT("Adapter description: %s\n", desc.Description);
            S3D_DBGPRINT("Adapter LUID: %llx\n", desc.AdapterLuid);

            return adapter;
        }

        adapterIndex++;
    }

    return NULL;
}

IDirect3D9* RendererD3D9On12::CreateDriver()
{
    IDXGIAdapter1* adapter;

    if ((adapter = CreateD3D12Adapter()) != NULL)
    {
        IDirect3D9* d3d = nullptr;
        ID3D12Device* d3d12Device = nullptr;

        D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device));

        LUID adapter_LUID = d3d12Device->GetAdapterLuid();

        D3D9ON12_ARGS dArgs;
        ZeroMemory(&dArgs, sizeof(D3D9ON12_ARGS));
        dArgs.Enable9On12 = TRUE;
        dArgs.pD3D12Device = d3d12Device;

        d3d = Direct3DCreate9On12(D3D_SDK_VERSION, &dArgs, 1);

        S3D_DBGPRINT("D3D12 memory address: %08X\n", d3d12Device);
        S3D_DBGPRINT("D3D12 device LUID: %llx\n", adapter_LUID);
        S3D_DBGPRINT("D3D9 memory address: %08X\n", d3d);

        return d3d;
    }

    return nullptr;
}

void RendererD3D9On12::ValidateDevice()
{
    IDirect3DDevice9On12* d3dtest = nullptr;
    HRESULT hr = GetDevice()->QueryInterface(IID_PPV_ARGS(&d3dtest));

    S3D_DBGPRINT("Query interface returned %08X\n", hr);
    S3D_DBGPRINT("IDirect3DDevice9On12 interface memory address: %08X\n", d3dtest);

    assert(m_pd3dDevice && d3dtest);

    d3dtest->Release();
}

const bool RendererD3D9On12::LoadDLLFunctions()
{
    HMODULE hMod = GetModuleHandleA("d3d9.dll");

    if (!hMod)
        hMod = LoadLibraryA("d3d9.dll");

    if (!hMod)
        return false;

    m_pfnDirect3DCreate9On12 = reinterpret_cast<PFN_Direct3DCreate9On12>(GetProcAddress(hMod, "Direct3DCreate9On12"));

    if (!m_pfnDirect3DCreate9On12)
        return false;

    return true;
}

IDirect3D9* WINAPI RendererD3D9On12::Direct3DCreate9On12(UINT SDKVersion, D3D9ON12_ARGS* pOverrideList, UINT NumOverrideEntries)
{
    return m_pfnDirect3DCreate9On12 ? m_pfnDirect3DCreate9On12(SDKVersion, pOverrideList, NumOverrideEntries) : nullptr;
}

#endif //ENABLE_D3D9_ON_12
