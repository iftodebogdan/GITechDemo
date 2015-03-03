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

#include "RendererDX9.h"
#include "MappingsDX9.h"
#include "TextureDX9.h"
using namespace LibRendererDll;

TextureDX9::TextureDX9(
	const PixelFormat texFormat, const TexType texType,
	const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
	const unsigned int mipmapLevelCount, const BufferUsage usage)
	: Texture(texFormat, texType, sizeX, sizeY, sizeZ, mipmapLevelCount, usage)
	, m_pTexture(nullptr)
	, m_pTempBuffer(nullptr)
	, m_nRowPitch(0)
	, m_nDepthPitch(0)
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();

	D3DPOOL pool;
	if (GetUsage() == BU_TEXTURE)
		pool = D3DPOOL_MANAGED;
	else
		pool = D3DPOOL_DEFAULT;

	HRESULT hr;
	DWORD usageFlags;
	switch (GetTextureType())
	{
	case TT_1D:
		hr = device->CreateTexture(
				GetWidth(), 1u, GetMipmapLevelCount(),
				BufferUsageDX9[m_eBufferUsage], TextureFormatDX9[m_eTexFormat],
				pool, (IDirect3DTexture9**)&m_pTexture, 0);
		break;

	case TT_2D:
		usageFlags = BufferUsageDX9[m_eBufferUsage];
		if (m_eBufferUsage == BU_RENDERTAGET && mipmapLevelCount == 0)
		{
			// automatic mipmap generation for RTs
			usageFlags |= D3DUSAGE_AUTOGENMIPMAP;
			m_bAutogenMipmaps = true;
		}
		hr = device->CreateTexture(
				GetWidth(), GetHeight(), m_bAutogenMipmaps ? 0 : GetMipmapLevelCount(),
				usageFlags, TextureFormatDX9[m_eTexFormat],
				pool, (IDirect3DTexture9**)&m_pTexture, 0);
		break;

	case TT_3D:
		hr = device->CreateVolumeTexture(
				GetWidth(), GetHeight(), GetDepth(), GetMipmapLevelCount(),
				BufferUsageDX9[m_eBufferUsage], TextureFormatDX9[m_eTexFormat],
				pool, (IDirect3DVolumeTexture9**)&m_pTexture, 0);
		break;

	case TT_CUBE:
		hr = device->CreateCubeTexture(
				GetWidth(), GetMipmapLevelCount(),
				BufferUsageDX9[m_eBufferUsage], TextureFormatDX9[m_eTexFormat],
				pool, (IDirect3DCubeTexture9**)&m_pTexture, 0);
	}
	assert(SUCCEEDED(hr));
}

TextureDX9::~TextureDX9()
{
	Unbind();
}

void TextureDX9::Enable(const unsigned int texUnit) const
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();

	HRESULT hr = device->SetTexture(texUnit, m_pTexture);
	assert(SUCCEEDED(hr));
}

void TextureDX9::Disable(const unsigned int texUnit) const
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

#ifdef _DEBUG
	IDirect3DBaseTexture9* activeTex = nullptr;
	hr = device->GetTexture(texUnit, &activeTex);
	assert(SUCCEEDED(hr));
	assert(activeTex == m_pTexture);
	unsigned int refCount = 1;
	refCount = activeTex->Release();
	assert(refCount == 1);
#endif

	hr = device->SetTexture(texUnit, 0);
	assert(SUCCEEDED(hr));
}

const bool TextureDX9::Lock(const unsigned int mipmapLevel, const BufferLocking lockMode)
{
	if (m_bIsLocked || m_eBufferUsage == BU_RENDERTAGET || m_eBufferUsage == BU_DEPTHSTENCIL)
		return false;

	assert(m_pTempBuffer == nullptr);
	D3DLOCKED_RECT rect;
	D3DLOCKED_BOX box;
	HRESULT hr;
	switch (GetTextureType())
	{
	case TT_1D:
	case TT_2D:
		hr = ((IDirect3DTexture9*)m_pTexture)->LockRect(mipmapLevel, &rect, 0, BufferLockingDX9[lockMode]);
		assert(SUCCEEDED(hr));
		m_pTempBuffer = rect.pBits;
		m_nRowPitch = rect.Pitch;
		m_nDepthPitch = 0;
		break;

	case TT_3D:
		hr = ((IDirect3DVolumeTexture9*)m_pTexture)->LockBox(mipmapLevel, &box, 0, BufferLockingDX9[lockMode]);
		assert(SUCCEEDED(hr));
		m_pTempBuffer = box.pBits;
		m_nRowPitch = box.RowPitch;
		m_nDepthPitch = box.SlicePitch;
	}

	return Texture::Lock(mipmapLevel, lockMode);
}

const bool TextureDX9::Lock(const unsigned int cubeFace, const unsigned int mipmapLevel, const BufferLocking lockMode)
{
	if (m_bIsLocked)
		return false;

	assert(m_pTempBuffer == nullptr);
	D3DLOCKED_RECT rect;
	HRESULT hr = ((IDirect3DCubeTexture9*)m_pTexture)->LockRect((D3DCUBEMAP_FACES)cubeFace, mipmapLevel, &rect, 0, BufferLockingDX9[lockMode]);
	assert(SUCCEEDED(hr));
	m_pTempBuffer = rect.pBits;
	m_nRowPitch = rect.Pitch;
	m_nDepthPitch = 0;

	return Texture::Lock(cubeFace, mipmapLevel, lockMode);
}

void TextureDX9::Unlock()
{
	assert(m_pTempBuffer != nullptr);
	HRESULT hr;
	if (m_eTexType == TT_CUBE)
		hr = ((IDirect3DCubeTexture9*)m_pTexture)->UnlockRect((D3DCUBEMAP_FACES)m_nLockedCubeFace, m_nLockedMipmap);
	else
		hr = ((IDirect3DTexture9*)m_pTexture)->UnlockRect((UINT)m_nLockedMipmap);
	assert(SUCCEEDED(hr));
	m_pTempBuffer = nullptr;

	Texture::Unlock();
}

void TextureDX9::Update()
{
	assert(m_pTempBuffer != nullptr);
	//memcpy(m_pTempBuffer, m_pData, m_nMipmapLevelByteCount[m_nLockedMipmap]);
	for (unsigned int j = 0; j < GetDepth(m_nLockedMipmap); j++)
	{
		for (unsigned int i = 0; i < GetHeight(m_nLockedMipmap); i++)
		{
			
			memcpy(
				(byte*)m_pTempBuffer + i * m_nRowPitch + j * m_nDepthPitch,
				(GetTextureType() == TT_CUBE ? GetMipmapLevelData(m_nLockedCubeFace, m_nLockedMipmap) : GetMipmapLevelData(m_nLockedMipmap))
				+ i * GetWidth(m_nLockedMipmap) * GetPixelSize() + j * GetWidth(m_nLockedMipmap) * GetHeight(m_nLockedMipmap) * GetPixelSize(),
				m_nRowPitch
				);
		}
	}
}

void TextureDX9::Bind()
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();

	D3DPOOL pool;
	if (GetUsage() == BU_TEXTURE)
		pool = D3DPOOL_MANAGED;
	else
		pool = D3DPOOL_DEFAULT;

	HRESULT hr;
	DWORD usageFlags;
	switch (GetTextureType())
	{
	case TT_1D:
		hr = device->CreateTexture(
			GetWidth(), 1u, GetMipmapLevelCount(),
			BufferUsageDX9[m_eBufferUsage], TextureFormatDX9[m_eTexFormat],
			pool, (IDirect3DTexture9**)&m_pTexture, 0);
		break;

	case TT_2D:
		usageFlags = BufferUsageDX9[m_eBufferUsage];
		if (m_eBufferUsage == BU_RENDERTAGET && m_bAutogenMipmaps == true)
		{
			// automatic mipmap generation for RTs
			usageFlags |= D3DUSAGE_AUTOGENMIPMAP;
		}
		hr = device->CreateTexture(
			GetWidth(), GetHeight(), m_bAutogenMipmaps ? 0 : GetMipmapLevelCount(),
			usageFlags, TextureFormatDX9[m_eTexFormat],
			pool, (IDirect3DTexture9**)&m_pTexture, 0);
		break;

	case TT_3D:
		hr = device->CreateVolumeTexture(
			GetWidth(), GetHeight(), GetDepth(), GetMipmapLevelCount(),
			BufferUsageDX9[m_eBufferUsage], TextureFormatDX9[m_eTexFormat],
			pool, (IDirect3DVolumeTexture9**)&m_pTexture, 0);
		break;

	case TT_CUBE:
		hr = device->CreateCubeTexture(
			GetWidth(), GetMipmapLevelCount(),
			BufferUsageDX9[m_eBufferUsage], TextureFormatDX9[m_eTexFormat],
			pool, (IDirect3DCubeTexture9**)&m_pTexture, 0);
	}
	assert(SUCCEEDED(hr));

	switch (GetTextureType())
	{
	case TT_1D:
	case TT_2D:
	case TT_3D:
		for (unsigned int mip = 0; mip < GetMipmapLevelCount(); mip++)
		{
			if (Lock(mip, BL_WRITE_ONLY))
			{
				Update();
				Unlock();
			}
			else
				if (m_eBufferUsage != BU_RENDERTAGET && m_eBufferUsage != BU_DEPTHSTENCIL)
					assert(false);
		}
		break;
	case TT_CUBE:
		for (unsigned int face = 0; face < 6; face++)
		{
			for (unsigned int mip = 0; mip < GetMipmapLevelCount(); mip++)
			{
				if (Lock(face, mip, BL_WRITE_ONLY))
				{
					Update();
					Unlock();
				}
				else
					assert(false);
			}
		}
	}
}

void TextureDX9::Unbind()
{
	unsigned int refCount = 0;
	if (m_pTexture)
		refCount = m_pTexture->Release();
	if (m_eBufferUsage == BU_TEXTURE)
		assert(refCount == 0);
	m_pTexture = nullptr;
}