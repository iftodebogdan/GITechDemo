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

#include "SamplerState.h"
using namespace LibRendererDll;

SamplerState::SamplerState()
{
	for (unsigned int i = 0; i < MAX_NUM_PSAMPLERS; i++)
	{
		m_tCurrentState[i].fAnisotropy = 1.f;
		m_tCurrentState[i].fLodBias = 0.f;
		m_tCurrentState[i].eFilter = SF_MIN_MAG_POINT_MIP_NONE;
		m_tCurrentState[i].vBorderColor = Vec4f(0.f, 0.f, 0.f, 0.f);
		for (unsigned int j = 0; j < 3; j++)
			m_tCurrentState[i].eAddressingMode[j] = SAM_WRAP;
		m_tCurrentState[i].bSRGBEnabled = false;
	}
}

SamplerState::~SamplerState()
{}

const bool SamplerState::SetAnisotropy(const unsigned int slot, const float anisotropy)
{
	assert(slot < MAX_NUM_PSAMPLERS);
	m_tCurrentState[slot].fAnisotropy = anisotropy;
	return true;
}

const bool SamplerState::SetMipLodBias(const unsigned int slot, const float lodBias)
{
	assert(slot < MAX_NUM_PSAMPLERS);
	m_tCurrentState[slot].fLodBias = lodBias;
	return true;
}

const bool SamplerState::SetFilter(const unsigned int slot, const SamplerFilter filter)
{
	assert(slot < MAX_NUM_PSAMPLERS);
	m_tCurrentState[slot].eFilter = filter;
	return true;
}

const bool SamplerState::SetBorderColor(const unsigned int slot, const Vec4f rgba)
{
	assert(slot < MAX_NUM_PSAMPLERS);
	m_tCurrentState[slot].vBorderColor = rgba;
	return true;
}

const bool SamplerState::SetAddressingModeU(const unsigned int slot, const SamplerAddressingMode samU)
{
	assert(slot < MAX_NUM_PSAMPLERS);
	m_tCurrentState[slot].eAddressingMode[0] = samU;
	return true;
}

const bool SamplerState::SetAddressingModeV(const unsigned int slot, const SamplerAddressingMode samV)
{
	assert(slot < MAX_NUM_PSAMPLERS);
	m_tCurrentState[slot].eAddressingMode[1] = samV;
	return true;
}

const bool SamplerState::SetAddressingModeW(const unsigned int slot, const SamplerAddressingMode samW)
{
	assert(slot < MAX_NUM_PSAMPLERS);
	m_tCurrentState[slot].eAddressingMode[2] = samW;
	return true;
}

const bool SamplerState::SetAddressingMode(const unsigned int slot, const SamplerAddressingMode samUVW)
{
	return
		SetAddressingModeU(slot, samUVW) &&
		SetAddressingModeV(slot, samUVW) &&
		SetAddressingModeW(slot, samUVW);
}

const bool SamplerState::SetSRGBEnabled(const unsigned int slot, const bool enabled)
{
	assert(slot < MAX_NUM_PSAMPLERS);
	m_tCurrentState[slot].bSRGBEnabled = enabled;
	return true;
}

const float SamplerState::GetAnisotropy(const unsigned int slot) const
{
	assert(slot < MAX_NUM_PSAMPLERS);
	return m_tCurrentState[slot].fAnisotropy;
}

const float SamplerState::GetMipLodBias(const unsigned int slot) const
{
	assert(slot < MAX_NUM_PSAMPLERS);
	return m_tCurrentState[slot].fLodBias;
}

const SamplerFilter SamplerState::GetFilter(const unsigned int slot) const
{
	assert(slot < MAX_NUM_PSAMPLERS);
	return m_tCurrentState[slot].eFilter;
}

const Vec4f SamplerState::GetBorderColor(const unsigned int slot) const
{
	assert(slot < MAX_NUM_PSAMPLERS);
	return m_tCurrentState[slot].vBorderColor;
}

const SamplerAddressingMode SamplerState::GetAddressingModeU(const unsigned int slot) const
{
	assert(slot < MAX_NUM_PSAMPLERS);
	return m_tCurrentState[slot].eAddressingMode[0];
}

const SamplerAddressingMode SamplerState::GetAddressingModeV(const unsigned int slot) const
{
	assert(slot < MAX_NUM_PSAMPLERS);
	return m_tCurrentState[slot].eAddressingMode[1];
}

const SamplerAddressingMode SamplerState::GetAddressingModeW(const unsigned int slot) const
{
	assert(slot < MAX_NUM_PSAMPLERS);
	return m_tCurrentState[slot].eAddressingMode[2];
}

const SamplerAddressingMode SamplerState::GetAddressingMode(const unsigned int slot) const
{
	return
		(GetAddressingModeU(slot) == GetAddressingModeV(slot) &&
		GetAddressingModeV(slot) == GetAddressingModeW(slot)) ?
		GetAddressingModeU(slot) : SAM_NONE;
}

const bool SamplerState::GetSRGBEnabled(const unsigned int slot) const
{
	assert(slot < MAX_NUM_PSAMPLERS);
	return m_tCurrentState[slot].bSRGBEnabled;
}