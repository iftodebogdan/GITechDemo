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
#ifndef SAMPLERSTATEDX9_H
#define SAMPLERSTATEDX9_H

#include "SamplerState.h"

namespace LibRendererDll
{
	class RendererDX9;

	class SamplerStateDX9 : public SamplerState
	{
	public:
		const SamplerFilter			MatchFilterType(const DWORD min, const DWORD mag, const DWORD mip);
		const SamplerAddressingMode	MatchAddressingMode(const DWORD sam);

		const bool SetAnisotropy(const unsigned int slot, const float anisotropy);
		const bool SetLodBias(const unsigned int slot, const float lodBias);
		const bool SetFilter(const unsigned int slot, const SamplerFilter filter);
		const bool SetBorderColor(const unsigned int slot, const Vec4f rgba);
		const bool SetAddressingModeU(const unsigned int slot, const SamplerAddressingMode samU);
		const bool SetAddressingModeV(const unsigned int slot, const SamplerAddressingMode samV);
		const bool SetAddressingModeW(const unsigned int slot, const SamplerAddressingMode samW);
		const bool SetAddressingMode(const unsigned int slot, const SamplerAddressingMode samUVW);

	protected:
		SamplerStateDX9();
		~SamplerStateDX9();

		friend class RendererDX9;
	};
}

#endif // SAMPLERSTATEDX9_H