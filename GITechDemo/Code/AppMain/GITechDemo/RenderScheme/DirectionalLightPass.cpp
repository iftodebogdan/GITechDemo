#include "stdafx.h"

#include <Renderer.h>
#include <RenderState.h>
#include <Texture.h>
#include <RenderTarget.h>
using namespace LibRendererDll;

#include "Poisson.h"

#include "DirectionalLightPass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

namespace GITechDemoApp
{
	bool DIRECTIONAL_LIGHT_ENABLED = true;

	extern const Vec<unsigned int, 2> SHADOW_MAP_SIZE;
	extern const unsigned int PCF_MAX_SAMPLE_COUNT;
}

DirectionalLightPass::DirectionalLightPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{
	f2PoissonDisk = new Vec2f[PCF_MAX_SAMPLE_COUNT];

	// Generate Poisson-disk sampling pattern
	std::vector<sPoint> poisson;
	float minDist = sqrt((float)PCF_MAX_SAMPLE_COUNT) / (float)PCF_MAX_SAMPLE_COUNT * 0.8f;
	float oneOverMinDist = 1.f / minDist;
	const float sqrt2 = sqrt(2.f);
	do
	{
		poisson =
			GeneratePoissonPoints(
				minDist,
				30,
				PCF_MAX_SAMPLE_COUNT
				);
	} while (poisson.size() != PCF_MAX_SAMPLE_COUNT);

	// Normalize the kernel
	for (unsigned int i = 0; i < PCF_MAX_SAMPLE_COUNT; i++)
	{
		f2PoissonDisk[i][0] = poisson[i].x * oneOverMinDist * sqrt2;
		f2PoissonDisk[i][1] = poisson[i].y * oneOverMinDist * sqrt2;
	}
}

DirectionalLightPass::~DirectionalLightPass()
{
	if (f2PoissonDisk)
		delete[] f2PoissonDisk;
}

void DirectionalLightPass::OnUpdate(const float fDeltaTime)
{
	f2HalfTexelOffset = Vec2f(0.5f / GBuffer.GetRenderTarget()->GetWidth(), 0.5f / GBuffer.GetRenderTarget()->GetHeight());
	texDiffuseBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(0);
	texNormalBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(1);
	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
	texMaterialBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(2);
	texShadowMap = ShadowMapDir.GetRenderTarget()->GetDepthBuffer();
	f2OneOverShadowMapSize = Vec2f(1.f / (float)SHADOW_MAP_SIZE[0], 1.f / (float)SHADOW_MAP_SIZE[1]);

	IrradianceTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
	IrradianceTexture.GetTexture()->SetSRGBEnabled(true);
	EnvironmentTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
	EnvironmentTexture.GetTexture()->SetSRGBEnabled(true);

	texIrradianceMap = IrradianceTexture.GetTextureIndex();
	texEnvMap = EnvironmentTexture.GetTextureIndex();
}

void DirectionalLightPass::OnDraw()
{
	if (!DIRECTIONAL_LIGHT_ENABLED)
		return;

	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	DeferredLightDirShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	DeferredLightDirShader.Disable();

	DrawChildren();
}