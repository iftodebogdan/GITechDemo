#include "stdafx.h"

#include <Renderer.h>
#include <RenderState.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderTarget.h>
using namespace LibRendererDll;

#include "DepthOfFieldPass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

#include "GITechDemo.h"

namespace GITechDemoApp
{
	bool DOF_ENABLED = false;
	bool DOF_USE_QUARTER_RESOLUTION_BUFFER = false;
	float DOF_AUTOFOCUS_TIME = 0.25f;
}

DepthOfFieldPass::DepthOfFieldPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{}

DepthOfFieldPass::~DepthOfFieldPass()
{}

void DepthOfFieldPass::OnUpdate(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	ResourceMgr->GetTexture(
		DepthOfFieldFullBuffer.GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(
		DepthOfFieldFullBuffer.GetRenderTarget()->GetColorBuffer(0)
		)->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(
		DepthOfFieldQuarterBuffer.GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(
		DepthOfFieldQuarterBuffer.GetRenderTarget()->GetColorBuffer(0)
		)->SetAddressingMode(SAM_CLAMP);

	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
	f2TexelSize = Vec2f(
		1.f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
		1.f / LightAccumulationBuffer.GetRenderTarget()->GetHeight()
		);
	f2TexSourceSize = Vec2f(
		(float)LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
		(float)LightAccumulationBuffer.GetRenderTarget()->GetHeight()
		);

	ResourceMgr->GetTexture(AutofocusBuffer[0]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(AutofocusBuffer[1]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	GITechDemoApp::RenderTarget* const rtBkp = AutofocusBuffer[0];
	AutofocusBuffer[0] = AutofocusBuffer[1];
	AutofocusBuffer[1] = rtBkp;

	texTargetFocus = AutofocusBuffer[0]->GetRenderTarget()->GetColorBuffer();
}

void DepthOfFieldPass::AutofocusPass()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	if (!bAutofocus)
		return;

	PUSH_PROFILE_MARKER("Autofocus pass");

	AutofocusBuffer[0]->Enable();

	f2HalfTexelOffset = Vec2f(0.5f / AutofocusBuffer[1]->GetRenderTarget()->GetWidth(), 0.5f / AutofocusBuffer[1]->GetRenderTarget()->GetHeight());
	texLumaCalcInput = AutofocusBuffer[1]->GetRenderTarget()->GetColorBuffer(0);
	texLumaTarget = GBuffer.GetRenderTarget()->GetDepthBuffer();
	bInitialLumaPass = false;
	bFinalLumaPass = false;
	bLumaAdaptationPass = true;

	const float bkp = fLumaAdaptSpeed;
	fLumaAdaptSpeed = DOF_AUTOFOCUS_TIME;
	fFrameTime = gmtl::Math::clamp(((GITechDemo*)AppMain)->GetDeltaTime(), 0.f, 1.f / fLumaAdaptSpeed);

	// Reuse the luminance animation shader
	LumaCalcShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	LumaCalcShader.Disable();

	fLumaAdaptSpeed = bkp;

	AutofocusBuffer[0]->Disable();

	POP_PROFILE_MARKER();
}

// Apply DoF effect to a separate target with
// CoC value in the alpha channel for compositing
void DepthOfFieldPass::CalculateDoF()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Calculate");

	if (DOF_USE_QUARTER_RESOLUTION_BUFFER)
		DepthOfFieldQuarterBuffer.Enable();
	else
		DepthOfFieldFullBuffer.Enable();

	const bool colorBlendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

	f2HalfTexelOffset = Vec2f(
		0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
		0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight()
		);
	texSource = LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0);

	BokehDofShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	BokehDofShader.Disable();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(colorBlendEnabled);

	if (DOF_USE_QUARTER_RESOLUTION_BUFFER)
		DepthOfFieldQuarterBuffer.Disable();
	else
		DepthOfFieldFullBuffer.Disable();

	POP_PROFILE_MARKER();
}

// Apply DoF buffer to the light accumulation buffer by blending
// using the information in the DoF buffer's alpha channel (CoC)
void DepthOfFieldPass::ApplyDoF()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Apply");

	LightAccumulationBuffer.Enable();

	const bool blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	const Blend dstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	const Blend srcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();
	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();

	if (DOF_USE_QUARTER_RESOLUTION_BUFFER)
	{
		RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
		RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_INVSRCALPHA);
		RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_SRCALPHA);
	}
	else
		RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	f2HalfTexelOffset = Vec2f(
		0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
		0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight()
		);
	texSource = (
		DOF_USE_QUARTER_RESOLUTION_BUFFER ?
		DepthOfFieldQuarterBuffer.GetRenderTarget() :
		DepthOfFieldFullBuffer.GetRenderTarget()
		)->GetColorBuffer(0);

	ColorCopyShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	ColorCopyShader.Disable();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	LightAccumulationBuffer.Disable();

	POP_PROFILE_MARKER();
}

void DepthOfFieldPass::OnDraw()
{
	if (!DOF_ENABLED)
		return;
	
	AutofocusPass();
	CalculateDoF();
	ApplyDoF();
}
