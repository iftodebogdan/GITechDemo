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
#ifndef SAMPLERSTATENULL_H
#define SAMPLERSTATENULL_H

#include "SamplerState.h"

namespace LibRendererDll
{
	class SamplerStateNULL : public SamplerState
	{
	public:
		const bool	SetAnisotropy(const unsigned int slot, const float anisotropy) { return true; }
		const bool	SetMipLodBias(const unsigned int slot, const float lodBias) { return true; }
		const bool	SetFilter(const unsigned int slot, const SamplerFilter filter) { return true; }
		const bool	SetBorderColor(const unsigned int slot, const Vec4f& rgba) { return true; }
		const bool	SetAddressingModeU(const unsigned int slot, const SamplerAddressingMode samU) { return true; }
		const bool	SetAddressingModeV(const unsigned int slot, const SamplerAddressingMode samV) { return true; }
		const bool	SetAddressingModeW(const unsigned int slot, const SamplerAddressingMode samW) { return true; }
		const bool	SetAddressingMode(const unsigned int slot, const SamplerAddressingMode samUVW) { return true; }

		void		Reset() {}

	protected:
		SamplerStateNULL() {}
		~SamplerStateNULL() {}

		friend class RendererNULL;
	};
}

#endif // SAMPLERSTATENULL_H