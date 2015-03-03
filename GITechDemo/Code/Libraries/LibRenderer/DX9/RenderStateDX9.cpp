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

#include "RendererDX9.h"
#include "MappingsDX9.h"
#include "RenderStateDX9.h"
using namespace LibRendererDll;

RenderStateDX9::RenderStateDX9()
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;
	DWORD value;

	hr = device->GetRenderState(D3DRS_ALPHABLENDENABLE, &value);
	assert(SUCCEEDED(hr));
	m_bAlphaBlendEnable = (value != 0);

	hr = device->GetRenderState(D3DRS_SRCBLENDALPHA, &value);
	assert(SUCCEEDED(hr));
	m_eAlphaSrcBlend = (Blend)MatchRenderState(value, BLEND);

	hr = device->GetRenderState(D3DRS_DESTBLENDALPHA, &value);
	assert(SUCCEEDED(hr));
	m_eAlphaDstBlend = (Blend)MatchRenderState(value, BLEND);

	hr = device->GetRenderState(D3DRS_ALPHATESTENABLE, &value);
	assert(SUCCEEDED(hr));
	m_bAlphaTestEnable = (value != 0);

	hr = device->GetRenderState(D3DRS_ALPHAFUNC, &value);
	assert(SUCCEEDED(hr));
	m_eAlphaFunc = (Cmp)MatchRenderState(value, CMP);

	hr = device->GetRenderState(D3DRS_ALPHAREF, &value);
	assert(SUCCEEDED(hr));
	m_fAlphaRef = (float)value / 255.f;

	hr = device->GetRenderState(D3DRS_BLENDFACTOR, &value);
	assert(SUCCEEDED(hr));
	m_vBlendFactor = Vec4f(
		(float)((value & (0xff << 16)) >> 16) / 255.f,	// red
		(float)((value & (0xff << 8)) >> 8) / 255.f,	// green
		(float)(value & 0xff) / 255.f,					// blue
		(float)((value & (0xff << 24)) >> 24) / 255.f);	// alpha

	hr = device->GetRenderState(D3DRS_CULLMODE, &value);
	assert(SUCCEEDED(hr));
	m_eCullMode = (Cull)MatchRenderState(value, CULL);



	hr = device->GetRenderState(D3DRS_ZENABLE, &value);
	assert(SUCCEEDED(hr));
	m_eZEnable = (ZBuffer)MatchRenderState(value, ZB);

	hr = device->GetRenderState(D3DRS_ZFUNC, &value);
	assert(SUCCEEDED(hr));
	m_eZFunc = (Cmp)MatchRenderState(value, CMP);

	hr = device->GetRenderState(D3DRS_ZWRITEENABLE, &value);
	assert(SUCCEEDED(hr));
	m_bZWriteEnable = (value != 0);



	hr = device->GetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, &value);
	assert(SUCCEEDED(hr));
	m_fSlopeScaleDepthBias = *(float*)&value;

	hr = device->GetRenderState(D3DRS_DEPTHBIAS, &value);
	assert(SUCCEEDED(hr));
	m_fDepthBias = *(float*)&value * 16777216.0f;



	hr = device->GetRenderState(D3DRS_STENCILENABLE, &value);
	assert(SUCCEEDED(hr));
	m_bStencilEnable = (value != 0);

	hr = device->GetRenderState(D3DRS_STENCILFUNC, &value);
	assert(SUCCEEDED(hr));
	m_eStencilFunc = (Cmp)MatchRenderState(value, CMP);

	hr = device->GetRenderState(D3DRS_STENCILREF, &value);
	assert(SUCCEEDED(hr));
	m_lStencilRef = value;

	hr = device->GetRenderState(D3DRS_STENCILMASK, &value);
	assert(SUCCEEDED(hr));
	m_lStencilMask = value;

	hr = device->GetRenderState(D3DRS_STENCILWRITEMASK, &value);
	assert(SUCCEEDED(hr));
	m_lStencilWriteMask = value;

	hr = device->GetRenderState(D3DRS_STENCILFAIL, &value);
	assert(SUCCEEDED(hr));
	m_eStencilFail = (StencilOp)MatchRenderState(value, STENCILOP);

	hr = device->GetRenderState(D3DRS_STENCILZFAIL, &value);
	assert(SUCCEEDED(hr));
	m_eStencilZFail = (StencilOp)MatchRenderState(value, STENCILOP);

	hr = device->GetRenderState(D3DRS_STENCILPASS, &value);
	assert(SUCCEEDED(hr));
	m_eStencilPass = (StencilOp)MatchRenderState(value, STENCILOP);



	hr = device->GetRenderState(D3DRS_FILLMODE, &value);
	assert(SUCCEEDED(hr));
	m_eFillMode = (Fill)MatchRenderState(value, FILL);
}

RenderStateDX9::~RenderStateDX9()
{}

const unsigned int RenderStateDX9::MatchRenderState(const DWORD rs, const unsigned int rsEnumClass) const
{
	unsigned int begin, end;
	switch (rsEnumClass)
	{
	case BLEND:
		begin = BLEND;
		end = BLEND_MAX;
		break;
	case CMP:
		begin = CMP;
		end = CMP_MAX;
		break;
	case CULL:
		begin = CULL;
		end = CULL_MAX;
		break;
	case ZB:
		begin = ZB;
		end = ZB_MAX;
		break;
	case STENCILOP:
		begin = STENCILOP;
		end = STENCILOP_MAX;
		break;
	case FILL:
		begin = FILL;
		end = FILL_MAX;
		break;
	default:
		assert(false);
		return 0;
	}

	for (unsigned int i = begin + 1; i < end; i++)
	{
		if (RenderStateMappingDX9[i] == rs)
			return i;
	}

	assert(false);
	return 0;
}

const bool RenderStateDX9::SetAlphaBlendEnable(const bool enabled)
{
	if (enabled == GetAlphaBlendEnable())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)enabled);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetAlphaBlendEnable(enabled);
	else
		return false;
}

const bool RenderStateDX9::SetAlphaSrcBlend(const Blend alphaSrc)
{
	assert(alphaSrc > BLEND && alphaSrc < BLEND_MAX);

	if (alphaSrc == GetAlphaSrcBlend())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_SRCBLENDALPHA, RenderStateMappingDX9[alphaSrc]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetAlphaSrcBlend(alphaSrc);
	else
		return false;
}

const bool RenderStateDX9::SetAlphaDstBlend(const Blend alphaDst)
{
	assert(alphaDst > BLEND && alphaDst < BLEND_MAX);

	if (alphaDst == GetAlphaDstBlend())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_DESTBLENDALPHA, RenderStateMappingDX9[alphaDst]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetAlphaDstBlend(alphaDst);
	else
		return false;
}

const bool RenderStateDX9::SetAlphaTestEnable(const bool enabled)
{
	if (enabled == GetAlphaTestEnable())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_ALPHATESTENABLE, (DWORD)enabled);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetAlphaTestEnable(enabled);
	else
		return false;
}

const bool RenderStateDX9::SetAlphaFunc(const Cmp alphaFunc)
{
	assert(alphaFunc > CMP && alphaFunc < CMP_MAX);

	if (alphaFunc == GetAlphaFunc())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_ALPHAFUNC, RenderStateMappingDX9[alphaFunc]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetAlphaFunc(alphaFunc);
	else
		return false;
}

const bool RenderStateDX9::SetAlphaRef(const float normAlphaRef)
{
	assert(normAlphaRef >= 0.f && normAlphaRef <= 1.f);

	if (normAlphaRef == GetAlphaRef())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_ALPHAREF, (DWORD)(255.f * normAlphaRef));
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetAlphaRef(normAlphaRef);
	else
		return false;
}

const bool RenderStateDX9::SetBlendFactor(const Vec4f rgba)
{
	assert(
		rgba[0] >= 0.f && rgba[0] <= 1.f &&
		rgba[1] >= 0.f && rgba[1] <= 1.f &&
		rgba[2] >= 0.f && rgba[2] <= 1.f &&
		rgba[3] >= 0.f && rgba[3] <= 1.f
		);

	if (rgba == GetBlendFactor())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_BLENDFACTOR, D3DCOLOR_COLORVALUE(rgba[0], rgba[1], rgba[2], rgba[3]));
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetBlendFactor(rgba);
	else
		return false;
}

const bool RenderStateDX9::SetCullMode(const Cull cullMode)
{
	assert(cullMode > CULL && cullMode < CULL_MAX);

	if (cullMode == GetCullMode())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_CULLMODE, RenderStateMappingDX9[cullMode]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetCullMode(cullMode);
	else
		return false;
}

const bool RenderStateDX9::SetZEnable(const ZBuffer enabled)
{
	assert(enabled > ZB && enabled < ZB_MAX);

	if (enabled == GetZEnable())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_ZENABLE, RenderStateMappingDX9[enabled]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetZEnable(enabled);
	else
		return false;
}

const bool RenderStateDX9::SetZFunc(const Cmp zFunc)
{
	assert(zFunc > CMP && zFunc < CMP_MAX);

	if (zFunc == GetZFunc())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_ZFUNC, RenderStateMappingDX9[zFunc]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetZFunc(zFunc);
	else
		return false;
}

const bool RenderStateDX9::SetZWriteEnabled(const bool enabled)
{
	if (enabled == GetZWriteEnabled())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_ZWRITEENABLE, (DWORD)enabled);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetZWriteEnabled(enabled);
	else
		return false;
}

const bool RenderStateDX9::SetSlopeScaleDepthBias(const float scale)
{
	if (scale == GetSlopeScaleDepthBias())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&scale);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetSlopeScaleDepthBias(scale);
	else
		return false;
}

const bool RenderStateDX9::SetDepthBias(const float bias)
{
	if (bias == GetDepthBias())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	float dwBias = bias / 16777216.0f;
	HRESULT hr = device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&dwBias);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetDepthBias(bias);
	else
		return false;
}

const bool RenderStateDX9::SetStencilEnable(const bool enabled)
{
	if (enabled == GetStencilEnable())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_STENCILENABLE, (DWORD)enabled);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetStencilEnable(enabled);
	else
		return false;
}

const bool RenderStateDX9::SetStencilFunc(const Cmp stencilFunc)
{
	assert(stencilFunc > CMP && stencilFunc < CMP_MAX);

	if (stencilFunc == GetStencilFunc())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_STENCILFUNC, RenderStateMappingDX9[stencilFunc]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetStencilFunc(stencilFunc);
	else
		return false;
}

const bool RenderStateDX9::SetStencilRef(const unsigned long stencilRef)
{
	if (stencilRef == GetStencilRef())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_STENCILREF, (DWORD)stencilRef);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetStencilRef(stencilRef);
	else
		return false;
}

const bool RenderStateDX9::SetStencilMask(const unsigned long stencilMask)
{
	if (stencilMask == GetStencilMask())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_STENCILMASK, (DWORD)stencilMask);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetStencilMask(stencilMask);
	else
		return false;
}

const bool RenderStateDX9::SetStencilWriteMask(const unsigned long stencilWriteMask)
{
	if (stencilWriteMask == GetStencilWriteMask())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_STENCILWRITEMASK, (DWORD)stencilWriteMask);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetStencilWriteMask(stencilWriteMask);
	else
		return false;
}

const bool RenderStateDX9::SetStencilFail(const StencilOp stencilFail)
{
	assert(stencilFail > STENCILOP && stencilFail < STENCILOP_MAX);

	if (stencilFail == GetStencilFail())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_STENCILFAIL, RenderStateMappingDX9[stencilFail]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetStencilFail(stencilFail);
	else
		return false;
}

const bool RenderStateDX9::SetStencilZFail(const StencilOp stencilZFail)
{
	assert(stencilZFail > STENCILOP && stencilZFail < STENCILOP_MAX);

	if (stencilZFail == GetStencilZFail())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_STENCILZFAIL, RenderStateMappingDX9[stencilZFail]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetStencilZFail(stencilZFail);
	else
		return false;
}

const bool RenderStateDX9::SetStencilPass(const StencilOp stencilPass)
{
	assert(stencilPass > STENCILOP && stencilPass < STENCILOP_MAX);

	if (stencilPass == GetStencilPass())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_STENCILPASS, RenderStateMappingDX9[stencilPass]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetStencilPass(stencilPass);
	else
		return false;
}

const bool RenderStateDX9::SetFillMode(const Fill fillMode)
{
	assert(fillMode > FILL && fillMode < FILL_MAX);

	if (fillMode == GetFillMode())
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetRenderState(D3DRS_FILLMODE, RenderStateMappingDX9[fillMode]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return RenderState::SetFillMode(fillMode);
	else
		return false;
}
