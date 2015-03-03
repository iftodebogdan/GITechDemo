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
#ifndef VERTEXFORMATDX9_H
#define VERTEXFORMATDX9_H

#include <d3d9.h>
#include "VertexFormat.h"

namespace LibRendererDll
{
	//This is the DX9 implementation of the VertexFormat class
	class VertexFormatDX9 : public VertexFormat
	{
	public:
		void	Enable();
		void	Disable();
		void	Update();

		void	Bind();
		void	Unbind();

	private:
		VertexFormatDX9(const unsigned int attributeCount);
		~VertexFormatDX9();

		D3DVERTEXELEMENT9 m_pVertexElements[VF_MAX_ATTRIBUTES + 1];
		IDirect3DVertexDeclaration9* m_pVertexDeclaration;

		friend class ResourceManagerDX9;
	};
}

#endif //VERTEXFORMATDX9_H
