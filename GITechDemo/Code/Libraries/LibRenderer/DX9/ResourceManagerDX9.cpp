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

#include "VertexFormatDX9.h"
#include "IndexBufferDX9.h"
#include "VertexBufferDX9.h"
#include "ShaderProgramDX9.h"
#include "TextureDX9.h"
#include "RenderTargetDX9.h"

#include "ResourceManagerDX9.h"
using namespace LibRendererDll;


const unsigned int ResourceManagerDX9::CreateVertexFormat(const unsigned int attributeCount)
{
	m_arrVertexFormat.push_back(new VertexFormatDX9(attributeCount));
	return (unsigned int)m_arrVertexFormat.size() - 1;
}

const unsigned int ResourceManagerDX9::CreateVertexFormat(
	const unsigned int attributeCount, const VertexAttributeUsage usage,
	const VertexAttributeType type, const unsigned int usageIdx, ...)
{
	VertexFormat* vf = new VertexFormatDX9(attributeCount);
	unsigned int offset = 0;

	vf->SetAttribute(0, offset, usage, type, usageIdx);
	offset += VertexFormat::GetAttributeTypeSize(type);

	va_list args;
	va_start(args, usageIdx);
	for (unsigned int i = 1, n = vf->GetAttributeCount(); i < n; i++)
	{
		VertexAttributeUsage tempUsage = va_arg(args, VertexAttributeUsage);
		VertexAttributeType tempType = va_arg(args, VertexAttributeType);
		unsigned int tempUsageIdx = va_arg(args, unsigned int);
		vf->SetAttribute(i, offset, tempUsage, tempType, tempUsageIdx);
		offset += VertexFormat::GetAttributeTypeSize(tempType);
	}
	va_end(args);

	vf->SetStride(offset);

	m_arrVertexFormat.push_back(vf);
	return (unsigned int)m_arrVertexFormat.size() - 1;
}

const unsigned int ResourceManagerDX9::CreateIndexBuffer(
	const unsigned int indexCount, const IndexBufferFormat indexFormat,
	const BufferUsage usage)
{
	m_arrIndexBuffer.push_back(new IndexBufferDX9(indexCount, indexFormat, usage));
	return (unsigned int)m_arrIndexBuffer.size() - 1;
}

const unsigned int ResourceManagerDX9::CreateVertexBuffer(
	VertexFormat* const vertexFormat, const unsigned int vertexCount,
	IndexBuffer* const indexBuffer, const BufferUsage usage)
{
	m_arrVertexBuffer.push_back(new VertexBufferDX9((VertexFormatDX9*)vertexFormat, vertexCount, (IndexBufferDX9*)indexBuffer, usage));
	return (unsigned int)m_arrVertexBuffer.size() - 1;
}

const unsigned int ResourceManagerDX9::CreateShaderProgram(const ShaderProgramType programType)
{
	m_arrShaderProgram.push_back(new ShaderProgramDX9(programType));
	return (unsigned int)m_arrShaderProgram.size() - 1;
}

const unsigned int ResourceManagerDX9::CreateTexture(
	const PixelFormat texFormat, const TexType texType,
	const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
	const unsigned int mipmapLevelCount, const BufferUsage usage)
{
	m_arrTexture.push_back(new TextureDX9(texFormat, texType, sizeX, sizeY, sizeZ, mipmapLevelCount, usage));
	return (unsigned int)m_arrTexture.size() - 1;
}

const unsigned int ResourceManagerDX9::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil)
{
	m_arrRenderTarget.push_back(new RenderTargetDX9(targetCount, pixelFormat, width, height, hasMipmaps, hasDepthStencil));
	return (unsigned int)m_arrRenderTarget.size() - 1;
}