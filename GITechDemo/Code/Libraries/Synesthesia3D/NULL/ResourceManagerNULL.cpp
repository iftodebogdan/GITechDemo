/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	ResourceManagerNULL.cpp
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

#include "stdafx.h"

#include <stdarg.h>

#include "VertexFormatNULL.h"
#include "IndexBufferNULL.h"
#include "VertexBufferNULL.h"
#include "ShaderProgramNULL.h"
#include "TextureNULL.h"
#include "RenderTargetNULL.h"

#include "ResourceManagerNULL.h"
using namespace Synesthesia3D;

#include <Utility/Mutex.h>

// Mutexes for each resource pool
extern MUTEX	VFMutex;
extern MUTEX	IBMutex;
extern MUTEX	VBMutex;
extern MUTEX	ShdInMutex;
extern MUTEX	ShdProgMutex;
extern MUTEX	ShdTmplMutex;
extern MUTEX	TexMutex;
extern MUTEX	RTMutex;
extern MUTEX	ModelMutex;

const unsigned int ResourceManagerNULL::CreateVertexFormat(const unsigned int attributeCount)
{
	VertexFormat* const vf = new VertexFormatNULL(attributeCount);
	MUTEX_LOCK(VFMutex);
	m_arrVertexFormat.push_back(vf);
	const unsigned int ret = (unsigned int)m_arrVertexFormat.size() - 1;
	MUTEX_UNLOCK(VFMutex);
	return ret;
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
	vf->Update();

	MUTEX_LOCK(VFMutex);
	m_arrVertexFormat.push_back(vf);
	const unsigned int ret = (unsigned int)m_arrVertexFormat.size() - 1;
	MUTEX_UNLOCK(VFMutex);

	return ret;
}

const unsigned int ResourceManagerNULL::CreateIndexBuffer(
	const unsigned int indexCount, const IndexBufferFormat indexFormat,
	const BufferUsage usage)
{
	IndexBuffer* ib = new IndexBufferNULL(indexCount, indexFormat, usage);
	MUTEX_LOCK(IBMutex);
	m_arrIndexBuffer.push_back(ib);
	const unsigned int ret = (unsigned int)m_arrIndexBuffer.size() - 1;
	MUTEX_UNLOCK(IBMutex);

	return ret;
}

const unsigned int ResourceManagerNULL::CreateVertexBuffer(
	VertexFormat* const vertexFormat, const unsigned int vertexCount,
	IndexBuffer* const indexBuffer, const BufferUsage usage)
{
	VertexBuffer* vb = new VertexBufferNULL((VertexFormatNULL*)vertexFormat, vertexCount, (IndexBufferNULL*)indexBuffer, usage);
	MUTEX_LOCK(VBMutex);
	m_arrVertexBuffer.push_back(vb);
	const unsigned int ret = (unsigned int)m_arrVertexBuffer.size() - 1;
	MUTEX_UNLOCK(VBMutex);

	return ret;
}

const unsigned int ResourceManagerNULL::CreateShaderProgram(const ShaderProgramType programType)
{
	ShaderProgram* sp = new ShaderProgramNULL(programType, "", "", "");
	MUTEX_LOCK(ShdProgMutex);
	m_arrShaderProgram.push_back(sp);
	const unsigned int ret = (unsigned int)m_arrShaderProgram.size() - 1;
	MUTEX_UNLOCK(ShdProgMutex);
	return ret;
}

const unsigned int ResourceManagerNULL::CreateTexture(
	const PixelFormat pixelFormat, const TextureType texType,
	const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
	const unsigned int mipCount, const BufferUsage usage)
{
	Texture* tex = new TextureNULL(pixelFormat, texType, sizeX, sizeY, sizeZ, mipCount, usage);
	MUTEX_LOCK(TexMutex);
	m_arrTexture.push_back(tex);
	const unsigned int ret = (unsigned int)m_arrTexture.size() - 1;
	MUTEX_UNLOCK(TexMutex);

	return ret;
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
	RenderTarget* rt = new RenderTargetNULL(targetCount, pixelFormat, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat);
	MUTEX_LOCK(RTMutex);
	m_arrRenderTarget.push_back(rt);
	const unsigned int ret = (unsigned int)m_arrRenderTarget.size() - 1;
	MUTEX_UNLOCK(RTMutex);

	return ret;
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
	const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
	RenderTarget* rt = new RenderTargetNULL(targetCount, pixelFormat, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat);
	MUTEX_LOCK(RTMutex);
	m_arrRenderTarget.push_back(rt);
	const unsigned int ret = (unsigned int)m_arrRenderTarget.size() - 1;
	MUTEX_UNLOCK(RTMutex);

	return ret;
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount,
	PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
	RenderTarget* rt = new RenderTargetNULL(targetCount,
		pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3,
		width, height, hasMipmaps, hasDepthStencil, depthStencilFormat);
	MUTEX_LOCK(RTMutex);
	m_arrRenderTarget.push_back(rt);
	const unsigned int ret = (unsigned int)m_arrRenderTarget.size() - 1;
	MUTEX_UNLOCK(RTMutex);

	return ret;
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount,
	PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
	const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
	RenderTarget* rt = new RenderTargetNULL(targetCount,
		pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3,
		widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat);
	MUTEX_LOCK(RTMutex);
	m_arrRenderTarget.push_back(rt);
	const unsigned int ret = (unsigned int)m_arrRenderTarget.size() - 1;
	MUTEX_UNLOCK(RTMutex);

	return ret;
}
