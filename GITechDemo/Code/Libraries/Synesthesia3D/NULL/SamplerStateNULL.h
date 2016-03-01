/**
 *	@file		SamplerStateNULL.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef SAMPLERSTATENULL_H
#define SAMPLERSTATENULL_H

#include "SamplerState.h"

namespace Synesthesia3D
{
	class SamplerStateNULL : public SamplerState
	{
	public:
		const bool	SetAnisotropy(const unsigned int slot, const unsigned int anisotropy);
		const bool	SetMipLodBias(const unsigned int slot, const float lodBias);
		const bool	SetFilter(const unsigned int slot, const SamplerFilter filter);
		const bool	SetBorderColor(const unsigned int slot, const Vec4f& rgba);
		const bool	SetAddressingModeU(const unsigned int slot, const SamplerAddressingMode samU);
		const bool	SetAddressingModeV(const unsigned int slot, const SamplerAddressingMode samV);
		const bool	SetAddressingModeW(const unsigned int slot, const SamplerAddressingMode samW);
		const bool	SetAddressingMode(const unsigned int slot, const SamplerAddressingMode samUVW);

		void		Reset() {}
		const bool	Flush() { return true; }

	protected:
		SamplerStateNULL() {}
		~SamplerStateNULL() {}

		friend class RendererNULL;
	};
}

#endif // SAMPLERSTATENULL_H