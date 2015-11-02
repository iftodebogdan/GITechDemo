#include "stdafx.h"

#include <Renderer.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderState.h>
#include <RenderTarget.h>
using namespace LibRendererDll;

#include "BloomPass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

namespace GITechDemoApp
{
	bool BLOOM_ENABLED = true;
	const unsigned int BLOOM_BLUR_KERNEL_COUNT = 5;
	const unsigned int BLOOM_BLUR_KERNEL[BLOOM_BLUR_KERNEL_COUNT] = { 0, 1, 2, 2, 3 };
}

BloomPass::BloomPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{}

BloomPass::~BloomPass()
{}

void BloomPass::Update(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	ResourceMgr->GetTexture(
		BloomBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
		)->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(
		BloomBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
		)->SetAddressingMode(SAM_CLAMP);

	nDownsampleFactor = 1;
	bApplyBrightnessFilter = true;
}

// Apply a brightness filter to the downsampled HDR scene color buffer
void BloomPass::BloomBrightnessFilter()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Brightness filter");

	BloomBuffer[0]->Enable();

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	const SamplerFilter samplerFilter = ResourceMgr->GetTexture(HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetColorBuffer(0))->GetFilter();
	ResourceMgr->GetTexture(HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);

	f2HalfTexelOffset = Vec2f(
		0.5f / BloomBuffer[0]->GetRenderTarget()->GetWidth(),
		0.5f / BloomBuffer[0]->GetRenderTarget()->GetHeight()
		);
	f2TexelSize = Vec2f(
		1.f / HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetWidth(),
		1.f / HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetHeight()
		);
	texSource = HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetColorBuffer(0);

	DownsampleShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	DownsampleShader.Disable();

	ResourceMgr->GetTexture(HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(samplerFilter);

	BloomBuffer[0]->Disable();

	POP_PROFILE_MARKER();
}

// Blur the bloom buffer
void BloomPass::BloomBlur()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Blur");

	for (unsigned int i = 0; i < BLOOM_BLUR_KERNEL_COUNT; i++)
	{
#if ENABLE_PROFILE_MARKERS
		char label[10];
		sprintf_s(label, "Kernel %d", BLOOM_BLUR_KERNEL[i]);
#endif
		PUSH_PROFILE_MARKER(label);

		BloomBuffer[(i + 1) % 2]->Enable();

		// Not necesarry
		//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

		f2HalfTexelOffset = Vec2f(
			0.5f / BloomBuffer[i % 2]->GetRenderTarget()->GetWidth(),
			0.5f / BloomBuffer[i % 2]->GetRenderTarget()->GetHeight()
			);
		ResourceMgr->GetTexture(
			BloomBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0)
			)->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
		texSource = BloomBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0);
		f2TexelSize = Vec2f(
			1.f / BloomBuffer[i % 2]->GetRenderTarget()->GetWidth(),
			1.f / BloomBuffer[i % 2]->GetRenderTarget()->GetHeight()
			);
		nKernel = BLOOM_BLUR_KERNEL[i];
		if (i == BLOOM_BLUR_KERNEL_COUNT - 1)
			bAdjustIntensity = true;
		else
			bAdjustIntensity = false;

		BloomShader.Enable();
		RenderContext->DrawVertexBuffer(FullScreenTri);
		BloomShader.Disable();

		BloomBuffer[(i + 1) % 2]->Disable();

		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

// Apply the bloom buffer to the light accumulation buffer
void BloomPass::BloomApply()
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
	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ONE);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	f2HalfTexelOffset = Vec2f(0.5f / BloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetWidth(), 0.5f / BloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetHeight());
	ResourceMgr->GetTexture(
		BloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	texSource = BloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetColorBuffer(0);

	ColorCopyShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	ColorCopyShader.Disable();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	LightAccumulationBuffer.Disable();

	POP_PROFILE_MARKER();
}

void BloomPass::Draw()
{
	if (!BLOOM_ENABLED)
		return;

	BloomBrightnessFilter();
	BloomBlur();
	BloomApply();
}
