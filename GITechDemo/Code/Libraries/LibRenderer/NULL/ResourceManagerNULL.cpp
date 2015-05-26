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

#include <stdarg.h>

#include "VertexFormatNULL.h"
#include "IndexBufferNULL.h"
#include "VertexBufferNULL.h"
#include "ShaderProgramNULL.h"
#include "TextureNULL.h"
#include "RenderTargetNULL.h"

#include "ResourceManagerNULL.h"
using namespace LibRendererDll;


const unsigned int ResourceManagerNULL::CreateVertexFormat(const unsigned int attributeCount)
{
	m_arrVertexFormat.push_back(new VertexFormatNULL(attributeCount));
	return (unsigned int)m_arrVertexFormat.size() - 1;
}

const unsigned int ResourceManagerNULL::CreateVertexFormat(
	const unsigned int attributeCount, const VertexAttributeUsage usage,
	const VertexAttributeType type, const unsigned int usageIdx, ...)
{
	VertexFormat* vf = new VertexFormatNULL(attributeCount);
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

const unsigned int ResourceManagerNULL::CreateIndexBuffer(
	const unsigned int indexCount, const IndexBufferFormat indexFormat,
	const BufferUsage usage)
{
	m_arrIndexBuffer.push_back(new IndexBufferNULL(indexCount, indexFormat, usage));
	return (unsigned int)m_arrIndexBuffer.size() - 1;
}

const unsigned int ResourceManagerNULL::CreateVertexBuffer(
	VertexFormat* const vertexFormat, const unsigned int vertexCount,
	IndexBuffer* const indexBuffer, const BufferUsage usage)
{
	m_arrVertexBuffer.push_back(new VertexBufferNULL((VertexFormatNULL*)vertexFormat, vertexCount, (IndexBufferNULL*)indexBuffer, usage));
	return (unsigned int)m_arrVertexBuffer.size() - 1;
}

const unsigned int ResourceManagerNULL::CreateShaderProgram(const ShaderProgramType programType)
{
	m_arrShaderProgram.push_back(new ShaderProgramNULL(programType));
	return (unsigned int)m_arrShaderProgram.size() - 1;
}

const unsigned int ResourceManagerNULL::CreateTexture(
	const PixelFormat texFormat, const TexType texType,
	const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
	const unsigned int mipCount, const BufferUsage usage)
{
	m_arrTexture.push_back(new TextureNULL(texFormat, texType, sizeX, sizeY, sizeZ, mipCount, usage));
	return (unsigned int)m_arrTexture.size() - 1;
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
	m_arrRenderTarget.push_back(new RenderTargetNULL(targetCount, pixelFormat, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat));
	return (unsigned int)m_arrRenderTarget.size() - 1;
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
	const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
	m_arrRenderTarget.push_back(new RenderTargetNULL(targetCount, pixelFormat, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat));
	return (unsigned int)m_arrRenderTarget.size() - 1;
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount,
	PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
	m_arrRenderTarget.push_back(new RenderTargetNULL(targetCount, pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat));
	return (unsigned int)m_arrRenderTarget.size() - 1;
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount,
	PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
	const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
	m_arrRenderTarget.push_back(new RenderTargetNULL(targetCount, pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat));
	return (unsigned int)m_arrRenderTarget.size() - 1;
}