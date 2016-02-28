/**
 *	@file		RendererDX9.cpp
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *	@copyright
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	@copyright
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *	@copyright
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"

#include "RendererDX9.h"

#include "VertexFormatDX9.h"
#include "VertexBufferDX9.h"
#include "IndexBufferDX9.h"
#include "TextureDX9.h"
#include "ResourceManagerDX9.h"
#include "RenderStateDX9.h"
#include "SamplerStateDX9.h"
using namespace Synesthesia3D;

#include <d3dx9.h>

D3DFORMAT BBFormats[] = { D3DFMT_A2R10G10B10, D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_A1R5G5B5, D3DFMT_X1R5G5B5, D3DFMT_R5G6B5 };
D3DFORMAT DSFormats[] = { D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D15S1, D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D16 }; // Prefer with stencil

RendererDX9::RendererDX9()
	: m_pD3D(nullptr)
	, m_pd3dDevice(nullptr)
	, m_bDeviceLost(false)
{}

RendererDX9::~RendererDX9()
{
	ULONG refCount = 0;

	if (m_pd3dDevice)
		refCount = m_pd3dDevice->Release();
	assert(refCount == 0);

	if (m_pD3D)
		refCount = m_pD3D->Release();
	assert(refCount == 0);
}

void RendererDX9::CheckDeviceCaps()
{
	std::vector<D3DFORMAT> arrValidBBFormats;

	for (unsigned int bbf = 0; bbf < ARRAYSIZE(BBFormats); bbf++)
	{
		const PixelFormat fmt = MatchPixelFormat(BBFormats[bbf]);
		if (fmt == PF_NONE)
			continue;

		// Verify supported backbuffer formats
		unsigned int nMaxAdaptorModes = m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, BBFormats[bbf]);
		for (unsigned int nMode = 0; nMode < nMaxAdaptorModes; ++nMode)
		{
			for (unsigned int bbf2 = 0; bbf2 < ARRAYSIZE(BBFormats); bbf2++)
			{
				const PixelFormat fmt2 = MatchPixelFormat(BBFormats[bbf2]);
				if (fmt2 == PF_NONE)
					continue;

				D3DDISPLAYMODE d3ddm;
				if (FAILED(m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, BBFormats[bbf2], nMode, &d3ddm)))
					continue;

				DeviceCaps::SupportedScreenFormat sf;
				sf.nWidth = d3ddm.Width;
				sf.nHeight = d3ddm.Height;
				sf.nRefreshRate = d3ddm.RefreshRate;
				sf.ePixelFormat = MatchPixelFormat(d3ddm.Format);

				if (sf.ePixelFormat != PF_NONE)
					m_tDeviceCaps.arrSupportedScreenFormats.push_back(sf);

				// Populate the temporary list of valid backbuffer formats
				bool bNotFound = true;
				for (unsigned int vbbf = 0; vbbf < arrValidBBFormats.size(); vbbf++)
				{
					if (d3ddm.Format == arrValidBBFormats[vbbf])
					{
						bNotFound = false;
						break;
					}
				}
				if (bNotFound)
					arrValidBBFormats.push_back(d3ddm.Format);
			}
		}
	}

	// Verify supported texture formats
	for (unsigned int tt = TT_1D; tt < TT_MAX; tt++)
	{
		for (unsigned int bu = BU_STATIC; bu < BU_MAX; bu++)
		{
			for (unsigned int pf = PF_NONE; pf < PF_MAX; pf++)
			{
				bool bValidPixelFormat = true;
				for (unsigned int vbbf = 0; vbbf < arrValidBBFormats.size(); vbbf++)
				{
					if (FAILED(m_pD3D->CheckDeviceFormat(
						D3DADAPTER_DEFAULT,
						D3DDEVTYPE_HAL,
						arrValidBBFormats[vbbf],
						BufferUsageDX9[bu],
						TextureTypeDX9[tt],
						PixelFormatDX9[pf])))
					{
						bValidPixelFormat = false;
						break;
					}
				}

				if (bValidPixelFormat)
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

	// Query the device for its capabilities.
	D3DCAPS9 deviceCaps;
	HRESULT hr = m_pD3D->GetDeviceCaps(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		&deviceCaps);
	assert(SUCCEEDED(hr));

	m_tDeviceCaps.bCanAutoGenMipmaps = (deviceCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP) != 0;
	m_tDeviceCaps.bDynamicTextures = (deviceCaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) != 0;
	m_tDeviceCaps.bPresentIntervalImmediate = (deviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) != 0;
	m_tDeviceCaps.bPresentIntervalOne = (deviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE) != 0;
	m_tDeviceCaps.bPresentIntervalTwo = (deviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_TWO) != 0;
	m_tDeviceCaps.bPresentIntervalThree = (deviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_THREE) != 0;
	m_tDeviceCaps.bPresentIntervalFour = (deviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_FOUR) != 0;
	m_tDeviceCaps.bMrtIndependentBitDepths = (deviceCaps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS) != 0;
	m_tDeviceCaps.bMrtPostPixelShaderBlending = (deviceCaps.PrimitiveMiscCaps & D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING) != 0;
	m_tDeviceCaps.bAnisotropicFiltering = (deviceCaps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY) != 0;
	m_tDeviceCaps.bDepthBias = (deviceCaps.RasterCaps & D3DPRASTERCAPS_DEPTHBIAS) != 0;
	m_tDeviceCaps.bSlopeScaledDepthBias = (deviceCaps.RasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS) != 0;
	m_tDeviceCaps.bMipmapLodBias = (deviceCaps.RasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS) != 0;
	m_tDeviceCaps.bWBuffer = (deviceCaps.RasterCaps & D3DPRASTERCAPS_WBUFFER) != 0;
	m_tDeviceCaps.bTextureNPOT =
		(deviceCaps.TextureCaps & D3DPTEXTURECAPS_POW2) == 0 &&
		(deviceCaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) == 0 &&
		(deviceCaps.TextureCaps & D3DPTEXTURECAPS_VOLUMEMAP_POW2) == 0 &&
		(deviceCaps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP_POW2) == 0;
	m_tDeviceCaps.nMaxTextureWidth = deviceCaps.MaxTextureWidth;
	m_tDeviceCaps.nMaxTextureHeight = deviceCaps.MaxTextureHeight;
	m_tDeviceCaps.nMaxTextureDepth = deviceCaps.MaxVolumeExtent;
	m_tDeviceCaps.nVertexShaderVersionMajor = D3DSHADER_VERSION_MAJOR(deviceCaps.VertexShaderVersion);
	m_tDeviceCaps.nVertexShaderVersionMinor = D3DSHADER_VERSION_MINOR(deviceCaps.VertexShaderVersion);
	m_tDeviceCaps.nPixelShaderVersionMajor = D3DSHADER_VERSION_MAJOR(deviceCaps.PixelShaderVersion);
	m_tDeviceCaps.nPixelShaderVersionMinor = D3DSHADER_VERSION_MINOR(deviceCaps.PixelShaderVersion);
	m_tDeviceCaps.nNumSimultaneousRTs = deviceCaps.NumSimultaneousRTs;
}

void RendererDX9::ValidatePresentParameters(D3DPRESENT_PARAMETERS& pp)
{
	// Validate backbuffer format
	D3DFORMAT validBBFmt = pp.BackBufferFormat;
	int i = 0;
	for (; i < ARRAYSIZE(BBFormats) && BBFormats[i] != validBBFmt; i++); // END FOR

	// Can we get the desired back buffer format?
	while (FAILED(m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		validBBFmt,
		validBBFmt,
		pp.Windowed)))
	{
		if (++i > ARRAYSIZE(BBFormats) - 1)
			break;
		validBBFmt = BBFormats[i];
	}
	pp.BackBufferFormat = validBBFmt;

	// Validate backbuffer size
	if (!pp.Windowed)
	{
		Vec2i screenSize(pp.BackBufferWidth, pp.BackBufferHeight);
		ValidateScreenResolution(screenSize);
		pp.BackBufferWidth = screenSize[0];
		pp.BackBufferHeight = screenSize[1];
	}

	// Validate backbuffer count
	pp.BackBufferCount = Math::clamp(pp.BackBufferCount, 0u, (unsigned int)D3DPRESENT_BACK_BUFFERS_MAX);

	// Validate depth-stencil format
	if (pp.EnableAutoDepthStencil)
	{
		// Can we get the desired depth buffer format?
		D3DFORMAT validDSFmt = pp.AutoDepthStencilFormat;
		int i = 0;
		for (; i < ARRAYSIZE(DSFormats) && DSFormats[i] != validDSFmt; i++); // END FOR

		while (FAILED(m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			pp.BackBufferFormat,
			D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE,
			validDSFmt)))
		{
			if (++i > ARRAYSIZE(DSFormats) - 1)
				break;
			validDSFmt = DSFormats[i];
		}
		pp.AutoDepthStencilFormat = validDSFmt;
	}

	// Validate refresh rate
	if (!pp.Windowed)
	{
		UINT validRefresh = 0;
		for (unsigned int i = 0; i < m_tDeviceCaps.arrSupportedScreenFormats.size(); i++)
		{
			if (GetBackBufferFormat() == m_tDeviceCaps.arrSupportedScreenFormats[i].ePixelFormat &&
				pp.BackBufferWidth == m_tDeviceCaps.arrSupportedScreenFormats[i].nWidth &&
				pp.BackBufferHeight == m_tDeviceCaps.arrSupportedScreenFormats[i].nHeight &&
				validRefresh < m_tDeviceCaps.arrSupportedScreenFormats[i].nRefreshRate &&
				pp.FullScreen_RefreshRateInHz >= m_tDeviceCaps.arrSupportedScreenFormats[i].nRefreshRate)
			{
				validRefresh = m_tDeviceCaps.arrSupportedScreenFormats[i].nRefreshRate;
			}
		}

		pp.FullScreen_RefreshRateInHz = validRefresh;
	}
	else
		pp.FullScreen_RefreshRateInHz = 0;

	// Validate MSAA
	if (pp.MultiSampleType != D3DMULTISAMPLE_NONE)
	{
		DWORD validQuality = 0;
		D3DMULTISAMPLE_TYPE validType = pp.MultiSampleType;
		while (FAILED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			pp.BackBufferFormat,
			pp.Windowed,
			validType,
			&validQuality)) ||
			FAILED(m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_HAL,
				pp.AutoDepthStencilFormat,
				pp.Windowed,
				validType,
				&validQuality)))
		{
			validType = (D3DMULTISAMPLE_TYPE)(validType - 1);
		}

		pp.MultiSampleType = validType;
		pp.MultiSampleQuality = Math::clamp(pp.MultiSampleQuality, 0ul, validQuality - 1);

		if(pp.MultiSampleType != D3DMULTISAMPLE_NONE)
			pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	}

	// Validate presentation interval
	UINT validPI = pp.PresentationInterval;
	switch (pp.PresentationInterval)
	{
	case D3DPRESENT_INTERVAL_FOUR:
		if (m_tDeviceCaps.bPresentIntervalFour)
		{
			validPI = D3DPRESENT_INTERVAL_FOUR;
			break;
		}
	case D3DPRESENT_INTERVAL_THREE:
		if (m_tDeviceCaps.bPresentIntervalThree)
		{
			validPI = D3DPRESENT_INTERVAL_THREE;
			break;
		}
	case D3DPRESENT_INTERVAL_TWO:
		if (m_tDeviceCaps.bPresentIntervalTwo)
		{
			validPI = D3DPRESENT_INTERVAL_TWO;
			break;
		}
	case D3DPRESENT_INTERVAL_ONE:
		if (m_tDeviceCaps.bPresentIntervalOne)
		{
			validPI = D3DPRESENT_INTERVAL_ONE;
			break;
		}
	case D3DPRESENT_INTERVAL_IMMEDIATE:
		if (m_tDeviceCaps.bPresentIntervalImmediate)
		{
			validPI = D3DPRESENT_INTERVAL_IMMEDIATE;
			break;
		}
	case D3DPRESENT_INTERVAL_DEFAULT:
		validPI = D3DPRESENT_INTERVAL_DEFAULT;
		break;
	default:
		break;
	}
	pp.PresentationInterval = validPI;

	// Validate swap effect
	if (pp.SwapEffect == D3DSWAPEFFECT_COPY)
		pp.BackBufferCount = 1;
}

void RendererDX9::Initialize(void* hWnd)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	// Create the D3D object, which is needed to create the D3DDevice.
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	assert(m_pD3D);

	// Set up the structure used to create the D3DDevice. Most parameters are
	// zeroed out. We set Windowed to TRUE, since we want to do D3D in a
	// window, and then set the SwapEffect to "discard", which is the most
	// efficient method of presenting the back buffer to the display. And 
	// we request a back buffer format that matches the current desktop display 
	// format.
	ZeroMemory(&m_ePresentParameters, sizeof(m_ePresentParameters));
	m_ePresentParameters.Windowed = TRUE;
	m_ePresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_ePresentParameters.BackBufferFormat = BBFormats[0];
	//m_ePresentParameters.BackBufferWidth = 1920;
	//m_ePresentParameters.BackBufferHeight = 1080;
	m_ePresentParameters.BackBufferCount = 1;
	m_ePresentParameters.EnableAutoDepthStencil = TRUE;
	m_ePresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_ePresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	m_ePresentParameters.hDeviceWindow = (HWND)hWnd;

	// Populate device caps structure
	CheckDeviceCaps();

	// Validate the present parameters before sending them off to the device
	ValidatePresentParameters(m_ePresentParameters);

	// Set lowest resolution at first, for safety
	m_ePresentParameters.BackBufferWidth = INT_MAX;
	m_ePresentParameters.BackBufferHeight = INT_MAX;
	for (unsigned int i = 0; i < m_tDeviceCaps.arrSupportedScreenFormats.size(); i++)
	{
		DeviceCaps::SupportedScreenFormat& sf = m_tDeviceCaps.arrSupportedScreenFormats[i];
		if (sf.ePixelFormat == GetBackBufferFormat() && (sf.nWidth < m_ePresentParameters.BackBufferWidth || sf.nHeight < m_ePresentParameters.BackBufferHeight))
		{
			m_ePresentParameters.BackBufferWidth = sf.nWidth;
			m_ePresentParameters.BackBufferHeight = sf.nHeight;
		}
	}

	// Create the Direct3D device. Here we are using the default adapter (most
	// systems only have one, unless they have multiple graphics hardware cards
	// installed) and requesting the HAL (which is saying we want the hardware
	// device rather than a software one). Software vertex processing is 
	// specified since we know it will work on all cards. On cards that support 
	// hardware vertex processing, though, we would see a big performance gain 
	// by specifying hardware vertex processing.

	//################# EXPERIMENTAL FIX ################################
	//-----------------------------------------------------------------------------
	// There is a problem with the function in GTK's graphics library, Cairo:
	//		static inline cairo_fixed_t _cairo_fixed_from_double (double d)
	// It relies on some floating point math that stops working when
	// computations are done with lower accuracy. Windows changes some
	// floating point precision flags when initializing a Direct3D device,
	// causing Cairo to malfunction and stop rendering our window. This
	// behaviour has only been encountered in the 32-bit version of the app.
	// Unfortunately, using D3DCREATE_FPU_PRESERVE may reduce Direct3D performance
	// in double-precision mode and could also cause undefined behavior.
	// http://msdn.microsoft.com/en-us/library/bb172527(VS.85).aspx
#if defined(_WIN32) && !defined(_WIN64)
	DWORD BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
#elif defined(_WIN32) && defined(_WIN64)
	DWORD BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
#endif
	//------------------------------------------------------------------------------

	// Support for multithreading
	BehaviorFlags |= D3DCREATE_MULTITHREADED;

	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)hWnd,
		BehaviorFlags,
		&m_ePresentParameters, &m_pd3dDevice)))
	{
		BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
		BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)hWnd,
			BehaviorFlags,
			&m_ePresentParameters, &m_pd3dDevice);
	}

	assert(m_pd3dDevice);

	m_pResourceManager = new ResourceManagerDX9();
	m_pRenderStateManager = new RenderStateDX9();
	m_pSamplerStateManager = new SamplerStateDX9();

	POP_PROFILE_MARKER();
}

const bool RendererDX9::SetScreenResolution(const Vec2i size, const Vec2i offset, const bool fullscreen)
{
	Vec2i validOffset(offset);
	if (fullscreen)
		validOffset[0] = validOffset[1] = 0;

	// If already set, skip
	if (size[0] == m_ePresentParameters.BackBufferWidth &&
		size[1] == m_ePresentParameters.BackBufferHeight &&
		validOffset == m_vBackBufferOffset &&
		fullscreen == !m_ePresentParameters.Windowed)
		return false;

	// Some sanity checks
	if (!m_pd3dDevice ||
		FAILED(m_pd3dDevice->TestCooperativeLevel()) ||
		!size[0] || !size[1])
		return false;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	HRESULT hr;
	// Get swap chain
	LPDIRECT3DSWAPCHAIN9 sc;
	hr = m_pd3dDevice->GetSwapChain(0, &sc);
	assert(SUCCEEDED(hr));

	// Get present parameters
	D3DPRESENT_PARAMETERS pp;
	hr = sc->GetPresentParameters(&pp);
	assert(SUCCEEDED(hr));

	ULONG refCount = 0;
	refCount = sc->Release();
	assert(refCount == 0);

	// Set back buffer size
	pp.BackBufferWidth = size[0];
	pp.BackBufferHeight = size[1];

	// Windowed or fullscreen?
	pp.Windowed = (BOOL)(!fullscreen);
	pp.FullScreen_RefreshRateInHz = fullscreen ? 60 : 0;

	// Unbind resources
	GetResourceManager()->UnbindAll();

	// Validate and correct the present parameters
	ValidatePresentParameters(pp);

	// Reset the device
	hr = m_pd3dDevice->Reset(&pp);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
	{
		m_ePresentParameters = pp;
		m_vBackBufferOffset = validOffset;

		// Rebind resources
		GetResourceManager()->BindAll();

		// Reset sampler states
		GetSamplerStateManager()->Reset();

		// Reset render states
		GetRenderStateManager()->Reset();
	}

	POP_PROFILE_MARKER();

	return SUCCEEDED(hr);
}

void RendererDX9::SetViewport(const Vec2i size, const Vec2i offset)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	D3DVIEWPORT9 vp;
	vp.X = offset[0];
	vp.Y = offset[1];
	vp.Width = size[0];
	vp.Height = size[1];
	vp.MinZ = 0.f;
	vp.MaxZ = 1.f;
	
	HRESULT hr = m_pd3dDevice->SetViewport(&vp);
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();
}

const bool RendererDX9::BeginFrame()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	HRESULT hr = m_pd3dDevice->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)
	{
		if (!m_bDeviceLost)
			m_bDeviceLost = true;

		Sleep(1);
		POP_PROFILE_MARKER();
		return false;
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		// We need to recreate all resources, apart from shaders and textures,
		// which are managed. However, we recreate everything for safety.
		GetResourceManager()->UnbindAll();

		// Reset the device
		hr = m_pd3dDevice->Reset(&m_ePresentParameters);
		if (FAILED(hr))
		{
			POP_PROFILE_MARKER();
			return false;
		}

		// Rebind resources
		GetResourceManager()->BindAll();

		// Reset sampler states
		GetSamplerStateManager()->Reset();

		// Reset render states
		GetRenderStateManager()->Reset();

		m_bDeviceLost = false;
	}

	if (m_bDeviceLost)
	{
		Sleep(1);
		POP_PROFILE_MARKER();
		return false;
	}

	hr = m_pd3dDevice->BeginScene();
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();

	return true;
}

void RendererDX9::EndFrame()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	HRESULT hr = m_pd3dDevice->EndScene();
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();
}

void RendererDX9::SwapBuffers()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	// Present the backbuffer contents to the display
	RECT dstRect;
	dstRect.left = m_vBackBufferOffset[0];
	dstRect.top = m_vBackBufferOffset[1];
	dstRect.right = m_ePresentParameters.BackBufferWidth + m_vBackBufferOffset[0];
	dstRect.bottom = m_ePresentParameters.BackBufferHeight + m_vBackBufferOffset[1];

	HRESULT hr = m_pd3dDevice->Present(NULL, IsFullscreen() ? NULL : &dstRect, NULL, NULL);
	assert(SUCCEEDED(hr) || hr == D3DERR_DEVICELOST);

	POP_PROFILE_MARKER();
}

void RendererDX9::DrawVertexBuffer(VertexBuffer* const vb)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(vb);
	vb->Enable();

	if (vb->GetIndexBuffer())
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, vb->GetElementCount(), 0, vb->GetIndexBuffer()->GetElementCount() / 3);
	else
		m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, vb->GetElementCount() / 3);

	vb->Disable();

	POP_PROFILE_MARKER();
}

void RendererDX9::Clear(const Vec4f rgba, const float z, const unsigned int stencil)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	HRESULT hr;
	DWORD flags = D3DCLEAR_TARGET;
	IDirect3DSurface9* depthStencil = nullptr;

	hr = m_pd3dDevice->GetDepthStencilSurface(&depthStencil);
	if (hr != D3DERR_NOTFOUND && depthStencil)
	{
		flags |= D3DCLEAR_ZBUFFER;
		D3DSURFACE_DESC desc;
		memset(&desc, 0, sizeof(desc));
		depthStencil->GetDesc(&desc);
		if (desc.Format == D3DFMT_D24S8)
			flags |= D3DCLEAR_STENCIL;
		depthStencil->Release();
	}

	hr = m_pd3dDevice->Clear(0, NULL, flags, D3DCOLOR_COLORVALUE(rgba[0], rgba[1], rgba[2], rgba[3]), z, stencil);
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();
}

void RendererDX9::CreatePerspectiveMatrix(Matrix44f& matProj, const float fovYRad, const float aspectRatio, const float zNear, const float zFar) const
{
	D3DXMATRIXA16 mat;
	D3DXMatrixPerspectiveFovLH(&mat, fovYRad, aspectRatio, zNear, zFar);
	assert(sizeof(mat.m) == sizeof(matProj.mData));
	matProj.set(mat);
}

void RendererDX9::CreateOrthographicMatrix(Matrix44f& matProj, const float left, const float top, const float right, const float bottom, const float zNear, const float zFar) const
{
	D3DXMATRIXA16 mat;
	D3DXMatrixOrthoOffCenterLH(&mat, left, right, bottom, top, zNear, zFar);
	assert(sizeof(mat.m) == sizeof(matProj.mData));
	matProj.set(mat);
}

void RendererDX9::PushProfileMarker(const char* const label)
{
#if ENABLE_PROFILE_MARKERS
	Renderer::PushProfileMarker(label);
	unsigned int len = (unsigned int)strlen(label) + 1;
	wchar_t* labelWide = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, label, -1, labelWide, len);
	D3DPERF_BeginEvent((D3DCOLOR)0xffffffff, labelWide);
	delete[] labelWide;
#endif
}

void RendererDX9::PopProfileMarker()
{
#if ENABLE_PROFILE_MARKERS
	Renderer::PopProfileMarker();
	D3DPERF_EndEvent();
#endif
}
