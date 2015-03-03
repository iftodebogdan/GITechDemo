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
#ifndef RESOURCEMANAGERDX9_H
#define RESOURCEMANAGERDX9_H

#include "ResourceManager.h"

namespace LibRendererDll
{
	class ResourceManagerDX9 : public ResourceManager
	{
	public:
		const unsigned int CreateVertexFormat(const unsigned int attributeCount);
		const unsigned int CreateVertexFormat(
			const unsigned int attributeCount, const VertexAttributeUsage usage,
			const VertexAttributeType type, const unsigned int usageIdx, ...);

		const unsigned int CreateIndexBuffer(
			const unsigned int indexCount, const IndexBufferFormat indexFormat = IBF_INDEX16,
			const BufferUsage usage = BU_STATIC);

		const unsigned int CreateVertexBuffer(
			VertexFormat* const vertexFormat, const unsigned int vertexCount,
			IndexBuffer* const indexBuffer = nullptr, const BufferUsage usage = BU_STATIC);

		const unsigned int CreateShaderInput(ShaderTemplate* const shaderTemplate);

		const unsigned int CreateShaderProgram(const ShaderProgramType programType);

		const unsigned int CreateShaderTemplate(ShaderProgram* const shaderProgram);

		const unsigned int CreateTexture(
			const PixelFormat texFormat, const TexType texType,
			const unsigned int sizeX, const unsigned int sizeY = 1, const unsigned int sizeZ = 1,
			const unsigned int mipmapLevelCount = 0, const BufferUsage usage = BU_TEXTURE);

		const unsigned int CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
			const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil);

	private:
		ResourceManagerDX9() {}
		~ResourceManagerDX9() {}

		friend class RendererDX9;
	};
}

#endif // RESOURCEMANAGERDX9_H