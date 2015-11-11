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
#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include "ResourceData.h"

namespace LibRendererDll
{
	class Renderer;

	// This is the platform independent render state manager class
	class RenderState
	{
	public:
		virtual	LIBRENDERER_DLL	const bool	SetAlphaTestEnabled(const bool enabled);
		virtual	LIBRENDERER_DLL	const bool	SetAlphaTestFunc(const Cmp alphaFunc);
		virtual	LIBRENDERER_DLL	const bool	SetAlphaTestRef(const float normAlphaRef); // normAlphaRef is normalized (i.e. between [0, 1])

		virtual	LIBRENDERER_DLL	const bool	SetColorBlendEnabled(const bool enabled);
		virtual	LIBRENDERER_DLL	const bool	SetColorSrcBlend(const Blend alphaSrc);
		virtual	LIBRENDERER_DLL	const bool	SetColorDstBlend(const Blend alphaDst);
		virtual	LIBRENDERER_DLL	const bool	SetColorBlendFactor(const Vec4f rgba); // Color components are normalized (i.e. between [0, 1])

		virtual	LIBRENDERER_DLL	const bool	SetCullMode(const Cull cullMode);

		virtual	LIBRENDERER_DLL	const bool	SetZEnabled(const ZBuffer enabled);
		virtual	LIBRENDERER_DLL	const bool	SetZFunc(const Cmp zFunc);
		virtual	LIBRENDERER_DLL	const bool	SetZWriteEnabled(const bool enabled);

		virtual	LIBRENDERER_DLL	const bool	SetColorWriteEnabled(const bool red, const bool green, const bool blue, const bool alpha);

		virtual	LIBRENDERER_DLL	const bool	SetSlopeScaledDepthBias(const float scale);
		virtual	LIBRENDERER_DLL	const bool	SetDepthBias(const float bias);

		virtual	LIBRENDERER_DLL	const bool	SetStencilEnabled(const bool enabled);
		virtual	LIBRENDERER_DLL	const bool	SetStencilFunc(const Cmp stencilFunc);
		virtual	LIBRENDERER_DLL	const bool	SetStencilRef(const unsigned long stencilRef);
		virtual	LIBRENDERER_DLL	const bool	SetStencilMask(const unsigned long stencilMask);
		virtual	LIBRENDERER_DLL	const bool	SetStencilWriteMask(const unsigned long stencilWriteMask);
		virtual	LIBRENDERER_DLL	const bool	SetStencilFail(const StencilOp stencilFail);
		virtual	LIBRENDERER_DLL	const bool	SetStencilZFail(const StencilOp stencilZFail);
		virtual	LIBRENDERER_DLL	const bool	SetStencilPass(const StencilOp stencilPass);

		virtual	LIBRENDERER_DLL	const bool	SetFillMode(const Fill fillMode);

		virtual	LIBRENDERER_DLL	const bool	SetScissorEnabled(const bool enabled);
		virtual	LIBRENDERER_DLL	const bool	SetScissor(const Vec2i size, const Vec2i offset = Vec2i(0, 0)) PURE_VIRTUAL

		virtual	LIBRENDERER_DLL	const bool	SetSRGBWriteEnabled(const bool enabled);


		LIBRENDERER_DLL	const bool			GetAlphaTestEnabled() const;
		LIBRENDERER_DLL	const float			GetAlphaTestRef() const;
		LIBRENDERER_DLL	const Cmp			GetAlphaTestFunc() const;

		LIBRENDERER_DLL	const bool			GetColorBlendEnabled() const;
		LIBRENDERER_DLL	const Blend			GetColorSrcBlend() const;
		LIBRENDERER_DLL	const Blend			GetColorDstBlend() const;
		LIBRENDERER_DLL	const Vec4f			GetColorBlendFactor() const;

		LIBRENDERER_DLL	const Cull			GetCullMode() const;

		LIBRENDERER_DLL	const ZBuffer		GetZEnabled() const;
		LIBRENDERER_DLL	const Cmp			GetZFunc() const;
		LIBRENDERER_DLL	const bool			GetZWriteEnabled() const;

		LIBRENDERER_DLL		void			GetColorWriteEnabled(bool& red, bool& green, bool& blue, bool& alpha) const;
		LIBRENDERER_DLL	const bool			GetColorWriteRedEnabled() const;
		LIBRENDERER_DLL	const bool			GetColorWriteGreenEnabled() const;
		LIBRENDERER_DLL	const bool			GetColorWriteBlueEnabled() const;
		LIBRENDERER_DLL	const bool			GetColorWriteAlphaEnabled() const;

		LIBRENDERER_DLL	const float			GetSlopeScaledDepthBias() const;
		LIBRENDERER_DLL	const float			GetDepthBias() const;

		LIBRENDERER_DLL	const bool			GetStencilEnabled() const;
		LIBRENDERER_DLL	const Cmp			GetStencilFunc() const;
		LIBRENDERER_DLL	const unsigned long	GetStencilRef() const;
		LIBRENDERER_DLL	const unsigned long	GetStencilMask() const;
		LIBRENDERER_DLL	const unsigned long	GetStencilWriteMask() const;
		LIBRENDERER_DLL	const StencilOp		GetStencilFail() const;
		LIBRENDERER_DLL	const StencilOp		GetStencilZFail() const;
		LIBRENDERER_DLL	const StencilOp		GetStencilPass() const;

		LIBRENDERER_DLL	const Fill			GetFillMode() const;

		LIBRENDERER_DLL	const bool			GetScissorEnabled() const;

		LIBRENDERER_DLL	const bool			GetSRGBWriteEnabled() const;

		virtual	LIBRENDERER_DLL		void	Reset();

	protected:
		RenderState();
		virtual ~RenderState();

		// Alpha states
		bool			m_bColorBlendEnabled;
		Blend			m_eColorSrcBlend;
		Blend			m_eColorDstBlend;
		bool			m_bAlphaTestEnabled;
		Cmp				m_eAlphaFunc;
		float			m_fAlphaRef;
		Vec4f			m_vColorBlendFactor;

		// Culling states
		Cull			m_eCullMode;

		// Depth states
		ZBuffer			m_eZEnabled;
		Cmp				m_eZFunc;
		bool			m_bZWriteEnabled;
		
		// Color write enable
		bool			m_bColorWriteRed;
		bool			m_bColorWriteGreen;
		bool			m_bColorWriteBlue;
		bool			m_bColorWriteAlpha;

		// Depth bias states
		float			m_fSlopeScaledDepthBias;
		float			m_fDepthBias;

		// Stencil states
		bool			m_bStencilEnabled;
		Cmp				m_eStencilFunc;
		unsigned long	m_lStencilRef;
		unsigned long	m_lStencilMask;
		unsigned long	m_lStencilWriteMask;
		StencilOp		m_eStencilFail;
		StencilOp		m_eStencilZFail;
		StencilOp		m_eStencilPass;

		// Fill mode
		Fill			m_eFillMode;

		// Scissor test
		bool			m_bScissorEnabled;

		// sRGB write
		bool			m_bSRGBEnabled;

		friend class Renderer;
	};
}

#endif //RENDERSTATE_H