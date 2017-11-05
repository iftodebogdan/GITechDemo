/**
 * @file        VertexFormat.cpp
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

#include "VertexFormat.h"
using namespace Synesthesia3D;

//This holds the size in bytes for each vertex attribute type
const unsigned int VertexFormat::VertexAttributeTypeSize[VAT_MAX] =
{
    0,      // AT_NONE
    4,      // AT_FLOAT1
    8,      // AT_FLOAT2
    12,     // AT_FLOAT3
    16,     // AT_FLOAT4
    4,      // AT_HALF2
    8,      // AT_HALF4
    4,      // AT_UBYTE4
    4,      // AT_SHORT2
    8       // AT_SHORT4
};

VertexFormat::VertexFormat(const unsigned int attributeCount)
    : m_nAttributeCount(attributeCount)
    , m_nStride(0)
{
    m_pElements = new VertexElement[m_nAttributeCount];

    for (unsigned int i = 0; i < m_nAttributeCount; i++)
    {
        m_pElements[i].nOffset = 0;
        m_pElements[i].eType    = VAT_NONE;
        m_pElements[i].eSemantic = VAS_NONE;
        m_pElements[i].nSemanticIdx = 0;
    }
}

void VertexFormat::Initialize(const VertexAttributeSemantic semantic, const VertexAttributeType type, const unsigned int semanticIdx, ...)
{
    unsigned int offset = 0;

    SetAttribute(0, offset, semantic, type, semanticIdx);
    offset += VertexAttributeTypeSize[type];

    va_list args;
    va_start(args, semanticIdx);
    for (unsigned int i = 1; i < m_nAttributeCount; i++)
    {
        VertexAttributeSemantic tempSemantic = (VertexAttributeSemantic)va_arg(args, int);
        VertexAttributeType tempType = (VertexAttributeType)va_arg(args, int);
        unsigned int tempSemanticIdx = va_arg(args, unsigned int);
        SetAttribute(i, offset, tempSemantic, tempType, tempSemanticIdx);
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
    const VertexAttributeSemantic semantic, const VertexAttributeType type, const unsigned int semanticIdx)
{
    assert(attrIdx < m_nAttributeCount);

#ifdef _DEBUG
    if (attrIdx > 0)
        assert(offset > m_pElements[attrIdx - 1].nOffset);
#endif //_DEBUG

    m_pElements[attrIdx].nOffset = offset;
    m_pElements[attrIdx].eType = type;
    m_pElements[attrIdx].eSemantic = semantic;
    m_pElements[attrIdx].nSemanticIdx = semanticIdx;
}

const unsigned int VertexFormat::CalculateStride() const
{
    unsigned int stride = 0;
    for (unsigned int i = 0; i < m_nAttributeCount; i++)
        stride += VertexAttributeTypeSize[m_pElements[i].eType];

    return stride;
}

void VertexFormat::SetStride(const unsigned int stride)
{
    m_nStride = stride;
}

const unsigned int VertexFormat::GetAttributeCount() const
{
    return m_nAttributeCount;
}

const unsigned int VertexFormat::GetOffset(const unsigned int attrIdx) const
{
    assert(attrIdx < m_nAttributeCount);
    return m_pElements[attrIdx].nOffset;
}

const VertexAttributeType VertexFormat::GetAttributeType(const unsigned int attrIdx) const
{
    assert(attrIdx < m_nAttributeCount);
    return m_pElements[attrIdx].eType;
}

const VertexAttributeSemantic VertexFormat::GetAttributeSemantic(const unsigned int attrIdx) const
{
    assert(attrIdx < m_nAttributeCount);
    return m_pElements[attrIdx].eSemantic;
}

const unsigned int VertexFormat::GetSemanticIndex(const unsigned int attrIdx) const
{
    assert(attrIdx < m_nAttributeCount);
    return m_pElements[attrIdx].nSemanticIdx;
}

const unsigned int VertexFormat::GetStride() const
{
    return m_nStride;
}

const unsigned int VertexFormat::GetAttributeTypeSize(const VertexAttributeType type)
{
    assert(type < VAT_MAX);
    return VertexAttributeTypeSize[type];
}
