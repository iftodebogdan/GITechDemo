#include "stdafx.h"

#include <Renderer.h>
#include <RenderState.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderTarget.h>
using namespace LibRendererDll;

#include "App.h"

#include "GBufferPass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

namespace GITechDemoApp
{
	/* G-Buffer generation */
	bool GBUFFER_Z_PREPASS = false;
}

GBufferPass::GBufferPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{}

GBufferPass::~GBufferPass()
{}

void GBufferPass::Update(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(1))->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetDepthBuffer())->SetAddressingMode(SAM_CLAMP);

	f44WorldViewMat = f44ViewMat * f44WorldMat;
	f44WorldViewProjMat = f44ProjMat * f44WorldViewMat;
}

void GBufferPass::Draw()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	GBuffer.Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	const bool zWriteEnabled = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	bool red, blue, green, alpha;
	RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);

	// A depth prepass is useful if we have expensive pixel shaders in our
	// G-Buffer generation process. It allows us to avoid shading pixels
	// that eventually get overwritten by other pixels that have smaller
	// depth values. We are not fill-rate bound, so the depth prepass is disabled.
	if (GBUFFER_Z_PREPASS)
	{
		PUSH_PROFILE_MARKER("Z prepass");

		RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);

		DepthPassShader.Enable();

		for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
		{
			PUSH_PROFILE_MARKER(SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
			RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
			POP_PROFILE_MARKER();
		}

		DepthPassShader.Disable();

		RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
		RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
		RenderContext->GetRenderStateManager()->SetZFunc(CMP_EQUAL);

		POP_PROFILE_MARKER();

		PUSH_PROFILE_MARKER("Capture");
	}

	// A visibility test would be useful if we were CPU bound (or vertex bound).
	// However, there isn't a reason to do such an optimization for now, since the
	// scene isn't very big and we are mostly pixel bound.
	for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
	{
		PUSH_PROFILE_MARKER(SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
		
		texDiffuse = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_DIFFUSE, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
		texNormal = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_HEIGHT, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
		bHasNormalMap = (texNormal != -1);

		// For Blinn-Phong BRDF
		texSpec = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_SPECULAR, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
		bHasSpecMap = (texSpec != -1);
		fSpecIntensity = SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->fShininessStrength;

		// For Cook-Torrance BRDF
		texMatType = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_AMBIENT, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
		texRoughness = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_SHININESS, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);

		GBufferGenerationShader.Enable();
		RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
		GBufferGenerationShader.Disable();

		POP_PROFILE_MARKER();
	}

	if (GBUFFER_Z_PREPASS)
	{
		RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWriteEnabled);
		RenderContext->GetRenderStateManager()->SetZFunc(zFunc);
		POP_PROFILE_MARKER();
	}

	GBuffer.Disable();
}
