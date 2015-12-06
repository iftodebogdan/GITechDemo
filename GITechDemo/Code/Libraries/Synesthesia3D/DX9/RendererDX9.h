/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	RendererDX9.h
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

#ifndef RENDERERDX9_H
#define RENDERERDX9_H

#include <d3d9.h>
#include "Renderer.h"
#include "MappingsDX9.h"

namespace Synesthesia3D
{
	class RendererDX9 : public Renderer
	{
		RendererDX9();
		~RendererDX9();

		void	CheckDeviceCaps();
		void	ValidatePresentParameters(D3DPRESENT_PARAMETERS& pp);

		// Used to create the D3DDevice
		IDirect3D9*				m_pD3D;
		// Our rendering device
		IDirect3DDevice9*		m_pd3dDevice;
		D3DPRESENT_PARAMETERS	m_ePresentParameters;
		bool					m_bDeviceLost;
		
	public:
		static	RendererDX9* const	GetInstance() { assert(ms_eAPI == API_DX9); return (RendererDX9*)ms_pInstance; };

		void		Initialize(void* hWnd);

		const bool			SetScreenResolution(const Vec2i size, const Vec2i offset = Vec2i(0, 0), const bool fullscreen = false);
		const Vec2i			GetScreenResolution() const { return Vec2i(m_ePresentParameters.BackBufferWidth, m_ePresentParameters.BackBufferHeight); }
		const PixelFormat	GetBackBufferFormat() const { return MatchPixelFormat(m_ePresentParameters.BackBufferFormat); }

		void		SetViewport(const Vec2i size, const Vec2i offset = Vec2i(0, 0));
		const bool	IsFullscreen() const { return !m_ePresentParameters.Windowed; }

		void		CreatePerspectiveMatrix(Matrix44f& matProj, const float fovYRad, const float aspectRatio, const float zNear, const float zFar) const;
		void		CreateOrthographicMatrix(Matrix44f& matProj, const float left, const float top, const float right, const float bottom, const float zNear, const float zFar) const;

		const bool	BeginFrame();
		void		EndFrame();
		void		SwapBuffers();
		void		Clear(const Vec4f rgba, const float z, const unsigned int stencil);
		void		DrawVertexBuffer(VertexBuffer* vb);

		void		PushProfileMarker(const char* const label);
		void		PopProfileMarker();

		IDirect3DDevice9* const	GetDevice() const { return m_pd3dDevice; };
		IDirect3D9* const		GetDriver() const { return m_pD3D; }
		const bool		 		IsDeviceLost() const { return m_bDeviceLost; }

		friend class Renderer;
	};
}

#endif	//RENDERDX9_H
