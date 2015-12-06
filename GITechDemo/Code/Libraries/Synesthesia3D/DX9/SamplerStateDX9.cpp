/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	SamplerStateDX9.cpp
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "stdafx.h"

#include "SamplerStateDX9.h"
#include "RendererDX9.h"
#include "MappingsDX9.h"
using namespace Synesthesia3D;

#include "d3d9.h"

SamplerStateDX9::SamplerStateDX9()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	Reset();

	POP_PROFILE_MARKER();
}

SamplerStateDX9::~SamplerStateDX9()
{}

const bool SamplerStateDX9::SetAnisotropy(const unsigned int slot, const float anisotropy)
{
	float aniso = Math::clamp(anisotropy, 1.f, (float)MAX_ANISOTROPY);

	if (aniso == GetAnisotropy(slot))
		return true;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_MAXANISOTROPY, (DWORD)aniso);
	assert(SUCCEEDED(hr));
	
	HRESULT hr1 = 0;
	if (SUCCEEDED(hr))
	{
		if (aniso > 1.f && m_eCurrentMinFilter[slot] != D3DTEXF_ANISOTROPIC)
		{
			hr1 = device->SetSamplerState(slot, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
			if (SUCCEEDED(hr1))
				m_eCurrentMinFilter[slot] = D3DTEXF_ANISOTROPIC;
		}
		else if (aniso <= 1.f && m_eCurrentMinFilter[slot] != (_D3DTEXTUREFILTERTYPE)MinMagFilterDX9[GetFilter(slot)])
		{
			hr1 = device->SetSamplerState(slot, D3DSAMP_MINFILTER, MinMagFilterDX9[GetFilter(slot)]);
			if (SUCCEEDED(hr1))
				m_eCurrentMinFilter[slot] = (_D3DTEXTUREFILTERTYPE)MinMagFilterDX9[GetFilter(slot)];
		}
	}
	assert(SUCCEEDED(hr1));

	POP_PROFILE_MARKER();

	if (SUCCEEDED(hr) && SUCCEEDED(hr1))
		return SamplerState::SetAnisotropy(slot, aniso);
	else
		return false;
}

const bool SamplerStateDX9::SetMipLodBias(const unsigned int slot, const float lodBias)
{
	if (lodBias == GetMipLodBias(slot))
		return true;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_MIPMAPLODBIAS, *(DWORD*)&lodBias);
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();

	if (SUCCEEDED(hr))
		return SamplerState::SetMipLodBias(slot, lodBias);
	else
		return false;
}

const bool SamplerStateDX9::SetFilter(const unsigned int slot, const SamplerFilter filter)
{
	if (filter == GetFilter(slot))
		return true;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	HRESULT hr1 = 0, hr2 = 0, hr3 = 0;
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	//if (1.f < GetAnisotropy(slot) && GetAnisotropy(slot) <= MAX_ANISOTROPY && m_eCurrentMinFilter[slot] != D3DTEXF_ANISOTROPIC)
	//{
	//		hr1 = device->SetSamplerState(slot, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	//		if (SUCCEEDED(hr1))
	//			m_eCurrentMinFilter[slot] = D3DTEXF_ANISOTROPIC;
	//}
	//else if (GetAnisotropy(slot) <= 1.f && m_eCurrentMinFilter[slot] != MinMagFilterDX9[filter])
	//{
	//	hr1 = device->SetSamplerState(slot, D3DSAMP_MINFILTER, MinMagFilterDX9[filter]);
	//	if (SUCCEEDED(hr1))
	//		m_eCurrentMinFilter[slot] = (_D3DTEXTUREFILTERTYPE)MinMagFilterDX9[filter];
	//}
	if (GetAnisotropy(slot) <= 1.f)
	{
		hr1 = device->SetSamplerState(slot, D3DSAMP_MINFILTER, MinMagFilterDX9[filter]);
		if (SUCCEEDED(hr1))
			m_eCurrentMinFilter[slot] = (_D3DTEXTUREFILTERTYPE)MinMagFilterDX9[filter];
	}
	assert(SUCCEEDED(hr1));
	hr2 = device->SetSamplerState(slot, D3DSAMP_MAGFILTER, MinMagFilterDX9[filter]);
	assert(SUCCEEDED(hr2));
	hr3 = device->SetSamplerState(slot, D3DSAMP_MIPFILTER, MipFilterDX9[filter]);
	assert(SUCCEEDED(hr3));

	POP_PROFILE_MARKER();

	if (SUCCEEDED(hr1) && SUCCEEDED(hr2) && SUCCEEDED(hr3))
		return SamplerState::SetFilter(slot, filter);
	else
		return false;
}

const bool SamplerStateDX9::SetBorderColor(const unsigned int slot, const Vec4f& rgba)
{
	assert(
		rgba[0] >= 0.f && rgba[0] <= 1.f &&
		rgba[1] >= 0.f && rgba[1] <= 1.f &&
		rgba[2] >= 0.f && rgba[2] <= 1.f &&
		rgba[3] >= 0.f && rgba[3] <= 1.f
		);

	if (rgba == GetBorderColor(slot))
		return true;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_BORDERCOLOR, D3DCOLOR_COLORVALUE(rgba[0], rgba[1], rgba[2], rgba[3]));
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();

	if (SUCCEEDED(hr))
		return SamplerState::SetBorderColor(slot, rgba);
	else
		return false;
}

const bool SamplerStateDX9::SetAddressingModeU(const unsigned int slot, const SamplerAddressingMode samU)
{
	if (samU == GetAddressingModeU(slot))
		return true;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_ADDRESSU, TextureAddressingModeDX9[samU]);
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();

	if (SUCCEEDED(hr))
		return SamplerState::SetAddressingModeU(slot, samU);
	else
		return false;
}

const bool SamplerStateDX9::SetAddressingModeV(const unsigned int slot, const SamplerAddressingMode samV)
{
	if (samV == GetAddressingModeV(slot))
		return true;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_ADDRESSV, TextureAddressingModeDX9[samV]);
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();

	if (SUCCEEDED(hr))
		return SamplerState::SetAddressingModeV(slot, samV);
	else
		return false;
}

const bool SamplerStateDX9::SetAddressingModeW(const unsigned int slot, const SamplerAddressingMode samW)
{
	if (samW == GetAddressingModeW(slot))
		return true;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_ADDRESSW, TextureAddressingModeDX9[samW]);
	assert(SUCCEEDED(hr));

	POP_PROFILE_MARKER();

	if (SUCCEEDED(hr))
		return SamplerState::SetAddressingModeW(slot, samW);
	else
		return false;
}

const bool SamplerStateDX9::SetAddressingMode(const unsigned int slot, const SamplerAddressingMode samUVW)
{
	if (samUVW == GetAddressingModeU(slot) && samUVW == GetAddressingModeV(slot) && samUVW == GetAddressingModeW(slot))
		return true;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	if (SetAddressingModeU(slot, samUVW) && SetAddressingModeV(slot, samUVW) && SetAddressingModeW(slot, samUVW))
	{
		POP_PROFILE_MARKER();
		return SamplerState::SetAddressingMode(slot, samUVW);
	}
	else
	{
		POP_PROFILE_MARKER();
		return false;
	}
}

const bool SamplerStateDX9::SetSRGBEnabled(const unsigned int slot, const bool enabled)
{
	if (enabled == GetSRGBEnabled(slot))
		return true;

	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_SRGBTEXTURE, enabled ? (DWORD)1 : (DWORD)0);

	POP_PROFILE_MARKER();

	if (SUCCEEDED(hr))
		return SamplerState::SetSRGBEnabled(slot, enabled);
	else
		return false;
}

void SamplerStateDX9::Reset()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

	for (unsigned int i = 0; i < MAX_NUM_PSAMPLERS; i++)
	{
		DWORD value, value2, value3;
		hr = device->GetSamplerState(i, D3DSAMP_MAXANISOTROPY, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentState[i].fAnisotropy = (float)value;

		hr = device->GetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentState[i].fLodBias = (float)value;

		hr = device->GetSamplerState(i, D3DSAMP_MINFILTER, &value);
		assert(SUCCEEDED(hr));
		hr = device->GetSamplerState(i, D3DSAMP_MAGFILTER, &value2);
		assert(SUCCEEDED(hr));
		hr = device->GetSamplerState(i, D3DSAMP_MIPFILTER, &value3);
		assert(SUCCEEDED(hr));
		m_tCurrentState[i].eFilter = MatchFilterType(value, value2, value3);

		hr = device->GetSamplerState(i, D3DSAMP_BORDERCOLOR, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentState[i].vBorderColor = Vec4f(
			(float)((value & (0xff << 16)) >> 16) / 255.f,	// red
			(float)((value & (0xff << 8)) >> 8) / 255.f,	// green
			(float)(value & 0xff) / 255.f,					// blue
			(float)((value & (0xff << 24)) >> 24) / 255.f);	// alpha

		hr = device->GetSamplerState(i, D3DSAMP_ADDRESSU, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentState[i].eAddressingMode[0] = MatchAddressingMode(value);

		hr = device->GetSamplerState(i, D3DSAMP_ADDRESSV, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentState[i].eAddressingMode[1] = MatchAddressingMode(value);

		hr = device->GetSamplerState(i, D3DSAMP_ADDRESSW, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentState[i].eAddressingMode[2] = MatchAddressingMode(value);

		hr = device->GetSamplerState(i, D3DSAMP_SRGBTEXTURE, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentState[i].bSRGBEnabled = (value != 0);
	}

	for (unsigned int i = 0; i < MAX_NUM_PSAMPLERS; i++)
	{
		SetAnisotropy(i, 1.f);
		SetMipLodBias(i, 0.f);
		SetFilter(i, SF_MIN_MAG_POINT_MIP_NONE);
		SetBorderColor(i, Vec4f(0.f, 0.f, 0.f, 0.f));
		for (unsigned int j = 0; j < 3; j++)
			SetAddressingMode(i, SAM_WRAP);
		m_eCurrentMinFilter[i] = D3DTEXF_POINT;
		SetSRGBEnabled(i, false);
	}

	POP_PROFILE_MARKER();
}