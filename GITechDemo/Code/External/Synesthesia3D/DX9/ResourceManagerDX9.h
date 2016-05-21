/**
 *	@file		ResourceManagerDX9.h
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

#ifndef RESOURCEMANAGERDX9_H
#define RESOURCEMANAGERDX9_H

#include "ResourceManager.h"

namespace Synesthesia3D
{
	class ResourceManagerDX9 : public ResourceManager
	{
	public:
		const unsigned int CreateVertexFormat(const unsigned int attributeCount);
		const unsigned int CreateVertexFormat(
			const unsigned int attributeCount, const VertexAttributeSemantic semantic,
			const VertexAttributeType type, const unsigned int semanticIdx, ...);

		const unsigned int CreateIndexBuffer(
			const unsigned int indexCount, const IndexBufferFormat indexFormat = IBF_INDEX16,
			const BufferUsage usage = BU_STATIC);

		const unsigned int CreateVertexBuffer(
			VertexFormat* const vertexFormat, const unsigned int vertexCount,
			IndexBuffer* const indexBuffer = nullptr, const BufferUsage usage = BU_STATIC);

		const unsigned int CreateShaderInput(ShaderProgram* const shaderProgram);

		const unsigned int CreateShaderProgram(const char* filePath, const ShaderProgramType programType, const char* entryPoint = "");

		const unsigned int CreateTexture(
			const PixelFormat pixelFormat, const TextureType texType,
			const unsigned int sizeX, const unsigned int sizeY = 1, const unsigned int sizeZ = 1,
			const unsigned int mipCount = 0, const BufferUsage usage = BU_TEXTURE);

		const unsigned int CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
			const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);
		const unsigned int CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
			const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);
		const unsigned int CreateRenderTarget(const unsigned int targetCount,
			PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
			const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);
		const unsigned int CreateRenderTarget(const unsigned int targetCount,
			PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
			const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);

	private:
		ResourceManagerDX9() {}
		~ResourceManagerDX9() {}

		friend class RendererDX9;
	};
}

#endif // RESOURCEMANAGERDX9_H