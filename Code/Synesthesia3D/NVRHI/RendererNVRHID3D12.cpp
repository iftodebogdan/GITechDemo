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

#include "RendererNVRHID3D12.h"
using namespace Synesthesia3D;

#if ENABLE_NVRHI_D3D12

//#define LOAD_WINPIX_GPU_CAPTURER_DLL (_PROFILE || _DEBUG)

#if LOAD_WINPIX_GPU_CAPTURER_DLL
#include <filesystem>
#include <shlobj.h>
#endif

#include <unordered_set>

#include <nvrhi/validation.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#if _DEBUG
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

#include "MappingsNVRHI.h"

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

#if LOAD_WINPIX_GPU_CAPTURER_DLL
static std::wstring GetLatestWinPixGpuCapturerPath_Cpp17()
{
    LPWSTR programFilesPath = nullptr;
    SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

    std::filesystem::path pixInstallationPath = programFilesPath;
    pixInstallationPath /= "Microsoft PIX";

    std::wstring newestVersionFound;

    for (auto const& directory_entry : std::filesystem::directory_iterator(pixInstallationPath))
    {
        if (directory_entry.is_directory())
        {
            if (newestVersionFound.empty() || newestVersionFound < directory_entry.path().filename().c_str())
            {
                newestVersionFound = directory_entry.path().filename().c_str();
            }
        }
    }

    if (newestVersionFound.empty())
    {
        S3D_DBGPRINT("Warning: Could not find WinPixGpuCapturer.dll for GPU capture.");
        return L"";
    }

    return pixInstallationPath / newestVersionFound / L"WinPixGpuCapturer.dll";
}
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

    S3D_DBGPRINT("%s: %s\n", severityString.c_str(), messageText);

    assert(severity == nvrhi::MessageSeverity::Info);
}

const nvrhi::Format SwapChainFormats[] = {
    nvrhi::Format::SRGBA8_UNORM,
    nvrhi::Format::SBGRA8_UNORM,
    nvrhi::Format::RGBA8_UNORM,
    nvrhi::Format::BGRA8_UNORM,
    nvrhi::Format::R10G10B10A2_UNORM,
    nvrhi::Format::RGBA16_FLOAT
};

RendererNVRHID3D12::RendererNVRHID3D12()
{
    m_eAPI = API_NVRHI_D3D12;

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
    m_pImmediateGraphicsCommandList = nullptr;

    DestroyDeviceAndSwapchain();

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
#if LOAD_WINPIX_GPU_CAPTURER_DLL
    // Check to see if a copy of WinPixGpuCapturer.dll has already been injected into the application.
    // This may happen if the application is launched through the PIX UI.
    if (GetModuleHandleW(L"WinPixGpuCapturer.dll") == 0)
    {
        const std::wstring dllPath = GetLatestWinPixGpuCapturerPath_Cpp17();
        if (!dllPath.empty())
            LoadLibraryW(dllPath.c_str());
    }
#endif

    CreateDevice();
    CreateSwapChain(hWnd);

    RendererNVRHI::Initialize(hWnd);
}

void RendererNVRHID3D12::SwapBuffers()
{
    assert(GetDeviceState() == DS_PRESENTING);
    if (GetDeviceState() != DS_PRESENTING)
        return;

    auto bufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

    UINT presentFlags = 0;
    if(!GetVSyncStatus() && !IsFullscreen() && m_TearingSupported)
        presentFlags |= DXGI_PRESENT_ALLOW_TEARING;

    HRESULT hr = m_SwapChain->Present(GetVSyncStatus() ? 1 : 0, presentFlags);
    S3D_VALIDATE_HRESULT(hr);

    m_FrameFence->SetEventOnCompletion(m_FrameCount, m_FrameFenceEvents[bufferIndex]);
    m_GraphicsQueue->Signal(m_FrameFence, m_FrameCount);
    m_FrameCount++;

    Renderer::SwapBuffers();
}

void RendererNVRHID3D12::CreateDevice()
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

#if _DEBUG
    VLDDisable();
#endif

    HRESULT hr = D3D12CreateDevice(
        m_DxgiAdapter,
        D3D_FEATURE_LEVEL_12_1, // TODO: check feature level support
        IID_PPV_ARGS(&m_Device12));

#if _DEBUG
    VLDEnable();
#endif

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

#if _DEBUG
    VLDDisable();
#endif

    D3D12_COMMAND_QUEUE_DESC queueDesc;
    ZeroMemory(&queueDesc, sizeof(queueDesc));
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.NodeMask = 1;
    hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_GraphicsQueue));
    S3D_VALIDATE_HRESULT(hr);
    m_GraphicsQueue->SetName(L"Graphics Queue");

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_ComputeQueue));
    S3D_VALIDATE_HRESULT(hr);
    m_ComputeQueue->SetName(L"Compute Queue");

    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CopyQueue));
    S3D_VALIDATE_HRESULT(hr);
    m_CopyQueue->SetName(L"Copy Queue");

#if _DEBUG
    VLDEnable();
#endif

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

    m_pDevice = nvrhi::d3d12::createDevice(deviceDesc);

#if _DEBUG
    m_pDevice = nvrhi::validation::createValidationLayer(m_pDevice);
#endif
}

const bool RendererNVRHID3D12::CreateSwapChain(void* hWnd)
{
    ZeroMemory(&m_SwapChainDesc, sizeof(m_SwapChainDesc));
    m_SwapChainDesc.Width = 1920;
    m_SwapChainDesc.Height = 1080;
    m_SwapChainDesc.SampleDesc.Count = 1;
    m_SwapChainDesc.SampleDesc.Quality = 0;
    m_SwapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_SwapChainDesc.BufferCount = 3;
    m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    m_SwapChainDesc.Flags = 0; // TODO: consider using DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH?

    // Special processing for sRGB swap chain formats.
    // DXGI will not create a swap chain with an sRGB format, but its contents will be interpreted as sRGB.
    // So we need to use a non-sRGB format here, but store the true sRGB format for later framebuffer creation.
    switch (SwapChainFormats[0])
    {
    case nvrhi::Format::SRGBA8_UNORM:
        m_SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
    case nvrhi::Format::SBGRA8_UNORM:
        m_SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        break;
    default:
        m_SwapChainDesc.Format = nvrhi::d3d12::convertFormat(SwapChainFormats[0]);
        break;
    }

    nvrhi::RefCountPtr<IDXGIFactory5> pDxgiFactory5;
    if (SUCCEEDED(m_DxgiFactory2->QueryInterface(IID_PPV_ARGS(&pDxgiFactory5))))
    {
        BOOL supported = 0;
        if (SUCCEEDED(pDxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof(supported))))
            m_TearingSupported = (supported != 0);
    }

    if (m_TearingSupported)
    {
        m_SwapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    }

    m_FullScreenDesc = {};
    m_FullScreenDesc.RefreshRate.Numerator = 60;
    m_FullScreenDesc.RefreshRate.Denominator = 1;
    m_FullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    m_FullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    m_FullScreenDesc.Windowed = true;

    nvrhi::RefCountPtr<IDXGISwapChain1> pSwapChain1;
    HRESULT hr = m_DxgiFactory2->CreateSwapChainForHwnd(m_GraphicsQueue, (HWND)hWnd, &m_SwapChainDesc, &m_FullScreenDesc, nullptr, &pSwapChain1);
    S3D_VALIDATE_HRESULT(hr);

    hr = pSwapChain1->QueryInterface(IID_PPV_ARGS(&m_SwapChain));
    S3D_VALIDATE_HRESULT(hr);

    if (!CreateRenderTargets())
        return false;

    hr = m_Device12->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_FrameFence));
    S3D_VALIDATE_HRESULT(hr);

    for (UINT bufferIndex = 0; bufferIndex < m_SwapChainDesc.BufferCount; bufferIndex++)
    {
        m_FrameFenceEvents.push_back(CreateEvent(nullptr, false, true, nullptr));
    }

    return true;
}

const bool Synesthesia3D::RendererNVRHID3D12::CreateRenderTargets()
{
    m_SwapChainBuffers.resize(m_SwapChainDesc.BufferCount);
    m_RhiSwapChainBuffers.resize(m_SwapChainDesc.BufferCount);

    for (UINT n = 0; n < m_SwapChainDesc.BufferCount; n++)
    {
        const HRESULT hr = m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_SwapChainBuffers[n]));
        S3D_VALIDATE_HRESULT(hr);

        const nvrhi::Format swapChainFormat = nvrhi::Format::SRGBA8_UNORM;

        nvrhi::TextureDesc textureDesc;
        textureDesc.width = m_SwapChainDesc.Width;
        textureDesc.height = m_SwapChainDesc.Height;
        textureDesc.sampleCount = m_SwapChainDesc.SampleDesc.Count;
        textureDesc.sampleQuality = m_SwapChainDesc.SampleDesc.Quality;
        textureDesc.format = swapChainFormat;
        textureDesc.debugName = "SwapChainBuffer";
        textureDesc.isRenderTarget = true;
        textureDesc.isUAV = false;
        textureDesc.initialState = nvrhi::ResourceStates::Present;
        textureDesc.keepInitialState = true;

        m_RhiSwapChainBuffers[n] = m_pDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(m_SwapChainBuffers[n]), textureDesc);
    }

    return true;
}

void RendererNVRHID3D12::DestroyDeviceAndSwapchain()
{
    m_RhiSwapChainBuffers.clear();

    ReleaseRenderTargets();

    m_pDevice = nullptr;

    for (auto fenceEvent : m_FrameFenceEvents)
    {
        WaitForSingleObject(fenceEvent, INFINITE);
        CloseHandle(fenceEvent);
    }

    m_FrameFenceEvents.clear();

    if (m_SwapChain)
    {
        m_SwapChain->SetFullscreenState(false, nullptr);
    }

    m_SwapChainBuffers.clear();

    m_FrameFence = nullptr;
    m_SwapChain = nullptr;
    m_GraphicsQueue = nullptr;
    m_ComputeQueue = nullptr;
    m_CopyQueue = nullptr;
    m_Device12 = nullptr;
}

void RendererNVRHID3D12::ReleaseRenderTargets()
{
    if (m_pDevice)
    {
        // Make sure that all frames have finished rendering
        m_pDevice->waitForIdle();

        // Release all in-flight references to the render targets
        m_pDevice->runGarbageCollection();
    }

    // Set the events so that WaitForSingleObject in OneFrame will not hang later
    for (auto e : m_FrameFenceEvents)
        SetEvent(e);

    // Release the old buffers because ResizeBuffers requires that
    m_RhiSwapChainBuffers.clear();
    m_SwapChainBuffers.clear();
}

namespace std {
    template<>
    struct hash<DeviceCaps::SupportedScreenFormat> {
        const size_t operator()(const DeviceCaps::SupportedScreenFormat& c) const
        {
            return std::hash<unsigned int>()(c.nWidth)
                ^ std::hash<unsigned int>()(c.nHeight)
                ^ std::hash<unsigned int>()(c.nRefreshRate)
                ^ std::hash<PixelFormat>()(c.ePixelFormat);
        }
    };

    template<>
    struct equal_to<DeviceCaps::SupportedScreenFormat> {
        const bool operator()(const DeviceCaps::SupportedScreenFormat& a, const DeviceCaps::SupportedScreenFormat& b) const
        {
            return a.nWidth == b.nWidth
                && a.nHeight == b.nHeight
                && a.nRefreshRate == b.nRefreshRate
                && a.ePixelFormat == b.ePixelFormat;
        }
    };
}

void RendererNVRHID3D12::CheckDeviceCaps()
{
    RendererNVRHI::CheckDeviceCaps();

    // Verify supported backbuffer formats
    std::unordered_set<DeviceCaps::SupportedScreenFormat> uniqueScreenFormats;

    unsigned int adapterCount = 0;
    while (true)
    {
        nvrhi::RefCountPtr<IDXGIAdapter> adapter;
        HRESULT hr = m_DxgiFactory2->EnumAdapters(adapterCount, &adapter);
        if (FAILED(hr))
            break;

        DXGI_ADAPTER_DESC desc;
        hr = adapter->GetDesc(&desc);
        if (FAILED(hr))
            break;

        unsigned int outputCount = 0;
        while (true)
        {
            nvrhi::RefCountPtr<IDXGIOutput> output;
            hr = adapter->EnumOutputs(outputCount, &output);
            if (FAILED(hr))
                break;

            for (nvrhi::Format scf : SwapChainFormats)
            {
                DXGI_FORMAT fmt = nvrhi::d3d12::convertFormat(scf);
                UINT numModes = 0;
                hr = output->GetDisplayModeList(fmt, 0, &numModes, nullptr);

                if (FAILED(hr))
                    break;

                std::vector<DXGI_MODE_DESC> modeDescs;
                modeDescs.resize(numModes);
                hr = output->GetDisplayModeList(fmt, 0, &numModes, modeDescs.data());

                if (FAILED(hr))
                    break;

                for (const DXGI_MODE_DESC& modeDesc : modeDescs)
                {
                    DeviceCaps::SupportedScreenFormat sf;
                    sf.nWidth = modeDesc.Width;
                    sf.nHeight = modeDesc.Height;
                    sf.nRefreshRate = modeDesc.RefreshRate.Numerator / modeDesc.RefreshRate.Denominator;
                    sf.ePixelFormat = MatchPixelFormat(scf);
                    uniqueScreenFormats.insert(sf);
                }
            }

            outputCount++;
        }

        adapterCount++;
    }

    m_tDeviceCaps.arrSupportedScreenFormats.insert(
        m_tDeviceCaps.arrSupportedScreenFormats.end(),
        uniqueScreenFormats.begin(),
        uniqueScreenFormats.end()
    );

    m_tDeviceCaps.nNumSimultaneousRTs = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;

    // Verify supported texture formats
    const D3D12_FORMAT_SUPPORT1 arrTextureTypeMap[] = {
        D3D12_FORMAT_SUPPORT1_TEXTURE1D,    // TT_1D,
        D3D12_FORMAT_SUPPORT1_TEXTURE2D,    // TT_2D,
        D3D12_FORMAT_SUPPORT1_TEXTURE3D,    // TT_3D,
        D3D12_FORMAT_SUPPORT1_TEXTURECUBE,  // TT_CUBE
    };

    const D3D12_FORMAT_SUPPORT1 arrBufferUsageMap[] = {
        D3D12_FORMAT_SUPPORT1_NONE,             // BU_STATIC
        D3D12_FORMAT_SUPPORT1_NONE,             // BU_DYNAMIC
        D3D12_FORMAT_SUPPORT1_RENDER_TARGET,    // BU_RENDERTAGET
        D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL,    // BU_DEPTHSTENCIL
        D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE     // BU_TEXTURE
    };

    for (unsigned int pf = PF_NONE; pf < PF_MAX; pf++)
    {
        D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = { nvrhi::d3d12::convertFormat(PixelFormatNVRHI[pf]) };
        HRESULT hr = m_Device12->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport));
        S3D_VALIDATE_HRESULT(hr);

        if (SUCCEEDED(hr))
        {
            for (unsigned int tt = TT_1D; tt < TT_MAX; tt++)
            {
                for (unsigned int bu = BU_STATIC; bu < BU_MAX; bu++)
                {
                    if (formatSupport.Support1 & (arrTextureTypeMap[tt] | arrBufferUsageMap[bu]))
                    {
                        DeviceCaps::SupportedPixelFormat tf;
                        tf.ePixelFormat = (PixelFormat)pf;
                        tf.eResourceUsage = (BufferUsage)bu;
                        tf.eTextureType = (TextureType)tt;

                        m_tDeviceCaps.arrSupportedPixelFormats.push_back(tf);
                    }
                }
            }
        }
    }
}

#endif // ENABLE_NVRHI_D3D12
