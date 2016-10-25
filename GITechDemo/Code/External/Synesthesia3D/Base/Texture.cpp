/**
 *	@file		Texture.cpp
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

#include "Texture.h"
#include "Renderer.h"
#include "Profiler.h"
#include "../Utility/ColorUtility.h"
using namespace Synesthesia3D;

#define DXT_PSEUDO_BPP (4u)

const unsigned int g_nDimensionCount[TT_MAX] =
{
	1,	// TT_1D
	2,	// TT_2D
	3,	// TT_3D
	2	// TT_CUBE
};

const bool g_bMipmapable[PF_MAX] =
{
	false,	// PF_NONE
	true,	// PF_R5G6B5
	true,	// PF_A1R5G5B5
	true,	// PF_A4R4G4B4
	true,	// PF_A8
	true,	// PF_L8
	true,	// PF_A8L8
	true,	// PF_R8G8B8
	true,	// PF_A8R8G8B8
	true,	// PF_A8B8G8R8
	true,	// PF_L16
	true,	// PF_G16R16
	true,	// PF_A16B16G16R16
	true,	// PF_R16F
	true,	// PF_G16R16F
	true,	// PF_A16B16G16R16F
	false,	// PF_R32F
	false,	// PF_G32R32F
	false,	// PF_A32B32G32R32F,
	true,	// PF_DXT1
	true,	// PF_DXT3
	true,	// PF_DXT5
	false,	// PF_D24S8
	false	// PF_INTZ
};

const unsigned int g_nBytesPerPixel[PF_MAX] =
{
	0,	// PF_NONE
	2,	// PF_R5G6B5
	2,	// PF_A1R5G5B5
	2,	// PF_A4R4G4B4
	1,	// PF_A8
	1,	// PF_L8
	2,	// PF_A8L8
	3,	// PF_R8G8B8
	4,	// PF_A8R8G8B8
	4,	// PF_A8B8G8R8
	2,	// PF_L16
	4,	// PF_G16R16
	8,	// PF_A16B16G16R16
	2,	// PF_R16F
	4,	// PF_G16R16F
	8,	// PF_A16B16G16R16F
	4,	// PF_R32F
	8,	// PF_G32R32F
	16,	// PF_A32B32G32R32F,
	8,	// PF_DXT1	// block size, instead of pixel
	16,	// PF_DXT3	// block size, instead of pixel
	16,	// PF_DXT5	// block size, instead of pixel
	4,	// PF_D24S8
	4	// PF_INTZ
};

enum ComponentOrder
{
	CO_NONE,	// N/A
	CO_ARGB,	// Most common
	CO_ABGR		// Relatively obscure
};

const ComponentOrder g_eComponentOrder[PF_MAX] =
{
	CO_NONE,	// PF_NONE
	CO_ARGB,	// PF_R5G6B5
	CO_ARGB,	// PF_A1R5G5B5
	CO_ARGB,	// PF_A4R4G4B4
	CO_NONE,	// PF_A8
	CO_NONE,	// PF_L8
	CO_ARGB,	// PF_A8L8
	CO_ARGB,	// PF_R8G8B8
	CO_ARGB,	// PF_A8R8G8B8
	CO_ABGR,	// PF_A8B8G8R8
	CO_NONE,	// PF_L16
	CO_ABGR,	// PF_G16R16
	CO_ABGR,	// PF_A16B16G16R16
	CO_NONE,	// PF_R16F
	CO_ABGR,	// PF_G16R16F
	CO_ABGR,	// PF_A16B16G16R16F
	CO_NONE,	// PF_R32F
	CO_ABGR,	// PF_G32R32F
	CO_ABGR,	// PF_A32B32G32R32F,
	CO_NONE,	// PF_DXT1
	CO_NONE,	// PF_DXT3
	CO_NONE,	// PF_DXT5
	CO_NONE,	// PF_D24S8
	CO_NONE		// PF_INTZ
};

const unsigned int g_nComponentCount[PF_MAX] =
{
	0,	// PF_NONE
	3,	// PF_R5G6B5
	4,	// PF_A1R5G5B5
	4,	// PF_A4R4G4B4
	1,	// PF_A8
	1,	// PF_L8
	2,	// PF_A8L8
	3,	// PF_R8G8B8
	4,	// PF_A8R8G8B8
	4,	// PF_A8B8G8R8
	1,	// PF_L16
	2,	// PF_G16R16
	4,	// PF_A16B16G16R16
	1,	// PF_R16F
	2,	// PF_G16R16F
	4,	// PF_A16B16G16R16F
	1,	// PF_R32F
	2,	// PF_G32R32F
	4,	// PF_A32B32G32R32F,
	4,	// PF_DXT1	// block size, instead of pixel
	4,	// PF_DXT3	// block size, instead of pixel
	4,	// PF_DXT5	// block size, instead of pixel
	2,	// PF_D24S8
	2	// PF_INTZ
};

const bool g_bFloatingPoint[PF_MAX] =
{
	false,	// PF_NONE
	false,	// PF_R5G6B5
	false,	// PF_A1R5G5B5
	false,	// PF_A4R4G4B4
	false,	// PF_A8
	false,	// PF_L8
	false,	// PF_A8L8
	false,	// PF_R8G8B8
	false,	// PF_A8R8G8B8
	false,	// PF_A8B8G8R8
	false,	// PF_L16
	false,	// PF_G16R16
	false,	// PF_A16B16G16R16
	true,	// PF_R16F
	true,	// PF_G16R16F
	true,	// PF_A16B16G16R16F
	true,	// PF_R32F
	true,	// PF_G32R32F
	true,	// PF_A32B32G32R32F,
	false,	// PF_DXT1
	false,	// PF_DXT3
	false,	// PF_DXT5
	false,	// PF_D24S8
	false	// PF_INTZ
};

const bool g_bDepthFormat[PF_MAX] =
{
	false,	// PF_NONE
	false,	// PF_R5G6B5
	false,	// PF_A1R5G5B5
	false,	// PF_A4R4G4B4
	false,	// PF_A8
	false,	// PF_L8
	false,	// PF_A8L8
	false,	// PF_R8G8B8
	false,	// PF_A8R8G8B8
	false,	// PF_A8B8G8R8
	false,	// PF_L16
	false,	// PF_G16R16
	false,	// PF_A16B16G16R16
	false,	// PF_R16F
	false,	// PF_G16R16F
	false,	// PF_A16B16G16R16F
	false,	// PF_R32F
	false,	// PF_G32R32F
	false,	// PF_A32B32G32R32F,
	false,	// PF_DXT1
	false,	// PF_DXT3
	false,	// PF_DXT5
	true,	// PF_D24S8
	true	// PF_INTZ
};

const bool g_bHasStencil[PF_MAX] =
{
	false,	// PF_NONE
	false,	// PF_R5G6B5
	false,	// PF_A1R5G5B5
	false,	// PF_A4R4G4B4
	false,	// PF_A8
	false,	// PF_L8
	false,	// PF_A8L8
	false,	// PF_R8G8B8
	false,	// PF_A8R8G8B8
	false,	// PF_A8B8G8R8
	false,	// PF_L16
	false,	// PF_G16R16
	false,	// PF_A16B16G16R16
	false,	// PF_R16F
	false,	// PF_G16R16F
	false,	// PF_A16B16G16R16F
	false,	// PF_R32F
	false,	// PF_G32R32F
	false,	// PF_A32B32G32R32F,
	false,	// PF_DXT1
	false,	// PF_DXT3
	false,	// PF_DXT5
	true,	// PF_D24S8
	true	// PF_INTZ
};

const bool g_bCompressed[PF_MAX] =
{
	false,	// PF_NONE
	false,	// PF_R5G6B5
	false,	// PF_A1R5G5B5
	false,	// PF_A4R4G4B4
	false,	// PF_A8
	false,	// PF_L8
	false,	// PF_A8L8
	false,	// PF_R8G8B8
	false,	// PF_A8R8G8B8
	false,	// PF_A8B8G8R8
	false,	// PF_L16
	false,	// PF_G16R16
	false,	// PF_A16B16G16R16
	false,	// PF_R16F
	false,	// PF_G16R16F
	false,	// PF_A16B16G16R16F
	false,	// PF_R32F
	false,	// PF_G32R32F
	false,	// PF_A32B32G32R32F,
	true,	// PF_DXT1
	true,	// PF_DXT3
	true,	// PF_DXT5
	false,	// PF_D24S8
	false	// PF_INTZ
};

Texture::Texture(
	const PixelFormat pixelFormat, const TextureType texType,
	const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
	const unsigned int mipCount, const BufferUsage usage)
	: Buffer(0, 0, usage)	// We don't know at this stage how much memory we need
	, m_ePixelFormat(pixelFormat)
	, m_eTexType(texType)
	, m_nMipCount(mipCount)
	, m_bIsLocked(false)
	, m_nLockedMip(~0u)
	, m_eLockedCubeFace(FACE_NONE)
	, m_bIsDynamicRT(false)
	, m_fWidthRatio(1.f)
	, m_fHeightRatio(1.f)
{
	m_tSamplerStates.nAnisotropy = 1u;
	m_tSamplerStates.fLodBias = 0.f;
	m_tSamplerStates.eFilter = SF_MIN_MAG_POINT_MIP_NONE;
	m_tSamplerStates.vBorderColor = Vec4f(0.f, 0.f, 0.f, 0.f);
	for (unsigned int dim = 0; dim < 3; dim++)
		m_tSamplerStates.eAddressingMode[dim] = SAM_WRAP;
	m_tSamplerStates.bSRGBEnabled = false;

	// Support for deferred initialization (loading from file)
	if (usage == BU_NONE)
		return;

	assert(sizeX > 0 || usage == BU_RENDERTAGET || usage == BU_DEPTHSTENCIL);	// Render targets may have dynamic resolution
	assert(sizeY > 0 || usage == BU_RENDERTAGET || usage == BU_DEPTHSTENCIL);	// (i.e. sync'ed with the resolution of the backbuffer)
	assert(sizeZ > 0);

	// Only 2D and cube textures can be compressed
	assert(!IsCompressed() || (m_eTexType != TT_1D && m_eTexType != TT_3D));

	// Render targets must not use compressed formats
	assert(!IsRenderTarget() || !IsCompressed());

	// Depth-stencil textures must be 2D
	assert(!IsDepthStencil() || m_eTexType == TT_2D);

	// No mipmaps for 32bit formats or depth-stencil formats
	assert(IsMipmapable() || GetMipCount() == 1);

	// Validate and correct (if necessary and possible) the pixel format
	ValidatePixelFormat(m_ePixelFormat, texType, usage);

	// Calculate mipmap properties (dimensions, memory used, etc.)
	ComputeTextureProperties(Vec3i(sizeX, sizeY, sizeZ));

	// Allocate memory for our texture
	m_pData = new s3dByte[m_nSize];
}

Texture::~Texture()
{}

void Texture::ComputeTextureProperties(const Vec3i dimensions)
{
	if ((dimensions[0] <= 0 || dimensions[1] <= 0) && (IsRenderTarget() || IsDepthStencil()))
		m_bIsDynamicRT = true;

	unsigned int width	= !m_bIsDynamicRT ? dimensions[0] : (unsigned int)((float)Renderer::GetInstance()->GetDisplayResolution()[0] * m_fWidthRatio);
	unsigned int height	= !m_bIsDynamicRT ? dimensions[1] : (unsigned int)((float)Renderer::GetInstance()->GetDisplayResolution()[1] * m_fHeightRatio);
	unsigned int depth	= !m_bIsDynamicRT ? dimensions[2] : 1u;
	if (IsCompressed())
	{
		// Calculate the smallest number divisible by 4 which is >= width/height
		// This is for block compression size restrictions
		width = width + (4 - width % 4) % 4;
		height = height + (4 - height % 4) % 4;
	}

	m_nDimensionCount = GetDimensionCount(m_eTexType);
	m_nElementSize = GetBytesPerPixel(m_ePixelFormat);

	m_nDimension[0][0] = width;

	if (m_nDimensionCount >= 2)
	{
		if (m_eTexType == TT_CUBE)
			m_nDimension[0][1] = width;	// Cube maps have the same size on X and Y axes ... they're cube faces ...
		else
			m_nDimension[0][1] = height;
	}
	else
		m_nDimension[0][1] = 1;	// 1D textures have a height of one texel

	if (m_nDimensionCount == 3)
		m_nDimension[0][2] = depth;
	else
		m_nDimension[0][2] = 1;	// 1D or 2D textures have a depth of one texel

	if (IsMipmapable() && m_nMipCount != 1)
	{
		// _BitScanReverse() is an intrinsic function which is specific to MSVC.
		// It searches from the most significant bit to the least significant bit
		// and returns the position of the first set bit (1). This value coincides
		// with the integer approximation of logarithm to the base 2 of the value
		// we feed to _BitScanReverse(), which in turn coincides with the maximum
		// number of mipmaps we can have for that texture size - 1 (because math!).
		// It's fast, but most likely not cross-platform so when the time comes,
		// a replacement will have to be found.
		unsigned int maxMipmapLevelsX, maxMipmapLevelsY, maxMipmapLevelsZ;
		_BitScanReverse((unsigned long*)&maxMipmapLevelsX, m_nDimension[0][0]);
		_BitScanReverse((unsigned long*)&maxMipmapLevelsY, m_nDimension[0][1]);
		_BitScanReverse((unsigned long*)&maxMipmapLevelsZ, m_nDimension[0][2]);
		unsigned int maxMipmapLevels = (unsigned int)Math::Max(maxMipmapLevelsX, maxMipmapLevelsY, maxMipmapLevelsX) + 1;

		if (m_nMipCount == 0 || m_nMipCount > maxMipmapLevels || IsRenderTarget())
			m_nMipCount = maxMipmapLevels;
		assert(m_nMipCount <= maxMipmapLevels && m_nMipCount > 0);
	}
	else
	{
		m_nMipCount = 1;
	}

	// Now we calculate and store the size of each mipmap
	unsigned int sizeX = m_nDimension[0][0];
	unsigned int sizeY = m_nDimension[0][1];
	unsigned int sizeZ = m_nDimension[0][2];
	m_nMipOffset[0] = 0; // Initialize the first element of the array

	// Reset the size
	m_nSize = 0;

	if (IsCompressed())
	{
		for (unsigned int level = 0; level < m_nMipCount; level++)
		{
			// When dealing with compressed formats, we consider a compressed block
			// to be the smallest addressable element instead of a pixel,
			// which will be reflected in the element count, pixel/block size, etc.
			unsigned int blocksX = sizeX / 4;
			if (blocksX < 1)
				blocksX = 1;

			unsigned int blocksY = sizeY / 4;
			if (blocksY < 1)
				blocksY = 1;

			// For compressed formats, GetBytesPerPixel() actually returns the size
			// in bytes for a compressed block (8 for DXT1 and 16 for DXT3/5)
			m_nMipSizeBytes[level] = GetElementSize() * blocksX * blocksY;
			m_nElementCount += blocksX * blocksY;
			m_nSize += m_nMipSizeBytes[level];

			if (level < m_nMipCount - 1)
				m_nMipOffset[level + 1] = m_nMipOffset[level] + m_nMipSizeBytes[level];

			m_nDimension[level][0] = sizeX;
			m_nDimension[level][1] = sizeY;
			m_nDimension[level][2] = sizeZ;

			if (sizeX > 1)
				sizeX >>= 1;
			if (sizeY > 1)
				sizeY >>= 1;
		}
	}
	else
	{
		for (unsigned int level = 0; level < m_nMipCount; level++)
		{
			m_nMipSizeBytes[level] = GetElementSize() * sizeX * sizeY * sizeZ;
			m_nElementCount += sizeX * sizeY * sizeZ;
			m_nSize += m_nMipSizeBytes[level];

			if (level < m_nMipCount - 1)
				m_nMipOffset[level + 1] = m_nMipOffset[level] + m_nMipSizeBytes[level];

			m_nDimension[level][0] = sizeX;
			m_nDimension[level][1] = sizeY;
			m_nDimension[level][2] = sizeZ;

			if (sizeX > 1)
				sizeX >>= 1;
			if (sizeY > 1)
				sizeY >>= 1;
			if (sizeZ > 1)
				sizeZ >>= 1;
		}
	}

	m_nElementCount *= (m_eTexType == TT_CUBE ? FACE_MAX : 1u);
	m_nSize *= (m_eTexType == TT_CUBE ? FACE_MAX : 1u);
}

void Texture::SetDynamicSizeRatios(const float widthRatio, const float heightRatio)
{
	if (widthRatio != m_fWidthRatio || heightRatio != m_fHeightRatio)
	{
		m_fWidthRatio = widthRatio;
		m_fHeightRatio = heightRatio;

		Unbind();

		ComputeTextureProperties(Vec3i(0, 0, 1));

		// Reallocate memory for our texture
		delete[] m_pData;
		m_pData = new s3dByte[m_nSize];

		Bind();
	}
}

s3dByte* const Texture::GetMipData(const unsigned int mipmapLevel)
{
	if (m_eTexType == TT_CUBE)
	{
		assert(false);
		return nullptr;
	}
	
	// Allocate the memory if required, since we don't allocate memory
	// when we create render targets so as to reduce their memory footprint.
	if (m_eBufferUsage == BU_RENDERTAGET && m_pData == nullptr)
	{
		m_pData = new s3dByte[m_nSize];
		assert(m_pData != nullptr);
		memset(m_pData, 0, m_nSize);
	}

	return m_pData + GetMipOffset(mipmapLevel);
}

s3dByte* const Texture::GetMipData(const CubeFace cubeFace, const unsigned int mipmapLevel) const
{
	if (m_eTexType != TT_CUBE)
	{
		assert(false);
		return nullptr;
	}
	
	assert(cubeFace >= FACE_XNEG && cubeFace < FACE_MAX);
	
	return m_pData + GetCubeFaceIndex(cubeFace) * GetCubeFaceOffset() + GetMipOffset(mipmapLevel);
}

const bool Texture::GenerateMips()
{
	// Just to be safe (might be useful later when porting on other platforms)
	assert(sizeof(Vec4f) == sizeof(float) * 4);

	if (!IsMipmapable())
		return false;

	const unsigned int numTexels = GetWidth() * GetHeight() * GetDepth();
	Vec4f* rgba = new Vec4f[numTexels];

	if (!numTexels)
		return false;

	unsigned int faceCount = GetTextureType() == TT_CUBE ? FACE_MAX : 1;

	for (CubeFace face = FACE_XNEG; face < FACE_MAX; face = (CubeFace)(face + 1))
	{
		for (unsigned int mip = 1; mip < GetMipCount(); mip++)
		{
			s3dByte* texelsSrc = faceCount == 1 ? GetMipData(mip - 1) : GetMipData(face, mip - 1);
			s3dByte* texelsDst = faceCount == 1 ? GetMipData(mip) : GetMipData(face, mip);

			unsigned int widthSrc = GetWidth(mip - 1);
			unsigned int heightSrc = GetHeight(mip - 1);
			unsigned int depthSrc = GetDepth(mip - 1);

			unsigned int widthDst = GetWidth(mip);
			unsigned int heightDst = GetHeight(mip);
			unsigned int depthDst = GetDepth(mip);

			unsigned int areaSrc = widthSrc * heightSrc;
			unsigned int areaDst = widthDst * heightDst;

			assert(widthDst < widthSrc || heightDst < heightSrc || depthDst < depthSrc);

			if(IsCompressed())
				ColorUtility::ConvertFrom[GetPixelFormat()](texelsSrc, rgba, Math::Max(widthSrc, 4u), Math::Max(heightSrc, 4u), 1);
			else
				ColorUtility::ConvertFrom[GetPixelFormat()](texelsSrc, rgba, widthSrc, heightSrc, depthSrc);

			for (unsigned int z = 0; z < depthDst; z++)
			{
				for (unsigned int y = 0; y < heightDst; y++)
				{
					for (unsigned int x = 0; x < widthDst; x++)
					{
						const unsigned int uvw = x + y * widthDst + z * areaDst;
						const unsigned int base = 2 * (x + y * widthSrc + z * areaSrc);
						for (unsigned int c = 0; c < 4; c++)
						{
							if (depthSrc > 1)
							{
								rgba[uvw][c] =
									0.125f *
									(
									rgba[base][c] +							// (   x	,	y	,   z	)
									rgba[base + 1][c] +						// ( x + 1	,	y	,   z	)
									rgba[base + widthSrc][c] +				// (   x	, y + 1	,   z	)
									rgba[base + widthSrc + 1][c] +			// ( x + 1	, y + 1	,   z	)
									rgba[base + areaSrc][c] +				// (   x	,   y	, z + 1	)
									rgba[base + 1 + areaSrc][c] +			// ( x + 1	,	y	, z + 1	)
									rgba[base + widthSrc + areaSrc][c] +	// (   x	, y + 1	, z + 1	)
									rgba[base + widthSrc + 1 + areaSrc][c]	// ( x + 1	, y + 1	, z + 1	)
									);
							}
							else if (heightSrc > 1)
							{
								rgba[uvw][c] =
									0.25f *
									(
									rgba[base][c] +					// (   x	,	  y	  )
									rgba[base + 1][c] +				// ( x + 1	,	  y   )
									rgba[base + widthSrc][c] +		// (   x	,	y + 1 )
									rgba[base + widthSrc + 1][c]	// ( x + 1	,	y + 1 )
									);
							}
							else if (widthSrc > 1 && base < numTexels - 1)
							{
								rgba[uvw][c] =
									0.5f *
									(
									rgba[base][c] +		// (   x   )
									rgba[base + 1][c]	// ( x + 1 )
									);
							}
						}
					}
				}
			}

			if(IsCompressed())
				ColorUtility::ConvertTo[GetPixelFormat()](rgba, texelsDst, Math::Max(widthDst, 4u), Math::Max(heightDst, 4u), 1);
			else
				ColorUtility::ConvertTo[GetPixelFormat()](rgba, texelsDst, widthDst, heightDst, depthDst);
		}
	}

	delete[] rgba;

	return true;
}

void Texture::Bind()
{
	if (m_bIsDynamicRT)
	{
		ComputeTextureProperties(Vec3i(0, 0, 1));

		// Reallocate memory for our texture
		delete[] m_pData;
		m_pData = new s3dByte[m_nSize];
	}
}

const PixelFormat Texture::GetPixelFormat() const
{
	return m_ePixelFormat;
}

const TextureType Texture::GetTextureType() const
{
	return m_eTexType;
}

const unsigned int Texture::GetMipCount() const
{
	return m_nMipCount;
}

const unsigned int Texture::GetWidth(const unsigned int mipmapLevel) const
{
	return m_nDimension[mipmapLevel][0];
}

const unsigned int Texture::GetHeight(const unsigned int mipmapLevel) const
{
	return m_nDimension[mipmapLevel][1];
}

const unsigned int Texture::GetDepth(const unsigned int mipmapLevel) const
{
	return m_nDimension[mipmapLevel][2];
}

const unsigned int Texture::GetDimensionCount() const
{
	return m_nDimensionCount;
}

const unsigned int Texture::GetMipSizeBytes(const unsigned int mipmapLevel) const
{
	assert(mipmapLevel < TEX_MAX_MIPMAP_LEVELS);
	return m_nMipSizeBytes[mipmapLevel];
}

const unsigned int Texture::GetMipOffset(const unsigned int mipmapLevel) const
{
	assert(mipmapLevel < TEX_MAX_MIPMAP_LEVELS);
	return m_nMipOffset[mipmapLevel];
}

const unsigned int Texture::GetCubeFaceOffset() const
{
	assert(m_eTexType == TT_CUBE);
	return m_nSize / FACE_MAX;
}

const bool Texture::IsCompressed() const
{
	return g_bCompressed[m_ePixelFormat];
}

const bool Texture::IsFloatingPoint() const
{
	return g_bFloatingPoint[m_ePixelFormat];
}

const bool Texture::IsDepthStencil() const
{
	return m_eBufferUsage == BU_DEPTHSTENCIL;
}

const bool Texture::IsRenderTarget() const
{
	return m_eBufferUsage == BU_RENDERTAGET;
}

const bool Texture::IsMipmapable() const
{
	return g_bMipmapable[m_ePixelFormat];
}

const bool Texture::Lock(const unsigned int mipmapLevel, const BufferLocking /*lockMode*/)
{
	assert(!m_bIsLocked);
	m_bIsLocked = true;
	m_nLockedMip = mipmapLevel;
	m_eLockedCubeFace = FACE_XNEG;
	return true;
}

const bool Texture::Lock(const CubeFace cubeFace, const unsigned int mipmapLevel, const BufferLocking /*lockMode*/)
{
	assert(!m_bIsLocked);
	m_bIsLocked = true;
	m_nLockedMip = mipmapLevel;
	m_eLockedCubeFace = cubeFace;
	return true;
}

void Texture::Unlock()
{
	assert(m_bIsLocked);
	m_bIsLocked = false;
	m_nLockedMip = ~0u;
	m_eLockedCubeFace = FACE_NONE;
}

const bool Texture::IsLocked() const
{
	return m_bIsLocked;
}

const unsigned int Texture::GetLockedMip() const
{
	assert(m_bIsLocked);
	return m_nLockedMip;
}

const CubeFace Texture::GetLockedCubeFace() const
{
	assert(m_bIsLocked);
	return m_eLockedCubeFace;
}

void Texture::SetAnisotropy(const unsigned int anisotropy)
{
	m_tSamplerStates.nAnisotropy = Math::clamp(anisotropy, 1u, (unsigned int)MAX_ANISOTROPY);
}

void Texture::SetMipLodBias(const float lodBias)
{
	m_tSamplerStates.fLodBias = lodBias;
}

void Texture::SetFilter(const SamplerFilter filter)
{
	m_tSamplerStates.eFilter = filter;
}

void Texture::SetBorderColor(const Vec4f rgba)
{
	m_tSamplerStates.vBorderColor = rgba;
}

void Texture::SetAddressingModeU(const SamplerAddressingMode samU)
{
	m_tSamplerStates.eAddressingMode[0] = samU;
}

void Texture::SetAddressingModeV(const SamplerAddressingMode samV)
{
	m_tSamplerStates.eAddressingMode[1] = samV;
}

void Texture::SetAddressingModeW(const SamplerAddressingMode samW)
{
	m_tSamplerStates.eAddressingMode[2] = samW;
}

void Texture::SetAddressingMode(const SamplerAddressingMode samUVW)
{
	SetAddressingModeU(samUVW);
	SetAddressingModeV(samUVW);
	SetAddressingModeW(samUVW);
}

void Texture::SetSRGBEnabled(const bool enabled)
{
	m_tSamplerStates.bSRGBEnabled = enabled;
}

const unsigned int Texture::GetAnisotropy() const
{
	return m_tSamplerStates.nAnisotropy;
}

const float Texture::GetMipLodBias() const
{
	return m_tSamplerStates.fLodBias;
}

const SamplerFilter Texture::GetFilter() const
{
	return m_tSamplerStates.eFilter;
}

const Vec4f Texture::GetBorderColor() const
{
	return m_tSamplerStates.vBorderColor;
}

const SamplerAddressingMode Texture::GetAddressingModeU() const
{
	return m_tSamplerStates.eAddressingMode[0];
}

const SamplerAddressingMode Texture::GetAddressingModeV() const
{
	return m_tSamplerStates.eAddressingMode[1];
}

const SamplerAddressingMode Texture::GetAddressingModeW() const
{
	return m_tSamplerStates.eAddressingMode[2];
}

const SamplerAddressingMode Texture::GetAddressingMode() const
{
	return
		(GetAddressingModeU() == GetAddressingModeV() &&
		GetAddressingModeV() == GetAddressingModeW()) ?
		GetAddressingModeU() : SAM_NONE;
}

const bool Texture::GetSRGBEnabled() const
{
	return m_tSamplerStates.bSRGBEnabled;
}

const char* Texture::GetSourceFileName() const
{
	return m_szSourceFile.c_str();
}

const unsigned int Texture::GetDimensionCount(const TextureType texType)
{
	return g_nDimensionCount[texType];
}

const bool Texture::IsMipmapable(const PixelFormat pixelFormat)
{
	return g_bMipmapable[pixelFormat];
}

const unsigned int Texture::GetBytesPerPixel(const PixelFormat pixelFormat)
{
	return g_nBytesPerPixel[pixelFormat];
}

void Texture::ValidatePixelFormat(PixelFormat& pixelFormat, const TextureType texType, const BufferUsage usage)
{
	if (Renderer::GetAPI() == API_NULL)
		return;

	const std::vector<const DeviceCaps::SupportedPixelFormat>& arrValidPF = Renderer::GetInstance()->GetDeviceCaps().arrSupportedPixelFormats;

	// Check if we have a valid pixel format
	for (unsigned int idx = 0; idx < arrValidPF.size(); idx++)
	{
		if (arrValidPF[idx].ePixelFormat == pixelFormat &&
			arrValidPF[idx].eResourceUsage == usage &&
			arrValidPF[idx].eTextureType == texType)
		{
			return;	// Pixel format is valid
		}
	}

	// Temporarily store intermediate results here
	PixelFormat suggestedPixelFormat = PF_NONE;

	// Do we have a color or depth format?
	if (g_bDepthFormat[pixelFormat])
	{
		// Depth format

		// Try to (not) keep stencil if original has(n't) one (do two loops if we can't satisfy this condition)
		for (int stencilPref = 1; stencilPref >= 0 && suggestedPixelFormat == PF_NONE; stencilPref--)
		{
			const bool bMaintainStencil = (stencilPref != 0);

			// Search for other formats with at least the original bpp (bits per pixel).
			// Then, if still not found, search for other formats with as high of a bpp as possible.
			for (int bppPref = 1; bppPref >= 0 && suggestedPixelFormat == PF_NONE; bppPref--)
			{
				const bool bMaintainBpp = (bppPref != 0);

				for (unsigned int pf = 0; pf < arrValidPF.size(); pf++)
				{
					if (arrValidPF[pf].eResourceUsage == usage &&	// Must be same BufferUsage
						arrValidPF[pf].eTextureType == texType &&	// Must be same TextureType
						g_bDepthFormat[arrValidPF[pf].ePixelFormat] &&	// Must be a depth/stencil format
						(bMaintainStencil ? (g_bHasStencil[arrValidPF[pf].ePixelFormat] == g_bHasStencil[pixelFormat]) : true) &&	// Try to keep stencil if original had it
						(g_nBytesPerPixel[arrValidPF[pf].ePixelFormat] >= g_nBytesPerPixel[pixelFormat]) == bMaintainBpp &&	// At least/most same bpp
						((g_nBytesPerPixel[arrValidPF[pf].ePixelFormat] < g_nBytesPerPixel[suggestedPixelFormat]) == bMaintainBpp || suggestedPixelFormat == PF_NONE))	// Lowest/highest bpp possible
					{
						suggestedPixelFormat = arrValidPF[pf].ePixelFormat;	// Found one!
					}
				}
			}
		}
	}
	else // if (g_bDepthFormat[pixelFormat])
	{
		// Color format

		// Try to keep original component count
		for (int compCountPref = 1; compCountPref >= 0 && suggestedPixelFormat == PF_NONE; compCountPref--)
		{
			const bool bMustMaintainComponentCount = (compCountPref != 0);

			// Try to keep the original ordering/swizzling (e.g. ARGB vs ABGR)
			for (int swizzlePref = 1; swizzlePref >= 0 && suggestedPixelFormat == PF_NONE; swizzlePref--)
			{
				const bool bMustMaintatinSwizzle = (swizzlePref != 0);

				// Try to keep the original data type (float/integer)
				for (int dataPref = 1; dataPref >= 0 && suggestedPixelFormat == PF_NONE; dataPref--)
				{
					const bool bMustMaintainDataType = (dataPref != 0);

					// Search for other formats with at least the original bpp
					// Then, if not found, search for other formats with as high of a bpp as possible
					for (int bppPref = 1; bppPref >= 0 && suggestedPixelFormat == PF_NONE; bppPref--)
					{
						const bool bMaintainBpp = (bppPref != 0);

						for (unsigned int pf = 0; pf < arrValidPF.size(); pf++)
						{
							const unsigned int testBpp = g_bCompressed[arrValidPF[pf].ePixelFormat] ? DXT_PSEUDO_BPP : g_nBytesPerPixel[arrValidPF[pf].ePixelFormat];
							const unsigned int originalBpp = g_bCompressed[pixelFormat] ? DXT_PSEUDO_BPP : g_nBytesPerPixel[pixelFormat];
							const unsigned int suggestedBpp = g_bCompressed[suggestedPixelFormat] ? DXT_PSEUDO_BPP : g_nBytesPerPixel[suggestedPixelFormat];

							if (arrValidPF[pf].eResourceUsage == usage &&	// Must be same BufferUsage
								arrValidPF[pf].eTextureType == texType &&	// Must be same TextureType
								!g_bDepthFormat[arrValidPF[pf].ePixelFormat] &&	// Must be a color format
								(bMustMaintainComponentCount ? (g_nComponentCount[arrValidPF[pf].ePixelFormat] == g_nComponentCount[pixelFormat]) : true) &&	// Try to keep component count
								(bMustMaintatinSwizzle ? (g_eComponentOrder[arrValidPF[pf].ePixelFormat] == g_eComponentOrder[pixelFormat]) : true) &&	// Try to keep swizzling
								(bMustMaintainDataType ? (g_bFloatingPoint[arrValidPF[pf].ePixelFormat] == g_bFloatingPoint[pixelFormat]) : true) &&	// Try to keep data type
								(testBpp >= originalBpp) == bMaintainBpp &&	// At least/most same bpp
								((testBpp < suggestedBpp) == bMaintainBpp || suggestedPixelFormat == PF_NONE))	// Lowest/highest bpp possible
							{
								suggestedPixelFormat = arrValidPF[pf].ePixelFormat;	// Found one!
							}
						}
					}
				}
			}
		}
	}

	pixelFormat = suggestedPixelFormat;
}
