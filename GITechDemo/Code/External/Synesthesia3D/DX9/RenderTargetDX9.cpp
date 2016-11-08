/**
 *	@file		RenderTargetDX9.cpp
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2016 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#include "RenderTargetDX9.h"
#include "TextureDX9.h"
#include "RendererDX9.h"
#include "MappingsDX9.h"
#include "ProfilerDX9.h"
using namespace Synesthesia3D;

IDirect3DSurface9* RenderTargetDX9::ms_pBBColorSurfBkp = nullptr;
IDirect3DSurface9* RenderTargetDX9::ms_pBBDepthSurfBkp = nullptr;

RenderTargetDX9::RenderTargetDX9(const unsigned int targetCount, PixelFormat pixelFormat,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: RenderTarget(targetCount, pixelFormat, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat)
	, m_pColorSurface(nullptr)
	, m_pDepthSurface(nullptr)
{
	HRESULT hr;

	m_pColorSurface = new IDirect3DSurface9*[m_nTargetCount];

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pColorBuffer[i])->GetTextureDX9();
		hr = dxTex->GetSurfaceLevel(0, &m_pColorSurface[i]);
		assert(SUCCEEDED(hr));
	}

	if (hasDepthStencil)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pDepthBuffer)->GetTextureDX9();
		hr = dxTex->GetSurfaceLevel(0, &m_pDepthSurface);
		assert(SUCCEEDED(hr));
	}
}

RenderTargetDX9::RenderTargetDX9(const unsigned int targetCount, PixelFormat pixelFormat,
	const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: RenderTarget(targetCount, pixelFormat, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat)
	, m_pColorSurface(nullptr)
	, m_pDepthSurface(nullptr)
{
	HRESULT hr;

	m_pColorSurface = new IDirect3DSurface9*[m_nTargetCount];

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pColorBuffer[i])->GetTextureDX9();
		hr = dxTex->GetSurfaceLevel(0, &m_pColorSurface[i]);
		assert(SUCCEEDED(hr));
	}

	if (hasDepthStencil)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pDepthBuffer)->GetTextureDX9();
		hr = dxTex->GetSurfaceLevel(0, &m_pDepthSurface);
		assert(SUCCEEDED(hr));
	}
}

RenderTargetDX9::RenderTargetDX9(const unsigned int targetCount,
	PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: RenderTarget(targetCount, pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat)
	, m_pColorSurface(nullptr)
	, m_pDepthSurface(nullptr)
{
	HRESULT hr;

	m_pColorSurface = new IDirect3DSurface9*[m_nTargetCount];

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pColorBuffer[i])->GetTextureDX9();
		hr = dxTex->GetSurfaceLevel(0, &m_pColorSurface[i]);
		assert(SUCCEEDED(hr));
	}

	if (hasDepthStencil)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pDepthBuffer)->GetTextureDX9();
		hr = dxTex->GetSurfaceLevel(0, &m_pDepthSurface);
		assert(SUCCEEDED(hr));
	}
}

RenderTargetDX9::RenderTargetDX9(const unsigned int targetCount,
	PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
	const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: RenderTarget(targetCount, pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat)
	, m_pColorSurface(nullptr)
	, m_pDepthSurface(nullptr)
{
	HRESULT hr;

	m_pColorSurface = new IDirect3DSurface9*[m_nTargetCount];

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pColorBuffer[i])->GetTextureDX9();
		hr = dxTex->GetSurfaceLevel(0, &m_pColorSurface[i]);
		assert(SUCCEEDED(hr));
	}

	if (hasDepthStencil)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pDepthBuffer)->GetTextureDX9();
		hr = dxTex->GetSurfaceLevel(0, &m_pDepthSurface);
		assert(SUCCEEDED(hr));
	}
}

RenderTargetDX9::~RenderTargetDX9()
{
	Unbind();

	if (m_pColorSurface)
	{
		delete[] m_pColorSurface;
		m_pColorSurface = nullptr;
	}
}

void RenderTargetDX9::Enable()
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

	// Make a backup copy of the pointers to the
	// backbuffer's color and depth surfaces.
	if (!GetActiveRenderTarget())
	{
		hr = device->GetRenderTarget(0, &ms_pBBColorSurfBkp);
		assert(SUCCEEDED(hr));

		hr = device->GetDepthStencilSurface(&ms_pBBDepthSurfBkp);
		assert(SUCCEEDED(hr));
	}

	for (unsigned int i = 0; i < Renderer::GetInstance()->GetDeviceCaps().nNumSimultaneousRTs; i++)
	{
		if (i < m_nTargetCount)
			hr = device->SetRenderTarget((DWORD)i, m_pColorSurface[i]);
		else
			hr = device->SetRenderTarget((DWORD)i, 0);
		assert(SUCCEEDED(hr));
	}

	// Viewport is automatically set
	hr = device->SetDepthStencilSurface(m_pDepthSurface);
	assert(SUCCEEDED(hr));

	RenderTarget::Enable();
}

void RenderTargetDX9::Disable()
{
	RenderTarget::Disable();

	if (!GetActiveRenderTarget())
	{
		// No more render targets on the stack, set the back buffer
		IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
		HRESULT hr;

		// This call restores the viewport automatically
		hr = device->SetRenderTarget(0, ms_pBBColorSurfBkp);
		assert(SUCCEEDED(hr));

		unsigned int refCount = 0;
		if (ms_pBBColorSurfBkp)
			refCount = ms_pBBColorSurfBkp->Release();
		assert(refCount == 0);
		ms_pBBColorSurfBkp = nullptr;

		for (unsigned int i = 1; i < Renderer::GetInstance()->GetDeviceCaps().nNumSimultaneousRTs; i++)
		{
			hr = device->SetRenderTarget((DWORD)i, 0);
			assert(SUCCEEDED(hr));
		}

		hr = device->SetDepthStencilSurface(ms_pBBDepthSurfBkp);
		assert(SUCCEEDED(hr));

		if (ms_pBBDepthSurfBkp)
			refCount = ms_pBBDepthSurfBkp->Release();
		assert(refCount == 0);
		ms_pBBDepthSurfBkp = nullptr;
	}
}

void RenderTargetDX9::CopyColorBuffer(const unsigned int colorBufferIdx, Texture* texture)
{
	assert(ms_pBBColorSurfBkp == nullptr);
	assert(colorBufferIdx >= 0 && colorBufferIdx < m_nTargetCount);
	if (colorBufferIdx < 0 && colorBufferIdx >= m_nTargetCount)
		return;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

	// Validate texture
	if (texture->GetWidth() != GetWidth() ||
		texture->GetHeight() != GetHeight() ||
		texture->GetPixelFormat() != GetPixelFormat() ||
		texture->GetTextureType() != TT_2D)
	{
		assert(false);
		return;
	}

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	// Back up our current render target, if required
	if (colorBufferIdx == 0)
	{
		hr = device->GetRenderTarget(0, &ms_pBBColorSurfBkp);
		assert(SUCCEEDED(hr));
	}
	
	hr = device->SetRenderTarget((DWORD)colorBufferIdx, m_pColorSurface[colorBufferIdx]);
	assert(SUCCEEDED(hr));

	// Create a temporary texture in system memory
	IDirect3DTexture9* tempTex = nullptr;
	hr = device->CreateTexture(GetWidth(), GetHeight(), 0, 0, PixelFormatDX9[GetPixelFormat()], D3DPOOL_SYSTEMMEM, &tempTex, NULL);
	assert(SUCCEEDED(hr));

	// Get the surface
	IDirect3DSurface9* tempSurf = nullptr;
	hr = tempTex->GetSurfaceLevel(0, &tempSurf);
	assert(SUCCEEDED(hr));

	// Copy the render target
	hr = device->GetRenderTargetData(m_pColorSurface[colorBufferIdx], tempSurf);
	assert(SUCCEEDED(hr));

	// Lock data
	D3DLOCKED_RECT rect;
	hr = tempSurf->LockRect(&rect, 0, 0);
	assert(SUCCEEDED(hr));

	// Write data to texture
	//memcpy(texture->GetMipData(0), rect.pBits, texture->GetMipSizeBytes(0));
	for (unsigned int i = 0; i < texture->GetHeight(); i++)
	{
		memcpy(
			texture->GetMipData(0) + i * texture->GetWidth() * texture->GetElementSize(),
			(s3dByte*)rect.pBits + i * rect.Pitch,
			texture->GetWidth() * texture->GetElementSize()
			);
	}

	hr = tempSurf->UnlockRect();
	assert(SUCCEEDED(hr));

	tempSurf->Release();
	tempTex->Release();

	// Restore previous RT, if necesary
	if (colorBufferIdx == 0)
	{
		hr = device->SetRenderTarget(0, ms_pBBColorSurfBkp);
		assert(SUCCEEDED(hr));
		if(ms_pBBColorSurfBkp)
			ms_pBBColorSurfBkp->Release();
		ms_pBBColorSurfBkp = nullptr;
	}

	//for (unsigned int i = 0, n = texture->GetMipCount(); i < n; i++)
	//{
	//	IDirect3DSurface9* destSurface = 0;
	//	hr = ((IDirect3DTexture9*)((TextureDX9*)texture)->GetTextureDX9())->GetSurfaceLevel(i, &destSurface);
	//	assert(SUCCEEDED(hr));
	//
	//	hr = device->StretchRect(m_pColorSurface[colorBufferIdx], NULL, destSurface, NULL, D3DTEXF_LINEAR);
	//	assert(SUCCEEDED(hr));
	//
	//	unsigned int refCount = 1;
	//	refCount = destSurface->Release();
	//	assert(refCount == 1);
	//}

	POP_PROFILE_MARKER();
}

void RenderTargetDX9::Bind()
{
	HRESULT hr;

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pColorBuffer[i])->GetTextureDX9();
		hr = dxTex->GetSurfaceLevel(0, &m_pColorSurface[i]);
		assert(SUCCEEDED(hr));
	}

	if (m_bHasDepthStencil)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pDepthBuffer)->GetTextureDX9();
		hr = dxTex->GetSurfaceLevel(0, &m_pDepthSurface);
		assert(SUCCEEDED(hr));
	}
}

void RenderTargetDX9::Unbind()
{
	unsigned int refCount = 0;
	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		if (m_pColorSurface && m_pColorSurface[i])
		{
			refCount = m_pColorSurface[i]->Release();
			m_pColorSurface[i] = nullptr;
		}
		// Inconsistent between the retail and debug DX9 runtimes
		//assert(refCount == 1);
	}
	
	if (m_pDepthSurface)
	{
		refCount = m_pDepthSurface->Release();
		m_pDepthSurface = nullptr;
	}
	// Inconsistent between the retail and debug DX9 runtimes
	//assert(refCount == 1);
}