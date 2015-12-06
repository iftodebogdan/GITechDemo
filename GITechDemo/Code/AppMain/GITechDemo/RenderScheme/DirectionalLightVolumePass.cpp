/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	DirectionalLightVolumePass.cpp
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
#include <ResourceManager.h>
#include <RenderState.h>
#include <RenderTarget.h>
#include <Texture.h>
using namespace Synesthesia3D;

#include "GITechDemo.h"
#include "DirectionalLightVolumePass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

namespace GITechDemoApp
{
	extern float CASCADE_MAX_VIEW_DEPTH;

	bool DIR_LIGHT_VOLUME_ENABLE = true;
	bool DIR_LIGHT_VOLUME_QUARTER_RES = true;
	bool DIR_LIGHT_VOLUME_BLUR_SAMPLES = true;
	bool DIR_LIGHT_VOLUME_BLUR_DEPTH_AWARE = true;
	bool DIR_LIGHT_VOLUME_UPSCALE_DEPTH_AWARE = true;
}

DirectionalLightVolumePass::DirectionalLightVolumePass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{
	fElapsedTime = 0.f;
}

DirectionalLightVolumePass::~DirectionalLightVolumePass()
{}

void DirectionalLightVolumePass::Update(const float fDeltaTime)
{
	if (DIR_LIGHT_VOLUME_QUARTER_RES)
		VolumetricLightAccumulationBuffer = VolumetricLightQuarterBuffer;
	else
		VolumetricLightAccumulationBuffer = VolumetricLightFullBuffer;

	Vec4f v4CamPosLightVS = f44ScreenToLightViewMat * Vec4f(0.f, 0.f, 0.f, 1.f);
	v4CamPosLightVS /= v4CamPosLightVS[3];
	f3CameraPositionLightVS = Vec3f(v4CamPosLightVS[0], v4CamPosLightVS[1], v4CamPosLightVS[2]);
	fRaymarchDistanceLimit = CASCADE_MAX_VIEW_DEPTH;
	f2TexSize = Vec2f(
		(float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
		(float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight()
		);
	bSingleChannelCopy = true;
	BayerMatrix.GetTexture()->SetAddressingMode(SAM_WRAP);
	BayerMatrix.GetTexture()->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	texDitherMap = BayerMatrix.GetTextureIndex();
	NoiseTexture.GetTexture()->SetAddressingMode(SAM_WRAP);
	NoiseTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	texNoise = NoiseTexture.GetTextureIndex();
	fElapsedTime += fDeltaTime;
	f3FogBox = Vec3f(CASCADE_MAX_VIEW_DEPTH, CASCADE_MAX_VIEW_DEPTH, CASCADE_MAX_VIEW_DEPTH);
}

void DirectionalLightVolumePass::CalculateLightVolume()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Raymarch (scatter)");

	VolumetricLightAccumulationBuffer[0]->Enable();

	const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

	f2HalfTexelOffset = Vec2f(0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(), 0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight());
	texShadowMap = ShadowMapDir.GetRenderTarget()->GetDepthBuffer();
	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();

	DirectionalLightVolumeShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	DirectionalLightVolumeShader.Disable();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);

	VolumetricLightAccumulationBuffer[0]->Disable();

	POP_PROFILE_MARKER();
}

void DirectionalLightVolumePass::GatherSamples()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Bilateral blur (gather)");

	const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

	const float fBlurDepthFalloffBkp = fBlurDepthFalloff;
	if (!DIR_LIGHT_VOLUME_BLUR_DEPTH_AWARE)
		fBlurDepthFalloff = 0.f;

	PUSH_PROFILE_MARKER("Horizontal");

	VolumetricLightAccumulationBuffer[1]->Enable();

	f2HalfTexelOffset = Vec2f(0.5f / VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(), 0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight());
	f2TexelSize = Vec2f(
		1.f / (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
		1.f / (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight()
		);
	f2DepthHalfTexelOffset = Vec2f(
		0.5f / (float)LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
		0.5f / (float)LightAccumulationBuffer.GetRenderTarget()->GetHeight()
		);
	ResourceMgr->GetTexture(
		VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(
		VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
		)->SetAddressingMode(SAM_CLAMP);
	texSource = VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0);
	f2BlurDir = Vec2f(1.f, 0.f);

	BilateralBlurShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	BilateralBlurShader.Disable();

	VolumetricLightAccumulationBuffer[1]->Disable();

	POP_PROFILE_MARKER();

	PUSH_PROFILE_MARKER("Vertical");

	VolumetricLightAccumulationBuffer[0]->Enable();

	f2HalfTexelOffset = Vec2f(0.5f / VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetWidth(), 0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight());
	f2TexelSize = Vec2f(
		1.f / (float)VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetWidth(),
		1.f / (float)VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetHeight()
		);
	f2DepthHalfTexelOffset = Vec2f(
		0.5f / (float)LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
		0.5f / (float)LightAccumulationBuffer.GetRenderTarget()->GetHeight()
		);
	ResourceMgr->GetTexture(
		VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(
		VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
		)->SetAddressingMode(SAM_CLAMP);
	texSource = VolumetricLightAccumulationBuffer[1]->GetRenderTarget()->GetColorBuffer(0);
	f2BlurDir = Vec2f(0.f, 1.f);

	BilateralBlurShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	BilateralBlurShader.Disable();

	VolumetricLightAccumulationBuffer[0]->Disable();

	POP_PROFILE_MARKER();
	
	if (!DIR_LIGHT_VOLUME_BLUR_DEPTH_AWARE)
		fBlurDepthFalloff = fBlurDepthFalloffBkp;

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);

	POP_PROFILE_MARKER();
}

void DirectionalLightVolumePass::ApplyLightVolume()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Apply (bilateral upscale)");

	const float fUpsampleDepthThresholdBkp = fUpsampleDepthThreshold;
	if (!DIR_LIGHT_VOLUME_UPSCALE_DEPTH_AWARE)
		fUpsampleDepthThreshold = 1.f;

	LightAccumulationBuffer.Enable();

	f2HalfTexelOffset = Vec2f(
		0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
		0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight()
		);
	f2TexelSize = Vec2f(
		1.f / (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
		1.f / (float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight()
		);
	f2DepthHalfTexelOffset = Vec2f(
		0.5f / (float)LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
		0.5f / (float)LightAccumulationBuffer.GetRenderTarget()->GetHeight()
		);
	f2TexSize = Vec2f(
		(float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetWidth(),
		(float)VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetHeight()
		);
	ResourceMgr->GetTexture(
		VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(
		VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
		)->SetAddressingMode(SAM_CLAMP);
	texSource = VolumetricLightAccumulationBuffer[0]->GetRenderTarget()->GetColorBuffer(0);
	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();

	if (DIR_LIGHT_VOLUME_QUARTER_RES)
	{
		NearestDepthUpscaleShader.Enable();
		RenderContext->DrawVertexBuffer(FullScreenTri);
		NearestDepthUpscaleShader.Disable();
	}
	else
	{
		ColorCopyShader.Enable();
		RenderContext->DrawVertexBuffer(FullScreenTri);
		ColorCopyShader.Disable();
	}

	LightAccumulationBuffer.Disable();

	if (!DIR_LIGHT_VOLUME_UPSCALE_DEPTH_AWARE)
		fUpsampleDepthThreshold = fUpsampleDepthThresholdBkp;

	POP_PROFILE_MARKER();
}

void DirectionalLightVolumePass::Draw()
{
	if (DIR_LIGHT_VOLUME_ENABLE)
	{
		Synesthesia3D::RenderTarget* pCurrRT = Synesthesia3D::RenderTarget::GetActiveRenderTarget();
		if (pCurrRT)
			pCurrRT->Disable();
		
		CalculateLightVolume();

		if (DIR_LIGHT_VOLUME_BLUR_SAMPLES)
			GatherSamples();

		ApplyLightVolume();

		pCurrRT->Enable();
	}
}
