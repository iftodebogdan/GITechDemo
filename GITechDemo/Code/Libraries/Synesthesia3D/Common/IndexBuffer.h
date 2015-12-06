/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	IndexBuffer.h
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "Buffer.h"

namespace Synesthesia3D
{
	// This is the platform independent implementation of the index buffer class
	class IndexBuffer : public Buffer
	{
	public:
		// Enable the index buffer
		virtual SYNESTHESIA3D_DLL void	Enable() PURE_VIRTUAL
		// Disable the index buffer
		virtual SYNESTHESIA3D_DLL void	Disable() PURE_VIRTUAL
		// Lock the index buffer for reading/writing
		virtual SYNESTHESIA3D_DLL void	Lock(const BufferLocking lockMode) PURE_VIRTUAL
		// Unlock the index buffer
		virtual SYNESTHESIA3D_DLL void	Unlock() PURE_VIRTUAL
		// Update the index buffer with the changes made
		virtual SYNESTHESIA3D_DLL void	Update() PURE_VIRTUAL

		// Create a corresponding platform specific resource
		virtual	SYNESTHESIA3D_DLL void	Bind() PURE_VIRTUAL
		// Destroy the platform specific resource
		virtual	SYNESTHESIA3D_DLL void	Unbind() PURE_VIRTUAL

				// Set an index
				SYNESTHESIA3D_DLL void	SetIndex(const unsigned int indexIdx, const unsigned int indexVal);
				// Copy [idxCount] elements to the index buffer from the array, starting from the [offset]-th element of the index buffer
				SYNESTHESIA3D_DLL void	SetIndices(const unsigned int indicesVal[], const unsigned int idxCount, const unsigned int offset = 0);

	protected:
		IndexBuffer(const unsigned int indexCount, const IndexBufferFormat indexFormat = IBF_INDEX16, const BufferUsage usage = BU_STATIC);
		virtual ~IndexBuffer();

		// The format of the index buffer
		IndexBufferFormat			m_eIndexFormat;

		// An array containing the sizes in bytes of indices of the specified format
		static const unsigned int	IndexBufferFormatSize[IBF_MAX];

		friend class ResourceManager;

		friend std::ostream& operator<<(std::ostream& output_out, IndexBuffer &ib_in);
		friend std::istream& operator>>(std::istream& s_in, IndexBuffer &ib_out);
	};
}

#endif //INDEXBUFFER_H
