#include "stdafx.h"

#include "GITechDemo.h"
using namespace GITechDemoApp;

CREATE_APP(GITechDemo)

//#include <thread>

#include <Renderer.h>
#include <ShaderTemplate.h>
#include <SamplerState.h>
#include <RenderTarget.h>
#include <ResourceManager.h>
#include <VertexBuffer.h>
#include <Texture.h>
#include <ShaderInput.h>
#include <VertexFormat.h>
#include <IndexBuffer.h>
#include <RenderState.h>
#include <../Utility/ColorUtility.h>
using namespace LibRendererDll;

#include "PerlinNoise.h"
#include "Poisson.h"
#include "GaussianFilter.h"

#include "RenderResourceDeclaration.h"

// Matrices
Matrix44f worldMat;
Matrix44f viewMat;
Matrix44f projMat;
Matrix44f invProjMat;
Matrix44f invViewMat;
Matrix44f invViewProjMat;
Matrix44f worldViewMat;
Matrix44f viewProjMat;
Matrix44f worldViewProjMat;
//////////////////////////////

// Configurable options
/* GBuffer generation */
bool GBUFFER_Z_PREPASS = false;
float Z_NEAR = 1.f;
float Z_FAR = 5000.f;

/* Lighting options */
bool AMBIENT_LIGHT_ENABLED = true;
bool DIRECTIONAL_LIGHT_ENABLED = true;
bool INDIRECT_LIGHT_ENABLED = true;
bool AMBIENT_OCCLUSION_ENABLED = true;

/* Directional light shadow map parameters */
const Vec<unsigned int, 2> SHADOW_MAP_SIZE = Vec<unsigned int, 2>(4096, 4096);
const unsigned int PCF_KERNEL_SIZE = 16;

/* Cascaded Shadow Map parameters */
const unsigned int NUM_CASCADES = 4;
float CASCADE_SPLIT_FACTOR = 0.7f;
float CASCADE_MAX_VIEW_DEPTH = 3000.f;

/* Reflective Shadow Map parameters */
const unsigned int RSM_SIZE = 512;
const unsigned int RSM_NUM_PASSES = 8;
const unsigned int RSM_SAMPLES_PER_PASS = 16;
const unsigned int RSM_NUM_SAMPLES = RSM_NUM_PASSES * RSM_SAMPLES_PER_PASS;
bool USE_QUARTER_RES_INDIRECT_LIGHT_ACCUMULATION_BUFFER = true;

/* Screen-Space Ambient Occlusion */
const unsigned int SSAO_BLUR_KERNEL_COUNT = 3;
unsigned int SSAO_BLUR_KERNEL[SSAO_BLUR_KERNEL_COUNT] = { 0, 1, 2 };

/* Postprocessing parameters */
bool POST_PROCESSING_ENABLED = true;
// Tonemapping
bool HDR_TONEMAPPING_ENABLED = true;
// Bloom
bool BLOOM_ENABLED = true;
const unsigned int BLOOM_BLUR_KERNEL_COUNT = 5;
unsigned int BLOOM_BLUR_KERNEL[BLOOM_BLUR_KERNEL_COUNT] = { 0, 1, 2, 2, 3 };
// FXAA
bool FXAA_ENABLED = true;
// DoF
bool DOF_ENABLED = true;
bool DOF_USE_QUARTER_RESOLUTION_BUFFER = false;

/* Debug options */
bool DEBUG_CSM_CAMERA = false;
bool DEBUG_RSM_CAMERA = false;
bool CAMERA_ANIMATION_ENABLED = false;
///////////////////////////////

// Cascaded Shadow Maps (CSM) and directional light related variables
#define MAX_NUM_CASCADES (9)
Vec2f cascadeBoundsMin[MAX_NUM_CASCADES];
Vec2f cascadeBoundsMax[MAX_NUM_CASCADES];
Matrix44f dirLightWorldViewProjMat[MAX_NUM_CASCADES];
Matrix44f dirLightWorldViewMat;
Matrix44f dirLightViewMat;
Matrix44f dirInvLightViewMat;
Matrix44f dirScreenToLightViewMat;
Matrix44f dirLightProjMat[MAX_NUM_CASCADES];
Matrix44f dirLightViewProjMat[MAX_NUM_CASCADES];
Vec3f directionalLightDir = Vec3f(0.f, -1.f, 0.f);
Matrix44f directionalLightRotMat;
Matrix44f skyViewProjMat;
/////////////////////////////////////////////

// Reflective Shadow Map (RSM) related variables
Matrix44f RSMProjMat;
Matrix44f RSMInvProjMat;
Matrix44f RSMWorldViewProjMat;
Matrix44f ViewToRSMViewMat;
Vec3f RSMKernel[RSM_NUM_SAMPLES];
/////////////////////////////////////////////

// The vertices corresponding to DX9's clip space cuboid
// used when partitioning the view frustum for CSM
const Vec4f cuboidVerts[8] =
{
	Vec4f(-1.f,	 1.f,	1.f,	1.f),
	Vec4f(1.f,	 1.f,	1.f,	1.f),
	Vec4f(-1.f,	-1.f,	1.f,	1.f),
	Vec4f(1.f,	-1.f,	1.f,	1.f),
	Vec4f(-1.f,	 1.f,	0.f,	1.f),
	Vec4f(1.f,	 1.f,	0.f,	1.f),
	Vec4f(-1.f,	-1.f,	0.f,	1.f),
	Vec4f(1.f,	-1.f,	0.f,	1.f)
};
////////////////////////////////////

Renderer*			RenderContext = nullptr;
ResourceManager*	ResourceMgr = nullptr;

// Used for fullscreen effects, postprocessing, etc.
// NB: in order to avoid tearing between the seams of a quad's triangles (the diagonal of the screen)
// we use a single over-sized triangle to fill the screen (2 x screen height, 2 x screen width)
VertexBuffer*	FullScreenQuad = nullptr;
// a cube used to draw the sky
VertexBuffer*	SkyBoxCube = nullptr;

// Axis-aligned bounding box of the scene, to be used when constructing
// the projection matrix for the directional light shadow map
AABoxf SponzaAABB;
///////////////////////////////////////

// Perlin Noise for animation of lights
Perlin PerlinNoise(1, USHRT_MAX, 1, GetTickCount());

// Poisson sampling for PCF
Vec2f PoissonDisk[PCF_KERNEL_SIZE];

// Gaussian kernel for blurring
float GaussianKernel[16];

GITechDemo::GITechDemo()
	: App()
{}

GITechDemo::~GITechDemo()
{
	Renderer::DestroyInstance();
}

void GITechDemo::Init(void* hWnd)
{
	pHWND = hWnd;

	// Renderer MUST be initialized on the SAME thread as the target window
	Renderer::CreateInstance(API_DX9);
	//Renderer::CreateInstance(API_NULL);
	RenderContext = Renderer::GetInstance();
	RenderContext->Initialize(hWnd);
	ResourceMgr = RenderContext->GetResourceManager();

	AllocateRenderResources();
}

void GITechDemo::Update(const float fDeltaTime)
{
	UpdateMatrices();
}

void GITechDemo::Draw()
{
	RECT rc;
	GetClientRect((HWND)pHWND, &rc);
	Vec2i vp = Vec2i(rc.right - rc.left, rc.bottom - rc.top);
	RenderContext->SetBackBufferSize(vp);

	RenderScene();
}

// Allocate/load resources required for rendering the scene
void GITechDemo::AllocateRenderResources()
{
	PUSH_PROFILE_MARKER("AllocateRenderResources()");

	// First of all, load the scene (models + textures)
	RenderResource::InitAllModels(ResourceMgr);
	// Load the sky cube map texture
	RenderResource::InitAllTextures(ResourceMgr);
	SkyTexture.GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);

	// Calculate the scene's AABB
	// This will be used later when calculating the cascade bounds for the CSM
	SponzaAABB.mMin = Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
	SponzaAABB.mMax = Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
	{
		const VertexBuffer* const vb = SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer;
		for (unsigned int vert = 0; vert < vb->GetElementCount(); vert++)
		{
			Vec3f vertPos = vb->Position<Vec3f>(vert);

			if (vertPos[0] < SponzaAABB.mMin[0])
			{
				SponzaAABB.mMin[0] = vertPos[0];
			}
			if (vertPos[1] < SponzaAABB.mMin[1])
			{
				SponzaAABB.mMin[1] = vertPos[1];
			}
			if (vertPos[2] < SponzaAABB.mMin[2])
			{
				SponzaAABB.mMin[2] = vertPos[2];
			}

			if (vertPos[0] > SponzaAABB.mMax[0])
			{
				SponzaAABB.mMax[0] = vertPos[0];
			}
			if (vertPos[1] > SponzaAABB.mMax[1])
			{
				SponzaAABB.mMax[1] = vertPos[1];
			}
			if (vertPos[2] > SponzaAABB.mMax[2])
			{
				SponzaAABB.mMax[2] = vertPos[2];
			}
		}
	}
	
	// Load shaders
	RenderResource::InitAllShaders(ResourceMgr);

	// Create misc. utilities
	std::vector<sPoint> poisson;
	float minDist = sqrt((float)PCF_KERNEL_SIZE) / (float)PCF_KERNEL_SIZE * 0.8f;
	float oneOverMinDist = 1.f / minDist;
	const float sqrt2 = sqrt(2.f);
	do
	{
		poisson =
			GeneratePoissonPoints(
				minDist,
				30,
				PCF_KERNEL_SIZE
				);
	} while (poisson.size() != PCF_KERNEL_SIZE);

	for (unsigned int i = 0; i < PCF_KERNEL_SIZE; i++)
	{
		PoissonDisk[i][0] = poisson[i].x * oneOverMinDist * sqrt2;
		PoissonDisk[i][1] = poisson[i].y * oneOverMinDist * sqrt2;
	}

	minDist = sqrt((float)RSM_NUM_SAMPLES) / (float)RSM_NUM_SAMPLES * 0.7f;
	do
	{
		poisson =
			GeneratePoissonPoints(
				minDist,
				30,
				RSM_NUM_SAMPLES
				);
	} while (poisson.size() != RSM_NUM_SAMPLES);

	for (unsigned int i = 0; i < RSM_NUM_SAMPLES; i++)
	{
		// Increase sample density towards the outside of the kernel
		RSMKernel[i][0] = sqrt(abs(poisson[i].x - 0.5f) * 2.f) * ((poisson[i].x < 0.5f) ? -1.f : 1.f);
		RSMKernel[i][1] = sqrt(abs(poisson[i].y - 0.5f) * 2.f) * ((poisson[i].y < 0.5f) ? -1.f : 1.f);
		// Linear weights combined with non-linear sample density has proven
		// to provide very good quality with very little jitter / noise
		RSMKernel[i][2] = length(Vec2f(poisson[i].x - 0.5f, poisson[i].y - 0.5f)) * 2.f;
	}

	CreateGaussianFilter(GaussianKernel, 16);

	// Create a full screen quad (it's actually an over-sized triangle) for fullscreen effects and processing
	/*
	You can view the triangle as half of the quad required to fill a screen that's twice as
	high and twice as wide as our actual screen. The diagram below should help you visualize.
	Note that, as opposed to the traditional fullscreen quad composed of two triangles, this
	method avoids any artifact on the screen's diagonal, where the two triangles would line up.
	Also, it avoids having to shade twice along the screen's diagonal.

	|-------------------------------------------------------------------------------------------------
	|||============================================||                                          /====/
	|||											   ||                                    /====/
	|||											   ||                              /====/
	|||											   ||                        /====/
	|||											   ||                  /====/
	|||											   ||            /====/
	|||											   ||      /====/
	|||============================================||/====/
	|                                         /====/
	|                                   /====/
	|                             /====/
	|                       /====/
	|                 /====/
	|           /====/
	|     /====/
	|====/
	*/

	unsigned int vfIdx = ResourceMgr->CreateVertexFormat(1, VAU_POSITION, VAT_FLOAT4, 0);
	VertexFormat* vf = ResourceMgr->GetVertexFormat(vfIdx);

	unsigned int ibIdx = ResourceMgr->CreateIndexBuffer(3);
	IndexBuffer* ib = ResourceMgr->GetIndexBuffer(ibIdx);
	const unsigned int fsqIndices[] = { 0, 1, 2 };
	ib->SetIndices(fsqIndices, 3);

	unsigned int vbIdx = ResourceMgr->CreateVertexBuffer(vf, 3, ib);
	FullScreenQuad = ResourceMgr->GetVertexBuffer(vbIdx);

	FullScreenQuad->Lock(BL_WRITE_ONLY);
	FullScreenQuad->Position<Vec4f>(0) = Vec4f(-1.f, 1.f, 1.f, 1.f);
	FullScreenQuad->Position<Vec4f>(1) = Vec4f(3.f, 1.f, 1.f, 1.f);
	FullScreenQuad->Position<Vec4f>(2) = Vec4f(-1.f, -3.f, 1.f, 1.f);
	FullScreenQuad->Update();
	FullScreenQuad->Unlock();

	// Create a cube VB for sky
	// The cube is aligned with world axes so that the camera can rotate freely,
	// giving the impressiong that the sky is stationary. Also, the center of the
	// cube is positioned at the camera's location so as not to break the illusion
	// that the sky "hemisphere" is actually a giant cube.
	// NB: in the pixel shader responsible for drawing the sky, the visible face(s)
	// are pushed back to the maximum depth (far plane) so as not to be drawn
	// over objects that are not inside the cube (bigger depth).
	vfIdx = ResourceMgr->CreateVertexFormat(1, VAU_POSITION, VAT_FLOAT4, 0);
	vf = ResourceMgr->GetVertexFormat(vfIdx);

	ibIdx = ResourceMgr->CreateIndexBuffer(36);
	ib = ResourceMgr->GetIndexBuffer(ibIdx);
	const unsigned int sbIndices[] = {
		0, 1, 2, 2, 1, 3,	// front
		5, 4, 7, 7, 4, 6,	// back
		4, 0, 6, 6, 0, 2,	// left
		1, 5, 3, 3, 5, 7,	// right
		4, 5, 0, 0, 5, 1,	// top
		2, 3, 6, 6, 3, 7	// bottom
	};
	ib->SetIndices(sbIndices, 36);

	vbIdx = ResourceMgr->CreateVertexBuffer(vf, 8, ib);
	SkyBoxCube = ResourceMgr->GetVertexBuffer(vbIdx);

	SkyBoxCube->Lock(BL_WRITE_ONLY);
	SkyBoxCube->Position<Vec4f>(0) = Vec4f(-1.f, 1.f, 1.f, 1.f);
	SkyBoxCube->Position<Vec4f>(1) = Vec4f(1.f, 1.f, 1.f, 1.f);
	SkyBoxCube->Position<Vec4f>(2) = Vec4f(-1.f, -1.f, 1.f, 1.f);
	SkyBoxCube->Position<Vec4f>(3) = Vec4f(1.f, -1.f, 1.f, 1.f);
	SkyBoxCube->Position<Vec4f>(4) = Vec4f(-1.f, 1.f, -1.f, 1.f);
	SkyBoxCube->Position<Vec4f>(5) = Vec4f(1.f, 1.f, -1.f, 1.f);
	SkyBoxCube->Position<Vec4f>(6) = Vec4f(-1.f, -1.f, -1.f, 1.f);
	SkyBoxCube->Position<Vec4f>(7) = Vec4f(1.f, -1.f, -1.f, 1.f);
	SkyBoxCube->Update();
	SkyBoxCube->Unlock();

	tCamera.vPos = Vec3f(-840.f, -600.f, -195.f);
	tCamera.mRot.set(
		-0.440301329f, 0.00776965916f, 0.897806108f, 0.f,
		-0.142924204f, 0.986597657f, -0.0786283761f, 0.f,
		-0.886387110f, -0.162937075f, -0.433295786f, 0.f,
		0.f, 0.f, 0.f, 1.f
		);

	// Initialize the various render targets we will be using
	RenderResource::InitAllRenderTargets(ResourceMgr);

	ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetColorBuffer(1))->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(GBuffer.GetRenderTarget()->GetDepthBuffer())->SetAddressingMode(SAM_CLAMP);

	ResourceMgr->GetTexture(ShadowMapDir.GetRenderTarget()->GetDepthBuffer())->SetAddressingMode(SAM_BORDER);
	ResourceMgr->GetTexture(ShadowMapDir.GetRenderTarget()->GetDepthBuffer())->SetBorderColor(Vec4f(1.f, 1.f, 1.f, 1.f));

	ResourceMgr->GetTexture(LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);

	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_BORDER);
	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(0))->SetBorderColor(Vec4f(0.f, 0.f, 0.f, 0.f));
	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetColorBuffer(1))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetDepthBuffer())->SetAddressingMode(SAM_BORDER);
	ResourceMgr->GetTexture(RSMBuffer.GetRenderTarget()->GetDepthBuffer())->SetBorderColor(Vec4f(0.f, 0.f, 0.f, 0.f));

	ResourceMgr->GetTexture(IndirectLightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);

	ResourceMgr->GetTexture(HDRDownsampleBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	ResourceMgr->GetTexture(AverageLuminanceBuffer[0]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(AverageLuminanceBuffer[1]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(AverageLuminanceBuffer[2]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(AverageLuminanceBuffer[3]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	ResourceMgr->GetTexture(AdaptedLuminance[0]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(AdaptedLuminance[1]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	ResourceMgr->GetTexture(HDRBloomBuffer[0]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(HDRBloomBuffer[0]->GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(HDRBloomBuffer[1]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(HDRBloomBuffer[1]->GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);

	ResourceMgr->GetTexture(LDRToneMappedImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(LDRToneMappedImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetSRGBEnabled(true);

	ResourceMgr->GetTexture(LDRFxaaImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(LDRFxaaImageBuffer.GetRenderTarget()->GetColorBuffer(0))->SetSRGBEnabled(true);

	ResourceMgr->GetTexture(AmbientOcclusionBuffer[0]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(AmbientOcclusionBuffer[1]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	ResourceMgr->GetTexture(DepthOfFieldFullBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(DepthOfFieldFullBuffer.GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(DepthOfFieldQuarterBuffer.GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(DepthOfFieldQuarterBuffer.GetRenderTarget()->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);

	POP_PROFILE_MARKER();
}

void GITechDemo::UpdateMatrices()
{
	PUSH_PROFILE_MARKER("UpdateMatrices()");

	// Calculate world matrix
	worldMat = makeTrans(Vec3f(0, 0, 0), Type2Type<Matrix44f>());

	// Calculate view matrix
	normalize(tCamera.vMoveVec);
	tCamera.vMoveVec *= 5.f;
	tCamera.vPos -=
		Vec3f(tCamera.mRot[2][0] * tCamera.vMoveVec[2] * tCamera.fSpeedFactor, tCamera.mRot[2][1] * tCamera.vMoveVec[2] * tCamera.fSpeedFactor, tCamera.mRot[2][2] * tCamera.vMoveVec[2] * tCamera.fSpeedFactor) +
		Vec3f(tCamera.mRot[0][0] * tCamera.vMoveVec[0] * tCamera.fSpeedFactor, tCamera.mRot[0][1] * tCamera.vMoveVec[0] * tCamera.fSpeedFactor, tCamera.mRot[0][2] * tCamera.vMoveVec[0] * tCamera.fSpeedFactor);

	// Animate camera
	if (CAMERA_ANIMATION_ENABLED)
	{
		static float time = 0.f;
		static float periodPos = 5000000.f;
		time += fFrameTime.GetCurrentValue();
		Vec3f perlinPos(
			PerlinNoise.Get(time / periodPos, 0.f),
			PerlinNoise.Get(0.f, time / periodPos),
			PerlinNoise.Get(time / periodPos, time / periodPos));
		perlinPos[0] = perlinPos[0] + 1.f / 2.f;
		perlinPos[1] = perlinPos[1] + 1.f / 2.f;
		perlinPos[2] = perlinPos[2] + 1.f / 2.f;
		perlinPos[0] = perlinPos[0] - floor(perlinPos[0]);
		perlinPos[1] = perlinPos[1] - floor(perlinPos[1]);
		perlinPos[2] = perlinPos[2] - floor(perlinPos[2]);

		tCamera.vPos = -Vec3f(
			((SponzaAABB.getMax()[0] - SponzaAABB.getMin()[0]) * perlinPos[0] + SponzaAABB.getMin()[0]) / 1.5f,
			((SponzaAABB.getMax()[1] - SponzaAABB.getMin()[1]) * perlinPos[1] + SponzaAABB.getMin()[1]) / 1.5f,
			((SponzaAABB.getMax()[2] - SponzaAABB.getMin()[2]) * perlinPos[2] + SponzaAABB.getMin()[2]) / 1.5f
			);

		Vec3f sponzaCenter(
			(SponzaAABB.getMax()[0] - SponzaAABB.getMin()[0]) / 2.f + SponzaAABB.getMin()[0],
			((SponzaAABB.getMax()[1] - SponzaAABB.getMin()[1]) / 2.f + SponzaAABB.getMin()[1]) / 2.f,
			(SponzaAABB.getMax()[2] - SponzaAABB.getMin()[2]) / 2.f + SponzaAABB.getMin()[2]);
		Vec3f zAxis = makeNormal(Vec3f(sponzaCenter + tCamera.vPos));
		//static Vec3f upVec = abs(zAxis[1]) == 1.f ? Vec3f(0.f, 0.f, 1.f) : Vec3f(0.f, 1.f, 0.f);
		Vec3f upVec = Vec3f(0.f, 1.f, 0.f);
		Vec3f xAxis = makeNormal(makeCross(upVec, zAxis));
		Vec3f yAxis = makeCross(zAxis, xAxis);
		upVec = yAxis;
		tCamera.mRot.set(
			xAxis[0], xAxis[1], xAxis[2], 0.f,
			yAxis[0], yAxis[1], yAxis[2], 0.f,
			zAxis[0], zAxis[1], zAxis[2], 0.f,
			0.f, 0.f, 0.f, 1.f
			);
	}

	viewMat = tCamera.mRot * makeTrans(tCamera.vPos, Type2Type<Matrix44f>());
	invertFull(invViewMat, viewMat);

	// Calculate projection matrix
	//RECT rc;
	//GetClientRect(hWnd, &rc);
	//Vec2i vp = Vec2i(rc.right - rc.left, rc.bottom - rc.top);
	Vec2i vp = RenderContext->GetBackBufferSize();
	RenderContext->CreatePerspectiveMatrix(projMat, Math::deg2Rad(60.f), (float)vp[0] / (float)vp[1], Z_NEAR, Z_FAR);
	invertFull(invProjMat, projMat);

	// Calculate composite matrices
	worldViewMat = viewMat * worldMat;
	viewProjMat = projMat * viewMat;
	invViewProjMat = invViewMat * invProjMat;
	worldViewProjMat = projMat * worldViewMat;

	// Animate directional light using Perlin Noise
	float noiseX = PerlinNoise.Get((float)GetTickCount64() / (float)INT_MAX, 0);
	float noiseZ = PerlinNoise.Get(0, (float)GetTickCount64() / (float)INT_MAX);
	directionalLightDir[0] = noiseX;//0.0f;//
	directionalLightDir[2] = noiseZ;//0.3f;//
	normalize(directionalLightDir);

	// Calculate directional light camera view matrix
	Vec3f zAxis = makeNormal(directionalLightDir);
	// Use the previous frame's up vector to avoid
	// the camera making sudden jumps when rolling over
	static Vec3f upVec = abs(zAxis[1]) == 1.f ? Vec3f(0.f, 0.f, 1.f) : Vec3f(0.f, 1.f, 0.f);
	Vec3f xAxis = makeNormal(makeCross(upVec, zAxis));
	Vec3f yAxis = makeCross(zAxis, xAxis);
	upVec = yAxis;
	dirLightViewMat.set(
		xAxis[0], xAxis[1], xAxis[2], 0.f,
		yAxis[0], yAxis[1], yAxis[2], 0.f,
		zAxis[0], zAxis[1], zAxis[2], 0.f,
		0.f, 0.f, 0.f, 1.f
		);
	dirLightWorldViewMat = dirLightViewMat * worldMat;
	invertFull(dirInvLightViewMat, dirLightViewMat);
	dirScreenToLightViewMat = dirLightViewMat * invViewProjMat;

	skyViewProjMat = viewProjMat * makeTrans(-tCamera.vPos, Type2Type<Matrix44f>());

	// Calculate the projection matrices for all shadow map cascades
	// Start with converting the scene AABB to a light view space OBB
	// and then calculating its light view space AABB
	Vec3f aabbVerts[8];
	aabbVerts[0] = SponzaAABB.getMin();
	aabbVerts[1] = Vec3f(SponzaAABB.getMin()[0], SponzaAABB.getMin()[1], SponzaAABB.getMax()[2]);
	aabbVerts[2] = Vec3f(SponzaAABB.getMax()[0], SponzaAABB.getMin()[1], SponzaAABB.getMax()[2]);
	aabbVerts[3] = Vec3f(SponzaAABB.getMax()[0], SponzaAABB.getMin()[1], SponzaAABB.getMin()[2]);
	aabbVerts[4] = Vec3f(SponzaAABB.getMin()[0], SponzaAABB.getMax()[1], SponzaAABB.getMin()[2]);
	aabbVerts[5] = Vec3f(SponzaAABB.getMin()[0], SponzaAABB.getMax()[1], SponzaAABB.getMax()[2]);
	aabbVerts[6] = Vec3f(SponzaAABB.getMax()[0], SponzaAABB.getMax()[1], SponzaAABB.getMin()[2]);
	aabbVerts[7] = SponzaAABB.getMax();
	// This is the scene AABB in light view space (which is actually the view frustum of the
	// directional light camera) aproximated from the scene's light view space OBB
	AABoxf SponzaLightSpaceAABB(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX), Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	for (unsigned int i = 0; i < 8; i++)
	{
		// For each AABB vertex, calculate the corresponding light view space OBB vertex
		aabbVerts[i] = dirLightWorldViewMat * aabbVerts[i];

		// Calculate the light view space AABB using the minimum and maximum values
		// on each axis of the light view space OBB
		if (aabbVerts[i][0] < SponzaLightSpaceAABB.getMin()[0])
			SponzaLightSpaceAABB.mMin[0] = aabbVerts[i][0];
		if (aabbVerts[i][1] < SponzaLightSpaceAABB.getMin()[1])
			SponzaLightSpaceAABB.mMin[1] = aabbVerts[i][1];
		if (aabbVerts[i][2] < SponzaLightSpaceAABB.getMin()[2])
			SponzaLightSpaceAABB.mMin[2] = aabbVerts[i][2];

		if (aabbVerts[i][0] > SponzaLightSpaceAABB.getMax()[0])
			SponzaLightSpaceAABB.mMax[0] = aabbVerts[i][0];
		if (aabbVerts[i][1] > SponzaLightSpaceAABB.getMax()[1])
			SponzaLightSpaceAABB.mMax[1] = aabbVerts[i][1];
		if (aabbVerts[i][2] > SponzaLightSpaceAABB.getMax()[2])
			SponzaLightSpaceAABB.mMax[2] = aabbVerts[i][2];
	}

	// Calculate each cascade properties
	for (unsigned int cascade = 0; cascade < NUM_CASCADES; cascade++)
	{
		// This is the part of the viewer's view frustum corresponding to the view frustum of the current cascade
		AABoxf ViewFrustumPartitionLightSpaceAABB(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX), Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));

		// Partition the viewer's view frustum (the can be viewed as Z slices into the view frustum)
		// This formula is derived from Nvidia's paper on Cascaded Shadow Maps:
		// http://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
		Vec4f partitionNear(0.f, 0.f, 0.f, 1.f);
		Vec4f partitionFar(0.f, 0.f, 0.f, 1.f);

		Math::lerp(partitionNear[2],
			CASCADE_SPLIT_FACTOR,
			Z_NEAR + ((float)cascade / NUM_CASCADES)*(CASCADE_MAX_VIEW_DEPTH - Z_NEAR),
			Z_NEAR * powf(CASCADE_MAX_VIEW_DEPTH / Z_NEAR, (float)cascade / NUM_CASCADES)
			);

		Math::lerp(partitionFar[2],
			CASCADE_SPLIT_FACTOR,
			Z_NEAR + (((float)cascade + 1.f) / NUM_CASCADES)*(CASCADE_MAX_VIEW_DEPTH - Z_NEAR),
			Z_NEAR * powf(CASCADE_MAX_VIEW_DEPTH / Z_NEAR, ((float)cascade + 1.f) / NUM_CASCADES)
			);

		// Calculate the partition's depth in projective space (viewer camera, i.e. perspective projection)
		partitionNear = projMat * partitionNear;
		partitionNear /= partitionNear[3]; // w-divide
		partitionFar = projMat * partitionFar;
		partitionFar /= partitionFar[3]; // w-divide

		for (unsigned int vert = 0; vert < 8; vert++)
		{
			// Calculate the vertices of each view frustum partition (in clip space)
			Vec4f partitionVert = cuboidVerts[vert];
			if (partitionVert[2] == 0.f)
				partitionVert[2] = partitionNear[2];
			else if (partitionVert[2] == 1.f)
				partitionVert[2] = partitionFar[2];

			// Calculate the current partition's vertices in world space coordinates,
			// then apply the directional light camera's view matrix in order to
			// obtain the light view space coordinates of the partitioned view frustum.
			// This is the light view space OBB of the current view frustum partition.
			Vec4f viewFrustumVertPreW = invViewProjMat * partitionVert;
			Vec4f viewFrustumVertPostW = viewFrustumVertPreW / viewFrustumVertPreW[3];
			Vec3f wsFrustumVert = Vec3f(viewFrustumVertPostW[0], viewFrustumVertPostW[1], viewFrustumVertPostW[2]);
			Vec3f lsFrustumVerts = dirLightViewMat * wsFrustumVert;

			// Calculate a light view space AABB from the light view space OBB of this view frustum partition.
			// In other words, this light view space AABB is the view frustum (light view space)
			// of the current cascade, encompassing the part of the world that the viewer
			// sees which is sufficiently far/near to the viewer's camera so as to
			// be considered valid for rendering into the current shadow map cascade.
			if (lsFrustumVerts[0] < ViewFrustumPartitionLightSpaceAABB.getMin()[0])
				ViewFrustumPartitionLightSpaceAABB.mMin[0] = lsFrustumVerts[0];
			if (lsFrustumVerts[0] > ViewFrustumPartitionLightSpaceAABB.getMax()[0])
				ViewFrustumPartitionLightSpaceAABB.mMax[0] = lsFrustumVerts[0];

			if (lsFrustumVerts[1] < ViewFrustumPartitionLightSpaceAABB.getMin()[1])
				ViewFrustumPartitionLightSpaceAABB.mMin[1] = lsFrustumVerts[1];
			if (lsFrustumVerts[1] > ViewFrustumPartitionLightSpaceAABB.getMax()[1])
				ViewFrustumPartitionLightSpaceAABB.mMax[1] = lsFrustumVerts[1];

			if (lsFrustumVerts[2] < ViewFrustumPartitionLightSpaceAABB.getMin()[2])
				ViewFrustumPartitionLightSpaceAABB.mMin[2] = lsFrustumVerts[2];
			if (lsFrustumVerts[2] > ViewFrustumPartitionLightSpaceAABB.getMax()[2])
				ViewFrustumPartitionLightSpaceAABB.mMax[2] = lsFrustumVerts[2];
		}

		// Enlarge the light view frustum in order to avoid GITechDemo::PCF shadow sampling from
		// sampling outside of a shadow map cascade
		const unsigned int cascadesPerRow = (unsigned int)Math::ceil(Math::sqrt((float)NUM_CASCADES));
		const unsigned int cascadeSize = SHADOW_MAP_SIZE[0] / cascadesPerRow;
		//float pcfScale = ((float)PCF_KERNEL_SIZE * 2.f ) / (float)cascadeSize;
		//Vec3f aabbDiag = ViewFrustumPartitionLightSpaceAABB.mMax - ViewFrustumPartitionLightSpaceAABB.mMin;
		float offsetForPCF = 4.f * (float)PCF_KERNEL_SIZE * sqrt(2.f); // TODO: recheck the math on this one

																	   // Snap the ortographic projection to texel-sized increments in order to prevent shadow edges from jittering.
																	   // However, because we're tightly fitting the cascade around the view frustum, jittering will still be
																	   // present when rotating the camera, but not when zooming or strafing.
		Vec2f worldUnitsPerTexel = Vec2f(
			ViewFrustumPartitionLightSpaceAABB.mMax[0] - ViewFrustumPartitionLightSpaceAABB.mMin[0] + 2.f * offsetForPCF,
			ViewFrustumPartitionLightSpaceAABB.mMax[1] - ViewFrustumPartitionLightSpaceAABB.mMin[1] + 2.f * offsetForPCF) /
			Math::floor((float)SHADOW_MAP_SIZE[0] / Math::ceil(Math::sqrt((float)NUM_CASCADES))/*cascades per row*/);

		// Calculate the projection matrix for the current shadow map cascade
		RenderContext->CreateOrthographicMatrix(
			dirLightProjMat[cascade],
			Math::floor((ViewFrustumPartitionLightSpaceAABB.mMin[0] - offsetForPCF) / worldUnitsPerTexel[0]) * worldUnitsPerTexel[0],
			Math::ceil((ViewFrustumPartitionLightSpaceAABB.mMax[1] + offsetForPCF) / worldUnitsPerTexel[1]) * worldUnitsPerTexel[1],
			Math::ceil((ViewFrustumPartitionLightSpaceAABB.mMax[0] + offsetForPCF) / worldUnitsPerTexel[0]) * worldUnitsPerTexel[0],
			Math::floor((ViewFrustumPartitionLightSpaceAABB.mMin[1] - offsetForPCF) / worldUnitsPerTexel[1]) * worldUnitsPerTexel[1],
			SponzaLightSpaceAABB.mMin[2], SponzaLightSpaceAABB.mMax[2]);

		// Store the light space coordinates of the bounds of the current shadow map cascade
		cascadeBoundsMin[cascade] = Vec2f(ViewFrustumPartitionLightSpaceAABB.mMin[0], ViewFrustumPartitionLightSpaceAABB.mMin[1]);
		cascadeBoundsMax[cascade] = Vec2f(ViewFrustumPartitionLightSpaceAABB.mMax[0], ViewFrustumPartitionLightSpaceAABB.mMax[1]);

		// Calculate the current shadow map cascade's corresponding composite matrices
		dirLightViewProjMat[cascade] = dirLightProjMat[cascade] * dirLightViewMat;
		dirLightWorldViewProjMat[cascade] = dirLightViewProjMat[cascade] * worldMat;
	}

	// RSM matrices
	RenderContext->CreateOrthographicMatrix(RSMProjMat,
		SponzaLightSpaceAABB.mMin[0],
		SponzaLightSpaceAABB.mMax[1],
		SponzaLightSpaceAABB.mMax[0],
		SponzaLightSpaceAABB.mMin[1],
		SponzaLightSpaceAABB.mMin[2], SponzaLightSpaceAABB.mMax[2]);
	RSMWorldViewProjMat = RSMProjMat * dirLightWorldViewMat;
	invertFull(RSMInvProjMat, RSMProjMat);
	ViewToRSMViewMat = dirLightViewMat * invViewMat;

	// Debug sun camera
	if (DEBUG_CSM_CAMERA)
	{
		viewMat = dirLightViewMat;
		invertFull(invViewMat, viewMat);
		projMat = dirLightProjMat[NUM_CASCADES - 1];
		invertFull(invProjMat, projMat);
		worldViewMat = viewMat * worldMat;
		viewProjMat = projMat * viewMat;
		invViewProjMat = invViewMat * invProjMat;
		worldViewProjMat = projMat * worldViewMat;
		dirScreenToLightViewMat = dirLightViewMat * invViewProjMat;
		skyViewProjMat = viewProjMat * makeTrans(-tCamera.vPos, Type2Type<Matrix44f>());
	}

	// Debug RSM camera
	if (DEBUG_RSM_CAMERA)
	{
		viewMat = dirLightViewMat;
		invertFull(invViewMat, viewMat);
		projMat = RSMProjMat;
		invertFull(invProjMat, projMat);
		worldViewMat = viewMat * worldMat;
		viewProjMat = projMat * viewMat;
		invViewProjMat = invViewMat * invProjMat;
		worldViewProjMat = projMat * worldViewMat;
		dirScreenToLightViewMat = dirLightViewMat * invViewProjMat;
		skyViewProjMat = viewProjMat * makeTrans(-tCamera.vPos, Type2Type<Matrix44f>());
	}

	POP_PROFILE_MARKER();
}

void GITechDemo::GenerateDirectionalShadowMap()
{
	PUSH_PROFILE_MARKER("GenerateDirectionalShadowMap()");

	bool red, blue, green, alpha;
	RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);
	bool scissorEnabled = RenderContext->GetRenderStateManager()->GetScissorEnabled();
	RenderContext->GetRenderStateManager()->SetScissorEnabled(true);

	ShadowMapDir.Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	assert(SHADOW_MAP_SIZE[0] == SHADOW_MAP_SIZE[1]);
	const unsigned int cascadesPerRow = (unsigned int)Math::ceil(Math::sqrt((float)NUM_CASCADES));
	const unsigned int cascadeSize = SHADOW_MAP_SIZE[0] / cascadesPerRow;
	for (unsigned int cascade = 0; cascade < NUM_CASCADES; cascade++)
	{
		char tmpBuf[16];
		sprintf_s(tmpBuf, "Cascade %d", cascade);
		PUSH_PROFILE_MARKER(tmpBuf);

		Vec2i size = Vec2i(cascadeSize, cascadeSize);
		Vec2i offset = Vec2i(cascadeSize * (cascade % cascadesPerRow), cascadeSize * (cascade / cascadesPerRow));
		RenderContext->SetViewport(size, offset);
		RenderContext->GetRenderStateManager()->SetScissor(size, offset);

		PUSH_PROFILE_MARKER("DepthPass.hlsl");
		f44WorldViewProjMat = dirLightWorldViewProjMat[cascade];

		DepthPassShader.Enable();

		// Normally, you would only render meshes whose AABB/OBB intersect with the cascade's
		// view frustum, but we don't have a big enough scene to care at the moment
		for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
		{
			PUSH_PROFILE_MARKER(SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
			RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
			POP_PROFILE_MARKER();
		}

		DepthPassShader.Disable();
		POP_PROFILE_MARKER();

		POP_PROFILE_MARKER();
	}

	ShadowMapDir.Disable();

	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetScissorEnabled(scissorEnabled);

	POP_PROFILE_MARKER();
}

void GITechDemo::GenerateRSM()
{
	PUSH_PROFILE_MARKER("GenerateRSM()");

	RSMBuffer.Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	PUSH_PROFILE_MARKER("RSMCapture.hlsl");
	f44LightWorldViewProjMat = RSMWorldViewProjMat;
	f44LightWorldViewMat = dirLightWorldViewMat;

	for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
	{
		const unsigned int diffTexIdx = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_DIFFUSE, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
		const unsigned int normalTexIdx = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_HEIGHT, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);

		texDiffuse = diffTexIdx;

		RSMCaptureShader.Enable();

		RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);

		RSMCaptureShader.Disable();
	}

	POP_PROFILE_MARKER();

	RSMBuffer.Disable();

	POP_PROFILE_MARKER();
}

void GITechDemo::GenerateGBuffer()
{
	PUSH_PROFILE_MARKER("GenerateGBuffer()");

	GBuffer.Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	// A depth prepass is useful if we have expensive pixel shaders in our
	// GBuffer generation process. It allows us to avoid GITechDemo::shading pixels
	// that eventually get overwritten by other pixels that have smaller
	// depth values. We are not fill-rate bound, so the depth prepass is disabled.
	if (GBUFFER_Z_PREPASS)
	{
		PUSH_PROFILE_MARKER("G-Buffer Z prepass");

		bool red, blue, green, alpha;
		RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);
		RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);

		PUSH_PROFILE_MARKER("DepthPass.hlsl");
		f44WorldViewProjMat = worldViewProjMat;

		DepthPassShader.Enable();

		for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
		{
			PUSH_PROFILE_MARKER(SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
			RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);
			POP_PROFILE_MARKER();
		}

		DepthPassShader.Disable();
		POP_PROFILE_MARKER();

		RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);

		RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
		RenderContext->GetRenderStateManager()->SetZFunc(CMP_EQUAL);

		POP_PROFILE_MARKER();

		PUSH_PROFILE_MARKER("G-Buffer capture pass");
	}

	PUSH_PROFILE_MARKER("GBufferGeneration.hlsl");

	f44WorldViewMat = worldViewMat;
	f44WorldViewProjMat = worldViewProjMat;

	// A visibility test would be useful if we were CPU bound (or vertex bound).
	// However, there isn't a reason to do such an optimization for now, since the
	// scene isn't very big and we are mostly pixel bound.
	for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
	{
		PUSH_PROFILE_MARKER(SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
		const unsigned int diffTexIdx = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_DIFFUSE, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
		const unsigned int normalTexIdx = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_HEIGHT, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);
		const unsigned int specTexIdx = SponzaScene.GetTexture(LibRendererDll::Model::TextureDesc::TT_SPECULAR, SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx);

		texDiffuse = diffTexIdx;
		texNormal = normalTexIdx;
		bHasNormalMap = (normalTexIdx != -1);
		texSpec = specTexIdx;
		bHasSpecMap = (specTexIdx != -1);
		fSpecIntensity = SponzaScene.GetModel()->arrMaterial[SponzaScene.GetModel()->arrMesh[mesh]->nMaterialIdx]->fShininessStrength;

		GBufferGenerationShader.Enable();

		RenderContext->DrawVertexBuffer(SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer);

		GBufferGenerationShader.Disable();
		POP_PROFILE_MARKER();
	}
	POP_PROFILE_MARKER();

	GBuffer.Disable();

	if (GBUFFER_Z_PREPASS)
	{
		RenderContext->GetRenderStateManager()->SetZWriteEnabled(true);
		RenderContext->GetRenderStateManager()->SetZFunc(CMP_LESSEQUAL);
		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

// This function copy-resolves the INTZ depth texture we use when generating the
// GBuffer (since we can sample it like a regular texture) to the D24S8 depth
// surface of the light accumulation buffer for depth testing (e.g. when rendering the sky)
// Having the correct depth allows us to do some more optimizations which also make use of
// the stencil (e.g. rendering a cone corresponding to the spot light and marking the pixels
// that pass the depth test, using the stencil buffer, so that we only calculate the light
// for those pixels and thus reducing pointless shading of pixels which are not lit)
void GITechDemo::CopyDepthBuffer()
{
	PUSH_PROFILE_MARKER("CopyDepthBuffer()");

	bool red, blue, green, alpha;
	RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);
	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(true);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	PUSH_PROFILE_MARKER("DepthCopy.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / GBuffer.GetRenderTarget()->GetWidth(), 0.5f / GBuffer.GetRenderTarget()->GetHeight());
	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();

	DepthCopyShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DepthCopyShader.Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	POP_PROFILE_MARKER();
}

// Draw a world axis-aligned cube with its' center coinciding with the camera's position.
// The vertex shader sets the visible face(s) at max depth so as to not draw over
// other objects that aren't inside the cube (which, at 2x2x2, is quite small)
void GITechDemo::DrawSky()
{
	PUSH_PROFILE_MARKER("DrawSky()");

	bool blendEnabled;
	Blend DstBlend, SrcBlend;
	blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	DstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	SrcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ZERO);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);

	bool zWriteEnable = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_LESSEQUAL);

	PUSH_PROFILE_MARKER("SkyBox.hlsl");
	f44SkyViewProjMat = skyViewProjMat;
	texSkyTex = SkyTexture.GetTextureIndex();
	f3LightDir = directionalLightDir;

	SkyBoxShader.Enable();

	RenderContext->DrawVertexBuffer(SkyBoxCube);

	SkyBoxShader.Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(DstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(SrcBlend);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWriteEnable);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	POP_PROFILE_MARKER();
}

// Generate ambient occlusion buffer
void GITechDemo::CalculateAmbientOcclusion()
{
	PUSH_PROFILE_MARKER("CalculateAmbientOcclusion()");
	AmbientOcclusionBuffer[0]->Enable();

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	PUSH_PROFILE_MARKER("SSAO.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / GBuffer.GetRenderTarget()->GetWidth(), 0.5f / GBuffer.GetRenderTarget()->GetHeight());
	texNormalBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(1);
	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
	f44InvProjMat = invProjMat;
	bBlurPass = false;

	SsaoShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	SsaoShader.Disable();
	POP_PROFILE_MARKER();

	AmbientOcclusionBuffer[0]->Disable();
	POP_PROFILE_MARKER();
}

// Blur ambient occlusion buffer
void GITechDemo::BlurAmbientOcclusion()
{
	PUSH_PROFILE_MARKER("BlurAmbientOcclusion()");

	for (unsigned int i = 0; i < SSAO_BLUR_KERNEL_COUNT; i++)
	{
		char label[10];
		sprintf_s(label, "Kernel %d", SSAO_BLUR_KERNEL[i]);
		PUSH_PROFILE_MARKER(label);
		AmbientOcclusionBuffer[(i + 1) % 2]->Enable();

		// Not necesarry
		//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

		PUSH_PROFILE_MARKER("SSAO.hlsl");
		f2HalfTexelOffset = Vec2f(0.5f / AmbientOcclusionBuffer[i % 2]->GetRenderTarget()->GetWidth(), 0.5f / AmbientOcclusionBuffer[i % 2]->GetRenderTarget()->GetHeight());
		bBlurPass = true;

		ResourceMgr->GetTexture(AmbientOcclusionBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
		texSource = AmbientOcclusionBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0);

		f2TexelSize = Vec2f(1.f / AmbientOcclusionBuffer[i % 2]->GetRenderTarget()->GetWidth(), 1.f / AmbientOcclusionBuffer[i % 2]->GetRenderTarget()->GetHeight());
		nKernel = SSAO_BLUR_KERNEL[i];

		SsaoShader.Enable();

		RenderContext->DrawVertexBuffer(FullScreenQuad);

		SsaoShader.Disable();
		POP_PROFILE_MARKER();

		AmbientOcclusionBuffer[(i + 1) % 2]->Disable();
		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

// Apply ambient occlusion to the light accumulation buffer
void GITechDemo::ApplyAmbientOcclusion()
{
	PUSH_PROFILE_MARKER("ApplyAmbientOcclusion()");
	LightAccumulationBuffer.Enable();

	const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	const Blend dstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	const Blend srcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_INVSRCCOLOR);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ZERO);

	PUSH_PROFILE_MARKER("ColorCopy.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / AmbientOcclusionBuffer[SSAO_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetWidth(), 0.5f / AmbientOcclusionBuffer[SSAO_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetHeight());

	ResourceMgr->GetTexture(AmbientOcclusionBuffer[SSAO_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	texSource = AmbientOcclusionBuffer[SSAO_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetColorBuffer(0);

	ColorCopyShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	ColorCopyShader.Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);

	LightAccumulationBuffer.Disable();
	POP_PROFILE_MARKER();
}

// Accumulate ambient occlusion
void GITechDemo::AccumulateAmbientOcclusion()
{
	PUSH_PROFILE_MARKER("AccumulateAmbientOcclusion()");

	const bool blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

	LibRendererDll::RenderTarget* const rtBkp = LibRendererDll::RenderTarget::GetActiveRenderTarget();
	rtBkp->Disable();

	CalculateAmbientOcclusion();
	BlurAmbientOcclusion();
	ApplyAmbientOcclusion();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);

	rtBkp->Enable();
	POP_PROFILE_MARKER();
}

// Accumulate ambient light
void GITechDemo::AccumulateAmbientLight()
{
	PUSH_PROFILE_MARKER("AccumulateAmbientLight()");

	PUSH_PROFILE_MARKER("DeferredLightAmb.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / GBuffer.GetRenderTarget()->GetWidth(), 0.5f / GBuffer.GetRenderTarget()->GetHeight());
	texDiffuseBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(0);

	DeferredLightAmbShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DeferredLightAmbShader.Disable();
	POP_PROFILE_MARKER();

	if (AMBIENT_OCCLUSION_ENABLED)
		AccumulateAmbientOcclusion();

	POP_PROFILE_MARKER();
}

// Accumulate directional light
void GITechDemo::AccumulateDirectionalLight()
{
	PUSH_PROFILE_MARKER("AccumulateDirectionalLight()");

	PUSH_PROFILE_MARKER("DeferredLightDir.hlsl");
	f44InvProjMat = invProjMat;
	f2HalfTexelOffset = Vec2f(0.5f / GBuffer.GetRenderTarget()->GetWidth(), 0.5f / GBuffer.GetRenderTarget()->GetHeight());
	texDiffuseBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(0);
	texNormalBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(1);
	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
	texShadowMap = ShadowMapDir.GetRenderTarget()->GetDepthBuffer();
	f2OneOverShadowMapSize = Vec2f(1.f / (float)SHADOW_MAP_SIZE[0], 1.f / (float)SHADOW_MAP_SIZE[1]);
	f44ViewMat = viewMat;
	f44ScreenToLightViewMat = dirScreenToLightViewMat;
	f3LightDir = directionalLightDir;
	f2CascadeBoundsMin = cascadeBoundsMin;
	f2CascadeBoundsMax = cascadeBoundsMax;
	f44CascadeProjMat = dirLightProjMat;
	poissonDisk = PoissonDisk;

	DeferredLightDirShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DeferredLightDirShader.Disable();
	POP_PROFILE_MARKER();

	POP_PROFILE_MARKER();
}

// Accumulate indirect lighting (RSM)
void GITechDemo::AccumulateIndirectLight()
{
	PUSH_PROFILE_MARKER("AccumulateIndirectLight()");

	LibRendererDll::RenderTarget* const rtBkp = LibRendererDll::RenderTarget::GetActiveRenderTarget();
	if (USE_QUARTER_RES_INDIRECT_LIGHT_ACCUMULATION_BUFFER)
	{
		rtBkp->Disable();
		IndirectLightAccumulationBuffer.Enable();

		RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);
	}

	PUSH_PROFILE_MARKER("RSMApply.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / GBuffer.GetRenderTarget()->GetWidth(), 0.5f / GBuffer.GetRenderTarget()->GetHeight());
	f2HalfTexelOffset = Vec2f(0.5f / GBuffer.GetRenderTarget()->GetWidth(), 0.5f / GBuffer.GetRenderTarget()->GetHeight());
	texRSMFluxBuffer = RSMBuffer.GetRenderTarget()->GetColorBuffer(0);
	texRSMNormalBuffer = RSMBuffer.GetRenderTarget()->GetColorBuffer(1);
	texRSMDepthBuffer = RSMBuffer.GetRenderTarget()->GetDepthBuffer();
	texNormalBuffer = GBuffer.GetRenderTarget()->GetColorBuffer(1);
	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
	f44ScreenToLightViewMat = dirScreenToLightViewMat;
	f44RSMProjMat = RSMProjMat;
	f44RSMInvProjMat = RSMInvProjMat;
	f44ViewToRSMViewMat = ViewToRSMViewMat;
	texIndirectLightAccumulationBuffer = IndirectLightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0);
	bIsUpscalePass = false;

	for (unsigned int i = 0; i < RSM_NUM_PASSES; i++)
	{
		f3RSMKernel = RSMKernel + i * RSM_SAMPLES_PER_PASS;

		char marker[16];
		sprintf_s(marker, "Pass %d", i);
		PUSH_PROFILE_MARKER(marker);

		RSMApplyShader.Enable();

		RenderContext->DrawVertexBuffer(FullScreenQuad);

		RSMApplyShader.Disable();

		POP_PROFILE_MARKER();
	}
	POP_PROFILE_MARKER();

	if (USE_QUARTER_RES_INDIRECT_LIGHT_ACCUMULATION_BUFFER)
	{
		IndirectLightAccumulationBuffer.Disable();
		rtBkp->Enable();

		PUSH_PROFILE_MARKER("RSMApply.hlsl (upsample)");

		//if (ShdInputLUT[RSMApplyVS][f2HalfTexelOffset] != ~0)
		//	RSMApplyVInput->SetFloat2(ShdInputLUT[RSMApplyVS][f2HalfTexelOffset], Vec2f(0.5f / IndirectLightAccumulationBuffer->GetWidth(), 0.5f / IndirectLightAccumulationBuffer->GetHeight()));

		f3RSMKernel = RSMKernel;
		bIsUpscalePass = true;

		RSMApplyShader.Enable();

		RenderContext->DrawVertexBuffer(FullScreenQuad);

		RSMApplyShader.Disable();

		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

// Accumulate contributions from all light sources into the light accumulation buffer
void GITechDemo::AccumulateLight()
{
	PUSH_PROFILE_MARKER("AccumulateLight()");

	LightAccumulationBuffer.Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	// Copy-resolve the depth buffer for later usage
	CopyDepthBuffer();

	// Disable Z writes, since we already have the correct depth buffer
	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	bool blendEnabled;
	Blend DstBlend, SrcBlend;
	blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	DstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	SrcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	// Additive color blending is required for accumulating light
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ONE);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);

	// Accumulate the contributions from the various light sources
	if (AMBIENT_LIGHT_ENABLED)
		AccumulateAmbientLight();

	if (DIRECTIONAL_LIGHT_ENABLED)
		AccumulateDirectionalLight();

	if (INDIRECT_LIGHT_ENABLED)
		AccumulateIndirectLight();

	// Reset the render states
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(DstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(SrcBlend);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	// Draw the sky where the depth value is at 1 (no other object has been drawn there)
	DrawSky();

	LightAccumulationBuffer.Disable();

	POP_PROFILE_MARKER();
}

// Downscale the HDR light accumulation texture
void GITechDemo::HDRDownsamplePass()
{
	PUSH_PROFILE_MARKER("HDRDownsamplePass()");

	HDRDownsampleBuffer.Enable();

	// Not necessary
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	PUSH_PROFILE_MARKER("Downsample.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / HDRDownsampleBuffer.GetRenderTarget()->GetWidth(), 0.5f / HDRDownsampleBuffer.GetRenderTarget()->GetHeight());
	f2TexelSize = Vec2f(1.f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(), 1.f / LightAccumulationBuffer.GetRenderTarget()->GetHeight());
	texSource = LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0);
	nDownsampleFactor = 16;
	bApplyBrightnessFilter = false;

	DownsampleShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DownsampleShader.Disable();
	POP_PROFILE_MARKER();

	HDRDownsampleBuffer.Disable();

	POP_PROFILE_MARKER();
}

// Measure average luminance level of scene
void GITechDemo::LuminanceMeasurementPass()
{
	PUSH_PROFILE_MARKER("LuminanceMeasurementPass()");

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

		PUSH_PROFILE_MARKER("LumaCalc.hlsl");
		f2HalfTexelOffset = Vec2f(0.5f / AverageLuminanceBuffer[i]->GetRenderTarget()->GetWidth(), 0.5f / AverageLuminanceBuffer[i]->GetRenderTarget()->GetHeight());

		if (i == 0)
		{
			f2TexelSize = Vec2f(1.f / HDRDownsampleBuffer.GetRenderTarget()->GetWidth(), 1.f / HDRDownsampleBuffer.GetRenderTarget()->GetHeight());
			texLumaCalcInput = HDRDownsampleBuffer.GetRenderTarget()->GetColorBuffer(0);
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

		RenderContext->DrawVertexBuffer(FullScreenQuad);

		LumaCalcShader.Disable();
		POP_PROFILE_MARKER();

		AverageLuminanceBuffer[i]->Disable();
		POP_PROFILE_MARKER();
	}

	PUSH_PROFILE_MARKER("Luminance adaptation");

	static unsigned long long lastTick = GetTickCount64();
	const float frameTime = gmtl::Math::clamp((float)(GetTickCount64() - lastTick) / 1000.f, 0.f, 1.f / fLumaAdaptSpeed.GetCurrentValue());
	lastTick = GetTickCount64();

	RenderTarget* const rtBkp = AdaptedLuminance[0];
	AdaptedLuminance[0] = AdaptedLuminance[1];
	AdaptedLuminance[1] = rtBkp;

	AdaptedLuminance[0]->Enable();

	PUSH_PROFILE_MARKER("LumaCalc.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / AdaptedLuminance[1]->GetRenderTarget()->GetWidth(), 0.5f / AdaptedLuminance[1]->GetRenderTarget()->GetHeight());
	texLumaCalcInput = AdaptedLuminance[1]->GetRenderTarget()->GetColorBuffer(0);
	texLumaTarget = AverageLuminanceBuffer[3]->GetRenderTarget()->GetColorBuffer(0);
	bInitialLumaPass = false;
	bFinalLumaPass = false;
	bLumaAdaptationPass = true;
	fFrameTime = frameTime;

	LumaCalcShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	LumaCalcShader.Disable();
	POP_PROFILE_MARKER();

	AdaptedLuminance[0]->Disable();
	POP_PROFILE_MARKER();

	POP_PROFILE_MARKER();
}

// Tone map the HDR image onto the LDR target
void GITechDemo::HDRToneMappingPass()
{
	PUSH_PROFILE_MARKER("HDRToneMappingPass");

	LuminanceMeasurementPass();

	LDRToneMappedImageBuffer.Enable();

	const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	PUSH_PROFILE_MARKER("HDRToneMapping.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(), 0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight());
	texLightAccumulationBuffer = LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0);
	texAvgLuma = AdaptedLuminance[0]->GetRenderTarget()->GetColorBuffer(0);

	HDRToneMappingShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	HDRToneMappingShader.Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);

	LDRToneMappedImageBuffer.Disable();

	POP_PROFILE_MARKER();
}

void GITechDemo::BloomDownsample()
{
	PUSH_PROFILE_MARKER("Downsample + brightness filter");
	HDRBloomBuffer[0]->Enable();

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	PUSH_PROFILE_MARKER("Downsample.hlsl (4x4)");
	f2HalfTexelOffset = Vec2f(0.5f / HDRBloomBuffer[0]->GetRenderTarget()->GetWidth(), 0.5f / HDRBloomBuffer[0]->GetRenderTarget()->GetHeight());
	f2TexelSize = Vec2f(1.f / HDRDownsampleBuffer.GetRenderTarget()->GetWidth(), 1.f / HDRDownsampleBuffer.GetRenderTarget()->GetHeight());
	texSource = HDRDownsampleBuffer.GetRenderTarget()->GetColorBuffer(0);
	nDownsampleFactor = 1;
	bApplyBrightnessFilter = true;

	DownsampleShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DownsampleShader.Disable();
	POP_PROFILE_MARKER();

	HDRBloomBuffer[0]->Disable();
	POP_PROFILE_MARKER();
}

void GITechDemo::BloomBlur()
{
	PUSH_PROFILE_MARKER("Bloom blur pass");

	for (unsigned int i = 0; i < BLOOM_BLUR_KERNEL_COUNT; i++)
	{
		char label[10];
		sprintf_s(label, "Kernel %d", BLOOM_BLUR_KERNEL[i]);
		PUSH_PROFILE_MARKER(label);
		HDRBloomBuffer[(i + 1) % 2]->Enable();

		// Not necesarry
		//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

		PUSH_PROFILE_MARKER("Bloom.hlsl");
		f2HalfTexelOffset = Vec2f(0.5f / HDRBloomBuffer[i % 2]->GetRenderTarget()->GetWidth(), 0.5f / HDRBloomBuffer[i % 2]->GetRenderTarget()->GetHeight());

		ResourceMgr->GetTexture(HDRBloomBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
		texSource = HDRBloomBuffer[i % 2]->GetRenderTarget()->GetColorBuffer(0);

		f2TexelSize = Vec2f(1.f / HDRBloomBuffer[i % 2]->GetRenderTarget()->GetWidth(), 1.f / HDRBloomBuffer[i % 2]->GetRenderTarget()->GetHeight());
		nKernel = BLOOM_BLUR_KERNEL[i];

		BloomShader.Enable();

		RenderContext->DrawVertexBuffer(FullScreenQuad);

		BloomShader.Disable();
		POP_PROFILE_MARKER();

		HDRBloomBuffer[(i + 1) % 2]->Disable();
		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

void GITechDemo::BloomApply()
{
	PUSH_PROFILE_MARKER("Bloom apply");
	LightAccumulationBuffer.Enable();

	const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	const Blend dstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	const Blend srcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ONE);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);

	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	PUSH_PROFILE_MARKER("ColorCopy.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / HDRBloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetWidth(), 0.5f / HDRBloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetHeight());

	ResourceMgr->GetTexture(HDRBloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	texSource = HDRBloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetRenderTarget()->GetColorBuffer(0);

	ColorCopyShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	ColorCopyShader.Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	LightAccumulationBuffer.Disable();
	POP_PROFILE_MARKER();
}

// Apply bloom effect
void GITechDemo::BloomPass()
{
	PUSH_PROFILE_MARKER("BloomPass()");

	BloomDownsample();
	BloomBlur();
	BloomApply();

	POP_PROFILE_MARKER();
}

void GITechDemo::FxaaPass()
{
	PUSH_PROFILE_MARKER("FxaaPass()");
	LDRFxaaImageBuffer.Enable();

	const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	LibRendererDll::RenderTarget* const srcRT = HDR_TONEMAPPING_ENABLED ? LDRToneMappedImageBuffer.GetRenderTarget() : LightAccumulationBuffer.GetRenderTarget();

	PUSH_PROFILE_MARKER("FXAA.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / srcRT->GetWidth(), 0.5f / srcRT->GetHeight());
	f2HalfTexelOffset = Vec2f(0.5f / srcRT->GetWidth(), 0.5f / srcRT->GetHeight());
	texSource = srcRT->GetColorBuffer(0);
	f2TexelSize = Vec2f(1.f / srcRT->GetWidth(), 1.f / srcRT->GetHeight());

	FxaaShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	FxaaShader.Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);

	LDRFxaaImageBuffer.Disable();
	POP_PROFILE_MARKER();
}

void GITechDemo::CalculateDoF()
{
	PUSH_PROFILE_MARKER("CalculateDoF()");
	if (DOF_USE_QUARTER_RESOLUTION_BUFFER)
		DepthOfFieldQuarterBuffer.Enable();
	else
		DepthOfFieldFullBuffer.Enable();

	const bool colorBlendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

	PUSH_PROFILE_MARKER("BokehDof.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(), 0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight());
	texSource = LightAccumulationBuffer.GetRenderTarget()->GetColorBuffer(0);
	texDepthBuffer = GBuffer.GetRenderTarget()->GetDepthBuffer();
	f2TexSourceSize = Vec2f((float)LightAccumulationBuffer.GetRenderTarget()->GetWidth(), (float)LightAccumulationBuffer.GetRenderTarget()->GetHeight());
	f2TexelSize = Vec2f(1.f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(), 1.f / LightAccumulationBuffer.GetRenderTarget()->GetHeight());

	BokehDofShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	BokehDofShader.Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(colorBlendEnabled);

	if (DOF_USE_QUARTER_RESOLUTION_BUFFER)
		DepthOfFieldQuarterBuffer.Disable();
	else
		DepthOfFieldFullBuffer.Disable();
	POP_PROFILE_MARKER();
}

void GITechDemo::ApplyDoF()
{
	PUSH_PROFILE_MARKER("ApplyDoF()");
	LightAccumulationBuffer.Enable();

	const bool blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	const Blend dstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	const Blend srcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	if (DOF_USE_QUARTER_RESOLUTION_BUFFER)
	{
		RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
		RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_INVSRCALPHA);
		RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_SRCALPHA);
	}

	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	PUSH_PROFILE_MARKER("ColorCopy.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / LightAccumulationBuffer.GetRenderTarget()->GetWidth(), 0.5f / LightAccumulationBuffer.GetRenderTarget()->GetHeight());
	texSource = (DOF_USE_QUARTER_RESOLUTION_BUFFER ? DepthOfFieldQuarterBuffer.GetRenderTarget() : DepthOfFieldFullBuffer.GetRenderTarget())->GetColorBuffer(0);

	ColorCopyShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	ColorCopyShader.Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	LightAccumulationBuffer.Disable();
	POP_PROFILE_MARKER();
}

// Apply DoF effect
void GITechDemo::DepthOfFieldPass()
{
	PUSH_PROFILE_MARKER("DepthOfFieldPass()");

	CalculateDoF();
	ApplyDoF();

	POP_PROFILE_MARKER();
}

// Apply postprocessing effects. Afterwards, copy results to the backbuffer.
void GITechDemo::ApplyPostProcessing()
{
	PUSH_PROFILE_MARKER("ApplyPostProcessing()");

	// Step 1: Downsampling
	if (HDR_TONEMAPPING_ENABLED || BLOOM_ENABLED)
		HDRDownsamplePass();

	// Step 2: Depth of Field
	if (DOF_ENABLED)
		DepthOfFieldPass();

	// Step 3: Bloom
	if (BLOOM_ENABLED)
		BloomPass();

	// Step 4: Tone mapping + gamma correction
	if (HDR_TONEMAPPING_ENABLED)
		HDRToneMappingPass();

	// Step 5: Antialiasing
	if (FXAA_ENABLED)
		FxaaPass();

	POP_PROFILE_MARKER();
}

// Copy texture to the back buffer
void GITechDemo::CopyResultToBackBuffer(LibRendererDll::RenderTarget* const rt)
{
	PUSH_PROFILE_MARKER("CopyResultToBackBuffer()");

	const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);

	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	PUSH_PROFILE_MARKER("ColorCopy.hlsl");
	f2HalfTexelOffset = Vec2f(0.5f / rt->GetWidth(), 0.5f / rt->GetHeight());
	texSource = rt->GetColorBuffer(0);

	ColorCopyShader.Enable();

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	ColorCopyShader.Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);

	POP_PROFILE_MARKER();
}

// Render a frame
void GITechDemo::RenderScene()
{
	if (RenderContext->BeginFrame())
	{
		//UpdateMatrices();

		GenerateDirectionalShadowMap();

		GenerateRSM();

		GenerateGBuffer();

		AccumulateLight();

		if (POST_PROCESSING_ENABLED)
			ApplyPostProcessing();

		LibRendererDll::RenderTarget* FinalImageBuffer = nullptr;
		if (POST_PROCESSING_ENABLED)
		{
			FinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

			if (DOF_ENABLED)
				FinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

			if (BLOOM_ENABLED)
				FinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

			if (HDR_TONEMAPPING_ENABLED)
				FinalImageBuffer = LDRToneMappedImageBuffer.GetRenderTarget();

			if (FXAA_ENABLED)
				FinalImageBuffer = LDRFxaaImageBuffer.GetRenderTarget();
		}
		else
			FinalImageBuffer = LightAccumulationBuffer.GetRenderTarget();

		if (FinalImageBuffer)
			CopyResultToBackBuffer(FinalImageBuffer);

		RenderContext->EndFrame();

		RenderContext->SwapBuffers();
	}
}
