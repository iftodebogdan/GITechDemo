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

#include "Texture.h"
using namespace LibRendererDll;

const unsigned int Texture::ms_nDimensionCount[TT_MAX] =
{
	1,	// TT_1D
	2,	// TT_2D
	3,	// TT_3D
	2	// TT_CUBE
};

const unsigned int Texture::ms_nPixelSize[PF_MAX] =
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
	0,	// PF_DXT1 (calculated at runtime)
	0,	// PF_DXT3 (calculated at runtime)
	0,	// PF_DXT5 (calculated at runtime)
	4	// PF_D24S8
};

const bool Texture::ms_bIsMipmapable[PF_MAX] =
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
	true,	// PF_DXT1 (calculated at runtime)
	true,	// PF_DXT3 (calculated at runtime)
	true,	// PF_DXT5 (calculated at runtime)
	false	// PF_D24S8
};

Texture::Texture(
	const PixelFormat texFormat, const TexType texType,
	const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
	const unsigned int mipmapLevelCount, const BufferUsage usage)
	: Buffer(0, 0, usage)	// We don't know at this stage how much memory we need
	, m_eTexFormat(texFormat)
	, m_eTexType(texType)
	, m_nMipmapLevelCount(mipmapLevelCount)
	, m_bIsLocked(false)
	, m_nLockedMipmap(-1)
	, m_nLockedCubeFace(-1)
	, m_bAutogenMipmaps(false)
{
	assert(sizeX > 0);
	assert(sizeY > 0);
	assert(sizeZ > 0);

	m_nDimensionCount = GetDimensionCount(texType);
	m_nElementSize = GetPixelSize(texFormat);

	m_nDimension[0][0] = sizeX;

	if (m_nDimensionCount >= 2)
	{
		if (m_eTexType == TT_CUBE)
			m_nDimension[0][1] = sizeX;	// Cube maps have the same size on X and Y axes ... they're cube faces ...
		else
			m_nDimension[0][1] = sizeY;
	}
	else
		m_nDimension[0][1] = 1;	// 1D textures have a height of one texel

	if (m_nDimensionCount == 3)
		m_nDimension[0][2] = sizeZ;
	else
		m_nDimension[0][2] = 1;	// 1D or 2D textures have a depth of one texel

	// Calculate mipmap properties (dimensions, memory used, etc.)
	ComputeMipmapProperties();

	// Allocate memory for our texture
	m_pData = new byte[m_nSize];
}

Texture::~Texture()
{}

void Texture::ComputeMipmapProperties()
{
	if (!IsMipmapable())
	{
		m_nMipmapLevelCount = 1;
	}
	else
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

		if (m_nMipmapLevelCount == 0)
			m_nMipmapLevelCount = maxMipmapLevels;
		assert(m_nMipmapLevelCount <= maxMipmapLevels && m_nMipmapLevelCount > 0);
	}

	// Now we calculate and save the size of each mipmap
	unsigned int totalPixelCount = 0;
	unsigned int sizeX = m_nDimension[0][0];
	unsigned int sizeY = m_nDimension[0][1];
	unsigned int sizeZ = m_nDimension[0][2];
	for (unsigned int level = 0; level < m_nMipmapLevelCount; level++)
	{
		totalPixelCount += sizeX * sizeY * sizeZ;
		m_nDimension[level][0] = sizeX;
		m_nDimension[level][1] = sizeY;
		m_nDimension[level][2] = sizeZ;

		if (sizeX > 1)
			sizeX /= 2;
		if (sizeY > 1)
			sizeY /= 2;
		if (sizeZ > 1)
			sizeZ /= 2;
	}
	// Save the total number of pixels so that we can calculate the space
	// required to store the texture in memory
	m_nElementCount = totalPixelCount * (m_eTexType == TT_CUBE ? 6u : 1u);

	// Calculate the size in memory of each mipmap level
	unsigned int totalByteCount = 0;
	m_nMipmapLevelOffset[0] = 0; // Initialize the first element of the array
	for (unsigned int level = 0; level < m_nMipmapLevelCount; level++)
	{
		m_nMipmapLevelByteCount[level] =
			m_nDimension[level][0] *
			m_nDimension[level][1] *
			m_nDimension[level][2] *
			GetPixelSize();
		if (level < m_nMipmapLevelCount - 1)
			m_nMipmapLevelOffset[level + 1] = m_nMipmapLevelOffset[level] + m_nMipmapLevelByteCount[level];
		totalByteCount += m_nMipmapLevelByteCount[level];
	}
	// Calculate the total memory space required to store the texture
	m_nSize = totalByteCount * (m_eTexType == TT_CUBE ? 6u : 1u);
}

const bool Texture::GenerateMipmaps()
{
	assert(GetTextureType() == TT_2D || GetTextureType() == TT_CUBE);
	assert(GetTextureFormat() == PF_A8R8G8B8 || GetTextureFormat() == PF_A8B8G8R8);

	if (IsLocked())
		return false;

	for (unsigned int face = 0; face < (m_eTexType == TT_CUBE ? 6u : 1u); face++)
	{
		for (unsigned int i = 1; i < GetMipmapLevelCount(); i++)
		{
			byte* srcBuffer;
			byte* destBuffer;
			if (m_eTexType == TT_CUBE)
			{
				Lock(face, i, BL_WRITE_ONLY);
				srcBuffer = GetMipmapLevelData(face, i - 1);
				destBuffer = GetMipmapLevelData(face, i);
			}
			else
			{
				Lock(i, BL_WRITE_ONLY);
				srcBuffer = GetMipmapLevelData(i - 1);
				destBuffer = GetMipmapLevelData(i);
			}
			for (unsigned int y = 0; y < GetHeight(i); y++)
			{
				for (unsigned int x = 0; x < GetWidth(i); x++)
				{
					for (unsigned int bpp = 0; bpp < GetPixelSize(); bpp++)
					{
						*(destBuffer + x * GetPixelSize() + y * GetWidth(i) * GetPixelSize() + bpp) =
							(
							*(srcBuffer + (2 * x)		* GetPixelSize() + (2 * y)		* GetWidth(i - 1) * GetPixelSize() + bpp) +
							*(srcBuffer + (2 * x + 1)	* GetPixelSize() + (2 * y)		* GetWidth(i - 1) * GetPixelSize() + bpp) +
							*(srcBuffer + (2 * x)		* GetPixelSize() + (2 * y + 1)	* GetWidth(i - 1) * GetPixelSize() + bpp) +
							*(srcBuffer + (2 * x + 1)	* GetPixelSize() + (2 * y + 1)	* GetWidth(i - 1) * GetPixelSize() + bpp)
							) / 4;
					}
				}
			}
			Update();
			Unlock();
		}
	}

	return true;
}