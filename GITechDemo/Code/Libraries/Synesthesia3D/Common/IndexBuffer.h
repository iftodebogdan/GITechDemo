/**
 *	@file		IndexBuffer.h
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

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "Buffer.h"

namespace Synesthesia3D
{
	/**
	 * @brief	Index buffer resource class that stores vertex order information.
	 */
	class IndexBuffer : public Buffer
	{

	public:

		/**
		 * @brief	Binds the index buffer to the current rendering context.
		 */
		virtual SYNESTHESIA3D_DLL			void			Enable() PURE_VIRTUAL;

		/**
		 * @brief	Unbinds the index buffer.
		 */
		virtual SYNESTHESIA3D_DLL			void			Disable() PURE_VIRTUAL;

		/**
		 * @brief	Locks the index buffer for reading/writing.
		 * 
		 * @param	lockMode	Specifies the type of operation for which the resource is locked.
		 *
		 * @note	The resource update flow is: @ref Lock() > @ref SetIndex()/@ref SetIndices() > @ref Update() > @ref Unlock()
		 */
		virtual SYNESTHESIA3D_DLL			void			Lock(const BufferLocking lockMode) PURE_VIRTUAL;

		/**
		 * @brief	Unlocks the index buffer.
		 *
		 * @note	The resource update flow is: @ref Lock() > @ref SetIndex()/@ref SetIndices() > @ref Update() > @ref Unlock()
		 */
		virtual SYNESTHESIA3D_DLL			void			Unlock() PURE_VIRTUAL;

		/**
		 * @brief	Flushes the user's changes to the resource.
		 *
		 * @details	The user can only access a memory buffer handled by the engine,
		 *			which is a shadow copy of the actual resource (which usually resides
		 *			in GPU accessible memory). As such, the user is required to flush the
		 *			contents of the shadow copy to the corresponding platform-specific resource.
		 *
		 * @note	The resource update flow is: @ref Lock() > @ref SetIndex()/@ref SetIndices() > @ref Update() > @ref Unlock()
		 */
		virtual SYNESTHESIA3D_DLL			void			Update() PURE_VIRTUAL;

		/**
		 * @brief	Creates the corresponding platform specific resource.
		 * @note	Not really of much interest to the average user. Mainly used by the @ref ResourceManager or in the device reset flow.
		 */
		virtual	SYNESTHESIA3D_DLL			void			Bind() PURE_VIRTUAL;

		/**
		 * @brief	Destroys the platform specific resource.
		 * @note	Not really of much interest to the average user. Mainly used by the @ref ResourceManager or in the device reset flow.
		 */
		virtual	SYNESTHESIA3D_DLL			void			Unbind() PURE_VIRTUAL;

		/**
		 * @brief	Sets an index.
		 *
		 * @param[in]	indexIdx	The position at which the index to be modified resides inside the index buffer.
		 * @param[in]	indexVal	The value to be written.
		 *
		 * @note	This function is for use with 32 bit index buffers.
		 */
				SYNESTHESIA3D_DLL			void			SetIndex(const unsigned int indexIdx, const unsigned int indexVal);

		/**
		 * @brief	Sets an index.
		 *
		 * @param[in]	indexIdx	The position at which the index to be modified resides inside the index buffer.
		 * @param[in]	indexVal	The value to be written.
		 *
		 * @note	This function is for use with 16 bit index buffers.
		 */
				SYNESTHESIA3D_DLL			void			SetIndex(const unsigned short indexIdx, const unsigned int indexVal);

		/**
		 * @brief	Copies several elements to the index buffer from the array, starting from the specified element of the index buffer.
		 *
		 * @param[in]	indicesVal	A pointer to an array of unsigned integer, representing index values.
		 * @param[in]	idxCount	The number of indices to be copied from the provided array.
		 * @param[in]	offset		The element offset in the provided array at which to start copying.
		 *
		 * @note	This function is for use with 32 bit index buffers.
		 */
				SYNESTHESIA3D_DLL			void			SetIndices(const unsigned int* const indicesVal, const unsigned int idxCount, const unsigned int offset = 0u);
				
		/**
		 * @brief	Copies several elements to the index buffer from the array, starting from the specified element of the index buffer.
		 *
		 * @param[in]	indicesVal	A pointer to an array of unsigned integer, representing index values.
		 * @param[in]	idxCount	The number of indices to be copied from the provided array.
		 * @param[in]	offset		The element offset in the provided array at which to start copying.
		 *
		 * @note	This function is for use with 16 bit index buffers.
		 */
				SYNESTHESIA3D_DLL			void			SetIndices(const unsigned short* const indicesVal, const unsigned int idxCount, const unsigned int offset = 0u);

		/**
		 * @brief	Returns the format of the index buffer.
		 */
				SYNESTHESIA3D_DLL const IndexBufferFormat	GetIndexFormat() const;

		/**
		 * @brief	Gets the value of the specified index.
		 */
				SYNESTHESIA3D_DLL	const unsigned int		GetIndex(const unsigned int indexIdx) const;

	protected:

		/**
		 * @brief	Constructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::CreateIndexBuffer()
		 *
		 * @param[in]	indexCount	The number of indices with which the resource is initialized.
		 * @param[in]	indexFormat	The data format of the indices (16 or 32 bit integers).
		 * @param[in]	usage		How the resource will be used.
		 */
		IndexBuffer(const unsigned int indexCount, const IndexBufferFormat indexFormat = IBF_INDEX16, const BufferUsage usage = BU_STATIC);

		/**
		 * @brief	Destructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::ReleaseIndexBuffer()
		 */
		virtual ~IndexBuffer();

		/**
		 * @brief	The format of the index buffer
		 * @see		GetIndexFormat()
		 */
			IndexBufferFormat		m_eIndexFormat;

		/**
		 * @brief	An array containing the sizes, in bytes, of an index of the specified format
		 * @note	For internal usage only.
		 */
		static const unsigned int	IndexBufferFormatSize[IBF_MAX];



		friend class ResourceManager;

		/**
		 * @brief	Serializes the index buffer object.
		 */
		friend std::ostream& operator<<(std::ostream& output_out, IndexBuffer &ib_in);

		/**
		 * @brief	Deserializes the index buffer object.
		 */
		friend std::istream& operator>>(std::istream& s_in, IndexBuffer &ib_out);
	};
}

#endif //INDEXBUFFER_H
