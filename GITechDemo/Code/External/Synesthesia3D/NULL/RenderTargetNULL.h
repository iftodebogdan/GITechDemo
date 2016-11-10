/**
 *	@file		RenderTargetNULL.h
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

#ifndef RENDERTARGETNULL_H
#define RENDERTARGETNULL_H

#include "RenderTarget.h"

namespace Synesthesia3D
{
	class RenderTargetNULL : public RenderTarget
	{
	public:
		void	Enable() { RenderTarget::Enable(); }
		void	Disable() { RenderTarget::Disable(); }
		void	CopyColorBuffer(const unsigned int /*colorBufferIdx*/, Texture* /*texture*/) {}

		void	Bind() {}
		void	Unbind() {}

	protected:
		RenderTargetNULL(const unsigned int targetCount, PixelFormat pixelFormat,
			const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
			: RenderTarget(targetCount, pixelFormat, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat) {}
		RenderTargetNULL(const unsigned int targetCount, PixelFormat pixelFormat,
			const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
			: RenderTarget(targetCount, pixelFormat, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat) {}
		RenderTargetNULL(const unsigned int targetCount,
			PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
			const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
			: RenderTarget(targetCount, pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3, width, height, hasMipmaps, hasDepthStencil, depthStencilFormat) {}
		RenderTargetNULL(const unsigned int targetCount,
			PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
			const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
			: RenderTarget(targetCount, pixelFormatRT0, pixelFormatRT1, pixelFormatRT2, pixelFormatRT3, widthRatio, heightRatio, hasMipmaps, hasDepthStencil, depthStencilFormat) {}
		~RenderTargetNULL() {}

		friend class ResourceManagerNULL;
	};
}

#endif // RENDERTARGETNULL_H