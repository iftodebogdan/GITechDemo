/**
 * @file        ResourceManagerD3D9.cpp
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

#include "VertexFormatD3D9.h"
#include "IndexBufferD3D9.h"
#include "VertexBufferD3D9.h"
#include "ShaderProgramD3D9.h"
#include "TextureD3D9.h"
#include "RenderTargetD3D9.h"
#include "Renderer.h"
#include "ResourceManagerD3D9.h"
#include "ProfilerD3D9.h"
using namespace Synesthesia3D;

const unsigned int ResourceManagerD3D9::CreateVertexFormat(const unsigned int attributeCount)
{
    VertexFormat* const vf = new VertexFormatD3D9(attributeCount);
    return AddVertexFormat(vf);
}

const unsigned int ResourceManagerD3D9::CreateVertexFormat(
    const unsigned int attributeCount, const VertexAttributeSemantic semantic,
    const VertexAttributeType type, const unsigned int semanticIdx, ...)
{
    VertexFormat* vf = new VertexFormatD3D9(attributeCount);
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

const unsigned int ResourceManagerD3D9::CreateIndexBuffer(
    const unsigned int indexCount, const IndexBufferFormat indexFormat,
    const BufferUsage usage)
{
    IndexBuffer* ib = new IndexBufferD3D9(indexCount, indexFormat, usage);
    return AddIndexBuffer(ib);
}

const unsigned int ResourceManagerD3D9::CreateVertexBuffer(
    VertexFormat* const vertexFormat, const unsigned int vertexCount,
    IndexBuffer* const indexBuffer, const BufferUsage usage)
{
    VertexBuffer* vb = new VertexBufferD3D9((VertexFormatD3D9*)vertexFormat, vertexCount, (IndexBufferD3D9*)indexBuffer, usage);
    return AddVertexBuffer(vb);
}

const unsigned int ResourceManagerD3D9::CreateShaderProgram(const char* filePath, const ShaderProgramType programType, const char* entryPoint)
{
    ShaderProgramD3D9* sp = new ShaderProgramD3D9(programType);
    sp->Compile(filePath, entryPoint);
    return AddShaderProgram(sp);
}

const unsigned int ResourceManagerD3D9::CreateTexture(
    const PixelFormat pixelFormat, const TextureType texType,
    const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
    const unsigned int mipCount, const BufferUsage usage)
{
    Texture* tex = new TextureD3D9(pixelFormat, texType, sizeX, sizeY, sizeZ, mipCount, usage);
    return AddTexture(tex);
}

const unsigned int ResourceManagerD3D9::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
    const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetD3D9(targetCount, pixelFormat, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat);
    return AddRenderTarget(rt);
}

const unsigned int ResourceManagerD3D9::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
    const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetD3D9(targetCount, pixelFormat, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat);
    return AddRenderTarget(rt);
}

const unsigned int ResourceManagerD3D9::CreateRenderTarget(const unsigned int targetCount,
    PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
    const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetD3D9(targetCount,
        pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3,
        width, height, hasMipmaps, hasDepthStencil, depthStencilFormat);
    return AddRenderTarget(rt);
}

const unsigned int ResourceManagerD3D9::CreateRenderTarget(const unsigned int targetCount,
    PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
    const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetD3D9(targetCount,
        pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3,
        widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat);
    return AddRenderTarget(rt);
}

void ResourceManagerD3D9::UnbindAll()
{
    ResourceManager::UnbindAll();

    if (RenderTargetD3D9::ms_pBBColorSurfBkp)
    {
        RenderTargetD3D9::ms_pBBColorSurfBkp->Release();
        RenderTargetD3D9::ms_pBBColorSurfBkp = nullptr;
    }

    if (RenderTargetD3D9::ms_pBBDepthSurfBkp)
    {
        RenderTargetD3D9::ms_pBBDepthSurfBkp->Release();
        RenderTargetD3D9::ms_pBBDepthSurfBkp = nullptr;
    }
}
