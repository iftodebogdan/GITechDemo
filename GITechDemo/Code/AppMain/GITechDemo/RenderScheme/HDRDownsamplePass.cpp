#include "stdafx.h"

#include <Renderer.h>
#include <RenderState.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderTarget.h>
using namespace LibRendererDll;

#include "HDRDownsamplePass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

HDRDownsamplePass::HDRDownsamplePass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{}

HDRDownsamplePass::~HDRDownsamplePass()
{}

void HDRDownsamplePass::Update(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	ResourceMgr->GetTexture(HDRDownsampleBuffer[QUARTER]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(HDRDownsampleBuffer[SIXTEENTH]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	nDownsampleFactor = 4;
	bApplyBrightnessFilter = false;

}

void HDRDownsamplePass::DownsamplePass(GITechDemoApp::RenderTarget* const pSource, GITechDemoApp::RenderTarget* const pDest)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	pDest->Enable();

	// Not necessary
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	f2HalfTexelOffset = Vec2f(
		0.5f / pDest->GetRenderTarget()->GetWidth(),
		0.5f / pDest->GetRenderTarget()->GetHeight()
		);
	f2TexelSize = Vec2f(
		1.f / pSource->GetRenderTarget()->GetWidth(),
		1.f / pSource->GetRenderTarget()->GetHeight()
		);
	texSource = pSource->GetRenderTarget()->GetColorBuffer(0);

	DownsampleShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	DownsampleShader.Disable();

	pDest->Disable();
}

void HDRDownsamplePass::Draw()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	DownsamplePass(&LightAccumulationBuffer, HDRDownsampleBuffer[QUARTER]);
	DownsamplePass(HDRDownsampleBuffer[QUARTER], HDRDownsampleBuffer[SIXTEENTH]);
}
