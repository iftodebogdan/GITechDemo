/**
 *	@file		RenderState.cpp
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2016 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#include "stdafx.h"

#include "RenderState.h"
#include "Renderer.h"
#include "Profiler.h"
using namespace Synesthesia3D;

RenderState::RenderState()
{}

RenderState::~RenderState()
{}

void RenderState::Reset()
{
	// Set defaults
	SetColorBlendEnabled(false);
	SetColorSrcBlend(BLEND_ONE);
	SetColorDstBlend(BLEND_ZERO);
	SetColorBlendFactor(Vec4f(1.f, 1.f, 1.f, 1.f));

	SetAlphaTestEnabled(false);
	SetAlphaTestFunc(CMP_ALWAYS);
	SetAlphaTestRef(0.f);

	SetCullMode(CULL_CCW);

	SetZEnabled(ZB_ENABLED);
	SetZFunc(CMP_LESSEQUAL);
	SetZWriteEnabled(true);

	SetColorWriteEnabled(true, true, true, true);

	SetSlopeScaledDepthBias(0.f);
	SetDepthBias(0.f);

	SetStencilEnabled(false);
	SetStencilFunc(CMP_ALWAYS);
	SetStencilRef(0);
	SetStencilMask(ULONG_MAX);
	SetStencilWriteMask(ULONG_MAX);
	SetStencilFail(STENCILOP_KEEP);
	SetStencilZFail(STENCILOP_KEEP);
	SetStencilPass(STENCILOP_KEEP);

	SetFillMode(FILL_SOLID);

	SetScissorEnabled(false);

	SetSRGBWriteEnabled(false);

	Flush();
}

const bool RenderState::SetAlphaTestEnabled(const bool enabled)
{
	m_bAlphaTestEnabled = enabled;
	return true;
}

const bool RenderState::SetAlphaTestFunc(const Cmp alphaFunc)
{
	assert(alphaFunc > CMP && alphaFunc < CMP_MAX);

	m_eAlphaFunc = alphaFunc;
	return true;
}

const bool RenderState::SetAlphaTestRef(const float alphaRef)
{
	assert(alphaRef >= 0.f && alphaRef <= 1.f);

	m_fAlphaRef = alphaRef;
	return true;
}

const bool RenderState::SetColorBlendEnabled(const bool enabled)
{
	m_bColorBlendEnabled = enabled;
	return true;
}

const bool RenderState::SetColorSrcBlend(const Blend colorSrc)
{
	assert(colorSrc > BLEND && colorSrc < BLEND_MAX);

	m_eColorSrcBlend = colorSrc;
	return true;
}

const bool RenderState::SetColorDstBlend(const Blend colorDst)
{
	assert(colorDst > BLEND && colorDst < BLEND_MAX);

	m_eColorDstBlend = colorDst;
	return true;
}

const bool RenderState::SetColorBlendFactor(const Vec4f rgba)
{
	assert(
		rgba[0] >= 0.f && rgba[0] <= 1.f &&
		rgba[1] >= 0.f && rgba[1] <= 1.f &&
		rgba[2] >= 0.f && rgba[2] <= 1.f &&
		rgba[3] >= 0.f && rgba[3] <= 1.f
		);

	m_vColorBlendFactor = rgba;
	return true;
}

const bool RenderState::SetCullMode(const Cull cullMode)
{
	assert(cullMode > CULL && cullMode < CULL_MAX);

	m_eCullMode = cullMode;
	return true;
}

const bool RenderState::SetZEnabled(const ZBuffer enabled)
{
	assert(enabled > ZB && enabled < ZB_MAX);

	m_eZEnabled = enabled;
	return true;
}

const bool RenderState::SetZFunc(const Cmp zFunc)
{
	assert(zFunc > CMP && zFunc < CMP_MAX);

	m_eZFunc = zFunc;
	return true;
}

const bool RenderState::SetZWriteEnabled(const bool enabled)
{
	m_bZWriteEnabled = enabled;
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

const bool RenderState::SetSlopeScaledDepthBias(const float scale)
{
	m_fSlopeScaledDepthBias = scale;
	return true;
}

const bool RenderState::SetDepthBias(const float bias)
{
	m_fDepthBias = bias;
	return true;
}

const bool RenderState::SetStencilEnabled(const bool enabled)
{
	m_bStencilEnabled = enabled;
	return true;
}

const bool RenderState::SetStencilFunc(const Cmp stencilFunc)
{
	assert(stencilFunc > CMP && stencilFunc < CMP_MAX);

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
	assert(stencilFail > STENCILOP && stencilFail < STENCILOP_MAX);

	m_eStencilFail = stencilFail;
	return true;
}

const bool RenderState::SetStencilZFail(const StencilOp stencilZFail)
{
	assert(stencilZFail > STENCILOP && stencilZFail < STENCILOP_MAX);

	m_eStencilZFail = stencilZFail;
	return true;
}

const bool RenderState::SetStencilPass(const StencilOp stencilPass)
{
	assert(stencilPass > STENCILOP && stencilPass < STENCILOP_MAX);

	m_eStencilPass = stencilPass;
	return true;
}

const bool RenderState::SetFillMode(const Fill fillMode)
{
	assert(fillMode > FILL && fillMode < FILL_MAX);

	m_eFillMode = fillMode;
	return true;
}

const bool RenderState::SetScissorEnabled(const bool enabled)
{
	m_bScissorEnabled = enabled;
	return true;
}

const bool RenderState::SetSRGBWriteEnabled(const bool enabled)
{
	m_bSRGBEnabled = enabled;
	return true;
}

const bool RenderState::GetAlphaTestEnabled() const
{
	return m_bAlphaTestEnabled;
}

const float RenderState::GetAlphaTestRef() const
{
	return m_fAlphaRef;
}

const Cmp RenderState::GetAlphaTestFunc() const
{
	return m_eAlphaFunc;
}

const bool RenderState::GetColorBlendEnabled() const
{
	return m_bColorBlendEnabled;
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

const ZBuffer RenderState::GetZEnabled() const
{
	return m_eZEnabled;
}

const Cmp RenderState::GetZFunc() const
{
	return m_eZFunc;
}

const bool RenderState::GetZWriteEnabled() const
{
	return m_bZWriteEnabled;
}

void RenderState::GetColorWriteEnabled(bool& red, bool& green, bool& blue, bool& alpha) const
{
	red = m_bColorWriteRed;
	green = m_bColorWriteGreen;
	blue = m_bColorWriteBlue;
	alpha = m_bColorWriteAlpha;
}

const bool RenderState::GetColorWriteRedEnabled() const
{
	return m_bColorWriteRed;
}

const bool RenderState::GetColorWriteGreenEnabled() const
{
	return m_bColorWriteGreen;
}

const bool RenderState::GetColorWriteBlueEnabled() const
{
	return m_bColorWriteBlue;
}

const bool RenderState::GetColorWriteAlphaEnabled() const
{
	return m_bColorWriteAlpha;
}

const float RenderState::GetSlopeScaledDepthBias() const
{
	return m_fSlopeScaledDepthBias;
}

const float RenderState::GetDepthBias() const
{
	return m_fDepthBias;
}

const bool RenderState::GetStencilEnabled() const
{
	return m_bStencilEnabled;
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

const bool RenderState::GetScissorEnabled() const
{
	return m_bScissorEnabled;
}

const bool RenderState::GetSRGBWriteEnabled() const
{
	return m_bSRGBEnabled;
}