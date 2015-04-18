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
		virtual	LIBRENDERER_DLL	const bool			SetAlphaTestEnable(const bool enabled) { m_bAlphaTestEnable = enabled; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetAlphaTestFunc(const Cmp alphaFunc) { m_eAlphaFunc = alphaFunc; return true; }
		// normAlphaRef is normalized (i.e. between [0, 1])
		virtual	LIBRENDERER_DLL	const bool			SetAlphaTestRef(const float normAlphaRef) { m_fAlphaRef = normAlphaRef; return true; }

		virtual	LIBRENDERER_DLL	const bool			SetColorBlendEnable(const bool enabled) { m_bColorBlendEnable = enabled; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetColorSrcBlend(const Blend alphaSrc) { m_eColorSrcBlend = alphaSrc; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetColorDstBlend(const Blend alphaDst) { m_eColorDstBlend = alphaDst; return true; }
		// Color components are normalized (i.e. between [0, 1])
		virtual	LIBRENDERER_DLL	const bool			SetColorBlendFactor(const Vec4f rgba) { m_vColorBlendFactor = rgba; return true; }

		virtual	LIBRENDERER_DLL	const bool			SetCullMode(const Cull cullMode) { m_eCullMode = cullMode; return true; }

		virtual	LIBRENDERER_DLL	const bool			SetZEnable(const ZBuffer enabled) { m_eZEnable = enabled; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetZFunc(const Cmp zFunc) { m_eZFunc = zFunc; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetZWriteEnabled(const bool enabled) { m_bZWriteEnable = enabled; return true; }

		virtual	LIBRENDERER_DLL	const bool			SetColorWriteEnabled(const bool red, const bool green, const bool blue, const bool alpha) { m_bColorWriteRed = red; m_bColorWriteGreen = green; m_bColorWriteBlue = blue; m_bColorWriteAlpha = alpha; return true; }

		virtual	LIBRENDERER_DLL	const bool			SetSlopeScaleDepthBias(const float scale) { m_fSlopeScaleDepthBias = scale; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetDepthBias(const float bias) { m_fDepthBias = bias; return true; }

		virtual	LIBRENDERER_DLL	const bool			SetStencilEnable(const bool enabled) { m_bStencilEnable = enabled; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetStencilFunc(const Cmp stencilFunc) { m_eStencilFunc = stencilFunc; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetStencilRef(const unsigned long stencilRef) { m_lStencilRef = stencilRef; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetStencilMask(const unsigned long stencilMask) { m_lStencilMask = stencilMask; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetStencilWriteMask(const unsigned long stencilWriteMask) { m_lStencilWriteMask = stencilWriteMask; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetStencilFail(const StencilOp stencilFail) { m_eStencilFail = stencilFail; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetStencilZFail(const StencilOp stencilZFail) { m_eStencilZFail = stencilZFail; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetStencilPass(const StencilOp stencilPass) { m_eStencilPass = stencilPass; return true; }

		virtual	LIBRENDERER_DLL	const bool			SetFillMode(const Fill fillMode) { m_eFillMode = fillMode; return true; }

		virtual	LIBRENDERER_DLL	const bool			SetScissorEnable(const bool enabled) { m_bScissorEnable = enabled; return true; }
		virtual	LIBRENDERER_DLL	const bool			SetScissor(const Vec2i size, const Vec2i offset = Vec2i(0, 0)) { return true; }


				LIBRENDERER_DLL	const bool			GetAlphaTestEnable() const { return m_bAlphaTestEnable; }
				LIBRENDERER_DLL	const float			GetAlphaTestRef() const { return m_fAlphaRef; }
				LIBRENDERER_DLL	const Cmp			GetAlphaTestFunc() const { return m_eAlphaFunc; }

				LIBRENDERER_DLL	const bool			GetColorBlendEnable() const { return m_bColorBlendEnable; }
				LIBRENDERER_DLL	const Blend			GetColorSrcBlend() const { return m_eColorSrcBlend; }
				LIBRENDERER_DLL	const Blend			GetColorDstBlend() const { return m_eColorDstBlend; }
				LIBRENDERER_DLL	const Vec4f			GetColorBlendFactor() const { return m_vColorBlendFactor; }

				LIBRENDERER_DLL	const Cull			GetCullMode() const { return m_eCullMode; }

				LIBRENDERER_DLL	const ZBuffer		GetZEnable() const { return m_eZEnable; }
				LIBRENDERER_DLL	const Cmp			GetZFunc() const { return m_eZFunc; }
				LIBRENDERER_DLL	const bool			GetZWriteEnabled() const { return m_bZWriteEnable; }

				LIBRENDERER_DLL		void			GetColorWriteEnabled(bool& red, bool& green, bool& blue, bool& alpha) { red = m_bColorWriteRed; green = m_bColorWriteGreen; blue = m_bColorWriteBlue; alpha = m_bColorWriteAlpha; }
				LIBRENDERER_DLL	const bool			GetColorWriteRedEnabled() { return m_bColorWriteRed; }
				LIBRENDERER_DLL	const bool			GetColorWriteGreenEnabled() { return m_bColorWriteGreen; }
				LIBRENDERER_DLL	const bool			GetColorWriteBlueEnabled() { return m_bColorWriteBlue; }
				LIBRENDERER_DLL	const bool			GetColorWriteAlphaEnabled() { return m_bColorWriteAlpha; }

				LIBRENDERER_DLL	const float			GetSlopeScaleDepthBias() const { return m_fSlopeScaleDepthBias; }
				LIBRENDERER_DLL	const float			GetDepthBias() const { return m_fDepthBias; }

				LIBRENDERER_DLL	const bool			GetStencilEnable() const { return m_bStencilEnable; }
				LIBRENDERER_DLL	const Cmp			GetStencilFunc() const { return m_eStencilFunc; }
				LIBRENDERER_DLL	const unsigned long	GetStencilRef() const { return m_lStencilRef; }
				LIBRENDERER_DLL	const unsigned long	GetStencilMask() const { return m_lStencilMask; }
				LIBRENDERER_DLL	const unsigned long	GetStencilWriteMask() const { return m_lStencilWriteMask; }
				LIBRENDERER_DLL	const StencilOp		GetStencilFail() const { return m_eStencilFail; }
				LIBRENDERER_DLL	const StencilOp		GetStencilZFail() const { return m_eStencilZFail; }
				LIBRENDERER_DLL	const StencilOp		GetStencilPass() const { return m_eStencilPass; }

				LIBRENDERER_DLL	const Fill			GetFillMode() const { return m_eFillMode; }

				LIBRENDERER_DLL	const bool			GetScissorEnable() { return m_bScissorEnable; }

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