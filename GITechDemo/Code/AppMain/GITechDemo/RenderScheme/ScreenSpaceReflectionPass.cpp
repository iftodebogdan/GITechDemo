/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	ScreenSpaceReflectionPass.cpp
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
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderTarget.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "ScreenSpaceReflectionPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

namespace GITechDemoApp
{
	bool SSR_ENABLED = true;
	bool SSR_MANUAL_MAX_STEPS = false;
	extern float CAMERA_FOV;
}

ScreenSpaceReflectionPass::ScreenSpaceReflectionPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{}

ScreenSpaceReflectionPass::~ScreenSpaceReflectionPass()
{}

void ScreenSpaceReflectionPass::Update(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	Synesthesia3D::RenderTarget* ltAccBuf = LightAccumulationBuffer.GetRenderTarget();

	if (!m_pLightAccumulationBufferCopyRT ||
		ltAccBuf->GetWidth() != m_pLightAccumulationBufferCopyRT->GetWidth() ||
		ltAccBuf->GetHeight() != m_pLightAccumulationBufferCopyRT->GetHeight())
	{
		if (m_pLightAccumulationBufferCopyRT)
		{
			ResourceMgr->ReleaseRenderTarget(m_nLightAccumulationBufferCopyRTIdx);
			m_pLightAccumulationBufferCopyRT = nullptr;
		}

		m_nLightAccumulationBufferCopyRTIdx = ResourceMgr->CreateRenderTarget(
			ltAccBuf->GetTargetCount(),
			ltAccBuf->GetPixelFormat(),
			ltAccBuf->GetWidth(),
			ltAccBuf->GetHeight(),
			true,
			false,
			PF_NONE);
		m_pLightAccumulationBufferCopyRT = ResourceMgr->GetRenderTarget(m_nLightAccumulationBufferCopyRTIdx);
	}

	Synesthesia3D::Texture* const ltAccBufCopy = ResourceMgr->GetTexture(m_pLightAccumulationBufferCopyRT->GetColorBuffer());
	ltAccBufCopy->SetAddressingMode(SAM_BORDER);
	ltAccBufCopy->SetBorderColor(Vec4f(0.f, 0.f, 0.f, 0.f));
	ltAccBufCopy->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);

	f2HalfTexelOffset = Vec2f(0.5f / m_pLightAccumulationBufferCopyRT->GetWidth(), 0.5f / m_pLightAccumulationBufferCopyRT->GetHeight());
	texSource = ltAccBuf->GetColorBuffer();
	bSingleChannelCopy = false;

	texHDRSceneTexture = m_pLightAccumulationBufferCopyRT->GetColorBuffer();
	texLinDepthBuffer = LinearFullDepthBuffer.GetRenderTarget()->GetColorBuffer();
	texNormalBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(1);

	f4TexSize = Vec4f(
		(float)ltAccBufCopy->GetWidth(),
		(float)ltAccBufCopy->GetHeight(),
		1.f / (float)ltAccBufCopy->GetWidth(),
		1.f / (float)ltAccBufCopy->GetHeight()
	);
	nTexMipCount = ltAccBufCopy->GetMipCount();
	
	texDitherMap = BayerMatrix.GetTextureIndex();

	Synesthesia3D::Texture* const linFullDepthBuf = ResourceMgr->GetTexture(LinearFullDepthBuffer.GetRenderTarget()->GetColorBuffer());
	linFullDepthBuf->SetAddressingMode(SAM_BORDER);
	linFullDepthBuf->SetBorderColor(Vec4f(0.f, 0.f, 0.f, 0.f));
	linFullDepthBuf->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	Matrix44f rasterScaleMat;
	const float width = (float)ltAccBuf->GetWidth(), height = (float)ltAccBuf->GetHeight();
	const float sx = width * 0.5f;
	const float sy = height * 0.5f;
	rasterScaleMat.set(
		sx,   0, 0, sx,
		 0, -sy, 0, sy,
		 0,   0, 1,  0,
		 0,   0, 0,  1);
	f44ViewToRasterMat = rasterScaleMat * f44ProjMat.GetCurrentValue();

	if(!SSR_MANUAL_MAX_STEPS)
		fMaxSteps = ceilf(sqrtf(width * width + height * height) / fSampleStride.GetCurrentValue());
}

void ScreenSpaceReflectionPass::Draw()
{
	if (!SSR_ENABLED)
		return;

	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	CopyLightAccumulationBuffer();
	ApplyScreenSpaceReflection();
}

void ScreenSpaceReflectionPass::CopyLightAccumulationBuffer()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Light accumulation buffer copy and auto mip generation");

	RenderState* RenderStateManager = RenderContext->GetRenderStateManager();
	const bool blendEnabled = RenderStateManager->GetColorBlendEnabled();
	RenderStateManager->SetColorBlendEnabled(false);
	
	m_pLightAccumulationBufferCopyRT->Enable();

	ColorCopyShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	ColorCopyShader.Disable();

	m_pLightAccumulationBufferCopyRT->Disable();

	RenderStateManager->SetColorBlendEnabled(blendEnabled);

	POP_PROFILE_MARKER();
}

void ScreenSpaceReflectionPass::ApplyScreenSpaceReflection()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("SSR Apply");

	//RenderState* RenderStateManager = RenderContext->GetRenderStateManager();
	//const bool blendEnabled = RenderStateManager->GetColorBlendEnabled();
	//
	//RenderStateManager->SetColorBlendEnabled(false);
	//RenderStateManager->SetColorSrcBlend(BLEND_ZERO);
	//RenderStateManager->SetColorDstBlend(BLEND_SRCCOLOR);

	ScreenSpaceReflectionShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	ScreenSpaceReflectionShader.Disable();

	//RenderStateManager->SetColorBlendEnabled(blendEnabled);

	POP_PROFILE_MARKER();
}
