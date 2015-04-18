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
#include "stdafx.h"

#include "RenderState.h"
using namespace LibRendererDll;

RenderState::RenderState()
{}

RenderState::~RenderState()
{}

void RenderState::Reset()
{
	// Set defaults
	SetColorBlendEnable(false);
	SetColorSrcBlend(BLEND_ONE);
	SetColorDstBlend(BLEND_ZERO);
	SetColorBlendFactor(Vec4f(1.f, 1.f, 1.f, 1.f));

	SetAlphaTestEnable(false);
	SetAlphaTestFunc(CMP_ALWAYS);
	SetAlphaTestRef(0.f);

	SetCullMode(CULL_CCW);

	SetZEnable(ZB_ENABLED);
	SetZFunc(CMP_LESSEQUAL);
	SetZWriteEnabled(true);

	SetSlopeScaleDepthBias(0.f);
	SetDepthBias(0.f);

	SetStencilEnable(false);
	SetStencilFunc(CMP_ALWAYS);
	SetStencilRef(0);
	SetStencilMask(ULONG_MAX);
	SetStencilWriteMask(ULONG_MAX);
	SetStencilFail(STENCILOP_KEEP);
	SetStencilZFail(STENCILOP_KEEP);
	SetStencilPass(STENCILOP_KEEP);

	SetFillMode(FILL_SOLID);

	SetScissorEnable(false);
}
