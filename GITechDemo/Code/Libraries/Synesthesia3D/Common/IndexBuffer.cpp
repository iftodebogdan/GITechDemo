/**
 *	@file		IndexBuffer.cpp
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *	@copyright
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	@copyright
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *	@copyright
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"

#include "IndexBuffer.h"
using namespace Synesthesia3D;

const unsigned int IndexBuffer::IndexBufferFormatSize[IBF_MAX] =
{
	2,  // IBF_INDEX16
	4,  // IBF_INDEX32
};

IndexBuffer::IndexBuffer(const unsigned int indexCount, const IndexBufferFormat indexFormat, const BufferUsage usage)
	: Buffer(indexCount, IndexBufferFormatSize[indexFormat], usage)
	, m_eIndexFormat(indexFormat)
{}

IndexBuffer::~IndexBuffer()
{}

void IndexBuffer::SetIndex(const unsigned int indexIdx, const unsigned int indexVal)
{
	assert(indexIdx < GetElementCount());
	memcpy(GetData() + (indexIdx * GetElementSize()), &indexVal, GetElementSize());
}

void IndexBuffer::SetIndex(const unsigned short indexIdx, const unsigned int indexVal)
{
	assert(indexIdx < GetElementCount());
	memcpy(GetData() + (indexIdx * GetElementSize()), &indexVal, GetElementSize());
}

void IndexBuffer::SetIndices(const unsigned int* const indicesVal, const unsigned int idxCount, const unsigned int offset)
{
	assert(offset + idxCount <= GetElementCount());
	assert(GetIndexFormat() == IBF_INDEX32);

	if(GetIndexFormat() == IBF_INDEX32)
		memcpy(GetData() + (offset * GetElementSize()), indicesVal, idxCount * GetElementSize());
	else
	{
		for (unsigned int i = 0; i < idxCount; i++)
			SetIndex(i + offset, indicesVal[i]);
	}
}

void IndexBuffer::SetIndices(const unsigned short* const indicesVal, const unsigned int idxCount, const unsigned int offset)
{
	assert(offset + idxCount <= GetElementCount());
	assert(GetIndexFormat() == IBF_INDEX16);

	if (GetIndexFormat() == IBF_INDEX16)
		memcpy(GetData() + (offset * GetElementSize()), indicesVal, idxCount * GetElementSize());
	else
	{
		for (unsigned int i = 0; i < idxCount; i++)
			SetIndex(i + offset, indicesVal[i]);
	}
}

const IndexBufferFormat IndexBuffer::GetIndexFormat() const
{
	return m_eIndexFormat;
}

const unsigned int IndexBuffer::GetIndex(const unsigned int indexIdx) const
{
	assert(indexIdx < GetElementCount());
	return (indexIdx < GetElementCount() ? ((unsigned int*)GetData())[indexIdx] : ~0u);
}
