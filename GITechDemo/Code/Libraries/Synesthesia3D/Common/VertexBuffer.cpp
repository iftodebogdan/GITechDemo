/**
 *	@file		VertexBuffer.cpp
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

#include "VertexBuffer.h"
#include "VertexFormat.h"
#include "IndexBuffer.h"
using namespace Synesthesia3D;

VertexBuffer::VertexBuffer(VertexFormat* const vertexFormat, const unsigned int vertexCount, IndexBuffer* const indexBuffer, const BufferUsage usage)
	: Buffer(vertexCount, vertexFormat->GetStride(), usage)
	, m_pVertexFormat(vertexFormat)
	, m_pIndexBuffer(indexBuffer)
{
	assert(vertexFormat != nullptr);
}

VertexBuffer::~VertexBuffer()
{}

VertexFormat* VertexBuffer::GetVertexFormat() const
{
	return m_pVertexFormat;
}

void VertexBuffer::SetIndexBuffer(IndexBuffer* const indexBuffer)
{
	m_pIndexBuffer = indexBuffer;
}

IndexBuffer* VertexBuffer::GetIndexBuffer() const
{
	return m_pIndexBuffer;
}

const bool VertexBuffer::HasPosition() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_POSITION)
			return true;

	return false;
}

const bool VertexBuffer::HasNormal() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_NORMAL)
			return true;

	return false;
}

const bool VertexBuffer::HasTangent() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_TANGENT)
			return true;

	return false;
}

const bool VertexBuffer::HasBinormal() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_BINORMAL)
			return true;

	return false;
}

const bool VertexBuffer::HasTexCoord(const unsigned int semanticIdx) const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_TEXCOORD
			&& m_pVertexFormat->GetSemanticIndex(i) == semanticIdx)
			return true;

	return false;
}

const bool VertexBuffer::HasColor(const unsigned int semanticIdx) const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_COLOR
			&& m_pVertexFormat->GetSemanticIndex(i) == semanticIdx)
			return true;

	return false;
}

const bool VertexBuffer::HasBlendIndices() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_BLENDINDICES)
			return true;

	return false;
}

const bool VertexBuffer::HasBlendWeight() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_BLENDWEIGHT)
			return true;

	return false;
}
