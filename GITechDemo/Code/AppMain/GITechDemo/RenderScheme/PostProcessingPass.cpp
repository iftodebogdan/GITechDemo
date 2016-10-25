/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	PostProcessingPass.cpp
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

#include <Renderer.h>
#include <RenderState.h>
#include <RenderTarget.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "PostProcessingPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

namespace GITechDemoApp
{
	bool POST_PROCESSING_ENABLED = true;

	// Tone mapping
	extern bool HDR_TONE_MAPPING_ENABLED;
	// Bloom
	extern bool BLOOM_ENABLED;
	// FXAA
	extern bool FXAA_ENABLED;
	// DoF
	extern bool DOF_ENABLED;
	// Motion blur
	extern bool MOTION_BLUR_ENABLED;
	// Lens flare
	extern bool LENS_FLARE_ENABLED;
}

PostProcessingPass::PostProcessingPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
	, m_pFinalImageBuffer(nullptr)
{}

PostProcessingPass::~PostProcessingPass()
{}

void PostProcessingPass::Update(const float fDeltaTime)
{
	if (POST_PROCESSING_ENABLED)
	{
		m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

		if (DOF_ENABLED)
			m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

		if (MOTION_BLUR_ENABLED)
			m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

		if (BLOOM_ENABLED)
			m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

		if (LENS_FLARE_ENABLED)
			m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

		if (HDR_TONE_MAPPING_ENABLED)
			m_pFinalImageBuffer = LDRToneMappedImageBuffer.GetRenderTarget();

		if (FXAA_ENABLED)
			m_pFinalImageBuffer = LDRFxaaImageBuffer.GetRenderTarget();
	}
	else
		m_pFinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();
}

// Copy texture to the back buffer
void PostProcessingPass::CopyResultToBackBuffer(Synesthesia3D::RenderTarget* const rt)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Copy to back buffer");

	const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
	const bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();

	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);
	
	f2HalfTexelOffset = Vec2f(0.5f / rt->GetWidth(), 0.5f / rt->GetHeight());
	texSource = rt->GetColorBuffer(0);
	bSingleChannelCopy = false;

	ColorCopyShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	ColorCopyShader.Disable();

	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	POP_PROFILE_MARKER();
}

void PostProcessingPass::Draw()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	if (POST_PROCESSING_ENABLED)
		DrawChildren();

	if (m_pFinalImageBuffer)
		CopyResultToBackBuffer(m_pFinalImageBuffer);
}