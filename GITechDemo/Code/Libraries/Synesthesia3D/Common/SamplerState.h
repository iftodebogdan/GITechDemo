/**
 *	@file		SamplerState.h
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

#ifndef SAMPLERSTATE_H
#define SAMPLERSTATE_H

#include "ResourceData.h"

namespace Synesthesia3D
{
	class Renderer;

	/**
	 * @brief	Manages texture sampler states.
	 */
	class SamplerState
	{

	public:

		/**
		 * @brief	Sets the anisotropic filter quality level.
		 *
		 * @param[in]	slot		Texture sampler index.
		 * @param[in]	anisotropy	Anisotropic filtering level.
		 *
		 * @return	Success of operation.
		 */
				SYNESTHESIA3D_DLL	const bool	SetAnisotropy(const unsigned int slot, const unsigned int anisotropy);

		/**
		 * @brief	Sets the mip LOD bias.
		 *
		 * @param[in]	slot		Texture sampler index.
		 * @param[in]	lodBias		Texture LOD bias.
		 *
		 * @return	Success of operation.
		 */
				SYNESTHESIA3D_DLL	const bool	SetMipLodBias(const unsigned int slot, const float lodBias);

		/**
		 * @brief	Sets the texture filtering type.
		 *
		 * @param[in]	slot		Texture sampler index.
		 * @param[in]	filter		Filtering type.
		 *
		 * @return	Success of operation.
		 *
		 * @see		SamplerFilter
		 */
				SYNESTHESIA3D_DLL	const bool	SetFilter(const unsigned int slot, const SamplerFilter filter);

		/**
		 * @brief	Sets the color of the border.
		 * @note	Has effect if addressing mode is @ref SAM_BORDER.
		 *
		 * @param[in]	slot		Texture sampler index.
		 * @param[in]	rgba		Color of border.
		 *
		 * @return	Success of operation.
		 */
				SYNESTHESIA3D_DLL	const bool	SetBorderColor(const unsigned int slot, const Vec4f& rgba);

		/**
		 * @brief	Sets the texture addressing mode on the U axis.
		 *
		 * @param[in]	slot		Texture sampler index.
		 * @param[in]	samU		Addressing mode on the U axis.
		 *
		 * @return	Success of operation.
		 */
				SYNESTHESIA3D_DLL	const bool	SetAddressingModeU(const unsigned int slot, const SamplerAddressingMode samU);

		/**
		 * @brief	Sets the texture addressing mode on the V axis.
		 *
		 * @param[in]	slot		Texture sampler index.
		 * @param[in]	samV		Addressing mode on the V axis.
		 *
		 * @return	Success of operation.
		 */
				SYNESTHESIA3D_DLL	const bool	SetAddressingModeV(const unsigned int slot, const SamplerAddressingMode samV);

		/**
		 * @brief	Sets the texture addressing mode on the W axis.
		 *
		 * @param[in]	slot		Texture sampler index.
		 * @param[in]	samW		Addressing mode on the W axis.
		 *
		 * @return	Success of operation.
		 */
				SYNESTHESIA3D_DLL	const bool	SetAddressingModeW(const unsigned int slot, const SamplerAddressingMode samW);

		/**
		 * @brief	Sets the texture addressing mode.
		 *
		 * @param[in]	slot		Texture sampler index.
		 * @param[in]	samUVW		Addressing mode on all axes.
		 *
		 * @return	Success of operation.
		 */
				SYNESTHESIA3D_DLL	const bool	SetAddressingMode(const unsigned int slot, const SamplerAddressingMode samUVW);

		/**
		 * @brief	Enables gamma correction when sampling from the texture.
		 * @note	Textures that have content encoded in gamma space (sRGB encoded)
		 *			should have this state enabled so as to linearize it when sampling.
		 *
		 * @param[in]	slot		Texture sampler index.
		 * @param[in]	enabled		Enable sRGB for texture.
		 *
		 * @return	Success of operation.
		 */
				SYNESTHESIA3D_DLL	const bool	SetSRGBEnabled(const unsigned int slot, const bool enabled);



		/**
		 * @brief	Retrieves the anisotropic filter quality level.
		 *
		 * @param[in]	slot		Texture sampler index.
		 *
		 * @return	The anisoptropy level for specified sampler slot.
		 */
				SYNESTHESIA3D_DLL	const unsigned int			GetAnisotropy(const unsigned int slot) const;

		/**
		 * @brief	Retrieves the mip LOD bias.
		 *
		 * @param[in]	slot		Texture sampler index.
		 *
		 * @return	Mip LoD bias for specified sampler slot.
		 */
				SYNESTHESIA3D_DLL	const float					GetMipLodBias(const unsigned int slot) const;

		/**
		 * @brief	Retrieves the texture filtering type.
		 *
		 * @param[in]	slot		Texture sampler index.
		 *
		 * @return	Filter type for specified sampler slot.
		 */
				SYNESTHESIA3D_DLL	const SamplerFilter			GetFilter(const unsigned int slot) const;

		/**
		 * @brief	Retrieves the color of the border.
		 *
		 * @param[in]	slot		Texture sampler index.
		 *
		 * @return	Border color for specified sampler slot.
		 */
				SYNESTHESIA3D_DLL	const Vec4f&				GetBorderColor(const unsigned int slot) const;

		/**
		 * @brief	Retrieves the texture addressing mode on the U axis.
		 *
		 * @param[in]	slot		Texture sampler index.
		 *
		 * @return	Sampler addressing mode on U axis for specified sampler slot.
		 */
				SYNESTHESIA3D_DLL	const SamplerAddressingMode	GetAddressingModeU(const unsigned int slot) const;

		/**
		 * @brief	Retrieves the texture addressing mode on the V axis.
		 *
		 * @param[in]	slot		Texture sampler index.
		 *
		 * @return	Sampler addressing mode on V axis for specified sampler slot.
		 */
				SYNESTHESIA3D_DLL	const SamplerAddressingMode	GetAddressingModeV(const unsigned int slot) const;

		/**
		 * @brief	Retrieves the texture addressing mode on the W axis.
		 *
		 * @param[in]	slot		Texture sampler index.
		 *
		 * @return	Sampler addressing mode on W axis for specified sampler slot.
		 */
				SYNESTHESIA3D_DLL	const SamplerAddressingMode	GetAddressingModeW(const unsigned int slot) const;

		/**
		 * @brief	Retrieves the texture addressing mode.
		 *
		 * @param[in]	slot		Texture sampler index.
		 *
		 * @return	Sampler addressing mode for specified sampler slot.
		 */
				SYNESTHESIA3D_DLL	const SamplerAddressingMode	GetAddressingMode(const unsigned int slot) const;

		/**
		 * @brief	Checks whether the texture is sRGB encoded.
		 *
		 * @param[in]	slot		Texture sampler index.
		 *
		 * @return	sRGB for specified sampler slot.
		 */
				SYNESTHESIA3D_DLL	const bool					GetSRGBEnabled(const unsigned int slot) const;

		/**
		 * @brief	Reset the sampler states to their default values.
		 * @note	Affects all @ref MAX_NUM_PSAMPLERS texture slots.
		 */
		virtual	SYNESTHESIA3D_DLL		void	Reset();

	protected:

		/**
		 * @brief	Constructor.
		 * @note	To be used only by @ref Renderer::Initialize().
		 */
		SamplerState();

		/**
		 * @brief	Destructor.
		 * @note	To be used only by @ref Renderer::DestroyInstance().
		 */
		virtual ~SamplerState();

		/**
		 * @brief	Pushes the current sampler states to the underlying API
		 */
		virtual	const bool	Flush() PURE_VIRTUAL;

		SamplerStateDesc m_tCurrentState[MAX_NUM_PSAMPLERS];	/**< @brief The current sampler states for all @ref MAX_NUM_PSAMPLERS slots. */

		friend class Renderer;
	};
}

#endif // SAMPLERSTATE_H
