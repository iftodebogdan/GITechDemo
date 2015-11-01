#include "stdafx.h"

#include <Renderer.h>
#include <RenderState.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderTarget.h>
using namespace LibRendererDll;

#include "HDRToneMappingPass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

namespace GITechDemoApp
{
	bool HDR_TONE_MAPPING_ENABLED = true;
}

HDRToneMappingPass::HDRToneMappingPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{}

HDRToneMappingPass::~HDRToneMappingPass()
{}

void HDRToneMappingPass::Update(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	ResourceMgr->GetTexture(AverageLuminanceBuffer[0]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(AverageLuminanceBuffer[1]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(AverageLuminanceBuffer[2]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(AverageLuminanceBuffer[3]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	ResourceMgr->GetTexture(AdaptedLuminance[0]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(AdaptedLuminance[1]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	ResourceMgr->GetTexture(LDRToneMappedImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(LDRToneMappedImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetSRGBEnabled(true);
	
	GITechDemoApp::RenderTarget* const rtBkp = AdaptedLuminance[0];
	AdaptedLuminance[0] = AdaptedLuminance[1];
	AdaptedLuminance[1] = rtBkp;

	fFrameTime = gmtl::Math::clamp(fDeltaTime, 0.f, 1.f / fLumaAdaptSpeed);

	texLumaTarget = AverageLuminanceBuffer[3]->GetRenderTarget()->GetColorBuffer(0);
}

// Measure average luminance level of scene
void HDRToneMappingPass::LuminanceMeasurementPass()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Luminance measurement");

	for (unsigned int i = 0; i < 4; i++)
	{
		switch (i)
		{
		case 0:
			PUSH_PROFILE_MARKER("64x64");
			break;
		case 1:
			PUSH_PROFILE_MARKER("16x16");
			break;
		case 2:
			PUSH_PROFILE_MARKER("4x4");
			break;
		case 3:
			PUSH_PROFILE_MARKER("1x1");
		}

		AverageLuminanceBuffer[i]->Enable();

		// Not necesarry
		//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

		f2HalfTexelOffset = Vec2f(
			0.5f / AverageLuminanceBuffer[i]->GetRenderTarget()->GetWidth(),
			0.5f / AverageLuminanceBuffer[i]->GetRenderTarget()->GetHeight()
			);

		if (i == 0)
		{
			f2TexelSize = Vec2f(1.f / HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetWidth(), 1.f / HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetHeight());
			texLumaCalcInput = HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetColorBuffer(0);
			bInitialLumaPass = true;
			bFinalLumaPass = false;
			bLumaAdaptationPass = false;
		}
		else
		{
			f2TexelSize = Vec2f(1.f / AverageLuminanceBuffer[i - 1]->GetRenderTarget()->GetWidth(), 1.f / AverageLuminanceBuffer[i - 1]->GetRenderTarget()->GetHeight());
			texLumaCalcInput = AverageLuminanceBuffer[i - 1]->GetRenderTarget()->GetColorBuffer(0);
			bInitialLumaPass = false;
			bLumaAdaptationPass = false;

			if (i == 3)
				bFinalLumaPass = true;
			else
				bFinalLumaPass = false;
		}

		LumaCalcShader.Enable();
		RenderContext->DrawVertexBuffer(FullScreenTri);
		LumaCalcShader.Disable();

		AverageLuminanceBuffer[i]->Disable();

		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

void HDRToneMappingPass::LuminanceAdaptationPass()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Luminance adaptation");

	AdaptedLuminance[0]->Enable();

	f2HalfTexelOffset = Vec2f(0.5f / AdaptedLuminance[1]->GetRenderTarget()->GetWidth(), 0.5f / AdaptedLuminance[1]->GetRenderTarget()->GetHeight());
	texLumaCalcInput = AdaptedLuminance[1]->GetRenderTarget()->GetColorBuffer(0);
	bInitialLumaPass = false;
	bFinalLumaPass = false;
	bLumaAdaptationPass = true;

	LumaCalcShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	LumaCalcShader.Disable();

	AdaptedLuminance[0]->Disable();

	POP_PROFILE_MARKER();
}

void HDRToneMappingPass::ToneMappingPass()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("Tone mapping");

	LDRToneMappedImageBuffer.Enable();

	const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	f2HalfTexelOffset = Vec2f(0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(), 0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight());
	texSource = LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0);
	texAvgLuma = AdaptedLuminance[0]->GetRenderTarget()->GetColorBuffer(0);

	HDRToneMappingShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	HDRToneMappingShader.Disable();

	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);

	LDRToneMappedImageBuffer.Disable();

	POP_PROFILE_MARKER();
}

void HDRToneMappingPass::Draw()
{
	if (!HDR_TONE_MAPPING_ENABLED)
		return;

	LuminanceMeasurementPass();
	LuminanceAdaptationPass();
	ToneMappingPass();
}
