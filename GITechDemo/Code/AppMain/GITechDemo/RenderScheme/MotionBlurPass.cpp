#include "stdafx.h"

#include <Renderer.h>
#include <RenderTarget.h>
#include <RenderState.h>
#include <ResourceManager.h>
#include <Texture.h>
using namespace LibRendererDll;

#include "MotionBlurPass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

namespace GITechDemoApp
{
	bool MOTION_BLUR_ENABLED = true;
}

MotionBlurPass::MotionBlurPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{}

MotionBlurPass::~MotionBlurPass()
{}

void MotionBlurPass::Update(const float fDeltaTime)
{
	bSingleChannelCopy = false;
}

void MotionBlurPass::Draw()
{
	if(!MOTION_BLUR_ENABLED)
		return;

	CalculateMotionBlur();
	ApplyMotionBlur();
}

void MotionBlurPass::CalculateMotionBlur()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Calculate");

	MotionBlurBuffer.Enable();

	const bool colorBlendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

	f2HalfTexelOffset = Vec2f(
		0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
		0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight()
		);
	ResourceMgr->GetTexture(LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer())->SetAddressingMode(SAM_MIRROR);
	texSource = LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer();
	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();

	MotionBlurShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	MotionBlurShader.Disable();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(colorBlendEnabled);

	MotionBlurBuffer.Disable();

	POP_PROFILE_MARKER();
}

void MotionBlurPass::ApplyMotionBlur()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Apply");

	LightAccumulationBuffer.Enable();

	const bool blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	f2HalfTexelOffset = Vec2f(
		0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(),
		0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight()
		);
	texSource = MotionBlurBuffer.GetRenderTarget()->GetColorBuffer();

	ColorCopyShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	ColorCopyShader.Disable();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	LightAccumulationBuffer.Disable();

	POP_PROFILE_MARKER();
}
