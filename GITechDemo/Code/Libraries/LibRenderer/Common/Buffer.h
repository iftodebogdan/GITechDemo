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
#ifndef BUFFER_H
#define BUFFER_H

#ifndef LIBRENDERER_DLL
#ifdef LIBRENDERER_EXPORTS
#define LIBRENDERER_DLL __declspec(dllexport) 
#else
#define LIBRENDERER_DLL __declspec(dllimport) 
#endif
#endif //LIBRENDERER_DLL

#include "ResourceData.h"

namespace LibRendererDll
{
	// This is the base class which manages memory buffers
	class Buffer
	{
	public:
		// Returns the number of elements (vertices, indices, pixels, etc.) in the buffer
		LIBRENDERER_DLL const unsigned int	GetElementCount() const { return m_nElementCount; }
		// Returns the size in bytes of an element
		LIBRENDERER_DLL const unsigned int	GetElementSize() const { return m_nElementSize; }
		// Returns the usage option of the buffer
		LIBRENDERER_DLL const BufferUsage	GetUsage() const { return m_eBufferUsage; }
		// Returns the size in bytes of the entire buffer
		LIBRENDERER_DLL const unsigned int	GetSize() const { return m_nSize; }
		// Returns a pointer to the beginning of the buffer
		LIBRENDERER_DLL	byte*				GetData() const { return m_pData; }

	protected:
		Buffer(const unsigned int elementCount, const unsigned int elementSize, const BufferUsage usage);
		virtual ~Buffer();

		unsigned int	m_nElementCount;	// Holds the number of elements
		unsigned int	m_nElementSize;		// Holds the size in bytes of an element
		BufferUsage		m_eBufferUsage;		// Holds the type of usage of the buffer
		unsigned int	m_nSize;			// Holds the total size in bytes of the buffer
		byte*			m_pData;			// Pointer to the beginning of the buffer

		friend class ResourceManager;

		friend std::ostream& operator<<(std::ostream& output_out, Buffer &buf_in);
		friend std::istream& operator>>(std::istream& s_in, Buffer &buf_out);
	};
}

#endif //BUFFER_H
