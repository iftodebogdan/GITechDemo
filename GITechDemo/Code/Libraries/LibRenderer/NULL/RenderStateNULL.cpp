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

#include "RenderStateNULL.h"
using namespace LibRendererDll;

RenderStateNULL::RenderStateNULL()
{}

RenderStateNULL::~RenderStateNULL()
{}

const bool RenderStateNULL::SetAlphaBlendEnable(const bool enabled)
{
	return true;
}

const bool RenderStateNULL::SetAlphaSrcBlend(const Blend alphaSrc)
{
	return true;
}

const bool RenderStateNULL::SetAlphaDstBlend(const Blend alphaDst)
{
	return true;
}

const bool RenderStateNULL::SetAlphaTestEnable(const bool enabled)
{
	return true;
}

const bool RenderStateNULL::SetAlphaFunc(const Cmp alphaFunc)
{
	return true;
}

const bool RenderStateNULL::SetAlphaRef(const float normAlphaRef)
{
	return true;
}

const bool RenderStateNULL::SetBlendFactor(const Vec4f rgba)
{
	return true;
}

const bool RenderStateNULL::SetCullMode(const Cull cullMode)
{
	return true;
}

const bool RenderStateNULL::SetZEnable(const ZBuffer enabled)
{
	return true;
}

const bool RenderStateNULL::SetZFunc(const Cmp zFunc)
{
	return true;
}

const bool RenderStateNULL::SetZWriteEnabled(const bool enabled)
{
	return true;
}

const bool RenderStateNULL::SetSlopeScaleDepthBias(const float scale)
{
	return true;
}

const bool RenderStateNULL::SetDepthBias(const float bias)
{
	return true;
}

const bool RenderStateNULL::SetStencilEnable(const bool enabled)
{
	return true;
}

const bool RenderStateNULL::SetStencilFunc(const Cmp stencilFunc)
{
	return true;
}

const bool RenderStateNULL::SetStencilRef(const unsigned long stencilRef)
{
	return true;
}

const bool RenderStateNULL::SetStencilMask(const unsigned long stencilMask)
{
	return true;
}

const bool RenderStateNULL::SetStencilWriteMask(const unsigned long stencilWriteMask)
{
	return true;
}

const bool RenderStateNULL::SetStencilFail(const StencilOp stencilFail)
{
	return true;
}

const bool RenderStateNULL::SetStencilZFail(const StencilOp stencilZFail)
{
	return true;
}

const bool RenderStateNULL::SetStencilPass(const StencilOp stencilPass)
{
	return true;
}

const bool RenderStateNULL::SetFillMode(const Fill fillMode)
{
	return true;
}

void RenderStateNULL::Reset()
{
}
