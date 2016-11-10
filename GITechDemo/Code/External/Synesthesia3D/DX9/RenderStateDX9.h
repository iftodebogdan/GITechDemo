/**
 *	@file		RenderStateDX9.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef RENDERSTATEDX9_H
#define RENDERSTATEDX9_H

#include "RenderState.h"

namespace Synesthesia3D
{
	class RenderStateDX9 : public RenderState
	{
	public:
		const unsigned int MatchRenderState(const DWORD rs, const unsigned int rsEnumClass) const;

		const bool	SetScissor(const Vec2i size, const Vec2i offset = Vec2i(0, 0));

		void		Reset();

	protected:
		RenderStateDX9();
		~RenderStateDX9();

		const bool Flush();

		// Local render states, sync'ed with underlying API
		bool			m_bColorBlendEnabledDX9;
		Blend			m_eColorSrcBlendDX9;
		Blend			m_eColorDstBlendDX9;
		Vec4f			m_vColorBlendFactorDX9;
		bool			m_bAlphaTestEnabledDX9;
		Cmp				m_eAlphaFuncDX9;
		float			m_fAlphaRefDX9;
		Cull			m_eCullModeDX9;
		ZBuffer			m_eZEnabledDX9;
		Cmp				m_eZFuncDX9;
		bool			m_bZWriteEnabledDX9;
		bool			m_bColorWriteRedDX9;
		bool			m_bColorWriteGreenDX9;
		bool			m_bColorWriteBlueDX9;
		bool			m_bColorWriteAlphaDX9;
		float			m_fSlopeScaledDepthBiasDX9;
		float			m_fDepthBiasDX9;
		bool			m_bStencilEnabledDX9;
		Cmp				m_eStencilFuncDX9;
		unsigned long	m_lStencilRefDX9;
		unsigned long	m_lStencilMaskDX9;
		unsigned long	m_lStencilWriteMaskDX9;
		StencilOp		m_eStencilFailDX9;
		StencilOp		m_eStencilZFailDX9;
		StencilOp		m_eStencilPassDX9;
		Fill			m_eFillModeDX9;
		bool			m_bScissorEnabledDX9;
		bool			m_bSRGBEnabledDX9;

		friend class RendererDX9;
	};
}

#endif // RENDERSTATEDX9_H