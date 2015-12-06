/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	ColorUtility.h
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

#ifndef COLORUTILITY_H
#define COLORUTILITY_H

#include "ResourceData.h"
#include "HalfFloat.h"

namespace Synesthesia3D
{
	// A utility class for converting between various color formats
	class ColorUtility
	{
	public:
		static SYNESTHESIA3D_DLL dword MakeR8G8B8(
			const byte red,
			const byte green,
			const byte blue);

		static SYNESTHESIA3D_DLL dword MakeR8G8B8A8(
			const byte red,
			const byte green,
			const byte blue,
			const byte alpha);

		static SYNESTHESIA3D_DLL void ExtractR8G8B8(
			const dword rgb,
			byte& red,
			byte& green,
			byte& blue);

		static SYNESTHESIA3D_DLL void ExtractR8G8B8A8(
			const dword rgba,
			byte& red,
			byte& green,
			byte& blue,
			byte& alpha);
		
		static SYNESTHESIA3D_DLL dword MakeB8G8R8(
			const byte red,
			const byte green,
			const byte blue);

		static SYNESTHESIA3D_DLL dword MakeB8G8R8A8(
			const byte red,
			const byte green,
			const byte blue,
			const byte alpha);

		static SYNESTHESIA3D_DLL void ExtractB8G8R8(
			const dword rgb,
			byte& red,
			byte& green,
			byte& blue);

		static SYNESTHESIA3D_DLL void ExtractB8G8R8A8(
			const dword rgba,
			byte& red,
			byte& green,
			byte& blue,
			byte& alpha);

		static SYNESTHESIA3D_DLL word MakeR5G6B5(
			const byte red,
			const byte green,
			const byte blue);

		static SYNESTHESIA3D_DLL void ExtractR5G6B5(
			const word rgb,
			byte& red,
			byte& green,
			byte& blue);

		static SYNESTHESIA3D_DLL void ConvertFromR5G6B5(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromA1R5G5B5(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromA4R4G4B4(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromA8(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromL8(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromA8L8(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromR8G8B8(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromA8R8G8B8(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromA8B8G8R8(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromL16(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromG16R16(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromA16B16G16R16(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromR16F(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromG16R16F(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromA16B16G16R16F(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromR32F(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromG32R32F(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromA32B32G32R32F(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromDXT1(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromDXT3(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertFromDXT5(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		static SYNESTHESIA3D_DLL void ConvertToR5G6B5(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToA1R5G5B5(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToA4R4G4B4(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToA8(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToL8(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToA8L8(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToR8G8B8(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToA8R8G8B8(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToA8B8G8R8(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToL16(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToG16R16(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToA16B16G16R16(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToR16F(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToG16R16F(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToA16B16G16R16F(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToR32F(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToG32R32F(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToA32B32G32R32F(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToDXT1(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToDXT3(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);
		static SYNESTHESIA3D_DLL void ConvertToDXT5(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		// Function look-up table, for convenience
		typedef void(*ConvertFromFunc)(const byte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth);
		static SYNESTHESIA3D_DLL ConvertFromFunc ConvertFrom[PF_MAX];

		typedef void(*ConvertToFunc)(const Vec4f* const inRGBA, byte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth);
		static SYNESTHESIA3D_DLL ConvertToFunc ConvertTo[PF_MAX];
	};
}

#endif // COLORUTILITY_H