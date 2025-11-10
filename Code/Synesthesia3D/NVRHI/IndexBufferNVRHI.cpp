/**
 * @file        IndexBufferNVRHI.cpp
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

#include "IndexBufferNVRHI.h"
using namespace Synesthesia3D;

#if ENABLE_NVRHI

#include "RendererNVRHI.h"

IndexBufferNVRHI::IndexBufferNVRHI(const unsigned int indexCount, const IndexBufferFormat indexFormat, const BufferUsage usage)
    : IndexBuffer(indexCount, indexFormat, usage)
{
    if (indexCount != 0)
        Bind();
}

IndexBufferNVRHI::~IndexBufferNVRHI()
{
    Unbind();
}

void IndexBufferNVRHI::Enable()
{

}

void IndexBufferNVRHI::Disable()
{

}

void IndexBufferNVRHI::Lock(const BufferLocking lockMode)
{

}

void IndexBufferNVRHI::Unlock()
{

}

void IndexBufferNVRHI::Update()
{
    RendererNVRHI::GetInstance()->GetCommandList()->writeBuffer(m_pIndexBuffer, GetData(), GetSize());
}

void IndexBufferNVRHI::Bind()
{
    nvrhi::BufferDesc indexBufferDesc;
    indexBufferDesc
        .setByteSize(GetSize())
        .setIsIndexBuffer(true)
        .setInitialState(nvrhi::ResourceStates::IndexBuffer)
        .setKeepInitialState(true)
        .setDebugName("IndexBufferNVRHI");
    m_pIndexBuffer = RendererNVRHI::GetInstance()->GetDevice()->createBuffer(indexBufferDesc);
}

void IndexBufferNVRHI::Unbind()
{

}

#endif // ENABLE_NVRHI
