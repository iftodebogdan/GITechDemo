#include "stdafx.h"

#include "Renderer.h"
#include "RenderState.h"
#include "ResourceManager.h"
#include "Texture.h"
using namespace LibRendererDll;

#include "FXAAPass.h"
using namespace GITechDemoApp;

#include "RenderResources.h"

namespace GITechDemoApp
{
	bool FXAA_ENABLED = true;
	extern bool HDR_TONEMAPPING_ENABLED;
}

FXAAPass::FXAAPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{}

FXAAPass::~FXAAPass()
{}

void FXAAPass::OnUpdate(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	ResourceMgr->GetTexture(LDRFxaaImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(LDRFxaaImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetSRGBEnabled(true);

	const LibRendererDll::RenderTarget* const srcRT = HDR_TONEMAPPING_ENABLED ? LDRToneMappedImageBuffer.GetRenderTarget() : LightAccumulationBuffer.GetRenderTarget();
	f2HalfTexelOffset = Vec2f(0.5f / srcRT->GetWidth(), 0.5f / srcRT->GetHeight());
	f2HalfTexelOffset = Vec2f(0.5f / srcRT->GetWidth(), 0.5f / srcRT->GetHeight());
	texSource = srcRT->GetColorBuffer(0);
	f2TexelSize = Vec2f(1.f / srcRT->GetWidth(), 1.f / srcRT->GetHeight());

}

void FXAAPass::OnDraw()
{
	if (!FXAA_ENABLED)
		return;

	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	LDRFxaaImageBuffer.Enable();

	const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	FxaaShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	FxaaShader.Disable();

	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);

	LDRFxaaImageBuffer.Disable();
}
