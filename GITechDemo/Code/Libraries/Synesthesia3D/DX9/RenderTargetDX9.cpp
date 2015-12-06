/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	RenderTargetDX9.cpp
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "stdafx.h"

#include "RenderTargetDX9.h"
#include "TextureDX9.h"
#include "RendererDX9.h"
#include "MappingsDX9.h"
using namespace Synesthesia3D;

RenderTargetDX9::RenderTargetDX9(const unsigned int targetCount, PixelFormat pixelFormat,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: RenderTarget(targetCount, pixelFormat, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat)
	, m_pColorSurface(nullptr)
	, m_pDepthSurface(nullptr)
	, m_pColorSurfaceBackup(nullptr)
	, m_pDepthSurfaceBackup(nullptr)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

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

	POP_PROFILE_MARKER();
}

RenderTargetDX9::RenderTargetDX9(const unsigned int targetCount, PixelFormat pixelFormat,
	const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: RenderTarget(targetCount, pixelFormat, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat)
	, m_pColorSurface(nullptr)
	, m_pDepthSurface(nullptr)
	, m_pColorSurfaceBackup(nullptr)
	, m_pDepthSurfaceBackup(nullptr)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

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

	POP_PROFILE_MARKER();
}

RenderTargetDX9::RenderTargetDX9(const unsigned int targetCount,
	PixelFormat PixelFormatRT0, PixelFormat PixelFormatRT1, PixelFormat PixelFormatRT2, PixelFormat PixelFormatRT3,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: RenderTarget(targetCount, PixelFormatRT0, PixelFormatRT1, PixelFormatRT2, PixelFormatRT3, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat)
	, m_pColorSurface(nullptr)
	, m_pDepthSurface(nullptr)
	, m_pColorSurfaceBackup(nullptr)
	, m_pDepthSurfaceBackup(nullptr)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

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

	POP_PROFILE_MARKER();
}

RenderTargetDX9::RenderTargetDX9(const unsigned int targetCount,
	PixelFormat PixelFormatRT0, PixelFormat PixelFormatRT1, PixelFormat PixelFormatRT2, PixelFormat PixelFormatRT3,
	const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: RenderTarget(targetCount, PixelFormatRT0, PixelFormatRT1, PixelFormatRT2, PixelFormatRT3, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat)
	, m_pColorSurface(nullptr)
	, m_pDepthSurface(nullptr)
	, m_pColorSurfaceBackup(nullptr)
	, m_pDepthSurfaceBackup(nullptr)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

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

	POP_PROFILE_MARKER();
}

RenderTargetDX9::~RenderTargetDX9()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	Unbind();

	if (m_pColorSurface)
	{
		delete[] m_pColorSurface;
		m_pColorSurface = nullptr;
	}

	POP_PROFILE_MARKER();
}

void RenderTargetDX9::Enable()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(m_pColorSurfaceBackup == nullptr);
	assert(m_pDepthSurfaceBackup == nullptr);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;
	hr = device->GetDepthStencilSurface(&m_pDepthSurfaceBackup);
	assert(SUCCEEDED(hr));

	hr = device->GetRenderTarget(0, &m_pColorSurfaceBackup);
	assert(SUCCEEDED(hr));

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		hr = device->SetRenderTarget((DWORD)i, m_pColorSurface[i]);
		assert(SUCCEEDED(hr));
	}

	// Viewport is automatically set
	hr = device->SetDepthStencilSurface(m_pDepthSurface);
	assert(SUCCEEDED(hr));

	RenderTarget::Enable();

	POP_PROFILE_MARKER();
}

void RenderTargetDX9::Disable()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(m_pColorSurfaceBackup);
	assert(m_pDepthSurfaceBackup);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

	// This call restores the viewport automatically
	hr = device->SetRenderTarget(0, m_pColorSurfaceBackup);
	assert(SUCCEEDED(hr));

	unsigned int refCount = 0;
	if(m_pColorSurfaceBackup)
		refCount = m_pColorSurfaceBackup->Release();
	assert(refCount == 0);
	m_pColorSurfaceBackup = nullptr;

	for (unsigned int i = 1; i < m_nTargetCount; i++)
	{
		hr = device->SetRenderTarget((DWORD)i, 0);
		assert(SUCCEEDED(hr));
	}

	hr = device->SetDepthStencilSurface(m_pDepthSurfaceBackup);
	assert(SUCCEEDED(hr));

	if(m_pDepthSurfaceBackup)
		refCount = m_pDepthSurfaceBackup->Release();
	assert(refCount == 0);
	m_pDepthSurfaceBackup = nullptr;

	RenderTarget::Disable();

	POP_PROFILE_MARKER();
}

void RenderTargetDX9::CopyColorBuffer(const unsigned int colorBufferIdx, Texture* texture)
{
	assert(m_pColorSurfaceBackup == nullptr);
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
		hr = device->GetRenderTarget(0, &m_pColorSurfaceBackup);
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
			(byte*)rect.pBits + i * rect.Pitch,
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
		hr = device->SetRenderTarget(0, m_pColorSurfaceBackup);
		assert(SUCCEEDED(hr));
		if(m_pColorSurfaceBackup)
			m_pColorSurfaceBackup->Release();
		m_pColorSurfaceBackup = nullptr;
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
	PUSH_PROFILE_MARKER(__FUNCSIG__);

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

	POP_PROFILE_MARKER();
}

void RenderTargetDX9::Unbind()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	unsigned int refCount = 0;

	if (m_pColorSurfaceBackup)
	{
		m_pColorSurfaceBackup->Release();
		m_pColorSurfaceBackup = nullptr;
	}

	if (m_pDepthSurfaceBackup)
	{
		m_pDepthSurfaceBackup->Release();
		m_pDepthSurfaceBackup = nullptr;
	}

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

	POP_PROFILE_MARKER();
}