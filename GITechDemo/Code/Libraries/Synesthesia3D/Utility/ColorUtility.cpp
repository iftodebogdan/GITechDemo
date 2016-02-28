/**
 *	@file		ColorUtility.cpp
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

#include "gmtl/gmtl.h"
#include "gmtl/Numerics/Eigen.h"

#include "ColorUtility.h"
using namespace Synesthesia3D;

ColorUtility::ConvertFromFunc ColorUtility::ConvertFrom[PF_MAX] =
{

	0,							// PF_NONE
	&ConvertFromR5G6B5,			// PF_R5G6B5
	&ConvertFromA1R5G5B5,		// PF_A1R5G5B5
	&ConvertFromA4R4G4B4,		// PF_A4R4G4B4
	&ConvertFromA8,				// PF_A8
	&ConvertFromL8,				// PF_L8
	&ConvertFromA8L8,			// PF_A8L8
	&ConvertFromR8G8B8,			// PF_R8G8B8
	&ConvertFromA8R8G8B8,		// PF_A8R8G8B8
	&ConvertFromA8B8G8R8,		// PF_A8B8G8R8
	&ConvertFromL16,			// PF_L16
	&ConvertFromG16R16,			// PF_G16R16
	&ConvertFromA16B16G16R16,	// PF_A16B16G16R16
	&ConvertFromR16F,			// PF_R16F
	&ConvertFromG16R16F,		// PF_G16R16F
	&ConvertFromA16B16G16R16F,	// PF_A16B16G16R16F
	&ConvertFromR32F,			// PF_R32F
	&ConvertFromG32R32F,		// PF_G32R32F
	&ConvertFromA32B32G32R32F,	// PF_A32B32G32R32F
	&ConvertFromDXT1,			// PF_DXT1
	&ConvertFromDXT3,			// PF_DXT3
	&ConvertFromDXT5,			// PF_DXT5
	0,							// PF_D24S8
	0							// PF_INTZ
};

ColorUtility::ConvertToFunc ColorUtility::ConvertTo[PF_MAX] =
{

	0,							// PF_NONE
	&ConvertToR5G6B5,			// PF_R5G6B5
	&ConvertToA1R5G5B5,			// PF_A1R5G5B5
	&ConvertToA4R4G4B4,			// PF_A4R4G4B4
	&ConvertToA8,				// PF_A8
	&ConvertToL8,				// PF_L8
	&ConvertToA8L8,				// PF_A8L8
	&ConvertToR8G8B8,			// PF_R8G8B8
	&ConvertToA8R8G8B8,			// PF_A8R8G8B8
	&ConvertToA8B8G8R8,			// PF_A8B8G8R8
	&ConvertToL16,				// PF_L16
	&ConvertToG16R16,			// PF_G16R16
	&ConvertToA16B16G16R16,		// PF_A16B16G16R16
	&ConvertToR16F,				// PF_R16F
	&ConvertToG16R16F,			// PF_G16R16F
	&ConvertToA16B16G16R16F,	// PF_A16B16G16R16F
	&ConvertToR32F,				// PF_R32F
	&ConvertToG32R32F,			// PF_G32R32F
	&ConvertToA32B32G32R32F,	// PF_A32B32G32R32F
	&ConvertToDXT1,				// PF_DXT1
	&ConvertToDXT3,				// PF_DXT3
	&ConvertToDXT5,				// PF_DXT5
	0,							// PF_D24S8
	0							// PF_INTZ
};

dword ColorUtility::MakeR8G8B8(const byte red, const byte green, const byte blue)
{
	return (red | (green << 8) | (blue << 16) | (255 << 24));
}

dword ColorUtility::MakeR8G8B8A8(const byte red, const byte green, const byte blue, const byte alpha)
{
	return (red | (green << 8) | (blue << 16) | (alpha << 24));
}

void ColorUtility::ExtractR8G8B8(const dword rgb, byte & red, byte & green, byte & blue)
{
	red		= (byte)((rgb & 0x000000FF));
	green	= (byte)((rgb & 0x0000FF00) >> 8);
	blue	= (byte)((rgb & 0x00FF0000) >> 16);
}

void ColorUtility::ExtractR8G8B8A8(const dword rgba, byte & red, byte & green, byte & blue, byte & alpha)
{
	red		= (byte)((rgba & 0x000000FF));
	green	= (byte)((rgba & 0x0000FF00) >> 8);
	blue	= (byte)((rgba & 0x00FF0000) >> 16);
	alpha	= (byte)((rgba & 0xFF000000) >> 24);
}

dword ColorUtility::MakeB8G8R8(const byte red, const byte green, const byte blue)
{
	return (blue | (green << 8) | (red << 16) | (255 << 24));
}

dword ColorUtility::MakeB8G8R8A8(const byte red, const byte green, const byte blue, const byte alpha)
{
	return (blue | (green << 8) | (red << 16) | (alpha << 24));
}

void ColorUtility::ExtractB8G8R8(const dword rgb, byte & red, byte & green, byte & blue)
{
	blue = (byte)((rgb & 0x000000FF));
	green = (byte)((rgb & 0x0000FF00) >> 8);
	red = (byte)((rgb & 0x00FF0000) >> 16);
}

void ColorUtility::ExtractB8G8R8A8(const dword rgba, byte & red, byte & green, byte & blue, byte & alpha)
{
	blue= (byte)(rgba & 0x000000FF);
	green = (byte)((rgba & 0x0000FF00) >> 8);
	red = (byte)((rgba & 0x00FF0000) >> 16);
	alpha = (byte)((rgba & 0xFF000000) >> 24);
}

word ColorUtility::MakeR5G6B5(const byte red, const byte green, const byte blue)
{
	word r = ((word)(red & 0x1F)) >> 3;
	word g = ((word)(green & 0x3F)) >> 2;
	word b = ((word)(blue & 0x1F)) >> 3;
	return (b | (g << 5) | (r << 11));
}

void ColorUtility::ExtractR5G6B5(const word rgb, byte & red, byte & green, byte & blue)
{
	blue = (byte)(((rgb & 0x001F) << 3) | ((rgb & 0x001F) >> 2));
	green = (byte)(((rgb & 0x07E0) >> 3) | ((rgb & 0x07E0) >> 9));
	red = (byte)(((rgb & 0xF800) >> 8) | ((rgb & 0xF800) >> 13));
}

void ColorUtility::ConvertFromR5G6B5(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const word* src = (const word*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++src, ++texel)
	{
		(*texel)[2] = (float)(((*src & 0x001F) << 3) | ((*src & 0x001F) >> 2)) / 31.f;
		(*texel)[1] = (float)(((*src & 0x07E0) >> 3) | ((*src & 0x07E0) >> 9)) / 63.f;
		(*texel)[0] = (float)(((*src & 0xF800) >> 8) | ((*src & 0xF800) >> 11 >> 2)) / 31.f;
		(*texel)[3] = 1.f;
	}
}

void ColorUtility::ConvertFromA1R5G5B5(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const word* src = (const word*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++src, ++texel)
	{
		(*texel)[2] = (float)(((*src & 0x001F) << 3) | ((*src & 0x001F) >> 2)) / 31.f;
		(*texel)[1] = (float)(((*src & 0x03E0) >> 2) | ((*src & 0x07E0) >> 7)) / 31.f;
		(*texel)[0] = (float)(((*src & 0x7C00) >> 7) | ((*src & 0xF800) >> 12)) / 31.f;
		(*texel)[3] = (float)((*src & 0x8000) >> 15);
	}
}

void ColorUtility::ConvertFromA4R4G4B4(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const word* src = (const word*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++src, ++texel)
	{
		(*texel)[2] = (float)(((*src & 0x000F) << 4) | (*src & 0x000F)) / 15.f;
		(*texel)[1] = (float)((*src & 0x00F0) | ((*src & 0x00F0) >> 4)) / 15.f;
		(*texel)[0] = (float)(((*src & 0x0F00) >> 4) | ((*src & 0x0F00) >> 8)) / 15.f;
		(*texel)[3] = (float)(((*src & 0xF000) >> 8) | ((*src & 0xF000) >> 12)) / 15.f;
	}
}

void ColorUtility::ConvertFromA8(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const byte* src = (const byte*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[0] = 1.0f;
		(*texel)[1] = 1.0f;
		(*texel)[2] = 1.0f;
		(*texel)[3] = (float)(*src++) / 255.f;
	}
}

void ColorUtility::ConvertFromL8(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const byte* src = (const byte*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		float luminance = (float)(*src++);
		(*texel)[0] = luminance / 255.f;
		(*texel)[1] = luminance / 255.f;
		(*texel)[2] = luminance / 255.f;
		(*texel)[3] = 1.0f;
	}
}

void ColorUtility::ConvertFromA8L8(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const byte* src = (const byte*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		float luminance = (float)(*src++);
		float alpha = (float)(*src++);
		(*texel)[0] = luminance / 255.f;
		(*texel)[1] = luminance / 255.f;
		(*texel)[2] = luminance / 255.f;
		(*texel)[3] = alpha / 255.f;
	}
}

void ColorUtility::ConvertFromR8G8B8(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const byte* src = (const byte*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[2] = (float)(*src++) / 255.f;
		(*texel)[1] = (float)(*src++) / 255.f;
		(*texel)[0] = (float)(*src++) / 255.f;
		(*texel)[3] = 1.0f;
	}
}

void ColorUtility::ConvertFromA8R8G8B8(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const byte* src = (const byte*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[2] = (float)(*src++) / 255.f;
		(*texel)[1] = (float)(*src++) / 255.f;
		(*texel)[0] = (float)(*src++) / 255.f;
		(*texel)[3] = (float)(*src++) / 255.f;
	}
}

void ColorUtility::ConvertFromA8B8G8R8(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const byte* src = (const byte*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[0] = (float)(*src++) / 255.f;
		(*texel)[1] = (float)(*src++) / 255.f;
		(*texel)[2] = (float)(*src++) / 255.f;
		(*texel)[3] = (float)(*src++) / 255.f;
	}
}

void ColorUtility::ConvertFromL16(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const word* src = (const word*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		float luminance = (float)(*src++);
		(*texel)[0] = luminance / 65535.0f;
		(*texel)[1] = luminance / 65535.0f;
		(*texel)[2] = luminance / 65535.0f;
		(*texel)[3] = 1.0f;
	}
}

void ColorUtility::ConvertFromG16R16(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const word* src = (const word*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[0] = (float)(*src++) / 65535.0f;
		(*texel)[1] = (float)(*src++) / 65535.0f;
		(*texel)[2] = 0.0f;
		(*texel)[3] = 1.0f;
	}
}

void ColorUtility::ConvertFromA16B16G16R16(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const word* src = (const word*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[0] = (float)(*src++) / 65535.0f;
		(*texel)[1] = (float)(*src++) / 65535.0f;
		(*texel)[2] = (float)(*src++) / 65535.0f;
		(*texel)[3] = (float)(*src++) / 65535.0f;
	}
}

void ColorUtility::ConvertFromR16F(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const half* src = (const half*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[0] = *src++;
		(*texel)[1] = 0.0f;
		(*texel)[2] = 0.0f;
		(*texel)[3] = 1.0f;
	}
}

void ColorUtility::ConvertFromG16R16F(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const half* src = (const half*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[0] = *src++;
		(*texel)[1] = *src++;
		(*texel)[2] = 0.0f;
		(*texel)[3] = 1.0f;
	}
}

void ColorUtility::ConvertFromA16B16G16R16F(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const half* src = (const half*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[0] = *src++;
		(*texel)[1] = *src++;
		(*texel)[2] = *src++;
		(*texel)[3] = *src++;
	}
}

void ColorUtility::ConvertFromR32F(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const float* src = (const float*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[0] = *src++;
		(*texel)[1] = 0.0f;
		(*texel)[2] = 0.0f;
		(*texel)[3] = 1.0f;
	}
}

void ColorUtility::ConvertFromG32R32F(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const float* src = (const float*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[0] = *src++;
		(*texel)[1] = *src++;
		(*texel)[2] = 0.0f;
		(*texel)[3] = 1.0f;
	}
}

void ColorUtility::ConvertFromA32B32G32R32F(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const float* src = (const float*)inData;
	Vec4f* texel = outRGBA;
	for (unsigned int i = 0; i < numTexels; ++i, ++texel)
	{
		(*texel)[0] = *src++;
		(*texel)[1] = *src++;
		(*texel)[2] = *src++;
		(*texel)[3] = *src++;
	}
}

void ColorUtility::ConvertFromDXT1(const byte * const inData, Vec4f * const outRGBA, const unsigned int width, const unsigned int height, const unsigned int /*depth*/)
{
	assert(width % 4 == 0 && height % 4 == 0);

	unsigned int numBlocksX = width / 4;
	unsigned int numBlocksY = height / 4;
	const qword* blockData = (const qword*)inData;
	for (unsigned blockY = 0; blockY < numBlocksY; blockY++)
		for (unsigned blockX = 0; blockX < numBlocksX; blockX++)
		{
			unsigned int block = blockX + blockY * numBlocksX;
			word color0 = *((word*)(blockData + block));
			word color1 = *(((word*)(blockData + block)) + 1);

			byte r0, g0, b0, r1, g1, b1;
			ExtractR5G6B5(color0, r0, g0, b0);
			ExtractR5G6B5(color1, r1, g1, b1);

			if (color0 > color1)
			{
				// Four-color block: derive the other two colors.    
				// 00 = color_0, 01 = color_1, 10 = color_2, 11 = color_3
				// These 2-bit codes correspond to the 2-bit fields 
				// stored in the 64-bit block.
				byte r2, g2, b2, r3, g3, b3;
				r2 = (2 * r0 + r1) / 3;
				g2 = (2 * g0 + g1) / 3;
				b2 = (2 * b0 + b1) / 3;
				r3 = (r0 + 2 * r1) / 3;
				g3 = (g0 + 2 * g1) / 3;
				b3 = (b0 + 2 * b1) / 3;

				byte* bitmap = (byte*)(((word*)(blockData + block)) + 2);
				for (unsigned int texelY = 0; texelY < 4; texelY++)
					for (unsigned int texelX = 0; texelX < 4; texelX++)
					{
						byte encodedColor = 0;
						switch (texelX)
						{
						case 0:
							encodedColor = bitmap[texelY] & 0x3;
							break;
						case 1:
							encodedColor = (bitmap[texelY] & 0xC) >> 2;
							break;
						case 2:
							encodedColor = (bitmap[texelY] & 0x30) >> 4;
							break;
						case 3:
							encodedColor = (bitmap[texelY] & 0xC0) >> 6;
						}

						unsigned int idx = 4 * (blockX + blockY * width) + texelX + texelY * width;
						switch (encodedColor)
						{
						case 0:
							outRGBA[idx][0] = r0 / 255.f;
							outRGBA[idx][1] = g0 / 255.f;
							outRGBA[idx][2] = b0 / 255.f;
							outRGBA[idx][3] = 1.f;
							break;
						case 1:
							outRGBA[idx][0] = r1 / 255.f;
							outRGBA[idx][1] = g1 / 255.f;
							outRGBA[idx][2] = b1 / 255.f;
							outRGBA[idx][3] = 1.f;
							break;
						case 2:
							outRGBA[idx][0] = r2 / 255.f;
							outRGBA[idx][1] = g2 / 255.f;
							outRGBA[idx][2] = b2 / 255.f;
							outRGBA[idx][3] = 1.f;
							break;
						case 3:
							outRGBA[idx][0] = r3 / 255.f;
							outRGBA[idx][1] = g3 / 255.f;
							outRGBA[idx][2] = b3 / 255.f;
							outRGBA[idx][3] = 1.f;
						}
					}
			}
			else
			{
				// Three-color block: derive the other color.
				// 00 = color_0,  01 = color_1,  10 = color_2,  
				// 11 = transparent.
				// These 2-bit codes correspond to the 2-bit fields 
				// stored in the 64-bit block.
				byte r2, g2, b2;
				r2 = (r0 + r1) / 2;
				g2 = (g0 + g1) / 2;
				b2 = (b0 + b1) / 2;

				byte* bitmap = (byte*)(((word*)(blockData + block)) + 2);
				for (unsigned int texelY = 0; texelY < 4; texelY++)
					for (unsigned int texelX = 0; texelX < 4; texelX++)
					{
						byte encodedColor = 0;
						switch (texelX)
						{
						case 0:
							encodedColor = bitmap[texelY] & 0x3;
							break;
						case 1:
							encodedColor = (bitmap[texelY] & 0xC) >> 2;
							break;
						case 2:
							encodedColor = (bitmap[texelY] & 0x30) >> 4;
							break;
						case 3:
							encodedColor = (bitmap[texelY] & 0xC0) >> 6;
						}

						unsigned int idx = 4 * (blockX + blockY * width) + texelX + texelY * width;
						switch (encodedColor)
						{
						case 0:
							outRGBA[idx][0] = r0 / 255.f;
							outRGBA[idx][1] = g0 / 255.f;
							outRGBA[idx][2] = b0 / 255.f;
							outRGBA[idx][3] = 1.f;
							break;
						case 1:
							outRGBA[idx][0] = r1 / 255.f;
							outRGBA[idx][1] = g1 / 255.f;
							outRGBA[idx][2] = b1 / 255.f;
							outRGBA[idx][3] = 1.f;
							break;
						case 2:
							outRGBA[idx][0] = r2 / 255.f;
							outRGBA[idx][1] = g2 / 255.f;
							outRGBA[idx][2] = b2 / 255.f;
							outRGBA[idx][3] = 1.f;
							break;
						case 3:
							outRGBA[idx][0] = 0.f;
							outRGBA[idx][1] = 0.f;
							outRGBA[idx][2] = 0.f;
							outRGBA[idx][3] = 0.f;
						}
					}
			}
		}
}

void ColorUtility::ConvertFromDXT3(const byte * const /*inData*/, Vec4f * const /*outRGBA*/, const unsigned int /*width*/, const unsigned int /*height*/, const unsigned int /*depth*/)
{
	assert(false); // TODO
}

void ColorUtility::ConvertFromDXT5(const byte * const /*inData*/, Vec4f * const /*outRGBA*/, const unsigned int /*width*/, const unsigned int /*height*/, const unsigned int /*depth*/)
{
	assert(false); // TODO
}

void ColorUtility::ConvertToR5G6B5(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	word* texel = (word*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		word r = (word)((*src)[0] * 31.f) >> 3;
		word g = (word)((*src)[1] * 63.f) >> 2;
		word b = (word)((*src)[2] * 31.f) >> 3;
		*texel++ = b | (g << 5) | (r << 11);
	}
}

void ColorUtility::ConvertToA1R5G5B5(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	word* texel = (word*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		word r = (word)((*src)[0] * 31.f);
		word g = (word)((*src)[1] * 31.f);
		word b = (word)((*src)[2] * 31.f);
		word a = (word)(*src)[3];
		*texel++ = b | (g << 5) | (r << 10) | (a << 15);
	}
}

void ColorUtility::ConvertToA4R4G4B4(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	word* texel = (word*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		word r = (word)((*src)[0] * 15.f);
		word g = (word)((*src)[1] * 15.f);
		word b = (word)((*src)[2] * 15.f);
		word a = (word)((*src)[3] * 15.f);
		*texel++ = b | (g << 4) | (r << 8) | (a << 12);
	}
}

void ColorUtility::ConvertToA8(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	byte* texel = (byte*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (byte)((*src)[3] * 255.f);
	}
}

void ColorUtility::ConvertToL8(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	byte* texel = (byte*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (byte)((*src)[0] * 255.f);
	}
}

void ColorUtility::ConvertToA8L8(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	byte* texel = (byte*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (byte)((*src)[0] * 255.f);
		*texel++ = (byte)((*src)[3] * 255.f);
	}
}

void ColorUtility::ConvertToR8G8B8(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	byte* texel = (byte*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (byte)((*src)[2] * 255.f);
		*texel++ = (byte)((*src)[1] * 255.f);
		*texel++ = (byte)((*src)[0] * 255.f);
	}
}

void ColorUtility::ConvertToA8R8G8B8(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	byte* texel = (byte*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (byte)((*src)[2] * 255.f);
		*texel++ = (byte)((*src)[1] * 255.f);
		*texel++ = (byte)((*src)[0] * 255.f);
		*texel++ = (byte)((*src)[3] * 255.f);
	}
}

void ColorUtility::ConvertToA8B8G8R8(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	byte* texel = (byte*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (byte)((*src)[0] * 255.f);
		*texel++ = (byte)((*src)[1] * 255.f);
		*texel++ = (byte)((*src)[2] * 255.f);
		*texel++ = (byte)((*src)[3] * 255.f);
	}
}

void ColorUtility::ConvertToL16(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	word* texel = (word*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (word)((*src)[0] * 65535.f);
	}
}

void ColorUtility::ConvertToG16R16(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	word* texel = (word*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (word)((*src)[0] * 65535.f);
		*texel++ = (word)((*src)[1] * 65535.f);
	}
}

void ColorUtility::ConvertToA16B16G16R16(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	word* texel = (word*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (word)((*src)[0] * 65535.f);
		*texel++ = (word)((*src)[1] * 65535.f);
		*texel++ = (word)((*src)[2] * 65535.f);
		*texel++ = (word)((*src)[3] * 65535.f);
	}
}

void ColorUtility::ConvertToR16F(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	half* texel = (half*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (*src)[0];
	}
}

void ColorUtility::ConvertToG16R16F(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	half* texel = (half*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (*src)[0];
		*texel++ = (*src)[1];
	}
}

void ColorUtility::ConvertToA16B16G16R16F(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	half* texel = (half*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (*src)[0];
		*texel++ = (*src)[1];
		*texel++ = (*src)[2];
		*texel++ = (*src)[3];
	}
}

void ColorUtility::ConvertToR32F(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	float* texel = (float*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (*src)[0];
	}
}

void ColorUtility::ConvertToG32R32F(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	float* texel = (float*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (*src)[0];
		*texel++ = (*src)[1];
	}
}

void ColorUtility::ConvertToA32B32G32R32F(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int depth)
{
	const unsigned int numTexels = width * height * depth;
	const Vec4f* src = inRGBA;
	float* texel = (float*)outData;
	for (unsigned int i = 0; i < numTexels; ++i, ++src)
	{
		*texel++ = (*src)[0];
		*texel++ = (*src)[1];
		*texel++ = (*src)[2];
		*texel++ = (*src)[3];
	}
}

void ColorUtility::ConvertToDXT1(const Vec4f * const inRGBA, byte * const outData, const unsigned int width, const unsigned int height, const unsigned int /*depth*/)
{
	// Algorithm inspired from http://developer.download.nvidia.com/compute/cuda/1.1-Beta/x86_website/projects/dxtc/doc/cuda_dxtc.pdf

	assert(width % 4 == 0 && height % 4 == 0);

	unsigned int numBlocksX = width / 4;
	unsigned int numBlocksY = height / 4;

	for (unsigned int blockY = 0; blockY < numBlocksY; blockY++)
		for (unsigned int blockX = 0; blockX < numBlocksX; blockX++)
		{
			bool oneBitAlpha = false;

			float** covMat;
			covMat = new float*[3];
			for (int i = 0; i < 3; i++)
				covMat[i] = new float[3];

			// This is the top-left corner of the 4x4 block we want to compress
			const Vec4f* const topLeft = &inRGBA[4 * (blockX + blockY * height)];

			// Build our covariance matrix
			for (unsigned int i = 0; i < 3; i++)
				for (unsigned int j = 0; j < 3; j++)
				{
					covMat[i][j] = 0.f;
					for (unsigned int y = 0; y < 4; y++)
						for (unsigned int x = 0; x < 4; x++)
						{
							const unsigned int idx = x + y * height;
							covMat[i][j] +=	(topLeft[idx][i]) *
											(topLeft[idx][j]);
						}
					//covMat[i][j] /= 15.f;
				}

			// Compute the eigenvectors of the covariance matrix (we only need the largest one)
			// NB: this could be optimized, since we only require the dominant eigenvector which
			// can be computed directly using the Power Method
			Eigen eigen(3);
			eigen.SetMatrix(covMat);
			eigen.DecrSortEigenStuff3();
			Vec3f eigenVec(eigen.GetEigenvector(0, 0), eigen.GetEigenvector(1, 0), eigen.GetEigenvector(2, 0));
			//normalize(eigenVec);

			for (int i = 0; i < 3; i++)
				delete[] covMat[i];
			delete[] covMat;

			float colorProj[4][4];
			float minColorProj = FLT_MAX;
			float maxColorProj = -FLT_MAX;

			// Project the color of each pixel of the block onto the eigenvector
			for (unsigned int y = 0; y < 4; y++)
				for (unsigned int x = 0; x < 4; x++)
				{
					const unsigned int idx = x + y * height;
					colorProj[x][y] = abs(dot(eigenVec, Vec3f(topLeft[idx][0], topLeft[idx][1], topLeft[idx][2])));
					if (colorProj[x][y] < minColorProj)
						minColorProj = colorProj[x][y];
					if (colorProj[x][y] > maxColorProj)
						maxColorProj = colorProj[x][y];
					if (topLeft[x + y * height][3] < 127.5f)
						oneBitAlpha = true;
				}

			// calculate the two colors
			Vec3f rgb0 = eigenVec * maxColorProj;
			Vec3f rgb1 = eigenVec * minColorProj;

			// saturate (we might overflow slightly, i.e. >1.0f)
			rgb0[0] = Math::clamp(abs(rgb0[0]), 0.f, 1.f);
			rgb0[1] = Math::clamp(abs(rgb0[1]), 0.f, 1.f);
			rgb0[2] = Math::clamp(abs(rgb0[2]), 0.f, 1.f);
			rgb1[0] = Math::clamp(abs(rgb1[0]), 0.f, 1.f);
			rgb1[1] = Math::clamp(abs(rgb1[1]), 0.f, 1.f);
			rgb1[2] = Math::clamp(abs(rgb1[2]), 0.f, 1.f);

			// R8G8B8A8 -> R5G6B6
			word color0 =
				((((byte)(rgb0[0] * 255.f)) >> 3) << 11) |
				((((byte)(rgb0[1] * 255.f)) >> 2) << 5) |
				((((byte)(rgb0[2] * 255.f)) >> 3));
			word color1 =
				((((byte)(rgb1[0] * 255.f)) >> 3) << 11) |
				((((byte)(rgb1[1] * 255.f)) >> 2) << 5) |
				((((byte)(rgb1[2] * 255.f)) >> 3));

			// 4 pixels / byte (one row of the block), 4 columns => 4 bytes
			byte bitmap[4];
			memset(&bitmap, 0, sizeof(bitmap));

			// if the colours are identical, it's faster
			// to go through the 1-bit alpha encoding path
			if (color0 == color1)
				oneBitAlpha = true;

			if (!oneBitAlpha)
			{
				assert(color0 > color1);

				// colors
				float c0 = maxColorProj;
				float c1 = minColorProj;
				float c2 = (2.f * maxColorProj + minColorProj) * 0.3333333f;
				float c3 = (maxColorProj + 2.f * minColorProj) * 0.3333333f;

				// boundaries
				float c0_c2 = (c0 + c2) * 0.5f;
				float c2_c3 = (c2 + c3) * 0.5f;
				float c1_c3 = (c1 + c3) * 0.5f;

				// select between the 4 colors
				for (unsigned int y = 0; y < 4; y++)
					for (unsigned int x = 0; x < 4; x++)
					{
						//if(colorProj[x][y] >= c0_c2) // closer to c0
						//	bitmap[y] |= (0x0 << (x * 2));
						//else
						if(colorProj[x][y] < c0_c2 && colorProj[x][y] >= c2_c3) // closer to c2
							bitmap[y] |= (0x2 << (x * 2));
						else if(colorProj[x][y] < c2_c3 && colorProj[x][y] >= c1_c3) // closer to c3
							bitmap[y] |= (0x3 << (x * 2));
						else if (colorProj[x][y] < c1_c3)
							bitmap[y] |= (0x1 << (x * 2)); // closer to c1
					}
			}
			else
			{
				if (color0 > color1)
				{
					word tmp = color0;
					color0 = color1;
					color1 = tmp;
				}
				assert(color0 <= color1);

				// colors
				float c0 = minColorProj;
				float c1 = maxColorProj;
				float c2 = (minColorProj + maxColorProj) * 0.5f;

				// boundaries
				float c0_c2 = (2 * c0 + c2) * 0.3333333f;
				float c2_c1 = (2 * c1 + c2) * 0.3333333f;

				// select between the 3 colors + transparency
				for (unsigned int y = 0; y < 4; y++)
					for (unsigned int x = 0; x < 4; x++)
					{
						if (topLeft[x + y * height][3] < 127.5f) // transparent
						{
							bitmap[y] |= (0x3 << (x * 2));
							continue;
						}

						//if (colorProj[x][y] >= c0 && colorProj[x][y] < c0_c2) // closer to c0
						//	bitmap[y] |= (0x0 << (x * 2));
						//else
						if (colorProj[x][y] >= c0_c2 && colorProj[x][y] < c2_c1) // closer to c2
							bitmap[y] |= (0x2 << (x * 2));
						else if (colorProj[x][y] >= c2_c1) // closer to c1
							bitmap[y] |= (0x1 << (x * 2));
					}
			}

			qword blockData = 0;
			((word*)&blockData)[0] = color0;
			((word*)&blockData)[1] = color1;
			((word*)&blockData)[2] = (((word)bitmap[1]) << 8) | bitmap[0];
			((word*)&blockData)[3] = (((word)bitmap[3]) << 8) | bitmap[2];
			memcpy(outData + (blockX + blockY * numBlocksX) * sizeof(qword), &blockData, sizeof(qword));
		}
}

void ColorUtility::ConvertToDXT3(const Vec4f * const /*inRGBA*/, byte * const /*outData*/, const unsigned int /*width*/, const unsigned int /*height*/, const unsigned int /*depth*/)
{
	assert(false); // TODO
}

void ColorUtility::ConvertToDXT5(const Vec4f * const /*inRGBA*/, byte * const /*outData*/, const unsigned int /*width*/, const unsigned int /*height*/, const unsigned int /*depth*/)
{
	assert(false); // TODO
}
