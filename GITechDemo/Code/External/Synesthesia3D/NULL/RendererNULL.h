/**
 *	@file		RendererNULL.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef RENDERERNULL_H
#define RENDERERNULL_H

#include "Renderer.h"

namespace Synesthesia3D
{
	class RendererNULL : public Renderer
	{
		RendererNULL()
			: Renderer()
			, m_vScreenSize(640, 480) {}
		~RendererNULL() {}

		Vec2i	m_vScreenSize;

	public:
		static	RendererNULL* const	GetInstance() { assert(ms_eAPI == API_NULL); return (RendererNULL*)ms_pInstance; };

		void	Initialize(void* hWnd);
		void	SetViewport(const Vec2i /*size*/, const Vec2i /*offset = Vec2i(0, 0)*/) {}
		void	CreatePerspectiveMatrix(Matrix44f& matProj, const float fovYRad, const float aspectRatio, const float zNear, const float zFar) const;
		void	CreateOrthographicMatrix(Matrix44f& matProj, const float left, const float top, const float right, const float bottom, const float zNear, const float zFar) const;
		
		const bool			SetDisplayResolution(const Vec2i size, const Vec2i offset = Vec2i(0, 0), const bool fullscreen = false, const unsigned int refreshRate = 0, const bool vsync = true) { m_vScreenSize = size; return true; }
		const Vec2i			GetDisplayResolution() const { return m_vScreenSize; }
		const bool			IsFullscreen() const { return true; }
		const bool			GetVSyncStatus() const { return true; };
		const unsigned int	GetDisplayRefreshRate() const { return 0; }
		const PixelFormat	GetBackBufferFormat() const { return PF_NONE; }

		const bool	BeginFrame() { return true; }
		void		EndFrame() {}
		void		SwapBuffers() {}
		void		Clear(const Vec4f /*rgba*/, const float /*z*/, const unsigned int /*stencil*/) {}
		void		DrawVertexBuffer(VertexBuffer* const vb) { Renderer::DrawVertexBuffer(vb); }

		friend class Renderer;
	};
}

#endif	//RENDERNULL_H
