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
#ifndef SAMPLERSTATE_H
#define SAMPLERSTATE_H

#ifndef LIBRENDERER_DLL
#ifdef LIBRENDERER_EXPORTS
#define LIBRENDERER_DLL __declspec(dllexport) 
#else
#define LIBRENDERER_DLL __declspec(dllimport) 
#endif
#endif // LIBRENDERER_DLL

#include <gmtl/gmtl.h>
using namespace gmtl;

#include "ResourceData.h"

namespace LibRendererDll
{
	class Renderer;

	// This class manager texture sampler states
	class SamplerState
	{
	public:
		virtual LIBRENDERER_DLL const bool	SetAnisotropy(const unsigned int slot, const float anisotropy);
		virtual LIBRENDERER_DLL const bool	SetMipLodBias(const unsigned int slot, const float lodBias);
		virtual LIBRENDERER_DLL const bool	SetFilter(const unsigned int slot, const SamplerFilter filter);
		virtual LIBRENDERER_DLL const bool	SetBorderColor(const unsigned int slot, const Vec4f rgba);
		virtual LIBRENDERER_DLL const bool	SetAddressingModeU(const unsigned int slot, const SamplerAddressingMode samU);
		virtual LIBRENDERER_DLL const bool	SetAddressingModeV(const unsigned int slot, const SamplerAddressingMode samV);
		virtual LIBRENDERER_DLL const bool	SetAddressingModeW(const unsigned int slot, const SamplerAddressingMode samW);
		virtual LIBRENDERER_DLL const bool	SetAddressingMode(const unsigned int slot, const SamplerAddressingMode samUVW);
		virtual	LIBRENDERER_DLL	const bool	SetSRGBEnabled(const unsigned int slot, const bool enabled);

		LIBRENDERER_DLL const float					GetAnisotropy(const unsigned int slot) const;
		LIBRENDERER_DLL const float					GetMipLodBias(const unsigned int slot) const;
		LIBRENDERER_DLL const SamplerFilter			GetFilter(const unsigned int slot) const;
		LIBRENDERER_DLL const Vec4f					GetBorderColor(const unsigned int slot) const;
		LIBRENDERER_DLL const SamplerAddressingMode	GetAddressingModeU(const unsigned int slot) const;
		LIBRENDERER_DLL const SamplerAddressingMode	GetAddressingModeV(const unsigned int slot) const;
		LIBRENDERER_DLL const SamplerAddressingMode	GetAddressingModeW(const unsigned int slot) const;
		LIBRENDERER_DLL const SamplerAddressingMode	GetAddressingMode(const unsigned int slot) const;
		LIBRENDERER_DLL	const bool					GetSRGBEnabled(const unsigned int slot) const;

		virtual	LIBRENDERER_DLL		void	Reset() = 0;

	protected:
		SamplerState();
		virtual ~SamplerState();

		SamplerStateDesc m_tCurrentState[MAX_NUM_PSAMPLERS];

		friend class Renderer;
	};
}

#endif // SAMPLERSTATE_H
