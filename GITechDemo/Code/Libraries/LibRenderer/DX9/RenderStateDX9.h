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
#ifndef RENDERSTATEDX9_H
#define RENDERSTATEDX9_H

#include "RenderState.h"

namespace LibRendererDll
{
	class RenderStateDX9 : public RenderState
	{
	public:
		const unsigned int MatchRenderState(const DWORD rs, const unsigned int rsEnumClass) const;

		const bool SetAlphaBlendEnable(const bool enabled);
		const bool SetAlphaSrcBlend(const Blend alphaSrc);
		const bool SetAlphaDstBlend(const Blend alphaDst);
		const bool SetAlphaTestEnable(const bool enabled);
		const bool SetAlphaFunc(const Cmp alphaFunc);
		const bool SetAlphaRef(const float normAlphaRef);
		const bool SetBlendFactor(const Vec4f rgba);
		const bool SetCullMode(const Cull cullMode);
		const bool SetZEnable(const ZBuffer enabled);
		const bool SetZFunc(const Cmp zFunc);
		const bool SetZWriteEnabled(const bool enabled);
		const bool SetSlopeScaleDepthBias(const float scale);
		const bool SetDepthBias(const float bias);
		const bool SetStencilEnable(const bool enabled);
		const bool SetStencilFunc(const Cmp stencilFunc);
		const bool SetStencilRef(const unsigned long stencilRef);
		const bool SetStencilMask(const unsigned long stencilMask);
		const bool SetStencilWriteMask(const unsigned long stencilWriteMask);
		const bool SetStencilFail(const StencilOp stencilFail);
		const bool SetStencilZFail(const StencilOp stencilZFail);
		const bool SetStencilPass(const StencilOp stencilPass);
		const bool SetFillMode(const Fill fillMode);

	protected:
		RenderStateDX9();
		~RenderStateDX9();

		friend class RendererDX9;
	};
}

#endif // RENDERSTATEDX9_H