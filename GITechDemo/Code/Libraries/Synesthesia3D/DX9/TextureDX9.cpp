/**
 *	@file		TextureDX9.cpp
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
#include "MappingsDX9.h"
#include "TextureDX9.h"
using namespace Synesthesia3D;

TextureDX9::TextureDX9(
	const PixelFormat pixelFormat, const TextureType texType,
	const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
	const unsigned int mipCount, const BufferUsage usage)
	: Texture(pixelFormat, texType, sizeX, sizeY, sizeZ, mipCount, usage)
	, m_pTexture(nullptr)
	, m_pTempBuffer(nullptr)
	, m_nRowPitch(0)
	, m_nDepthPitch(0)
{
	// Support for deferred initialization (loading from file)
	if (usage != BU_NONE)
	{
		PUSH_PROFILE_MARKER(__FUNCSIG__);
		Bind();
		POP_PROFILE_MARKER();
	}
}

TextureDX9::~TextureDX9()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	Unbind();

	POP_PROFILE_MARKER();
}

void TextureDX9::Enable(const unsigned int texUnit) const
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();

	HRESULT hr = device->SetTexture(texUnit, m_pTexture);
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();
}

void TextureDX9::Disable(const unsigned int texUnit) const
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

#ifdef _DEBUG
	IDirect3DBaseTexture9* activeTex = nullptr;
	hr = device->GetTexture(texUnit, &activeTex);
	assert(SUCCEEDED(hr));
	assert(activeTex == m_pTexture);
	unsigned int refCount = 1;
	refCount = activeTex->Release();
	// Inconsistent between the retail and debug DX9 runtimes
	//assert(refCount == 1/* + IsRenderTarget() ? 1 : 0*/);
#endif

	hr = device->SetTexture(texUnit, 0);
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();
}

const bool TextureDX9::Lock(const unsigned int mipmapLevel, const BufferLocking lockMode)
{
	if (m_eTexType == TT_CUBE)
	{
		assert(false);
		return false;
	}

	if (m_bIsLocked || m_eBufferUsage == BU_RENDERTAGET || m_eBufferUsage == BU_DEPTHSTENCIL)
		return false;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(m_pTempBuffer == nullptr);
	D3DLOCKED_RECT rect;
	D3DLOCKED_BOX box;
	HRESULT hr;
	switch (GetTextureType())
	{
	case TT_1D:
	case TT_2D:
		hr = ((IDirect3DTexture9*)m_pTexture)->LockRect(mipmapLevel, &rect, 0, BufferLockingDX9[lockMode]);
		if (!SUCCEEDED(hr))
		{
			assert(false);
			POP_PROFILE_MARKER();
			return false;
		}
		m_pTempBuffer = rect.pBits;
		m_nRowPitch = rect.Pitch;
		m_nDepthPitch = 0;
		break;

	case TT_3D:
		hr = ((IDirect3DVolumeTexture9*)m_pTexture)->LockBox(mipmapLevel, &box, 0, BufferLockingDX9[lockMode]);
		if (!SUCCEEDED(hr))
		{
			assert(false);
			POP_PROFILE_MARKER();
			return false;
		}
		m_pTempBuffer = box.pBits;
		m_nRowPitch = box.RowPitch;
		m_nDepthPitch = box.SlicePitch;
	}

	POP_PROFILE_MARKER();

	return Texture::Lock(mipmapLevel, lockMode);
}

const bool TextureDX9::Lock(const CubeFace cubeFace, const unsigned int mipmapLevel, const BufferLocking lockMode)
{
	if (m_eTexType != TT_CUBE)
	{
		assert(false);
		return false;
	}

	if (m_bIsLocked)
		return false;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(m_pTempBuffer == nullptr);
	D3DLOCKED_RECT rect;
	HRESULT hr = ((IDirect3DCubeTexture9*)m_pTexture)->LockRect((D3DCUBEMAP_FACES)GetCubeFaceIndex(cubeFace), mipmapLevel, &rect, 0, BufferLockingDX9[lockMode]);
	if (!SUCCEEDED(hr))
	{
		assert(false);
		POP_PROFILE_MARKER();
		return false;
	}
	m_pTempBuffer = rect.pBits;
	m_nRowPitch = rect.Pitch;
	m_nDepthPitch = 0;

	POP_PROFILE_MARKER();

	return Texture::Lock(cubeFace, mipmapLevel, lockMode);
}

void TextureDX9::Unlock()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(m_pTempBuffer != nullptr);
	HRESULT hr;
	if (m_eTexType == TT_CUBE)
		hr = ((IDirect3DCubeTexture9*)m_pTexture)->UnlockRect((D3DCUBEMAP_FACES)GetCubeFaceIndex(m_eLockedCubeFace), m_nLockedMip);
	else
		hr = ((IDirect3DTexture9*)m_pTexture)->UnlockRect((UINT)m_nLockedMip);
	assert(SUCCEEDED(hr));
	m_pTempBuffer = nullptr;

	Texture::Unlock();

	POP_PROFILE_MARKER();
}

void TextureDX9::Update()
{
	assert(m_pTempBuffer != nullptr);

	unsigned int width = GetWidth(m_nLockedMip);
	unsigned int height = GetHeight(m_nLockedMip);
	unsigned int depth = GetDepth(m_nLockedMip);
	if (IsCompressed())
	{
		width /= 4;
		height /= 4;

		if (width < 1)
			width = 1;
		if (height < 1)
			height = 1;
	}

	for (unsigned int j = 0; j < depth; j++)
	{
		for (unsigned int i = 0; i < height; i++)
		{
			memcpy(
				(byte*)m_pTempBuffer + i * m_nRowPitch + j * m_nDepthPitch,
				(GetTextureType() == TT_CUBE ? GetMipData(m_eLockedCubeFace, m_nLockedMip) : GetMipData(m_nLockedMip))
				+ i * width * GetElementSize()
				+ j * width * height * GetElementSize(),
				m_nRowPitch
				);
		}
	}
}

void TextureDX9::Bind()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	Texture::Bind();

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();

	D3DPOOL pool;
	if (GetUsage() == BU_TEXTURE)
		pool = D3DPOOL_MANAGED;
	else
		pool = D3DPOOL_DEFAULT;

	HRESULT hr = E_FAIL;
	DWORD usageFlags;
	unsigned int mipCount;
	switch (GetTextureType())
	{
	case TT_1D:
		hr = device->CreateTexture(
			GetWidth(), 1u, GetMipCount(),
			BufferUsageDX9[m_eBufferUsage], PixelFormatDX9[m_ePixelFormat],
			pool, (IDirect3DTexture9**)&m_pTexture, 0);
		break;

	case TT_2D:
		usageFlags = BufferUsageDX9[m_eBufferUsage];
		mipCount = GetMipCount();
		if (IsRenderTarget() && mipCount != 1)
		{
			// automatic mipmap generation for RTs
			usageFlags |= D3DUSAGE_AUTOGENMIPMAP;
			mipCount = 0;
		}
		hr = device->CreateTexture(
			GetWidth(), GetHeight(), mipCount,
			usageFlags, PixelFormatDX9[m_ePixelFormat],
			pool, (IDirect3DTexture9**)&m_pTexture, 0);
		break;

	case TT_3D:
		hr = device->CreateVolumeTexture(
			GetWidth(), GetHeight(), GetDepth(), GetMipCount(),
			BufferUsageDX9[m_eBufferUsage], PixelFormatDX9[m_ePixelFormat],
			pool, (IDirect3DVolumeTexture9**)&m_pTexture, 0);
		break;

	case TT_CUBE:
		hr = device->CreateCubeTexture(
			GetWidth(), GetMipCount(),
			BufferUsageDX9[m_eBufferUsage], PixelFormatDX9[m_ePixelFormat],
			pool, (IDirect3DCubeTexture9**)&m_pTexture, 0);
	}
	assert(SUCCEEDED(hr));

	switch (GetTextureType())
	{
	case TT_1D:
	case TT_2D:
	case TT_3D:
		for (unsigned int mip = 0; mip < GetMipCount(); mip++)
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
		for (CubeFace face = FACE_XNEG; face < FACE_MAX; face = (CubeFace)(face + 1))
		{
			for (unsigned int mip = 0; mip < GetMipCount(); mip++)
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

	POP_PROFILE_MARKER();
}

void TextureDX9::Unbind()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	unsigned int refCount = 0;
	if (m_pTexture)
		refCount = m_pTexture->Release();
	if (m_eBufferUsage == BU_TEXTURE)
		assert(refCount == 0);
	m_pTexture = nullptr;

	POP_PROFILE_MARKER();
}