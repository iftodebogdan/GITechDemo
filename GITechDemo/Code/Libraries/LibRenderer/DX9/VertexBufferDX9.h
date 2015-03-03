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
#ifndef VERTEXBUFFERDX9_H
#define VERTEXBUFFERDX9_H

#include <d3d9.h>
#include "VertexBuffer.h"

namespace LibRendererDll
{
	class VertexFormatDX9;
	class IndexBufferDX9;

	//This is the DX9 implementation of the VertexBuffer class
	class VertexBufferDX9 : public VertexBuffer
	{
	public:
		void	Enable(const unsigned int offset = 0);
		void	Disable();
		void	Lock(const BufferLocking lockMode);
		void	Unlock();
		void	Update();

		void	Bind();
		void	Unbind();

	private:
		VertexBufferDX9(
			VertexFormatDX9* const vertexFormat, const unsigned int vertexCount,
			IndexBufferDX9* const indexBuffer = nullptr, const BufferUsage usage = BU_STATIC);
		~VertexBufferDX9();

		IDirect3DVertexBuffer9*		m_pVertexBuffer;

		// A temporary pointer used in the Lock->Update->Unlock flow
		void*						m_pTempBuffer;

		friend class ResourceManagerDX9;
	};
}

#endif //VERTEXBUFFERDX9_H
