/**
 *	@file		SamplerStateDX9.cpp
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

#include "SamplerStateDX9.h"
#include "RendererDX9.h"
#include "MappingsDX9.h"
#include "ProfilerDX9.h"
using namespace Synesthesia3D;

#include "d3d9.h"

SamplerStateDX9::SamplerStateDX9()
{
	Reset();
}

SamplerStateDX9::~SamplerStateDX9()
{}

void SamplerStateDX9::Reset()
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

	for (unsigned int slot = 0; slot < MAX_NUM_PSAMPLERS; slot++)
	{
		DWORD value, value2, value3;
		hr = device->GetSamplerState(slot, D3DSAMP_MAXANISOTROPY, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentStateDX9[slot].nAnisotropy = (unsigned int)value;

		hr = device->GetSamplerState(slot, D3DSAMP_MIPMAPLODBIAS, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentStateDX9[slot].fLodBias = (float)value;

		hr = device->GetSamplerState(slot, D3DSAMP_MINFILTER, &value);
		assert(SUCCEEDED(hr));
		hr = device->GetSamplerState(slot, D3DSAMP_MAGFILTER, &value2);
		assert(SUCCEEDED(hr));
		hr = device->GetSamplerState(slot, D3DSAMP_MIPFILTER, &value3);
		assert(SUCCEEDED(hr));
		m_tCurrentStateDX9[slot].eFilter = MatchFilterType(value, value2, value3);

		hr = device->GetSamplerState(slot, D3DSAMP_BORDERCOLOR, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentStateDX9[slot].vBorderColor = Vec4f(
			(float)((value & (0xff << 16)) >> 16) / 255.f,	// red
			(float)((value & (0xff << 8)) >> 8) / 255.f,	// green
			(float)(value & 0xff) / 255.f,					// blue
			(float)((value & (0xff << 24)) >> 24) / 255.f);	// alpha

		hr = device->GetSamplerState(slot, D3DSAMP_ADDRESSU, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentStateDX9[slot].eAddressingMode[0] = MatchAddressingMode(value);

		hr = device->GetSamplerState(slot, D3DSAMP_ADDRESSV, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentStateDX9[slot].eAddressingMode[1] = MatchAddressingMode(value);

		hr = device->GetSamplerState(slot, D3DSAMP_ADDRESSW, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentStateDX9[slot].eAddressingMode[2] = MatchAddressingMode(value);

		hr = device->GetSamplerState(slot, D3DSAMP_SRGBTEXTURE, &value);
		assert(SUCCEEDED(hr));
		m_tCurrentStateDX9[slot].bSRGBEnabled = (value != 0);

		m_eCurrentMinFilter[slot] = D3DTEXF_POINT;
	}

	SamplerState::Reset();
}

const bool SamplerStateDX9::Flush()
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = E_FAIL;

	for (unsigned int slot = 0; slot < MAX_NUM_PSAMPLERS; slot++)
	{
		if (m_tCurrentStateDX9[slot].nAnisotropy != GetAnisotropy(slot))
		{
			hr = device->SetSamplerState(slot, D3DSAMP_MAXANISOTROPY, (DWORD)GetAnisotropy(slot));
			assert(SUCCEEDED(hr));

			HRESULT hr1 = 0;
			if (SUCCEEDED(hr))
			{
				if (GetAnisotropy(slot) > 1u && m_eCurrentMinFilter[slot] != D3DTEXF_ANISOTROPIC)
				{
					hr1 = device->SetSamplerState(slot, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
					if (SUCCEEDED(hr1))
						m_eCurrentMinFilter[slot] = D3DTEXF_ANISOTROPIC;
				}
				else if (GetAnisotropy(slot) <= 1u && m_eCurrentMinFilter[slot] != (_D3DTEXTUREFILTERTYPE)MinMagFilterDX9[GetFilter(slot)])
				{
					hr1 = device->SetSamplerState(slot, D3DSAMP_MINFILTER, MinMagFilterDX9[GetFilter(slot)]);
					if (SUCCEEDED(hr1))
						m_eCurrentMinFilter[slot] = (_D3DTEXTUREFILTERTYPE)MinMagFilterDX9[GetFilter(slot)];
				}
			}
			assert(SUCCEEDED(hr1));

			if (SUCCEEDED(hr) && SUCCEEDED(hr1))
				m_tCurrentStateDX9[slot].nAnisotropy = GetAnisotropy(slot);
			else
				return false;
		}

		if (m_tCurrentStateDX9[slot].fLodBias != GetMipLodBias(slot))
		{
			const float lodBias = GetMipLodBias(slot);
			hr = device->SetSamplerState(slot, D3DSAMP_MIPMAPLODBIAS, *(DWORD*)&lodBias);
			assert(SUCCEEDED(hr));

			if (SUCCEEDED(hr))
				m_tCurrentStateDX9[slot].fLodBias = GetMipLodBias(slot);
			else
				return false;
		}

		if (m_tCurrentStateDX9[slot].eFilter != GetFilter(slot))
		{
			HRESULT hr1 = 0, hr2 = 0, hr3 = 0;
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
			if (GetAnisotropy(slot) <= 1u)
			{
				hr1 = device->SetSamplerState(slot, D3DSAMP_MINFILTER, MinMagFilterDX9[GetFilter(slot)]);
				if (SUCCEEDED(hr1))
					m_eCurrentMinFilter[slot] = (_D3DTEXTUREFILTERTYPE)MinMagFilterDX9[GetFilter(slot)];
			}
			assert(SUCCEEDED(hr1));
			hr2 = device->SetSamplerState(slot, D3DSAMP_MAGFILTER, MinMagFilterDX9[GetFilter(slot)]);
			assert(SUCCEEDED(hr2));
			hr3 = device->SetSamplerState(slot, D3DSAMP_MIPFILTER, MipFilterDX9[GetFilter(slot)]);
			assert(SUCCEEDED(hr3));

			if (SUCCEEDED(hr1) && SUCCEEDED(hr2) && SUCCEEDED(hr3))
				m_tCurrentStateDX9[slot].eFilter = GetFilter(slot);
			else
				return false;
		}

		if (m_tCurrentStateDX9[slot].vBorderColor != GetBorderColor(slot))
		{
			hr = device->SetSamplerState(slot, D3DSAMP_BORDERCOLOR, D3DCOLOR_COLORVALUE(GetBorderColor(slot)[0], GetBorderColor(slot)[1], GetBorderColor(slot)[2], GetBorderColor(slot)[3]));
			assert(SUCCEEDED(hr));

			if (SUCCEEDED(hr))
				m_tCurrentStateDX9[slot].vBorderColor = GetBorderColor(slot);
			else
				return false;
		}

		if (m_tCurrentStateDX9[slot].eAddressingMode[0] != GetAddressingModeU(slot))
		{
			hr = device->SetSamplerState(slot, D3DSAMP_ADDRESSU, TextureAddressingModeDX9[GetAddressingModeU(slot)]);
			assert(SUCCEEDED(hr));

			if (SUCCEEDED(hr))
				m_tCurrentStateDX9[slot].eAddressingMode[0] = GetAddressingModeU(slot);
			else
				return false;
		}

		if (m_tCurrentStateDX9[slot].eAddressingMode[1] != GetAddressingModeV(slot))
		{
			hr = device->SetSamplerState(slot, D3DSAMP_ADDRESSV, TextureAddressingModeDX9[GetAddressingModeV(slot)]);
			assert(SUCCEEDED(hr));

			if (SUCCEEDED(hr))
				m_tCurrentStateDX9[slot].eAddressingMode[1] = GetAddressingModeV(slot);
			else
				return false;
		}

		if (m_tCurrentStateDX9[slot].eAddressingMode[2] == GetAddressingModeW(slot))
		{
			hr = device->SetSamplerState(slot, D3DSAMP_ADDRESSW, TextureAddressingModeDX9[GetAddressingModeW(slot)]);
			assert(SUCCEEDED(hr));

			if (SUCCEEDED(hr))
				m_tCurrentStateDX9[slot].eAddressingMode[2] = GetAddressingModeW(slot);
			else
				return false;
		}

		if (m_tCurrentStateDX9[slot].bSRGBEnabled != GetSRGBEnabled(slot))
		{
			hr = device->SetSamplerState(slot, D3DSAMP_SRGBTEXTURE, (DWORD)GetSRGBEnabled(slot));

			if (SUCCEEDED(hr))
				m_tCurrentStateDX9[slot].bSRGBEnabled = GetSRGBEnabled(slot);
			else
				return false;
		}
	}

	return true;
}
