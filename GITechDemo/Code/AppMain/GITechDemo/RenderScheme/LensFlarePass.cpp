#include "stdafx.h"

#include <Renderer.h>
#include <ResourceManager.h>
#include <RenderTarget.h>
#include <Texture.h>
#include <RenderState.h>
using namespace LibRendererDll;

#include "LensFlarePass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

namespace GITechDemoApp
{
	bool LENS_FLARE_ENABLED = true;
	float LENS_FLARE_BRIGHTNESS_THRESHOLD = 4.f;

	const unsigned int LENS_FLARE_BLUR_KERNEL_COUNT = 3;
	const unsigned int LENS_FLARE_BLUR_KERNEL[LENS_FLARE_BLUR_KERNEL_COUNT] = { 0, 1, 2 };
}

LensFlarePass::LensFlarePass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{}

LensFlarePass::~LensFlarePass()
{}

void LensFlarePass::Update(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	LensFlareGhostColorLUT.GetTexture()->SetAddressingMode(SAM_MIRROR);
	LensFlareGhostColorLUT.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	texGhostColorLUT = LensFlareGhostColorLUT.GetTextureIndex();

	LensFlareDirt.GetTexture()->SetAddressingMode(SAM_CLAMP);
	LensFlareDirt.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	texLensFlareDirt = LensFlareDirt.GetTextureIndex();

	LensFlareStarBurst.GetTexture()->SetAddressingMode(SAM_CLAMP);
	LensFlareStarBurst.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	texLensFlareStarBurst = LensFlareStarBurst.GetTextureIndex();

	nDownsampleFactor = 1;
	bApplyBrightnessFilter = true;
	bAdjustIntensity = false;

	// Calculate star burst matrix
	const Vec3f camX = Vec3f(
		f44ViewMat.GetCurrentValue()[0][0],
		f44ViewMat.GetCurrentValue()[0][1],
		f44ViewMat.GetCurrentValue()[0][2]);
	
	const Vec3f camZ = Vec3f(
		f44ViewMat.GetCurrentValue()[1][0],
		f44ViewMat.GetCurrentValue()[1][1],
		f44ViewMat.GetCurrentValue()[1][2]);
	
	const float camRot = dot(camX, Vec3f(0.f, 0.f, 1.f)) + dot(camZ, Vec3f(0.f, 1.f, 0.f));

	Matrix33f scaleBias1;
	scaleBias1.set(
		2.f,	0.f,	-1.f,
		0.f,	2.f,	-1.f,
		0.f,	0.f,	1.f);

	Matrix33f rotMat;
	rotMat.set(
		cosf(camRot),	-sinf(camRot),	0.f,
		sinf(camRot),	cosf(camRot),	0.f,
			0.f,			0.f,		1.f);

	Matrix33f scaleBias2;
	scaleBias2.set(
		0.5f,	0.f,	0.5f,
		0.f,	0.5f,	0.5f,
		0.f,	0.f,	1.f);

	f33LensFlareStarBurstMat = scaleBias2 * rotMat * scaleBias1;
}

void LensFlarePass::Draw()
{
	if (!LENS_FLARE_ENABLED)
		return;

	ApplyBrightnessFilter();
	GenerateFeatures();
	Blur();
	UpscaleAndBlend();
}

void LensFlarePass::ApplyBrightnessFilter()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Brightness filter");

	LensFlareBuffer[0]->Enable();

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	f2HalfTexelOffset = Vec2f(
		0.5f / HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetWidth(),
		0.5f / HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetHeight()
		);
	f2TexelSize = Vec2f(
		1.f / HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetWidth(),
		1.f / HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetHeight()
		);
	texSource = HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetColorBuffer(0);

	const float bkp = fBrightnessThreshold;
	fBrightnessThreshold = LENS_FLARE_BRIGHTNESS_THRESHOLD;

	DownsampleShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	DownsampleShader.Disable();

	fBrightnessThreshold = bkp;

	LensFlareBuffer[0]->Disable();

	POP_PROFILE_MARKER();
}

void LensFlarePass::GenerateFeatures()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Feature generation");

	ResourceMgr->GetTexture(
		LensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
		)->SetAddressingMode(SAM_WRAP);

	ResourceMgr->GetTexture(
		LensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);

	LensFlareBuffer[1]->Enable();

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	f2HalfTexelOffset = Vec2f(
		0.5f / LensFlareBuffer[0]->GetRenderTarget()->GetWidth(),
		0.5f / LensFlareBuffer[0]->GetRenderTarget()->GetHeight()
		);
	f2TexelSize = Vec2f(
		1.f / LensFlareBuffer[0]->GetRenderTarget()->GetWidth(),
		1.f / LensFlareBuffer[0]->GetRenderTarget()->GetHeight()
		);
	texSource = LensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer();

	LensFlareFeaturesShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	LensFlareFeaturesShader.Disable();

	LensFlareBuffer[1]->Disable();

	POP_PROFILE_MARKER();
}

void LensFlarePass::Blur()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Blur");

	ResourceMgr->GetTexture(
		LensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
		)->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(
		LensFlareBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
		)->SetAddressingMode(SAM_CLAMP);

	ResourceMgr->GetTexture(
		LensFlareBuffer[0]->GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(
		LensFlareBuffer[1]->GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	for (unsigned int i = 0; i < LENS_FLARE_BLUR_KERNEL_COUNT; i++)
	{
#if ENABLE_PROFILE_MARKERS
		char label[10];
		sprintf_s(label, "Kernel %d", LENS_FLARE_BLUR_KERNEL[i]);
#endif
		PUSH_PROFILE_MARKER(label);

		LensFlareBuffer[i % 2]->Enable();

		// Not necesarry
		//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

		f2HalfTexelOffset = Vec2f(
			0.5f / LensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
			0.5f / LensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight()
			);
		ResourceMgr->GetTexture(
			LensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetColorBuffer(0)
			)->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
		texSource = LensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetColorBuffer(0);
		f2TexelSize = Vec2f(
			1.f / LensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetWidth(),
			1.f / LensFlareBuffer[(i + 1) % 2]->GetRenderTarget()->GetHeight()
			);
		nKernel = LENS_FLARE_BLUR_KERNEL[i];

		// Reuse bloom shader for Kawase blur
		BloomShader.Enable();
		RenderContext->DrawVertexBuffer(FullScreenTri);
		BloomShader.Disable();

		LensFlareBuffer[i % 2]->Disable();

		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

void LensFlarePass::UpscaleAndBlend()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Upscale and blend + full-resolution features");

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

	f2HalfTexelOffset = Vec2f(0.5f / LensFlareBuffer[(LENS_FLARE_BLUR_KERNEL_COUNT + 1) % 2]->GetRenderTarget()->GetWidth(), 0.5f / LensFlareBuffer[(LENS_FLARE_BLUR_KERNEL_COUNT + 1) % 2]->GetRenderTarget()->GetHeight());
	ResourceMgr->GetTexture(
		LensFlareBuffer[(LENS_FLARE_BLUR_KERNEL_COUNT + 1) % 2]->GetRenderTarget()->GetColorBuffer(0)
		)->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	texLensFlareFeatures = LensFlareBuffer[(LENS_FLARE_BLUR_KERNEL_COUNT + 1) % 2]->GetRenderTarget()->GetColorBuffer();

	LensFlareApplyShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	LensFlareApplyShader.Disable();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	LightAccumulationBuffer.Disable();

	POP_PROFILE_MARKER();
}
