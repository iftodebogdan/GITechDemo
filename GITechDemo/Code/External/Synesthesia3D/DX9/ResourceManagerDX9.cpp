/**
 * @file        ResourceManagerDX9.cpp
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

#include "VertexFormatDX9.h"
#include "IndexBufferDX9.h"
#include "VertexBufferDX9.h"
#include "ShaderProgramDX9.h"
#include "TextureDX9.h"
#include "RenderTargetDX9.h"
#include "Renderer.h"
#include "ResourceManagerDX9.h"
#include "ProfilerDX9.h"
using namespace Synesthesia3D;

#include <Utility/Mutex.h>

// Mutexes for each resource pool
extern MUTEX    VFMutex;
extern MUTEX    IBMutex;
extern MUTEX    VBMutex;
extern MUTEX    ShdInMutex;
extern MUTEX    ShdProgMutex;
extern MUTEX    TexMutex;
extern MUTEX    RTMutex;
extern MUTEX    ModelMutex;

const unsigned int ResourceManagerDX9::CreateVertexFormat(const unsigned int attributeCount)
{
    VertexFormat* const vf = new VertexFormatDX9(attributeCount);
    MUTEX_LOCK(VFMutex);
    m_arrVertexFormat.push_back(vf);
    const unsigned int ret = (unsigned int)m_arrVertexFormat.size() - 1;
    MUTEX_UNLOCK(VFMutex);
    return ret;
}

const unsigned int ResourceManagerDX9::CreateVertexFormat(
    const unsigned int attributeCount, const VertexAttributeSemantic semantic,
    const VertexAttributeType type, const unsigned int semanticIdx, ...)
{
    VertexFormat* vf = new VertexFormatDX9(attributeCount);
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

    MUTEX_LOCK(VFMutex);
    m_arrVertexFormat.push_back(vf);
    const unsigned int ret = (unsigned int)m_arrVertexFormat.size() - 1;
    MUTEX_UNLOCK(VFMutex);

    return ret;
}

const unsigned int ResourceManagerDX9::CreateIndexBuffer(
    const unsigned int indexCount, const IndexBufferFormat indexFormat,
    const BufferUsage usage)
{
    IndexBuffer* ib = new IndexBufferDX9(indexCount, indexFormat, usage);
    MUTEX_LOCK(IBMutex);
    m_arrIndexBuffer.push_back(ib);
    const unsigned int ret = (unsigned int)m_arrIndexBuffer.size() - 1;
    MUTEX_UNLOCK(IBMutex);

    return ret;
}

const unsigned int ResourceManagerDX9::CreateVertexBuffer(
    VertexFormat* const vertexFormat, const unsigned int vertexCount,
    IndexBuffer* const indexBuffer, const BufferUsage usage)
{
    VertexBuffer* vb = new VertexBufferDX9((VertexFormatDX9*)vertexFormat, vertexCount, (IndexBufferDX9*)indexBuffer, usage);
    MUTEX_LOCK(VBMutex);
    m_arrVertexBuffer.push_back(vb);
    const unsigned int ret = (unsigned int)m_arrVertexBuffer.size() - 1;
    MUTEX_UNLOCK(VBMutex);

    return ret;
}

const unsigned int ResourceManagerDX9::CreateShaderProgram(const char* filePath, const ShaderProgramType programType, const char* entryPoint)
{
    ShaderProgramDX9* sp = new ShaderProgramDX9(programType);
    MUTEX_LOCK(ShdProgMutex);
    m_arrShaderProgram.push_back(sp);
    const unsigned int ret = (unsigned int)m_arrShaderProgram.size() - 1;
    MUTEX_UNLOCK(ShdProgMutex);
    sp->Compile(filePath, entryPoint);

    return ret;
}

const unsigned int ResourceManagerDX9::CreateTexture(
    const PixelFormat pixelFormat, const TextureType texType,
    const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
    const unsigned int mipCount, const BufferUsage usage)
{
    Texture* tex = new TextureDX9(pixelFormat, texType, sizeX, sizeY, sizeZ, mipCount, usage);
    MUTEX_LOCK(TexMutex);
    m_arrTexture.push_back(tex);
    const unsigned int ret = (unsigned int)m_arrTexture.size() - 1;
    MUTEX_UNLOCK(TexMutex);

    return ret;
}

const unsigned int ResourceManagerDX9::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
    const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetDX9(targetCount, pixelFormat, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat);
    MUTEX_LOCK(RTMutex);
    m_arrRenderTarget.push_back(rt);
    const unsigned int ret = (unsigned int)m_arrRenderTarget.size() - 1;
    MUTEX_UNLOCK(RTMutex);

    return ret;
}

const unsigned int ResourceManagerDX9::CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
    const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetDX9(targetCount, pixelFormat, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat);
    MUTEX_LOCK(RTMutex);
    m_arrRenderTarget.push_back(rt);
    const unsigned int ret = (unsigned int)m_arrRenderTarget.size() - 1;
    MUTEX_UNLOCK(RTMutex);

    return ret;
}

const unsigned int ResourceManagerDX9::CreateRenderTarget(const unsigned int targetCount,
    PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
    const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetDX9(targetCount,
        pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3,
        width, height, hasMipmaps, hasDepthStencil, depthStencilFormat);
    MUTEX_LOCK(RTMutex);
    m_arrRenderTarget.push_back(rt);
    const unsigned int ret = (unsigned int)m_arrRenderTarget.size() - 1;
    MUTEX_UNLOCK(RTMutex);

    return ret;
}

const unsigned int ResourceManagerDX9::CreateRenderTarget(const unsigned int targetCount,
    PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
    const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
{
    RenderTarget* rt = new RenderTargetDX9(targetCount,
        pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3,
        widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat);
    MUTEX_LOCK(RTMutex);
    m_arrRenderTarget.push_back(rt);
    const unsigned int ret = (unsigned int)m_arrRenderTarget.size() - 1;
    MUTEX_UNLOCK(RTMutex);

    return ret;
}

void ResourceManagerDX9::UnbindAll()
{
    ResourceManager::UnbindAll();

    if (RenderTargetDX9::ms_pBBColorSurfBkp)
    {
        RenderTargetDX9::ms_pBBColorSurfBkp->Release();
        RenderTargetDX9::ms_pBBColorSurfBkp = nullptr;
    }

    if (RenderTargetDX9::ms_pBBDepthSurfBkp)
    {
        RenderTargetDX9::ms_pBBDepthSurfBkp->Release();
        RenderTargetDX9::ms_pBBDepthSurfBkp = nullptr;
    }
}
