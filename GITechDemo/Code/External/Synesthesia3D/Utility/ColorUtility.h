/**
 *	@file		ColorUtility.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef COLORUTILITY_H
#define COLORUTILITY_H

#include "ResourceData.h"
#include "HalfFloat.h"

namespace Synesthesia3D
{
	/**
	 * @brief	Utility class for converting between various color formats.
	 */
	class ColorUtility
	{

	public:

		/**
		 * @brief	Encode provided red, green and blue values into a single R8G8B8X8 value.
		 * @note	Default alpha value is 255.
		 *
		 * @param[in]	red		Red channel value.
		 * @param[in]	green	Green channel value.
		 * @param[in]	blue	Blue channel value.
		 *
		 * @return	A single R8G8B8X8 value.
		 */
		static SYNESTHESIA3D_DLL s3dDword MakeR8G8B8(
			const s3dByte red,
			const s3dByte green,
			const s3dByte blue);
		
		/**
		 * @brief	Encode provided red, green, blue and alpha values into a single R8G8B8A8 value.
		 *
		 * @param[in]	red		Red channel value.
		 * @param[in]	green	Green channel value.
		 * @param[in]	blue	Blue channel value.
		 * @param[in]	alpha	Alpha channel value.
		 *
		 * @return	A single R8G8B8A8 value.
		 */
		static SYNESTHESIA3D_DLL s3dDword MakeR8G8B8A8(
			const s3dByte red,
			const s3dByte green,
			const s3dByte blue,
			const s3dByte alpha);
		
		/**
		 * @brief	Decode provided R8G8B8X8 value into red, green and blue values.
		 *
		 * @param[in]	rgb		R8G8B8X8 value.
		 * @param[out]	red		Red channel value.
		 * @param[out]	green	Green channel value.
		 * @param[out]	blue	Blue channel value.
		 */
		static SYNESTHESIA3D_DLL void ExtractR8G8B8(
			const s3dDword rgb,
			s3dByte& red,
			s3dByte& green,
			s3dByte& blue);
		
		/**
		 * @brief	Decode provided R8G8B8A8 value into red, green, blue and alpha values.
		 *
		 * @param[in]	rgba	R8G8B8A8 value.
		 * @param[out]	red		Red channel value.
		 * @param[out]	green	Green channel value.
		 * @param[out]	blue	Blue channel value.
		 * @param[out]	alpha	Alpha channel value.
		 */
		static SYNESTHESIA3D_DLL void ExtractR8G8B8A8(
			const s3dDword rgba,
			s3dByte& red,
			s3dByte& green,
			s3dByte& blue,
			s3dByte& alpha);
		
		/**
		 * @brief	Encode provided red, green and blue values into a single B8G8R8X8 value.
		 * @note	Default alpha value is 255.
		 *
		 * @param[in]	red		Red channel value.
		 * @param[in]	green	Green channel value.
		 * @param[in]	blue	Blue channel value.
		 *
		 * @return	A single B8G8R8X8 value.
		 */
		static SYNESTHESIA3D_DLL s3dDword MakeB8G8R8(
			const s3dByte red,
			const s3dByte green,
			const s3dByte blue);

		/**
		 * @brief	Encode provided red, green, blue and alpha values into a single B8G8R8A8 value.
		 *
		 * @param[in]	red		Red channel value.
		 * @param[in]	green	Green channel value.
		 * @param[in]	blue	Blue channel value.
		 * @param[in]	alpha	Alpha channel value.
		 *
		 * @return	A single B8G8R8A8 value.
		 */
		static SYNESTHESIA3D_DLL s3dDword MakeB8G8R8A8(
			const s3dByte red,
			const s3dByte green,
			const s3dByte blue,
			const s3dByte alpha);
		
		/**
		 * @brief	Decode provided B8G8R8X8 value into red, green and blue values.
		 *
		 * @param[in]	bgr		B8G8R8X8 value.
		 * @param[out]	red		Red channel value.
		 * @param[out]	green	Green channel value.
		 * @param[out]	blue	Blue channel value.
		 */
		static SYNESTHESIA3D_DLL void ExtractB8G8R8(
			const s3dDword bgr,
			s3dByte& red,
			s3dByte& green,
			s3dByte& blue);
		
		/**
		 * @brief	Decode provided B8G8R8A8 value into red, green, blue and alpha values.
		 *
		 * @param[in]	bgra	B8G8R8A8 value.
		 * @param[out]	red		Red channel value.
		 * @param[out]	green	Green channel value.
		 * @param[out]	blue	Blue channel value.
		 * @param[out]	alpha	Alpha channel value.
		 */
		static SYNESTHESIA3D_DLL void ExtractB8G8R8A8(
			const s3dDword bgra,
			s3dByte& red,
			s3dByte& green,
			s3dByte& blue,
			s3dByte& alpha);
		
		/**
		 * @brief	Encode provided red, green and blue values into a single R5G6B5 value.
		 *
		 * @param[in]	red		Red channel value.
		 * @param[in]	green	Green channel value.
		 * @param[in]	blue	Blue channel value.
		 *
		 * @return	A single R5G6B5 value.
		 */
		static SYNESTHESIA3D_DLL s3dWord MakeR5G6B5(
			const s3dByte red,
			const s3dByte green,
			const s3dByte blue);
		
		/**
		 * @brief	Decode provided R5G6B5 value into red, green and blue values.
		 *
		 * @param[in]	rgb		R5G6B5 value.
		 * @param[out]	red		Red channel value.
		 * @param[out]	green	Green channel value.
		 * @param[out]	blue	Blue channel value.
		 */
		static SYNESTHESIA3D_DLL void ExtractR5G6B5(
			const s3dWord rgb,
			s3dByte& red,
			s3dByte& green,
			s3dByte& blue);



		/**
		 * @brief	Decodes a R5G6B5 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of R5G6B5 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromR5G6B5(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a A1R5G5B5 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of A1R5G5B5 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromA1R5G5B5(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a A4R4G4B4 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of A4R4G4B4 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromA4R4G4B4(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a A8 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of A8 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromA8(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a L8 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of L8 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromL8(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a A8L8 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of A8L8 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromA8L8(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a R8G8B8 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of R8G8B8 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromR8G8B8(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a A8R8G8B8 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of A8R8G8B8 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromA8R8G8B8(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a A8B8G8R8 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of A8B8G8R8 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromA8B8G8R8(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a L16 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of L16 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromL16(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a G16R16 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of G16R16 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromG16R16(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a A16B16G16R16 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of A16B16G16R16 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromA16B16G16R16(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a R16F texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 *
		 * @param[in]	inData		An array of R16F values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromR16F(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a G16R16F texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 *
		 * @param[in]	inData		An array of G16R16F values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromG16R16F(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a A16B16G16R16F texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 *
		 * @param[in]	inData		An array of A16B16G16R16F values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromA16B16G16R16F(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a R32F texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 *
		 * @param[in]	inData		An array of R32F values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromR32F(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a G32R32F texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 *
		 * @param[in]	inData		An array of G32R32F values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromG32R32F(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a A32B32G32R32F texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 *
		 * @param[in]	inData		An array of A32B32G32R32F values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromA32B32G32R32F(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a DXT1 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of DXT1 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromDXT1(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a DXT3 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of DXT3 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromDXT3(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Decodes a DXT5 texture buffer to an array of floating-point red, green, blue and alpha values (in this order).
		 * @note	Output color channels are normalized.
		 *
		 * @param[in]	inData		An array of DXT5 values (texture data).
		 * @param[out]	outRGBA		Array of decoded floating-point red, green, blue and alpha values (in this order).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertFromDXT5(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth = 1);



		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a R5G6B5 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of R5G6B5 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToR5G6B5(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a A1R5G5B5 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of A1R5G5B5 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToA1R5G5B5(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a A4R4G4B4 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of A4R4G4B4 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToA4R4G4B4(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a A8 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of A8 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToA8(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a L8 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of L8 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToL8(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a A8L8 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of A8L8 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToA8L8(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a R8G8B8 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of R8G8B8 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToR8G8B8(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a A8R8G8B8 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of A8R8G8B8 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToA8R8G8B8(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a A8B8G8R8 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of A8B8G8R8 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToA8B8G8R8(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a L16 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of L16 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToL16(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a G16R16 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of G16R16 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToG16R16(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a A16B16G16R16 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of A16B16G16R16 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToA16B16G16R16(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a R16F texture buffer.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of R16F values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToR16F(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a G16R16F texture buffer.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of G16R16F values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToG16R16F(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a A16B16G16R16F texture buffer.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of A16B16G16R16F values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToA16B16G16R16F(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a R32F texture buffer.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of R32F values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToR32F(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a G32R32F texture buffer.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of G32R32F values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToG32R32F(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a A32B32G32R32F texture buffer.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of A32B32G32R32F values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToA32B32G32R32F(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a DXT1 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of DXT1 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToDXT1(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a DXT3 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of DXT3 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToDXT3(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Encodes an array of floating-point red, green, blue and alpha values (in this order) into a DXT5 texture buffer.
		 * @note	Input color channels must be normalized.
		 *
		 * @param[in]	inRGBA		Array of floating-point red, green, blue and alpha values (in this order).
		 * @param[out]	outData		Array of DXT5 values (texture data).
		 * @param[in]	width		Texture width.
		 * @param[in]	height		Texture height.
		 * @param[in]	depth		Texture depth.
		 */
		static SYNESTHESIA3D_DLL void ConvertToDXT5(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth = 1);

		/**
		 * @brief	Allows convenient usage of "convert from" functions.
		 */
		typedef void(*ConvertFromFunc)(const s3dByte* const inData, Vec4f* const outRGBA, const unsigned int width, const unsigned int height, const unsigned int depth);
		static SYNESTHESIA3D_DLL ConvertFromFunc ConvertFrom[PF_MAX]; /**< @brief "Convert from" function look up table, for convenience. */
		
		/**
		 * @brief	Allows convenient usage of "convert to" functions.
		 */
		typedef void(*ConvertToFunc)(const Vec4f* const inRGBA, s3dByte* const outData, const unsigned int width, const unsigned int height, const unsigned int depth);
		static SYNESTHESIA3D_DLL ConvertToFunc ConvertTo[PF_MAX]; /**< @brief "Convert to" function look up table, for convenience. */
	};
}

#endif // COLORUTILITY_H
