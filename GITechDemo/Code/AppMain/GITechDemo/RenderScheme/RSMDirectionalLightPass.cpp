#include "stdafx.h"

#include <Renderer.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderTarget.h>
using namespace LibRendererDll;

#include "RSMDirectionalLightPass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

namespace GITechDemoApp
{
	bool DEBUG_RSM_CAMERA = false;

	extern bool INDIRECT_LIGHT_ENABLED;
	extern AABoxf SceneLightSpaceAABB;
}

RSMDirectionalLightPass::RSMDirectionalLightPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{}

RSMDirectionalLightPass::~RSMDirectionalLightPass()
{}

void RSMDirectionalLightPass::Update(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_BORDER);
	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(0))->SetBorderColor(Vec4f(0.f, 0.f, 0.f, 0.f));
	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(1))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetDepthBuffer())->SetAddressingMode(SAM_BORDER);
	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetDepthBuffer())->SetBorderColor(Vec4f(0.f, 0.f, 0.f, 0.f));

	// RSM matrices
	RenderContext->CreateOrthographicMatrix(f44RSMProjMat,
		SceneLightSpaceAABB.mMin[0],
		SceneLightSpaceAABB.mMax[1],
		SceneLightSpaceAABB.mMax[0],
		SceneLightSpaceAABB.mMin[1],
		SceneLightSpaceAABB.mMin[2], SceneLightSpaceAABB.mMax[2]);
	f44RSMWorldViewProjMat = f44RSMProjMat * f44LightWorldViewMat;
	invertFull((Matrix44f&)f44RSMInvProjMat, (Matrix44f&)f44RSMProjMat);
	f44ViewToRSMViewMat = f44LightViewMat * f44InvViewMat;

	// Debug RSM camera
	if (DEBUG_RSM_CAMERA)
	{
		f44ViewMat = f44LightViewMat;
		invertFull((Matrix44f&)f44InvViewMat, (Matrix44f&)f44ViewMat);
		f44ProjMat = f44RSMProjMat;
		invertFull((Matrix44f&)f44InvProjMat, (Matrix44f&)f44ProjMat);
		f44WorldViewMat = f44ViewMat * f44WorldMat;
		f44ViewProjMat = f44ProjMat * f44ViewMat;
		f44InvViewProjMat = f44InvViewMat * f44InvProjMat;
		f44WorldViewProjMat = f44ProjMat * f44WorldViewMat;
		f44ScreenToLightViewMat = f44LightViewMat * f44InvViewProjMat;
	}
}

void RSMDirectionalLightPass::Draw()
{
	if (!INDIRECT_LIGHT_ENABLED)
		return;

	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	RSMBuffer.Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
	{
		texDiffuse = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_DIFFUSE, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);

		RSMCaptureShader.Enable();
		RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
		RSMCaptureShader.Disable();
	}

	RSMBuffer.Disable();
}
