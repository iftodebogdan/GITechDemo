/**
 * @file        VertexBuffer.inl
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

namespace Synesthesia3D
{
    template <typename T>
    inline T& VertexBuffer::Position(const unsigned int vertexIdx) const
    {
        unsigned int positionOffset = 0;

        for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
            if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_POSITION)
            {
                positionOffset = m_pVertexFormat->GetOffset(i);
                break;
            }

        return *(T*)(m_pData + positionOffset + vertexIdx * GetVertexFormat()->GetStride());
    }

    template <typename T>
    inline T& VertexBuffer::Normal(const unsigned int vertexIdx) const
    {
        unsigned int normalOffset = 0;

        for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
            if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_NORMAL)
            {
                normalOffset = m_pVertexFormat->GetOffset(i);
                break;
            }

        return *(T*)(m_pData + normalOffset + vertexIdx * GetVertexFormat()->GetStride());
    }

    template <typename T>
    inline T& VertexBuffer::Tangent(const unsigned int vertexIdx) const
    {
        unsigned int tangentOffset = 0;

        for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
            if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_TANGENT)
            {
                tangentOffset = m_pVertexFormat->GetOffset(i);
                break;
            }

        return *(T*)(m_pData + tangentOffset + vertexIdx * GetVertexFormat()->GetStride());
    }

    template <typename T>
    inline T& VertexBuffer::Binormal(const unsigned int vertexIdx) const
    {
        unsigned int binormalOffset = 0;

        for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
            if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_BINORMAL)
            {
                binormalOffset = m_pVertexFormat->GetOffset(i);
                break;
            }

        return *(T*)(m_pData + binormalOffset + vertexIdx * GetVertexFormat()->GetStride());
    }

    template <typename T>
    inline T& VertexBuffer::TexCoord(const unsigned int vertexIdx, const unsigned int semanticIdx) const
    {
        unsigned int tcoordOffset = 0;

        for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
            if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_TEXCOORD
                && m_pVertexFormat->GetSemanticIndex(i) == semanticIdx)
            {
                tcoordOffset = m_pVertexFormat->GetOffset(i);
                break;
            }

        return *(T*)(m_pData + tcoordOffset + vertexIdx * GetVertexFormat()->GetStride());
    }

    template <typename T>
    inline T& VertexBuffer::Color(const unsigned int vertexIdx, const unsigned int semanticIdx) const
    {
        unsigned int colorOffset = 0;

        for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
            if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_COLOR
                && m_pVertexFormat->GetSemanticIndex(i) == semanticIdx)
            {
                colorOffset = m_pVertexFormat->GetOffset(i);
                break;
            }

        return *(T*)(m_pData + colorOffset + vertexIdx * GetVertexFormat()->GetStride());
    }

    template <typename T>
    inline T& VertexBuffer::BlendIndices(const unsigned int vertexIdx) const
    {
        unsigned int blendIndOffset = 0;

        for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
            if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_BLENDINDICES)
            {
                blendIndOffset = m_pVertexFormat->GetOffset(i);
                break;
            }

        return *(T*)(m_pData + blendIndOffset + vertexIdx * GetVertexFormat()->GetStride());
    }

    template <typename T>
    inline T& VertexBuffer::BlendWeight(const unsigned int vertexIdx) const
    {
        unsigned int blendWeightOffset = 0;

        for (int i = 0, n = m_pVertexFormat->GetAttributeCount(); i < n; i++)
            if (m_pVertexFormat->GetAttributeSemantic(i) == VAS_BLENDWEIGHT)
            {
                blendWeightOffset = m_pVertexFormat->GetOffset(i);
                break;
            }

        return *(T*)(m_pData + blendWeightOffset + vertexIdx * GetVertexFormat()->GetStride());
    }
}
