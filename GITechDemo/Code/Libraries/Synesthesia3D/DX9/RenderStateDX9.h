/**
 *	@file		RenderStateDX9.h
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

#ifndef RENDERSTATEDX9_H
#define RENDERSTATEDX9_H

#include "RenderState.h"

namespace Synesthesia3D
{
	class RenderStateDX9 : public RenderState
	{
	public:
		const unsigned int MatchRenderState(const DWORD rs, const unsigned int rsEnumClass) const;

		const bool	SetColorBlendEnabled(const bool enabled);
		const bool	SetColorSrcBlend(const Blend alphaSrc);
		const bool	SetColorDstBlend(const Blend alphaDst);
		const bool	SetAlphaTestEnabled(const bool enabled);
		const bool	SetAlphaTestFunc(const Cmp alphaFunc);
		const bool	SetAlphaTestRef(const float alphaRef);
		const bool	SetColorBlendFactor(const Vec4f rgba);
		const bool	SetCullMode(const Cull cullMode);
		const bool	SetZEnabled(const ZBuffer enabled);
		const bool	SetZFunc(const Cmp zFunc);
		const bool	SetZWriteEnabled(const bool enabled);
		const bool	SetColorWriteEnabled(const bool red, const bool green, const bool blue, const bool alpha);
		const bool	SetSlopeScaledDepthBias(const float scale);
		const bool	SetDepthBias(const float bias);
		const bool	SetStencilEnabled(const bool enabled);
		const bool	SetStencilFunc(const Cmp stencilFunc);
		const bool	SetStencilRef(const unsigned long stencilRef);
		const bool	SetStencilMask(const unsigned long stencilMask);
		const bool	SetStencilWriteMask(const unsigned long stencilWriteMask);
		const bool	SetStencilFail(const StencilOp stencilFail);
		const bool	SetStencilZFail(const StencilOp stencilZFail);
		const bool	SetStencilPass(const StencilOp stencilPass);
		const bool	SetFillMode(const Fill fillMode);
		const bool	SetScissorEnabled(const bool enabled);
		const bool	SetScissor(const Vec2i size, const Vec2i offset = Vec2i(0, 0));
		const bool	SetSRGBWriteEnabled(const bool enabled);

		void		Reset();

	protected:
		RenderStateDX9();
		~RenderStateDX9();

		friend class RendererDX9;
	};
}

#endif // RENDERSTATEDX9_H