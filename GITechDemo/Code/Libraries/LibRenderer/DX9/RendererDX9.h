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
#ifndef RENDERERDX9_H
#define RENDERERDX9_H

#include <d3d9.h>
#include "Renderer.h"

namespace LibRendererDll
{
	class RendererDX9 : public Renderer
	{
		RendererDX9();
		~RendererDX9();

		// Used to create the D3DDevice
		IDirect3D9*				m_pD3D;
		// Our rendering device
		IDirect3DDevice9*		m_pd3dDevice;
		D3DPRESENT_PARAMETERS	m_ePresentParameters;
		bool					m_bDeviceLost;
		
	public:
		static	RendererDX9*	GetInstance() { assert(m_eAPI == API_DX9); return (RendererDX9*)m_pInstance; };

		void	Initialize(void* hWnd);
		void	SetViewport(const Vec2i size, const Vec2i offset = Vec2i(0, 0));
		void	CreateProjectionMatrix(Matrix44f& matProj, float fovYRad, float aspectRatio, float zNear, float zFar);

		const bool	BeginFrame();
		void		EndFrame();
		void		SwapBuffers();
		void		Clear(const Vec4f rgba, const float z, const unsigned int stencil);
		void		DrawVertexBuffer(VertexBuffer* vb);

		IDirect3DDevice9*	GetDevice() const { return m_pd3dDevice; };
		IDirect3D9*		 	GetDriver() const { return m_pD3D; }
		const bool		 	IsDeviceLost() const { return m_bDeviceLost; }

		friend class Renderer;
	};
}

#endif	//RENDERDX9_H
