//////////////////////////////////////////////////////////////////////////
// This file is part of the "LibRenderer" 3D graphics library           //
//                                                                      //
// Copyright (C) 2014 - Iftode Bogdan-Marius <iftode.bogdan@gmail.com>  //
//                                                                      //
// This program is free software: you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program. If not, see <http://www.gnu.org/licenses/>. //
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "RenderTargetDX9.h"
#include "TextureDX9.h"
#include "RendererDX9.h"
#include "MappingsDX9.h"
using namespace LibRendererDll;

RenderTargetDX9::RenderTargetDX9(const unsigned int targetCount, PixelFormat pixelFormat,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil)
	: RenderTarget(targetCount, pixelFormat, width, height, hasMipmaps, hasDepthStencil)
	, m_pColorSurface(nullptr)
	, m_pDepthSurface(nullptr)
	, m_pColorSurfaceBackup(nullptr)
	, m_pDepthSurfaceBackup(nullptr)
{
	HRESULT hr;

	m_pColorSurface = new IDirect3DSurface9*[m_nTargetCount];

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		//m_pColorBuffer[i] = new TextureDX9(pixelFormat, TT_2D, width, height, 1,
		//	m_bHasMipmaps ? 0 : 1, BU_RENDERTAGET);
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pColorBuffer[i])->GetTextureDX9();
		hr = ((IDirect3DTexture9*)(((TextureDX9*)(m_pColorBuffer[i]))->GetTextureDX9()))->GetSurfaceLevel(0, &m_pColorSurface[i]);
		assert(SUCCEEDED(hr));
	}

	if (hasDepthStencil)
	{
		//m_pDepthBuffer = new TextureDX9(PF_D24S8, TT_2D, width, height, 1, 1, BU_DEPTHSTENCIL);
		hr = ((IDirect3DTexture9*)(((TextureDX9*)m_pDepthBuffer)->GetTextureDX9()))->GetSurfaceLevel(0, &m_pDepthSurface);
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
	assert(m_pColorSurfaceBackup == nullptr);
	assert(m_pDepthSurfaceBackup == nullptr);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

	hr = device->GetRenderTarget(0, &m_pColorSurfaceBackup);
	assert(SUCCEEDED(hr));

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		((TextureDX9*)(m_pColorBuffer[i]))->GetTextureDX9();
		hr = device->SetRenderTarget((DWORD)i, m_pColorSurface[i]);
		assert(SUCCEEDED(hr));
	}

	hr = device->GetDepthStencilSurface(&m_pDepthSurfaceBackup);
	assert(SUCCEEDED(hr));

	// Viewport is automatically set
	hr = device->SetDepthStencilSurface(m_pDepthSurface);
	assert(SUCCEEDED(hr));
}

void RenderTargetDX9::Disable()
{
	assert(m_pColorSurfaceBackup);
	assert(m_pDepthSurfaceBackup);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

	// This call restores the viewport automatically
	hr = device->SetRenderTarget(0, m_pColorSurfaceBackup);
	assert(SUCCEEDED(hr));

	unsigned int refCount = 0;
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

	refCount = m_pDepthSurfaceBackup->Release();
	assert(refCount == 0);
	m_pDepthSurfaceBackup = nullptr;
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
		texture->GetTextureFormat() != GetFormat() ||
		texture->GetTextureType() != TT_2D)
	{
		assert(false);
		return;
	}

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
	hr = device->CreateTexture(GetWidth(), GetHeight(), 0, 0, TextureFormatDX9[GetFormat()], D3DPOOL_SYSTEMMEM, &tempTex, NULL);
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
	//memcpy(texture->GetMipmapLevelData(0), rect.pBits, texture->GetMipmapLevelByteCount(0));
	for (unsigned int i = 0; i < texture->GetHeight(); i++)
	{
		memcpy(
			texture->GetMipmapLevelData(0) + i * texture->GetWidth() * texture->GetPixelSize(),
			(byte*)rect.pBits + i * rect.Pitch,
			texture->GetWidth() * texture->GetPixelSize()
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
		m_pColorSurfaceBackup->Release();
		m_pColorSurfaceBackup = nullptr;
	}

	//for (unsigned int i = 0, n = texture->GetMipmapLevelCount(); i < n; i++)
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
}

void RenderTargetDX9::Bind()
{
	HRESULT hr;

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		IDirect3DTexture9* dxTex = (IDirect3DTexture9*)((TextureDX9*)m_pColorBuffer[i])->GetTextureDX9();
		hr = ((IDirect3DTexture9*)(((TextureDX9*)(m_pColorBuffer[i]))->GetTextureDX9()))->GetSurfaceLevel(0, &m_pColorSurface[i]);
		assert(SUCCEEDED(hr));
	}

	if (m_bHasDepthStencil)
	{
		hr = ((IDirect3DTexture9*)(((TextureDX9*)m_pDepthBuffer)->GetTextureDX9()))->GetSurfaceLevel(0, &m_pDepthSurface);
		assert(SUCCEEDED(hr));
	}
}

void RenderTargetDX9::Unbind()
{
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
}