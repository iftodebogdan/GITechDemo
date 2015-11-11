#include "stdafx.h"

#include <Renderer.h>
#include <ResourceManager.h>
#include <RenderState.h>
#include <Texture.h>
#include <RenderTarget.h>
using namespace LibRendererDll;

#include "SSAOPass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

namespace GITechDemoApp
{
	bool SSAO_ENABLED = true;
	bool SSAO_USE_QUARTER_RESOLUTION_BUFFER = true;

	const unsigned int SSAO_BLUR_KERNEL_COUNT = 3;
	const unsigned int SSAO_BLUR_KERNEL[SSAO_BLUR_KERNEL_COUNT] = { 0, 1, 2 };
}

SSAOPass::SSAOPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
	, SSAOBuffer(SSAOFullBuffer)
	, BlurKernelCount(SSAO_BLUR_KERNEL_COUNT)
{}

SSAOPass::~SSAOPass()
{}

void SSAOPass::Update(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	texNormalBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(1);
	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();

	if (SSAO_USE_QUARTER_RESOLUTION_BUFFER)
	{
		SSAOBuffer = SSAOQuarterBuffer;
		BlurKernelCount = SSAO_BLUR_KERNEL_COUNT - 1;
	}
	else
	{
		SSAOBuffer = SSAOFullBuffer;
		BlurKernelCount = SSAO_BLUR_KERNEL_COUNT;
	}

	bSingleChannelCopy = true;
}

// Generate ambient occlusion buffer
void SSAOPass::CalculateSSAO()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Calculate");

	SSAOBuffer[0]->Enable();

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	f2HalfTexelOffset = Vec2f(
		0.5f / GBuffer.GetRenderTarget()->GetWidth(),
		0.5f / GBuffer.GetRenderTarget()->GetHeight()
		);
	bBlurPass = false;

	SsaoShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	SsaoShader.Disable();

	SSAOBuffer[0]->Disable();

	POP_PROFILE_MARKER();
}

// Blur ambient occlusion buffer
void SSAOPass::BlurSSAO()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Blur");

	for (unsigned int i = 0; i < BlurKernelCount; i++)
	{
#if ENABLE_PROFILE_MARKERS
		char label[10];
		sprintf_s(label, "Kernel %d", SSAO_BLUR_KERNEL[i]);
#endif
		PUSH_PROFILE_MARKER(label);

		SSAOBuffer[(i + 1) % 2]->Enable();

		// Not necesarry
		//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

		f2HalfTexelOffset = Vec2f(
			0.5f / SSAOBuffer[i % 2]->GetRenderTarget()->GetWidth(),
			0.5f / SSAOBuffer[i % 2]->GetRenderTarget()->GetHeight()
			);
		ResourceMgr->GetTexture(
			SSAOBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0)
			)->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
		texSource = SSAOBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0);
		f2TexelSize = Vec2f(
			1.f / SSAOBuffer[i % 2]->GetRenderTarget()->GetWidth(),
			1.f / SSAOBuffer[i % 2]->GetRenderTarget()->GetHeight()
			);
		nKernel = SSAO_BLUR_KERNEL[i];
		bAdjustIntensity = false;

		// Reuse the bloom shader for blurring the ambient occlusion render target
		BloomShader.Enable();
		RenderContext->DrawVertexBuffer(FullScreenTri);
		BloomShader.Disable();

		SSAOBuffer[(i + 1) % 2]->Disable();

		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

// Apply ambient occlusion to the light accumulation buffer
void SSAOPass::ApplySSAO()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Apply");

	LightAccumulationBuffer.Enable();

	const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	const Blend dstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	const Blend srcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();
	const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_INVSRCCOLOR);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ZERO);
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_GREATER);

	f2HalfTexelOffset = Vec2f(
		0.5f / SSAOBuffer[BlurKernelCount % 2]->GetRenderTarget()->GetWidth(),
		0.5f / SSAOBuffer[BlurKernelCount % 2]->GetRenderTarget()->GetHeight()
		);
	ResourceMgr->GetTexture(
		SSAOBuffer[BlurKernelCount % 2]->GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	texSource = SSAOBuffer[BlurKernelCount % 2]->GetRenderTarget()->GetColorBuffer(0);

	ColorCopyShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	ColorCopyShader.Disable();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	LightAccumulationBuffer.Disable();

	POP_PROFILE_MARKER();
}

void SSAOPass::Draw()
{
	if (!SSAO_ENABLED)
		return;

	CalculateSSAO();
	BlurSSAO();
	ApplySSAO();
}
