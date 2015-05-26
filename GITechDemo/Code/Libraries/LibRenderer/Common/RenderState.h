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

	// This is the platform independent render state manager class
	class RenderState
	{
	public:
		virtual	LIBRENDERER_DLL	const bool			SetAlphaTestEnable(const bool enabled);
		virtual	LIBRENDERER_DLL	const bool			SetAlphaTestFunc(const Cmp alphaFunc);
		// normAlphaRef is normalized (i.e. between [0, 1])
		virtual	LIBRENDERER_DLL	const bool			SetAlphaTestRef(const float normAlphaRef);

		virtual	LIBRENDERER_DLL	const bool			SetColorBlendEnable(const bool enabled);
		virtual	LIBRENDERER_DLL	const bool			SetColorSrcBlend(const Blend alphaSrc);
		virtual	LIBRENDERER_DLL	const bool			SetColorDstBlend(const Blend alphaDst);
		// Color components are normalized (i.e. between [0, 1])
		virtual	LIBRENDERER_DLL	const bool			SetColorBlendFactor(const Vec4f rgba);

		virtual	LIBRENDERER_DLL	const bool			SetCullMode(const Cull cullMode);

		virtual	LIBRENDERER_DLL	const bool			SetZEnable(const ZBuffer enabled);
		virtual	LIBRENDERER_DLL	const bool			SetZFunc(const Cmp zFunc);
		virtual	LIBRENDERER_DLL	const bool			SetZWriteEnabled(const bool enabled);

		virtual	LIBRENDERER_DLL	const bool			SetColorWriteEnabled(const bool red, const bool green, const bool blue, const bool alpha);

		virtual	LIBRENDERER_DLL	const bool			SetSlopeScaleDepthBias(const float scale);
		virtual	LIBRENDERER_DLL	const bool			SetDepthBias(const float bias);

		virtual	LIBRENDERER_DLL	const bool			SetStencilEnable(const bool enabled);
		virtual	LIBRENDERER_DLL	const bool			SetStencilFunc(const Cmp stencilFunc);
		virtual	LIBRENDERER_DLL	const bool			SetStencilRef(const unsigned long stencilRef);
		virtual	LIBRENDERER_DLL	const bool			SetStencilMask(const unsigned long stencilMask);
		virtual	LIBRENDERER_DLL	const bool			SetStencilWriteMask(const unsigned long stencilWriteMask);
		virtual	LIBRENDERER_DLL	const bool			SetStencilFail(const StencilOp stencilFail);
		virtual	LIBRENDERER_DLL	const bool			SetStencilZFail(const StencilOp stencilZFail);
		virtual	LIBRENDERER_DLL	const bool			SetStencilPass(const StencilOp stencilPass);

		virtual	LIBRENDERER_DLL	const bool			SetFillMode(const Fill fillMode);

		virtual	LIBRENDERER_DLL	const bool			SetScissorEnable(const bool enabled);
		virtual	LIBRENDERER_DLL	const bool			SetScissor(const Vec2i size, const Vec2i offset = Vec2i(0, 0)) = 0;


		LIBRENDERER_DLL	const bool			GetAlphaTestEnable() const;
		LIBRENDERER_DLL	const float			GetAlphaTestRef() const;
		LIBRENDERER_DLL	const Cmp			GetAlphaTestFunc() const;

		LIBRENDERER_DLL	const bool			GetColorBlendEnable() const;
		LIBRENDERER_DLL	const Blend			GetColorSrcBlend() const;
		LIBRENDERER_DLL	const Blend			GetColorDstBlend() const;
		LIBRENDERER_DLL	const Vec4f			GetColorBlendFactor() const;

		LIBRENDERER_DLL	const Cull			GetCullMode() const;

		LIBRENDERER_DLL	const ZBuffer		GetZEnable() const;
		LIBRENDERER_DLL	const Cmp			GetZFunc() const;
		LIBRENDERER_DLL	const bool			GetZWriteEnabled() const;

		LIBRENDERER_DLL		void			GetColorWriteEnabled(bool& red, bool& green, bool& blue, bool& alpha);
		LIBRENDERER_DLL	const bool			GetColorWriteRedEnabled();
		LIBRENDERER_DLL	const bool			GetColorWriteGreenEnabled();
		LIBRENDERER_DLL	const bool			GetColorWriteBlueEnabled();
		LIBRENDERER_DLL	const bool			GetColorWriteAlphaEnabled();

		LIBRENDERER_DLL	const float			GetSlopeScaleDepthBias() const;
		LIBRENDERER_DLL	const float			GetDepthBias() const;

		LIBRENDERER_DLL	const bool			GetStencilEnable() const;
		LIBRENDERER_DLL	const Cmp			GetStencilFunc() const;
		LIBRENDERER_DLL	const unsigned long	GetStencilRef() const;
		LIBRENDERER_DLL	const unsigned long	GetStencilMask() const;
		LIBRENDERER_DLL	const unsigned long	GetStencilWriteMask() const;
		LIBRENDERER_DLL	const StencilOp		GetStencilFail() const;
		LIBRENDERER_DLL	const StencilOp		GetStencilZFail() const;
		LIBRENDERER_DLL	const StencilOp		GetStencilPass() const;

		LIBRENDERER_DLL	const Fill			GetFillMode() const;

		LIBRENDERER_DLL	const bool			GetScissorEnable();

		virtual	LIBRENDERER_DLL		void			Reset();

	protected:
		RenderState();
		virtual ~RenderState();

		// Alpha states
		bool			m_bColorBlendEnable;
		Blend			m_eColorSrcBlend;
		Blend			m_eColorDstBlend;
		bool			m_bAlphaTestEnable;
		Cmp				m_eAlphaFunc;
		float			m_fAlphaRef;
		Vec4f			m_vColorBlendFactor;

		// Culling states
		Cull			m_eCullMode;

		// Depth states
		ZBuffer			m_eZEnable;
		Cmp				m_eZFunc;
		bool			m_bZWriteEnable;
		
		// Color write enable
		bool			m_bColorWriteRed;
		bool			m_bColorWriteGreen;
		bool			m_bColorWriteBlue;
		bool			m_bColorWriteAlpha;

		// Depth bias states
		float			m_fSlopeScaleDepthBias;
		float			m_fDepthBias;

		// Stencil states
		bool			m_bStencilEnable;
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
		bool			m_bScissorEnable;

		friend class Renderer;
	};
}

#endif //RENDERSTATE_H