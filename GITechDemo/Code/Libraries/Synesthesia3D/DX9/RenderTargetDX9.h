/**
 *	@file		RenderTargetDX9.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef RENDERTARGETDX9_H
#define RENDERTARGETDX9_H

#include <d3d9.h>

#include "RenderTarget.h"

namespace Synesthesia3D
{
	class RenderTargetDX9 : public RenderTarget
	{
	public:
		void	Enable();
		void	Disable();
		void	CopyColorBuffer(const unsigned int colorBufferIdx, Texture* texture);

		void	Bind();
		void	Unbind();

	protected:
		RenderTargetDX9(const unsigned int targetCount, PixelFormat pixelFormat,
			const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);
		RenderTargetDX9(const unsigned int targetCount, PixelFormat pixelFormat,
			const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);
		RenderTargetDX9(const unsigned int targetCount,
			PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
			const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);
		RenderTargetDX9(const unsigned int targetCount,
			PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
			const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);
		~RenderTargetDX9();

		IDirect3DSurface9**		m_pColorSurface;
		IDirect3DSurface9*		m_pDepthSurface;

		// These are used in the Enable/Disable() flow in order to
		// save and restore render target 0 (i.e. the backbuffer)
		static IDirect3DSurface9*	ms_pBBColorSurfBkp;
		static IDirect3DSurface9*	ms_pBBDepthSurfBkp;

		friend class ResourceManagerDX9;
	};
}

#endif // RENDERTARGETDX9_H