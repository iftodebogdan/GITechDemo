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

#include "SamplerStateDX9.h"
#include "RendererDX9.h"
#include "MappingsDX9.h"
using namespace LibRendererDll;

#include "d3d9.h"

SamplerStateDX9::SamplerStateDX9()
{
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
	}
}

SamplerStateDX9::~SamplerStateDX9()
{}

const SamplerFilter SamplerStateDX9::MatchFilterType(const DWORD min, const DWORD mag, const DWORD mip)
{
	for (unsigned int i = 0; i < SF_MAX; i++)
	{
		if (min == MinMagFilterDX9[i] && mag == MinMagFilterDX9[i])
		{
			for (unsigned int j = 0; j < SF_MAX; j++)
			{
				if (mip == MipFilterDX9[i])
					return (SamplerFilter)i;
			}
		}
	}

	assert(false);
	return SF_NONE;
}

const SamplerAddressingMode SamplerStateDX9::MatchAddressingMode(const DWORD sam)
{
	for (unsigned int i = 0; i < SAM_MAX; i++)
	{
		if (TextureAddressingModeDX9[i] == sam)
			return (SamplerAddressingMode)i;
	}

	assert(false);
	return SAM_NONE;
}

const bool SamplerStateDX9::SetAnisotropy(const unsigned int slot, const float anisotropy)
{
	if (anisotropy == GetAnisotropy(slot))
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_MAXANISOTROPY, (DWORD)anisotropy);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return SamplerState::SetAnisotropy(slot, anisotropy);
	else
		return false;
}

const bool SamplerStateDX9::SetLodBias(const unsigned int slot, const float lodBias)
{
	if (lodBias == GetLodBias(slot))
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_MIPMAPLODBIAS, (DWORD)lodBias);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return SamplerState::SetLodBias(slot, lodBias);
	else
		return false;
}

const bool SamplerStateDX9::SetFilter(const unsigned int slot, const SamplerFilter filter)
{
	if (filter == GetFilter(slot))
		return true;

	HRESULT hr1, hr2, hr3;
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	hr1 = device->SetSamplerState(slot, D3DSAMP_MINFILTER, MinMagFilterDX9[filter]);
	assert(SUCCEEDED(hr1));
	hr2 = device->SetSamplerState(slot, D3DSAMP_MAGFILTER, MinMagFilterDX9[filter]);
	assert(SUCCEEDED(hr2));
	hr3 = device->SetSamplerState(slot, D3DSAMP_MIPFILTER, MipFilterDX9[filter]);
	assert(SUCCEEDED(hr3));

	if (SUCCEEDED(hr1) && SUCCEEDED(hr2) && SUCCEEDED(hr3))
		return SamplerState::SetFilter(slot, filter);
	else
		return false;
}

const bool SamplerStateDX9::SetBorderColor(const unsigned int slot, const Vec4f rgba)
{
	assert(
		rgba[0] >= 0.f && rgba[0] <= 1.f &&
		rgba[1] >= 0.f && rgba[1] <= 1.f &&
		rgba[2] >= 0.f && rgba[2] <= 1.f &&
		rgba[3] >= 0.f && rgba[3] <= 1.f
		);

	if (rgba == GetBorderColor(slot))
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_BORDERCOLOR, D3DCOLOR_COLORVALUE(rgba[0], rgba[1], rgba[2], rgba[3]));
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return SamplerState::SetBorderColor(slot, rgba);
	else
		return false;
}

const bool SamplerStateDX9::SetAddressingModeU(const unsigned int slot, const SamplerAddressingMode samU)
{
	if (samU == GetAddressingModeU(slot))
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_ADDRESSU, TextureAddressingModeDX9[samU]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return SamplerState::SetAddressingModeU(slot, samU);
	else
		return false;
}

const bool SamplerStateDX9::SetAddressingModeV(const unsigned int slot, const SamplerAddressingMode samV)
{
	if (samV == GetAddressingModeV(slot))
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_ADDRESSV, TextureAddressingModeDX9[samV]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return SamplerState::SetAddressingModeV(slot, samV);
	else
		return false;
}

const bool SamplerStateDX9::SetAddressingModeW(const unsigned int slot, const SamplerAddressingMode samW)
{
	if (samW == GetAddressingModeW(slot))
		return true;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetSamplerState(slot, D3DSAMP_ADDRESSW, TextureAddressingModeDX9[samW]);
	assert(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
		return SamplerState::SetAddressingModeW(slot, samW);
	else
		return false;
}

const bool SamplerStateDX9::SetAddressingMode(const unsigned int slot, const SamplerAddressingMode samUVW)
{
	if (samUVW == GetAddressingModeU(slot) && samUVW == GetAddressingModeV(slot) && samUVW == GetAddressingModeW(slot))
		return true;

	if (SetAddressingModeU(slot, samUVW) && SetAddressingModeV(slot, samUVW) && SetAddressingModeW(slot, samUVW))
		return SamplerState::SetAddressingMode(slot, samUVW);
	else
		return false;
}
