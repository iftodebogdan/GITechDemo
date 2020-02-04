/**
 * @file        ResourceManagerNULL.cpp
 *
 * @note        This file is part of the "Synesthesia3D" graphics engine
 *
 * @copyright   Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * @copyright
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * @copyright
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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

const unsigned int ResourceManagerNULL::CreateVertexFormat(const unsigned int attributeCount)
{
    VertexFormat* const vf = new VertexFormatNULL(attributeCount);
    return AddVertexFormat(vf);
}

const unsigned int ResourceManagerNULL::CreateVertexFormat(
    const unsigned int attributeCount, const VertexAttributeSemantic semantic,
    const VertexAttributeType type, const unsigned int semanticIdx, ...)
{
    VertexFormat* vf = new VertexFormatNULL(attributeCount);
    unsigned int offset = 0;

    vf->SetAttribute(0, offset, semantic, type, semanticIdx);
    offset += VertexFormat::GetAttributeTypeSize(type);

    va_list args;
    va_start(args, semanticIdx);
    for (unsigned int i = 1, n = vf->GetAttributeCount(); i < n; i++)
    {
        VertexAttributeSemantic tempSemantic = va_arg(args, VertexAttributeSemantic);
        VertexAttributeType tempType = va_arg(args, VertexAttributeType);
        unsigned int tempSemanticIdx = va_arg(args, unsigned int);
        vf->SetAttribute(i, offset, tempSemantic, tempType, tempSemanticIdx);
        offset += VertexFormat::GetAttributeTypeSize(tempType);
    }
    va_end(args);

    vf->SetStride(offset);
    vf->Update();

    return AddVertexFormat(vf);
}

const unsigned int ResourceManagerNULL::CreateIndexBuffer(
    const unsigned int indexCount, const IndexBufferFormat indexFormat,
    const BufferUsage usage)
{
    IndexBuffer* ib = new IndexBufferNULL(indexCount, indexFormat, usage);
    return AddIndexBuffer(ib);
}

const unsigned int ResourceManagerNULL::CreateVertexBuffer(
    VertexFormat* const vertexFormat, const unsigned int vertexCount,
    IndexBuffer* const indexBuffer, const BufferUsage usage)
{
    VertexBuffer* vb = new VertexBufferNULL((VertexFormatNULL*)vertexFormat, vertexCount, (IndexBufferNULL*)indexBuffer, usage);
    return AddVertexBuffer(vb);
}

const unsigned int ResourceManagerNULL::CreateShaderProgram(const char* filePath, const ShaderProgramType programType, const char* entryPoint)
{
    ShaderProgramNULL* sp = new ShaderProgramNULL(programType, "", "", "");
    sp->Compile(filePath, entryPoint);
    return AddShaderProgram(sp);
}

const unsigned int ResourceManagerNULL::CreateTexture(
    const PixelFormat pixelFormat, const TextureType texType,
    const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
    const unsigned int mipCount, const BufferUsage usage)
{
    Texture* tex = new TextureNULL(pixelFormat, texType, sizeX, sizeY, sizeZ, mipCount, usage);
    return AddTexture(tex);
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
    const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetNULL(targetCount, pixelFormat, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat);
    return AddRenderTarget(rt);
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
    const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetNULL(targetCount, pixelFormat, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat);
    return AddRenderTarget(rt);
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount,
    PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
    const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetNULL(targetCount,
        pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3,
        width, height, hasMipmaps, hasDepthStencil, depthStencilFormat);
    return AddRenderTarget(rt);
}

const unsigned int ResourceManagerNULL::CreateRenderTarget(const unsigned int targetCount,
    PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
    const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetNULL(targetCount,
        pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3,
        widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat);
    return AddRenderTarget(rt);
}
