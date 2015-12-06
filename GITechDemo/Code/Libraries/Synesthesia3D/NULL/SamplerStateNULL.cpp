/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	SamplerStateNULL.cpp
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

#include "SamplerStateNULL.h"
using namespace Synesthesia3D;

const bool SamplerStateNULL::SetAnisotropy(const unsigned int slot, const float anisotropy)
{
	return SamplerState::SetAnisotropy(slot, anisotropy);
}

const bool SamplerStateNULL::SetMipLodBias(const unsigned int slot, const float lodBias)
{
	return SamplerState::SetMipLodBias(slot, lodBias);
}

const bool SamplerStateNULL::SetFilter(const unsigned int slot, const SamplerFilter filter)
{
	return SamplerState::SetFilter(slot, filter);
}

const bool SamplerStateNULL::SetBorderColor(const unsigned int slot, const Vec4f& rgba)
{
	return SamplerState::SetBorderColor(slot, rgba);
}

const bool SamplerStateNULL::SetAddressingModeU(const unsigned int slot, const SamplerAddressingMode samU)
{
	return SamplerState::SetAddressingModeU(slot, samU);
}

const bool SamplerStateNULL::SetAddressingModeV(const unsigned int slot, const SamplerAddressingMode samV)
{
	return SamplerState::SetAddressingModeV(slot, samV);
}

const bool SamplerStateNULL::SetAddressingModeW(const unsigned int slot, const SamplerAddressingMode samW)
{
	return SamplerState::SetAddressingModeW(slot, samW);
}

const bool SamplerStateNULL::SetAddressingMode(const unsigned int slot, const SamplerAddressingMode samUVW)
{
	return SamplerState::SetAddressingMode(slot, samUVW);
}
