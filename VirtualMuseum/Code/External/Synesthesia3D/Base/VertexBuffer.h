/**
 * @file        VertexBuffer.h
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

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "Buffer.h"

namespace Synesthesia3D
{
    class IndexBuffer;
    class VertexFormat;

    /**
     * @brief   Allows the creation and manipulation of a vertex buffer.
     */
    class VertexBuffer : public Buffer
    {

    public:

        /**
         * @brief   Binds the vertex buffer.
         * @note    It also binds the proper vertex format and index buffer (if it exists) associated with the vertex buffer.
         *
         * @param[in]   offset      An offset in the vertex list. Used to render only a subset of the contained vertices.
         */
        virtual SYNESTHESIA3D_DLL       void        Enable(const unsigned int offset = 0) PURE_VIRTUAL;

        /**
         * @brief   Unbinds the vertex buffer.
         * @note    It also unbinds the proper vertex format and index buffer (if it exists) associated with the vertex buffer.
         */
        virtual SYNESTHESIA3D_DLL       void        Disable() PURE_VIRTUAL;

        /**
         * @brief   Locks the buffer so that modifications can be made to its' contents. The general workflow is @ref Lock() -> @ref Update() -> @ref Unlock().
         */
        virtual SYNESTHESIA3D_DLL       void        Lock(const BufferLocking lockMode) PURE_VIRTUAL;

        /**
         * @brief   Unlocks the buffer.
         */
        virtual SYNESTHESIA3D_DLL       void        Unlock() PURE_VIRTUAL;

        /**
         * @brief   Updates the underlying platform-specific resource's content.
         */
        virtual SYNESTHESIA3D_DLL       void        Update() PURE_VIRTUAL;

        /**
         * @brief   Creates a corresponding platform-specific resource.
         */
        virtual SYNESTHESIA3D_DLL       void        Bind() PURE_VIRTUAL;

        /**
         * @brief   Destroys the platform-specific resource.
         */
        virtual SYNESTHESIA3D_DLL       void        Unbind() PURE_VIRTUAL;

        /**
         * @brief   Retrieves the associated vertex format object.
         */
        SYNESTHESIA3D_DLL VertexFormat* GetVertexFormat() const;

        /**
         * @brief   Associates an index buffer object with this vertex buffer.
         */
        SYNESTHESIA3D_DLL void          SetIndexBuffer(IndexBuffer* const indexBuffer);

        /**
         * @brief   Retrieves the associated index buffer object, if one exists.
         */
        SYNESTHESIA3D_DLL IndexBuffer*  GetIndexBuffer() const;

        /**
         * @brief   Position attribute accessor.
         */
        template <typename T>
            inline          T&          Position(const unsigned int vertexIdx) const;

        /**
         * @brief   Checks for the existence of the position attribute.
         */
        SYNESTHESIA3D_DLL const bool    HasPosition() const;
        
        /**
         * @brief   Normal attribute accessor.
         */
        template <typename T>
            inline          T&          Normal(const unsigned int vertexIdx) const;

        /**
         * @brief   Checks for the existence of the normal attribute.
         */
        SYNESTHESIA3D_DLL const bool    HasNormal() const;
        
        /**
         * @brief   Tangent attribute accessor.
         */
        template <typename T>
            inline          T&          Tangent(const unsigned int vertexIdx) const;

        /**
         * @brief   Checks for the existence of the tangent attribute.
         */
        SYNESTHESIA3D_DLL const bool    HasTangent() const;

        /**
         * @brief   Binormal attribute accessor.
         */
        template <typename T>
            inline          T&          Binormal(const unsigned int vertexIdx) const;
            
        /**
         * @brief   Checks for the existence of the binormal attribute.
         */
        SYNESTHESIA3D_DLL const bool    HasBinormal() const;
        
        /**
         * @brief   Texture coordinate attribute accessor.
         */
        template <typename T>
            inline          T&          TexCoord(const unsigned int vertexIdx, const unsigned int semanticIdx) const;
            
        /**
         * @brief   Checks for the existence of the texture coordinate attribute.
         */
        SYNESTHESIA3D_DLL const bool    HasTexCoord(const unsigned int semanticIdx) const;
        
        /**
         * @brief   Color attribute accessor.
         */
        template <typename T>
            inline          T&          Color(const unsigned int vertexIdx, const unsigned int semanticIdx) const;
            
        /**
         * @brief   Checks for the existence of the color attribute.
         */
        SYNESTHESIA3D_DLL const bool    HasColor(const unsigned int semanticIdx) const;
        
        /**
         * @brief   Blend indices attribute accessor.
         */
        template <typename T>
            inline          T&          BlendIndices(const unsigned int vertexIdx) const;
            
        /**
         * @brief   Checks for the existence of the blend indices attribute.
         */
        SYNESTHESIA3D_DLL const bool    HasBlendIndices() const;
        
        /**
         * @brief   Blend weights attribute accessor.
         */
        template <typename T>
            inline          T&          BlendWeight(const unsigned int vertexIdx) const;
            
        /**
         * @brief   Checks for the existence of the blend weights attribute.
         */
        SYNESTHESIA3D_DLL const bool    HasBlendWeight() const;

    protected:

        /**
         * @brief   Constructor.
         *
         * @details Meant to be used only by @ref ResourceManager.
         * @see     ResourceManager::CreateVertexBuffer()
         *
         * @param[in]   vertexFormat    The format of the vertex buffer. See @ref VertexFormat.
         * @param[in]   vertexCount     The number of vertices contained in the vertex buffer.
         * @param[in]   indexBuffer     The associated index buffer. See @ref IndexBuffer.
         * @param[in]   usage           The usage mode. See @ref BufferUsage.
         */
        VertexBuffer(
            VertexFormat* const vertexFormat, const unsigned int vertexCount,
            IndexBuffer* const indexBuffer = nullptr, const BufferUsage usage = BU_STATIC);
        
        /**
         * @brief   Destructor.
         *
         * @details Meant to be used only by @ref ResourceManager.
         * @see     ResourceManager::ReleaseVertexBuffer()
         */
        virtual ~VertexBuffer();

        VertexFormat*   m_pVertexFormat;    /**< @brief Holds a pointer to the associated vertex format. */
        IndexBuffer*    m_pIndexBuffer;     /**< @brief Holds a pointer to the associated index buffer. */

        friend class ResourceManager;

        /**
         * @brief   Serializes the vertex buffer object.
         */
        friend std::ostream& operator<<(std::ostream& output_out, VertexBuffer &vb_in);
        
        /**
         * @brief   Deserializes the vertex buffer object.
         */
        friend std::istream& operator>>(std::istream& s_in, VertexBuffer &vb_out);
    };
}

//Include our inline functions file
#include "VertexBuffer.inl"

#endif //VERTEXBUFFER_H
