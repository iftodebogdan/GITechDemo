/**
 *	@file		Buffer.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2016 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef BUFFER_H
#define BUFFER_H

#include "ResourceData.h"

namespace Synesthesia3D
{
	/**
	 * @brief	Base class which manages memory buffers.
	 */
	class Buffer
	{

	public:

		/**
		 * @brief	Returns the number of elements (vertices, indices, pixels, etc.) in the buffer.
		 */
		SYNESTHESIA3D_DLL const unsigned int	GetElementCount() const;

		/**
		 * @brief	Returns the size, in bytes, of an element.
		 */
		SYNESTHESIA3D_DLL const unsigned int	GetElementSize() const;

		/**
		 * @brief	Returns the usage option of the buffer.
		 */
		SYNESTHESIA3D_DLL const BufferUsage		GetUsage() const;

		/**
		 * @brief	Returns the size, in bytes, of the entire buffer.
		 */
		SYNESTHESIA3D_DLL const unsigned int	GetSize() const;

		/**
		 * @brief	Returns a pointer to the beginning of the buffer.
		 */
		SYNESTHESIA3D_DLL		byte*			GetData() const;

	protected:

		/**
		 * @brief	Constructor.
		 *
		 * @details	Meant to be used only by child classes.
		 * @see		IndexBuffer
		 * @see		ShaderInput
		 * @see		Texture
		 * @see		VertexBuffer
		 *
		 * @param[in]	elementCount	The number of elements.
		 * @param[in]	elementSize		The size, in bytes, of a single element.
		 * @param[in]	usage			How the resource will be used.
		 */
		Buffer(const unsigned int elementCount, const unsigned int elementSize, const BufferUsage usage);

		/**
		 * @brief	Destructor.
		 *
		 * @details	Meant to be used only by child classes.
		 * @see		IndexBuffer
		 * @see		ShaderInput
		 * @see		Texture
		 * @see		VertexBuffer
		 */
		virtual ~Buffer();

		unsigned int	m_nElementCount;	/**< @brief	Holds the number of elements. */
		unsigned int	m_nElementSize;		/**< @brief	Holds the size in bytes of an element. */
		BufferUsage		m_eBufferUsage;		/**< @brief	Holds the type of usage of the buffer. */
		unsigned int	m_nSize;			/**< @brief	Holds the total size in bytes of the buffer. */
		byte*			m_pData;			/**< @brief	Pointer to the beginning of the buffer. */



		friend class ResourceManager;

		/**
		 * @brief	Serializes buffer data.
		 */
		friend std::ostream& operator<<(std::ostream& output_out, Buffer &buf_in);
		
		/**
		 * @brief	Deserializes buffer data.
		 */
		friend std::istream& operator>>(std::istream& s_in, Buffer &buf_out);
	};
}

#endif //BUFFER_H
