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
#ifndef VERTEXBUFFERNULL_H
#define VERTEXBUFFERNULL_H

#include "VertexBuffer.h"

namespace LibRendererDll
{
	class VertexFormatNULL;
	class IndexBufferNULL;

	//This is the NULL implementation of the VertexBuffer class
	class VertexBufferNULL : public VertexBuffer
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
		VertexBufferNULL(
			VertexFormatNULL* const vertexFormat, const unsigned int vertexCount,
			IndexBufferNULL* const indexBuffer = nullptr, const BufferUsage usage = BU_STATIC);
		~VertexBufferNULL();

		friend class ResourceManagerNULL;
	};
}

#endif //VERTEXBUFFERNULL_H
