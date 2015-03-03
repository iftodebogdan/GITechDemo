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
template <typename T>
inline T& VertexBuffer::Position(const unsigned int vertexIdx) const
{
	unsigned int positionOffset = 0;

	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_POSITION)
		{
			positionOffset = m_pVertexFormat->GetOffset(i);
			break;
		}

	return *(T*)(m_pData + positionOffset + vertexIdx * GetVertexFormat()->GetStride());
}

inline const bool VertexBuffer::HasPosition() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_POSITION)
			return true;

	return false;
}

template <typename T>
inline T& VertexBuffer::Normal(const unsigned int vertexIdx) const
{
	unsigned int normalOffset = 0;

	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_NORMAL)
		{
			normalOffset = m_pVertexFormat->GetOffset(i);
			break;
		}

	return *(T*)(m_pData + normalOffset + vertexIdx * GetVertexFormat()->GetStride());
}

inline const bool VertexBuffer::HasNormal() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_NORMAL)
			return true;

	return false;
}

template <typename T>
inline T& VertexBuffer::Tangent(const unsigned int vertexIdx) const
{
	unsigned int tangentOffset = 0;

	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_TANGENT)
		{
			tangentOffset = m_pVertexFormat->GetOffset(i);
			break;
		}

	return *(T*)(m_pData + tangentOffset + vertexIdx * GetVertexFormat()->GetStride());
}

inline const bool VertexBuffer::HasTangent() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_TANGENT)
			return true;

	return false;
}

template <typename T>
inline T& VertexBuffer::Binormal(const unsigned int vertexIdx) const
{
	unsigned int binormalOffset = 0;

	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_BINORMAL)
		{
			binormalOffset = m_pVertexFormat->GetOffset(i);
			break;
		}

	return *(T*)(m_pData + binormalOffset + vertexIdx * GetVertexFormat()->GetStride());
}

inline const bool VertexBuffer::HasBinormal() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_BINORMAL)
			return true;

	return false;
}

template <typename T>
inline T& VertexBuffer::TexCoord(const unsigned int vertexIdx, const unsigned int usageIdx) const
{
	unsigned int tcoordOffset = 0;

	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_TEXCOORD
			&& m_pVertexFormat->GetUsageIndex(i) == usageIdx)
		{
			tcoordOffset = m_pVertexFormat->GetOffset(i);
			break;
		}

	return *(T*)(m_pData + tcoordOffset + vertexIdx * GetVertexFormat()->GetStride());
}

inline const bool VertexBuffer::HasTexCoord(const unsigned int usageIdx) const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_TEXCOORD
			&& m_pVertexFormat->GetUsageIndex(i) == usageIdx)
			return true;

	return false;
}

template <typename T>
inline T& VertexBuffer::Color(const unsigned int vertexIdx, const unsigned int usageIdx) const
{
	unsigned int colorOffset = 0;

	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_COLOR
			&& m_pVertexFormat->GetUsageIndex(i) == usageIdx)
		{
			colorOffset = m_pVertexFormat->GetOffset(i);
			break;
		}

	return *(T*)(m_pData + colorOffset + vertexIdx * GetVertexFormat()->GetStride());
}

inline const bool VertexBuffer::HasColor(const unsigned int usageIdx) const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_COLOR
			&& m_pVertexFormat->GetUsageIndex(i) == usageIdx)
			return true;

	return false;
}

template <typename T>
inline T& VertexBuffer::BlendIndices(const unsigned int vertexIdx) const
{
	unsigned int blendIndOffset = 0;

	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_BLENDINDICES)
		{
			blendIndOffset = m_pVertexFormat->GetOffset(i);
			break;
		}

	return *(T*)(m_pData + blendIndOffset + vertexIdx * GetVertexFormat()->GetStride());
}

inline const bool VertexBuffer::HasBlendIndices() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_BLENDINDICES)
			return true;

	return false;
}

template <typename T>
inline T& VertexBuffer::BlendWeight(const unsigned int vertexIdx) const
{
	unsigned int blendWeightOffset = 0;

	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_BLENDWEIGHT)
		{
			blendWeightOffset = m_pVertexFormat->GetOffset(i);
			break;
		}

	return *(T*)(m_pData + blendWeightOffset + vertexIdx * GetVertexFormat()->GetStride());
}

inline const bool VertexBuffer::HasBlendWeight() const
{
	for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
		if (m_pVertexFormat->GetAttributeUsage(i) == VAU_BLENDWEIGHT)
			return true;

	return false;
}
