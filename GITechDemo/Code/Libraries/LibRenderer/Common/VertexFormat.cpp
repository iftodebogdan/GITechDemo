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
#include "VertexFormat.h"
using namespace LibRendererDll;

//This holds the size in bytes for each vertex attribute type
const unsigned int VertexFormat::VertexAttributeTypeSize[VAT_MAX] =
{
	0,  // AT_NONE
	4,  // AT_FLOAT1
	8,  // AT_FLOAT2
	12, // AT_FLOAT3
	16, // AT_FLOAT4
	4,  // AT_HALF2
	8,  // AT_HALF4
	4,  // AT_UBYTE4
	4,  // AT_SHORT2
	8   // AT_SHORT4
};

VertexFormat::VertexFormat(const unsigned int attributeCount)
	: m_nAttributeCount(attributeCount)
	, m_nStride(0)
{
	m_pElements = new VertexElement[m_nAttributeCount];

	for (unsigned int i = 0; i < m_nAttributeCount; i++)
	{
		m_pElements[i].nOffset = 0;
		m_pElements[i].eType	= VAT_NONE;
		m_pElements[i].eUsage = VAU_NONE;
		m_pElements[i].nUsageIdx = 0;
	}
}

void VertexFormat::Initialize(const VertexAttributeUsage usage, const VertexAttributeType type, const unsigned int usageIdx, ...)
{
	unsigned int offset = 0;

	SetAttribute(0, offset, usage, type, usageIdx);
	offset += VertexAttributeTypeSize[type];

	va_list args;
	va_start(args, usageIdx);
	for (unsigned int i = 1; i < m_nAttributeCount; i++)
	{
		VertexAttributeUsage tempUsage = va_arg(args, VertexAttributeUsage);
		VertexAttributeType tempType = va_arg(args, VertexAttributeType);
		unsigned int tempUsageIdx = va_arg(args, unsigned int);
		SetAttribute(i, offset, tempUsage, tempType, tempUsageIdx);
		offset += VertexAttributeTypeSize[tempType];
	}
	va_end(args);

	SetStride(offset);
}

VertexFormat::~VertexFormat()
{
	delete[] m_pElements;
}
			 
void VertexFormat::SetAttribute(const unsigned int attrIdx, const unsigned int offset,
	VertexAttributeUsage usage, VertexAttributeType type, unsigned int usageIdx)
{
	assert(attrIdx < m_nAttributeCount);

#ifdef _DEBUG
	if (attrIdx > 0)
		assert(offset > m_pElements[attrIdx - 1].nOffset);
#endif //_DEBUG

	m_pElements[attrIdx].nOffset = offset;
	m_pElements[attrIdx].eType = type;
	m_pElements[attrIdx].eUsage = usage;
	m_pElements[attrIdx].nUsageIdx = usageIdx;
}

const unsigned int VertexFormat::CalculateStride() const
{
	unsigned int stride = 0;
	for (unsigned int i = 0; i < m_nAttributeCount; i++)
		stride += VertexAttributeTypeSize[m_pElements[i].eType];
	return stride;
}
