/**
 * @file        VertexBufferD3D9.cpp
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

#include "RendererD3D9.h"
#include "MappingsD3D9.h"

#include "VertexFormatD3D9.h"
#include "VertexBufferD3D9.h"
#include "IndexBufferD3D9.h"
#include "ProfilerD3D9.h"
using namespace Synesthesia3D;

VertexBufferD3D9::VertexBufferD3D9(VertexFormatD3D9* const vertexFormat, const unsigned int vertexCount, IndexBufferD3D9* const indexBuffer, const BufferUsage usage)
    : VertexBuffer(vertexFormat, vertexCount, indexBuffer, usage)
    , m_pVertexBuffer(nullptr)
    , m_pTempBuffer(nullptr)
{
    if (vertexCount != 0)
        Bind();
}

VertexBufferD3D9::~VertexBufferD3D9()
{
    Unbind();
}

void VertexBufferD3D9::Enable(const unsigned int offset)
{
    assert(offset < GetElementCount());

    //Enable the proper vertex format for our vertex buffer
    assert(m_pVertexFormat != nullptr);
    m_pVertexFormat->Enable();

    if (m_pIndexBuffer)
        m_pIndexBuffer->Enable();

    IDirect3DDevice9* device = RendererD3D9::GetInstance()->GetDevice();
    HRESULT hr = device->SetStreamSource(0, m_pVertexBuffer, offset * m_nElementSize, m_pVertexFormat->GetStride());
    S3D_VALIDATE_HRESULT(hr);
}

void VertexBufferD3D9::Disable()
{
    IDirect3DDevice9* device = RendererD3D9::GetInstance()->GetDevice();
    HRESULT hr;

#ifdef _DEBUG
    //Check to see if this vertex buffer is the one currently enabled
    IDirect3DVertexBuffer9* dbgBuffer = 0;
    unsigned int dbgOffset = 0;
    unsigned int dbgStride = 0;
    hr = device->GetStreamSource(0, &dbgBuffer, &dbgOffset, &dbgStride);
    assert(hr == D3D_OK);
    assert(dbgBuffer == m_pVertexBuffer);
    ULONG refCount = 1;
    refCount = dbgBuffer->Release();
    assert(refCount == 1);
#endif
    
    hr = device->SetStreamSource(0, 0, 0, 0);
    S3D_VALIDATE_HRESULT(hr);

    if (m_pIndexBuffer)
        m_pIndexBuffer->Disable();

    //Disable our vertex format
    assert(m_pVertexFormat != nullptr);
    m_pVertexFormat->Disable();
}

void VertexBufferD3D9::Lock(const BufferLocking lockMode)
{
    //The pointer to the locked data is saved for future use
    assert(m_pTempBuffer == nullptr);
    HRESULT hr = m_pVertexBuffer->Lock(0, 0, &m_pTempBuffer, BufferLockingD3D9[lockMode]);
    S3D_VALIDATE_HRESULT(hr);
}

void VertexBufferD3D9::Unlock()
{
    //Unlock the vertex data
    assert(m_pTempBuffer != nullptr);
    HRESULT hr = m_pVertexBuffer->Unlock();
    S3D_VALIDATE_HRESULT(hr);
    m_pTempBuffer = nullptr;
}

void VertexBufferD3D9::Update()
{
    //Copy the local changes to our vertex buffer to where the locked data is
    assert(m_pTempBuffer != nullptr);
    memcpy(m_pTempBuffer, GetData(), GetSize());
}

void VertexBufferD3D9::Bind()
{
    IDirect3DDevice9* device = RendererD3D9::GetInstance()->GetDevice();
    HRESULT hr = device->CreateVertexBuffer((UINT)m_nSize, BufferUsageD3D9[m_eBufferUsage], 0, D3DPOOL_DEFAULT, &m_pVertexBuffer, 0);
    S3D_VALIDATE_HRESULT(hr);

    Lock(BL_WRITE_ONLY);
    Update();
    Unlock();
}

void VertexBufferD3D9::Unbind()
{
    ULONG refCount = 0;
    if(m_pVertexBuffer)
        refCount = m_pVertexBuffer->Release();
    assert(refCount == 0);
    m_pVertexBuffer = nullptr;
}