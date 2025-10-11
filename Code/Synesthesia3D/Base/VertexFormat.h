/**
 * @file        VertexFormat.h
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

#ifndef VERTEXFORMAT_H
#define VERTEXFORMAT_H

#include "ResourceData.h"

namespace Synesthesia3D
{
    /**
     * @brief   Allows the definition of a vertex format for a vertex buffer.
     */
    class VertexFormat
    {

    public:

        /**
         * @brief   Sets each vertex format attribute's properties.
         *
         * @param[in]   semantic    The purpose of the attribute, it's semantic.
         * @param[in]   type        The data type of the attribute.
         * @param[in]   semanticIdx The attribute's semantic index. For example, there can be multiple @ref VAS_TEXCOORD attributes, but they must have different indices.
         */
                SYNESTHESIA3D_DLL           void                Initialize(const VertexAttributeSemantic semantic, const VertexAttributeType type, const unsigned int semanticIdx, ...);

        /**
         * @brief   Sets a single attribute's properties.
         * @note    After modifying attributes' properties, @ref SetStride() must be called to set the proper vertex stride. To obtain the correct vertex stride, you can use @ref CalculateStride().
         *
         * @param[in]   attrIdx     The index of the attribute.
         * @param[in]   offset      The memory offset of the attribute.
         * @param[in]   semantic    The purpose of the attribute, it's semantic.
         * @param[in]   type        The data type of the attribute.
         * @param[in]   semanticIdx The attribute's semantic index. For example, there can be multiple @ref VAS_TEXCOORD attributes, but they must have different indices.
         */
                SYNESTHESIA3D_DLL           void                SetAttribute(const unsigned int attrIdx, const unsigned int offset, const VertexAttributeSemantic semantic, const VertexAttributeType type, const unsigned int semanticIdx);

        /**
         * @brief   Sets the vertex format stride.
         * @note    To obtain the correct vertex format stride, based on the current attribute configuration, use @ref CalculateStride().
         *
         * @param[in]   stride      The stride of the vertex format.
         */
                SYNESTHESIA3D_DLL           void                SetStride(const unsigned int stride);

        /**
         * @brief   Calculates the correct vertex format stride based on the current attribute configuration.
         */
                SYNESTHESIA3D_DLL   const   unsigned int        CalculateStride() const;

        /**
         * @brief   Retrives the number of attributes contained in this vertex format object.
         */
                SYNESTHESIA3D_DLL   const   unsigned int        GetAttributeCount() const;

        /**
         * @brief   Retrieves the offset, in bytes, in the vertex format buffer of the attribute with the specified index.
         */
                SYNESTHESIA3D_DLL   const   unsigned int        GetOffset(const unsigned int attrIdx) const;

        /**
         * @brief   Retrieves the data type of the attribute with the specified index.
         */
                SYNESTHESIA3D_DLL const VertexAttributeType     GetAttributeType(const unsigned int attrIdx) const;

        /**
         * @brief   Retrieves the semantic of the attribute with the specified index.
         */
                SYNESTHESIA3D_DLL const VertexAttributeSemantic GetAttributeSemantic(const unsigned int attrIdx) const;

        /**
         * @brief   Retrieves the semantic index of the attributes with the specified index.
         */
                SYNESTHESIA3D_DLL   const   unsigned int        GetSemanticIndex(const unsigned int attrIdx) const;

        /**
         * @brief   Retrieves the vertex format's stride.
         */
                SYNESTHESIA3D_DLL   const   unsigned int        GetStride() const;

        /**
         * @brief   Retrieves the size of a specified attribute type.
         *
         * @param[in]   type    The type of the vertex attribute.
         *
         * @return  The size of the specified attribute type.
         */
        static  SYNESTHESIA3D_DLL   const   unsigned int        GetAttributeTypeSize(const VertexAttributeType type);



        /**
         * @brief   Binds the vertex format object.
         * @note    Vertex format object are automatically bound by their respective vertex buffer objects.
         */
        virtual SYNESTHESIA3D_DLL void  Enable() PURE_VIRTUAL;

        /**
         * @brief   Unbinds the vertex format object.
         */
        virtual SYNESTHESIA3D_DLL void  Disable() PURE_VIRTUAL;

        /**
         * @brief   Updates the vertex format object with the changes made.
         * @note    Unlike other buffer types, vertex formats do not have a lock-unlock mechanic.
         */
        virtual SYNESTHESIA3D_DLL void  Update() PURE_VIRTUAL;

        /**
         * @brief   Creates a corresponding platform specific resource.
         */
        virtual SYNESTHESIA3D_DLL void  Bind() PURE_VIRTUAL;

        /**
         * @brief   Destroys the platform specific resource.
         */
        virtual SYNESTHESIA3D_DLL void  Unbind() PURE_VIRTUAL;

    protected:

        /**
         * @brief   Constructor.
         *
         * @details Meant to be used only by @ref ResourceManager.
         * @see     ResourceManager::CreateVertexFormat()
         *
         * @param[in]   attributeCount  The number of attributes contained in this vertex format.
         *
         * @note    The number of attributes cannot be changed later. Each attribute can be initialized separately using SetAttribute(), or all at once with Initialize().
         */
        VertexFormat(const unsigned int attributeCount);
        
        /**
         * @brief   Destructor.
         *
         * @details Meant to be used only by @ref ResourceManager.
         * @see     ResourceManager::ReleaseVertexFormat()
         */
        virtual     ~VertexFormat();

        unsigned int    m_nAttributeCount;  /**< @brief The total number of attributes. */
        VertexElement*  m_pElements;        /**< @brief A pointer to the array of elements. */
        unsigned int    m_nStride;          /**< @brief The stride of the vertex format. */

        static const unsigned int VertexAttributeTypeSize[VAT_MAX]; /**< @brief The size, in bytes, of each vertex attribute type. */

        friend class ResourceManager;

        /**
         * @brief   Serializes the vertex format object.
         */
        friend std::ostream& operator<<(std::ostream& output_out, VertexFormat &vf_in);
        
        /**
         * @brief   Deserializes the vertex format object.
         */
        friend std::istream& operator>>(std::istream& s_in, VertexFormat &vf_out);
    };
}

#endif //VERTEXFORMAT_H
