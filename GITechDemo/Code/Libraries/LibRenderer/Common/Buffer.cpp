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

#include "Renderer.h"
#include "Buffer.h"
using namespace LibRendererDll;

Buffer::Buffer(const unsigned int elementCount, const unsigned int elementSize, const BufferUsage usage)
	: m_nElementCount(elementCount)
	, m_nElementSize(elementSize)
	, m_eBufferUsage(usage)
	, m_nSize(elementCount * elementSize)
	, m_pData(nullptr)
{
	assert(elementCount >= 0);
	assert(elementSize >= 0);
	assert(usage >= 0 && usage < BU_MAX);

	// In order to save some memory, do not allocate render target data in main
	// memory, unless it is actually required (e.g. RenderTarget::CopyColorBuffer() )
	if (elementCount > 0 && elementSize > 0 && usage != BU_RENDERTAGET)
	{
		m_pData = new byte[m_nSize];
		assert(m_pData != nullptr);
		memset(m_pData, 0, m_nSize);
	}
}

Buffer::~Buffer()
{
	delete[] m_pData;
}

const unsigned int Buffer::GetElementCount() const
{
	return m_nElementCount;
}

const unsigned int Buffer::GetElementSize() const
{
	return m_nElementSize;
}

const BufferUsage Buffer::GetUsage() const
{
	return m_eBufferUsage;
}

const unsigned int Buffer::GetSize() const
{
	return m_nSize;
}

byte* Buffer::GetData() const
{
	return m_pData;
}
