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

#include "IndexBuffer.h"
using namespace LibRendererDll;

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
	assert(indexIdx < m_nElementCount);
	memcpy(m_pData + (indexIdx * m_nElementSize), &indexVal, m_nElementSize);
}

void IndexBuffer::SetIndices(const unsigned int indicesVal[], const unsigned int size, const unsigned int offset)
{
	for (unsigned int i = 0; i < size; i++)
		SetIndex(i + offset, indicesVal[i]);
}

std::ostream& LibRendererDll::operator<<(std::ostream& output_out, IndexBuffer& ib_in)
{
	output_out << *(Buffer*)&ib_in;
	output_out.write((const char*)&ib_in.m_eIndexFormat, sizeof(IndexBufferFormat));

	return output_out;
}

std::istream& LibRendererDll::operator>>(std::istream& s_in, IndexBuffer& ib_out)
{
	s_in >> *(Buffer*)&ib_out;
	s_in.read((char*)&ib_out.m_eIndexFormat, sizeof(IndexBufferFormat));

	return s_in;
}
