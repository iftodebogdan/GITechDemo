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
#include "Renderer.h"
using namespace LibRendererDll;

RenderState::RenderState()
{}

RenderState::~RenderState()
{}

void RenderState::Reset()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

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

	POP_PROFILE_MARKER();
}

const bool RenderState::SetAlphaTestEnable(const bool enabled)
{
	m_bAlphaTestEnable = enabled;
	return true;
}

const bool RenderState::SetAlphaTestFunc(const Cmp alphaFunc)
{
	m_eAlphaFunc = alphaFunc;
	return true;
}

const bool RenderState::SetAlphaTestRef(const float normAlphaRef)
{
	m_fAlphaRef = normAlphaRef;
	return true;
}

const bool RenderState::SetColorBlendEnable(const bool enabled)
{
	m_bColorBlendEnable = enabled;
	return true;
}

const bool RenderState::SetColorSrcBlend(const Blend alphaSrc)
{
	m_eColorSrcBlend = alphaSrc;
	return true;
}

const bool RenderState::SetColorDstBlend(const Blend alphaDst)
{
	m_eColorDstBlend = alphaDst;
	return true;
}

const bool RenderState::SetColorBlendFactor(const Vec4f rgba)
{
	m_vColorBlendFactor = rgba;
	return true;
}

const bool RenderState::SetCullMode(const Cull cullMode)
{
	m_eCullMode = cullMode;
	return true;
}

const bool RenderState::SetZEnable(const ZBuffer enabled)
{
	m_eZEnable = enabled;
	return true;
}

const bool RenderState::SetZFunc(const Cmp zFunc)
{
	m_eZFunc = zFunc;
	return true;
}

const bool RenderState::SetZWriteEnabled(const bool enabled)
{
	m_bZWriteEnable = enabled;
	return true;
}

const bool RenderState::SetColorWriteEnabled(const bool red, const bool green, const bool blue, const bool alpha)
{
	m_bColorWriteRed = red;
	m_bColorWriteGreen = green;
	m_bColorWriteBlue = blue;
	m_bColorWriteAlpha = alpha;
	return true;
}

const bool RenderState::SetSlopeScaleDepthBias(const float scale)
{
	m_fSlopeScaleDepthBias = scale;
	return true;
}

const bool RenderState::SetDepthBias(const float bias)
{
	m_fDepthBias = bias;
	return true;
}

const bool RenderState::SetStencilEnable(const bool enabled)
{
	m_bStencilEnable = enabled;
	return true;
}

const bool RenderState::SetStencilFunc(const Cmp stencilFunc)
{
	m_eStencilFunc = stencilFunc;
	return true;
}

const bool RenderState::SetStencilRef(const unsigned long stencilRef)
{
	m_lStencilRef = stencilRef;
	return true;
}

const bool RenderState::SetStencilMask(const unsigned long stencilMask)
{
	m_lStencilMask = stencilMask;
	return true;
}

const bool RenderState::SetStencilWriteMask(const unsigned long stencilWriteMask)
{
	m_lStencilWriteMask = stencilWriteMask;
	return true;
}

const bool RenderState::SetStencilFail(const StencilOp stencilFail)
{
	m_eStencilFail = stencilFail;
	return true;
}

const bool RenderState::SetStencilZFail(const StencilOp stencilZFail)
{
	m_eStencilZFail = stencilZFail;
	return true;
}

const bool RenderState::SetStencilPass(const StencilOp stencilPass)
{
	m_eStencilPass = stencilPass;
	return true;
}

const bool RenderState::SetFillMode(const Fill fillMode)
{
	m_eFillMode = fillMode;
	return true;
}

const bool RenderState::SetScissorEnable(const bool enabled)
{
	m_bScissorEnable = enabled;
	return true;
}

const bool RenderState::GetAlphaTestEnable() const
{
	return m_bAlphaTestEnable;
}

const float RenderState::GetAlphaTestRef() const
{
	return m_fAlphaRef;
}

const Cmp RenderState::GetAlphaTestFunc() const
{
	return m_eAlphaFunc;
}

const bool RenderState::GetColorBlendEnable() const
{
	return m_bColorBlendEnable;
}

const Blend RenderState::GetColorSrcBlend() const
{
	return m_eColorSrcBlend;
}

const Blend RenderState::GetColorDstBlend() const
{
	return m_eColorDstBlend;
}

const Vec4f RenderState::GetColorBlendFactor() const
{
	return m_vColorBlendFactor;
}

const Cull RenderState::GetCullMode() const
{
	return m_eCullMode;
}

const ZBuffer RenderState::GetZEnable() const
{
	return m_eZEnable;
}

const Cmp RenderState::GetZFunc() const
{
	return m_eZFunc;
}

const bool RenderState::GetZWriteEnabled() const
{
	return m_bZWriteEnable;
}

void RenderState::GetColorWriteEnabled(bool& red, bool& green, bool& blue, bool& alpha)
{
	red = m_bColorWriteRed;
	green = m_bColorWriteGreen;
	blue = m_bColorWriteBlue;
	alpha = m_bColorWriteAlpha;
}

const bool RenderState::GetColorWriteRedEnabled()
{
	return m_bColorWriteRed;
}

const bool RenderState::GetColorWriteGreenEnabled()
{
	return m_bColorWriteGreen;
}

const bool RenderState::GetColorWriteBlueEnabled()
{
	return m_bColorWriteBlue;
}

const bool RenderState::GetColorWriteAlphaEnabled()
{
	return m_bColorWriteAlpha;
}

const float RenderState::GetSlopeScaleDepthBias() const
{
	return m_fSlopeScaleDepthBias;
}

const float RenderState::GetDepthBias() const
{
	return m_fDepthBias;
}

const bool RenderState::GetStencilEnable() const
{
	return m_bStencilEnable;
}

const Cmp RenderState::GetStencilFunc() const
{
	return m_eStencilFunc;
}

const unsigned long RenderState::GetStencilRef() const
{
	return m_lStencilRef;
}

const unsigned long RenderState::GetStencilMask() const
{
	return m_lStencilMask;
}

const unsigned long RenderState::GetStencilWriteMask() const
{
	return m_lStencilWriteMask;
}

const StencilOp RenderState::GetStencilFail() const
{
	return m_eStencilFail;
}

const StencilOp RenderState::GetStencilZFail() const
{
	return m_eStencilZFail;
}

const StencilOp RenderState::GetStencilPass() const
{
	return m_eStencilPass;
}


const Fill RenderState::GetFillMode() const
{
	return m_eFillMode;
}

const bool RenderState::GetScissorEnable()
{
	return m_bScissorEnable;
}
