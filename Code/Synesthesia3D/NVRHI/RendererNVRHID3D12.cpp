/**
 * @file        RendererNVRHID3D12.cpp
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

#include <nvrhi/validation.h>

#if _DEBUG
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

#include "RendererNVRHID3D12.h"
using namespace Synesthesia3D;

#ifdef _DEBUG
    #include <DxErr.h>
    #define S3D_VALIDATE_HRESULT(hr) \
        { \
            if (FAILED(hr)) { \
                S3D_DBGPRINT("Error %s %s\n", DXGetErrorString(hr), DXGetErrorDescription(hr)); \
            } \
            assert(SUCCEEDED(hr)); \
        }
#else
    #define S3D_VALIDATE_HRESULT(hr) ((void)0)  
#endif

struct DefaultMessageCallback : public nvrhi::IMessageCallback
{
    static DefaultMessageCallback& GetInstance();

    void message(nvrhi::MessageSeverity severity, const char* messageText) override;
};

DefaultMessageCallback& DefaultMessageCallback::GetInstance()
{
    static DefaultMessageCallback Instance;
    return Instance;
}

void DefaultMessageCallback::message(nvrhi::MessageSeverity severity, const char* messageText)
{
    std::string severityString;

    switch (severity)
    {
    case nvrhi::MessageSeverity::Info:
        severityString = "Info";
        break;
    case nvrhi::MessageSeverity::Warning:
        severityString = "Warning";
        break;
    case nvrhi::MessageSeverity::Error:
        severityString = "Error";
        break;
    case nvrhi::MessageSeverity::Fatal:
        severityString = "Fatal";
        break;
    }

    S3D_DBGPRINT("%s: %s", severityString.c_str(), messageText);
}

RendererNVRHID3D12::RendererNVRHID3D12()
{
    if (!m_DxgiFactory2)
    {
        HRESULT hres = CreateDXGIFactory2(
#if _DEBUG
            DXGI_CREATE_FACTORY_DEBUG,
#else
            0,
#endif
            IID_PPV_ARGS(&m_DxgiFactory2)
        );

        if (hres != S_OK)
        {
            S3D_VALIDATE_HRESULT(hres);
            S3D_DBGPRINT("ERROR in CreateDXGIFactory2.\n"
                "For more info, get log from debug D3D runtime: (1) Install DX SDK, and enable Debug D3D from DX Control Panel Utility. (2) Install and start DbgView. (3) Try running the program again.\n");
            assert(false);
        }
    }
}

RendererNVRHID3D12::~RendererNVRHID3D12()
{
    DestroyDevice();

    m_DxgiAdapter = nullptr;
    m_DxgiFactory2 = nullptr;

#if _DEBUG
    nvrhi::RefCountPtr<IDXGIDebug> pDebug;
    DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug));

    if (pDebug)
    {
        DXGI_DEBUG_RLO_FLAGS flags = (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_IGNORE_INTERNAL | DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_DETAIL);
        HRESULT hr = pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, flags);
        if (FAILED(hr))
        {
            S3D_VALIDATE_HRESULT(hr);
            S3D_DBGPRINT("ReportLiveObjects failed, HRESULT = 0x%08x", hr);
        }
    }
#endif
}

void RendererNVRHID3D12::Initialize(void* hWnd)
{
#if _DEBUG
    {
        nvrhi::RefCountPtr<ID3D12Debug> pDebug;
        HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&pDebug));

        if (SUCCEEDED(hr))
            pDebug->EnableDebugLayer();
        else
        {
            S3D_VALIDATE_HRESULT(hr);
            S3D_DBGPRINT("Error: Cannot enable DX12 debug runtime, ID3D12Debug is not available.");
        }
    }

    {
        nvrhi::RefCountPtr<ID3D12Debug3> debugController3;
        HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController3));

        if (SUCCEEDED(hr))
            debugController3->SetEnableGPUBasedValidation(true);
        else
        {
            S3D_VALIDATE_HRESULT(hr);
            S3D_DBGPRINT("Error: Cannot enable GPU-based validation, ID3D12Debug3 is not available.");
        }
    }
#endif

    int adapterIndex = 0; // TODO: enumerate adapters and determine index?

    if (FAILED(m_DxgiFactory2->EnumAdapters(adapterIndex, &m_DxgiAdapter)))
    {
        if (adapterIndex == 0)
            S3D_DBGPRINT("Error: Cannot find any DXGI adapters in the system.");
        else
            S3D_DBGPRINT("Error: The specified DXGI adapter %d does not exist.", adapterIndex);

        assert(false);
    }

    /*
    {
        DXGI_ADAPTER_DESC aDesc;
        m_DxgiAdapter->GetDesc(&aDesc);

        m_RendererString = GetAdapterName(aDesc);
        m_IsNvidia = IsNvDeviceID(aDesc.VendorId);
    }
    */

    HRESULT hr = D3D12CreateDevice(
        m_DxgiAdapter,
        D3D_FEATURE_LEVEL_12_2,
        IID_PPV_ARGS(&m_Device12));

    if (FAILED(hr))
    {
        S3D_VALIDATE_HRESULT(hr);
        S3D_DBGPRINT("Error: D3D12CreateDevice failed, error code = 0x%08x", hr);
        assert(false);
    }

#if _DEBUG
    nvrhi::RefCountPtr<ID3D12InfoQueue> pInfoQueue;
    m_Device12->QueryInterface(&pInfoQueue);

    if (pInfoQueue)
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

        D3D12_MESSAGE_ID disableMessageIDs[] = {
            D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
            D3D12_MESSAGE_ID_COMMAND_LIST_STATIC_DESCRIPTOR_RESOURCE_DIMENSION_MISMATCH, // descriptor validation doesn't understand acceleration structures
            D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED, // NGX currently generates benign resource creation warnings
        };

        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.pIDList = disableMessageIDs;
        filter.DenyList.NumIDs = sizeof(disableMessageIDs) / sizeof(disableMessageIDs[0]);
        pInfoQueue->AddStorageFilterEntries(&filter);
    }
#endif

    D3D12_COMMAND_QUEUE_DESC queueDesc;
    ZeroMemory(&queueDesc, sizeof(queueDesc));
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.NodeMask = 1;
    hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_GraphicsQueue));
    S3D_VALIDATE_HRESULT(hr);
    m_GraphicsQueue->SetName(L"Graphics Queue");

    /*
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_ComputeQueue));
    S3D_VALIDATE_HRESULT(hr);
    m_ComputeQueue->SetName(L"Compute Queue");

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CopyQueue));
    S3D_VALIDATE_HRESULT(hr);
    m_CopyQueue->SetName(L"Copy Queue");
    */

    nvrhi::d3d12::DeviceDesc deviceDesc;
    deviceDesc.errorCB = &DefaultMessageCallback::GetInstance();
    deviceDesc.pDevice = m_Device12;
    deviceDesc.pGraphicsCommandQueue = m_GraphicsQueue;
    deviceDesc.pComputeCommandQueue = m_ComputeQueue;
    deviceDesc.pCopyCommandQueue = m_CopyQueue;
#if _DEBUG
    deviceDesc.logBufferLifetime = true;
#else
    deviceDesc.logBufferLifetime = false;
#endif
    deviceDesc.enableHeapDirectlyIndexed = false; // TODO: allows ResourceDescriptorHeap on DX12 - is this required?

    m_NvrhiDevice = nvrhi::d3d12::createDevice(deviceDesc);

#if _DEBUG
    m_NvrhiDevice = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
#endif
}

void Synesthesia3D::RendererNVRHID3D12::DestroyDevice()
{
    m_NvrhiDevice = nullptr;

    m_GraphicsQueue = nullptr;
    m_ComputeQueue = nullptr;
    m_CopyQueue = nullptr;
    m_Device12 = nullptr;
}
