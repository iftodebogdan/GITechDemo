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
#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#ifndef LIBRENDERER_DLL
#ifdef LIBRENDERER_EXPORTS
#define LIBRENDERER_DLL __declspec(dllexport) 
#else
#define LIBRENDERER_DLL __declspec(dllimport) 
#endif
#endif // LIBRENDERER_DLL

#include "ResourceData.h"
#include "Texture.h"

namespace LibRendererDll
{
	// This platform independent class manages render targets
	class RenderTarget
	{
	public:
		// Enable rendering in this RT
		virtual LIBRENDERER_DLL void					Enable() = 0;
		// Disable from rendering in this RT
		virtual LIBRENDERER_DLL void					Disable() = 0;
		// Copy the contents of the specified color buffer to a texture (only mip 0 is copied)
		// NB: Texture must be 2D and have same width, height and format
		virtual LIBRENDERER_DLL void					CopyColorBuffer(const unsigned int colorBufferIdx, Texture* texture) = 0;

		// Create platform specific resource
		virtual LIBRENDERER_DLL void					Bind();
		// Destroy platform specific resource
		virtual LIBRENDERER_DLL void					Unbind();

				// Get the number of render targets (MRT)
				LIBRENDERER_DLL const unsigned int		GetTargetCount() const { return m_nTargetCount; }
				// Get the pixel format of the color buffer
				LIBRENDERER_DLL const PixelFormat		GetFormat() const { return m_pColorBuffer[0]->GetTextureFormat(); }
				// Get the width of the color buffer
				LIBRENDERER_DLL const unsigned int		GetWidth() const { return m_pColorBuffer[0]->GetWidth(); }
				// Get the height of the color buffer
				LIBRENDERER_DLL const unsigned int		GetHeight() const { return m_pColorBuffer[0]->GetHeight(); }
				// Get the texture corresponding to the specified color buffer
				LIBRENDERER_DLL const Texture* const	GetColorBuffer(const unsigned int colorBufferIdx) const { assert(colorBufferIdx < m_nTargetCount); return m_pColorBuffer[colorBufferIdx]; }
				// Get the texture corresponding to the specified depth buffer
				LIBRENDERER_DLL const Texture* const	GetDepthBuffer() const { return m_pDepthBuffer; }
				// Determines if the color buffer has mipmaps
				LIBRENDERER_DLL const bool				HasMipmaps() const { return m_bHasMipmaps; }
				// Determines if the render target has a depth buffer
				LIBRENDERER_DLL const bool				HasDepthBuffer() const { return m_pDepthBuffer != 0; }

	protected:
		RenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
			const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil);
		virtual ~RenderTarget();

		unsigned int	m_nTargetCount;
		PixelFormat		m_ePixelFormat;
		unsigned int	m_nWidth;
		unsigned int	m_nHeight;
		bool			m_bHasMipmaps;
		bool			m_bHasDepthStencil;

		unsigned int*	m_nColorBufferTexIdx;
		unsigned int	m_nDepthBufferTexIdx;
		Texture**		m_pColorBuffer;
		Texture*		m_pDepthBuffer;

		friend class ResourceManager;
	};
}

#endif // RENDERTARGET_H