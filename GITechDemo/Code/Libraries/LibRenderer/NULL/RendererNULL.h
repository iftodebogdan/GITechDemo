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
#ifndef RENDERERNULL_H
#define RENDERERNULL_H

#include "Renderer.h"

namespace LibRendererDll
{
	class RendererNULL : public Renderer
	{
		RendererNULL()
			: Renderer() {}
		~RendererNULL() {}

	public:
		static	RendererNULL*	GetInstance() { assert(ms_eAPI == API_NULL); return (RendererNULL*)ms_pInstance; };

		void	Initialize(void* hWnd);
		void	SetBackBufferSize(const Vec2i size, const Vec2i offset = Vec2i(0, 0)) {}
		void	SetViewport(const Vec2i size, const Vec2i offset = Vec2i(0, 0)) {}
		void	CreatePerspectiveMatrix(Matrix44f& matProj, float fovYRad, float aspectRatio, float zNear, float zFar) {}
		void	CreateOrthographicMatrix(Matrix44f& matProj, float left, float top, float right, float bottom, float zNear, float zFar) {}

		const bool	BeginFrame() { return true; }
		void		EndFrame() {}
		void		SwapBuffers() {}
		void		Clear(const Vec4f rgba, const float z, const unsigned int stencil) {}
		void		DrawVertexBuffer(VertexBuffer* vb) {}

		void		PushProfileMarker(const char* const label) {}
		void		PopProfileMarker() {}

		friend class Renderer;
	};
}

#endif	//RENDERNULL_H
