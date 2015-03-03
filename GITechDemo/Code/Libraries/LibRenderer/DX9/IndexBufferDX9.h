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
#ifndef INDEXBUFFERDX9_H
#define INDEXBUFFERDX9_H

#include <d3d9.h>
#include "IndexBuffer.h"

namespace LibRendererDll
{
	class IndexBufferDX9 : public IndexBuffer
	{
	public:
		void	Enable();
		void	Disable();
		void	Lock(const BufferLocking lockMode);
		void	Unlock();
		void	Update();

		void	Bind();
		void	Unbind();

	private:
		IndexBufferDX9::IndexBufferDX9(
			const unsigned int indexCount, const IndexBufferFormat indexFormat,
			const BufferUsage usage = BU_STATIC);
		IndexBufferDX9::~IndexBufferDX9();

		IDirect3DIndexBuffer9* m_pIndexBuffer;

		// A temporary pointer used in the Lock->Update->Unlock flow
		void*	m_pTempBuffer;

		friend class ResourceManagerDX9;
	};
}

#endif //INDEXBUFFERDX9_H
