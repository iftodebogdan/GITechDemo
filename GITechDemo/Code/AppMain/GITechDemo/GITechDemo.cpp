#include "stdafx.h"

#include <Renderer.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <VertexFormat.h>
using namespace LibRendererDll;

#include "GITechDemo.h"
#include "RenderResources.h"
#include "RenderScheme.h"
#include "PerlinNoise.h"
using namespace GITechDemoApp;

CREATE_APP(GITechDemo)

namespace GITechDemoApp
{
	bool CAMERA_ANIMATION_ENABLED = true;
	int CAMERA_ANIMATION_TIMEOUT_SECONDS = 30;

	bool DIRECTIONAL_LIGHT_ANIMATION_ENABLED = true;

	Perlin PerlinNoise(1, USHRT_MAX, 1, GetTickCount());

	extern AABoxf SceneAABB;
}

GITechDemo::GITechDemo()
	: App()
	, m_fDeltaTime(0.f)
	, m_pHWND(nullptr)
{}

GITechDemo::~GITechDemo()
{
	Renderer::DestroyInstance();
}

void GITechDemo::Init(void* hWnd)
{
	m_pHWND = hWnd;

	// Renderer MUST be initialized on the SAME thread as the target window
	Renderer::CreateInstance(API_DX9);
	//Renderer::CreateInstance(API_NULL);

	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	RenderContext->Initialize(hWnd);

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	PUSH_PROFILE_MARKER("Initialize render resources");

	RenderResource::SetResourceManager(ResourceMgr);

	/*
	// First of all, load the scene (models + textures)
	RenderResource::InitAllModels();

	// Load the sky cube map texture
	RenderResource::InitAllTextures();

	// Load shaders
	RenderResource::InitAllShaders();

	// Initialize the various render targets we will be using
	RenderResource::InitAllRenderTargets();
	*/

	RenderResource::InitAllResources();

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
	FullScreenTri = ResourceMgr->GetVertexBuffer(vbIdx);

	FullScreenTri->Lock(BL_WRITE_ONLY);
	FullScreenTri->Position<Vec4f>(0) = Vec4f(-1.f, 1.f, 1.f, 1.f);
	FullScreenTri->Position<Vec4f>(1) = Vec4f(3.f, 1.f, 1.f, 1.f);
	FullScreenTri->Position<Vec4f>(2) = Vec4f(-1.f, -3.f, 1.f, 1.f);
	FullScreenTri->Update();
	FullScreenTri->Unlock();

	// Set initial camera position
	tCamera.vPos = Vec3f(-840.f, -600.f, -195.f);
	tCamera.mRot.set(
		-0.440301329f, 0.00776965916f, 0.897806108f, 0.f,
		-0.142924204f, 0.986597657f, -0.0786283761f, 0.f,
		-0.886387110f, -0.162937075f, -0.433295786f, 0.f,
		0.f, 0.f, 0.f, 1.f
		);

	POP_PROFILE_MARKER();
}

void GITechDemo::Update(const float fDeltaTime)
{
	m_fDeltaTime = fDeltaTime;

	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	PUSH_PROFILE_MARKER("UpdateMatrices()");

	// Update camera
	normalize(tCamera.vMoveVec);
	tCamera.vMoveVec *= 5.f;
	tCamera.vPos -=
		Vec3f(tCamera.mRot[2][0] * tCamera.vMoveVec[2] * tCamera.fSpeedFactor, tCamera.mRot[2][1] * tCamera.vMoveVec[2] * tCamera.fSpeedFactor, tCamera.mRot[2][2] * tCamera.vMoveVec[2] * tCamera.fSpeedFactor) +
		Vec3f(tCamera.mRot[0][0] * tCamera.vMoveVec[0] * tCamera.fSpeedFactor, tCamera.mRot[0][1] * tCamera.vMoveVec[0] * tCamera.fSpeedFactor, tCamera.mRot[0][2] * tCamera.vMoveVec[0] * tCamera.fSpeedFactor);

	// Animate camera
	if (CAMERA_ANIMATION_ENABLED)
	{
		static Camera tCameraLastFrame = tCamera;
		static float lastInput = 0.f;
		if (tCamera.vPos != tCameraLastFrame.vPos || tCamera.mRot != tCameraLastFrame.mRot)
		{
			lastInput = 0.f;
		}
		else
		{
			lastInput += fDeltaTime;

			if (lastInput > CAMERA_ANIMATION_TIMEOUT_SECONDS)
			{
				static float time = 0.f;
				static float periodPos = 5000000.f;
				time += fDeltaTime;
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
					((SceneAABB.getMax()[0] - SceneAABB.getMin()[0]) * perlinPos[0] + SceneAABB.getMin()[0]) / 1.5f,
					((SceneAABB.getMax()[1] - SceneAABB.getMin()[1]) * perlinPos[1] + SceneAABB.getMin()[1]) / 1.5f,
					((SceneAABB.getMax()[2] - SceneAABB.getMin()[2]) * perlinPos[2] + SceneAABB.getMin()[2]) / 1.5f
					);

				Vec3f sponzaCenter(
					(SceneAABB.getMax()[0] - SceneAABB.getMin()[0]) / 2.f + SceneAABB.getMin()[0],
					((SceneAABB.getMax()[1] - SceneAABB.getMin()[1]) / 2.f + SceneAABB.getMin()[1]) / 2.f,
					(SceneAABB.getMax()[2] - SceneAABB.getMin()[2]) / 2.f + SceneAABB.getMin()[2]);
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
		}

		tCameraLastFrame = tCamera;
	}

	// Animate directional light using Perlin Noise
	float noiseX = PerlinNoise.Get((float)GetTickCount64() / (float)INT_MAX, 0);
	float noiseZ = PerlinNoise.Get(0, (float)GetTickCount64() / (float)INT_MAX);
	if (DIRECTIONAL_LIGHT_ANIMATION_ENABLED)
	{
		((Vec3f&)f3LightDir)[0] = noiseX;
		((Vec3f&)f3LightDir)[2] = noiseZ;
	}
	else
	{
		((Vec3f&)f3LightDir)[0] = 0.0f;
		((Vec3f&)f3LightDir)[2] = 0.3f;
	}
	((Vec3f&)f3LightDir)[1] = -1.f;
	normalize((Vec3f&)f3LightDir);

	// Precalculate some parts of the equation for reconstructing
	// linear depth from hyperbolic depth
	f2LinearDepthEquation = Vec2f(fZNear * fZFar / (fZNear - fZFar), fZFar / (fZFar - fZNear));

	// Calculate world matrix
	f44WorldMat = makeTrans(Vec3f(0, 0, 0), Type2Type<Matrix44f>());

	// Calculate view matrix
	f44ViewMat = tCamera.mRot * makeTrans(tCamera.vPos, Type2Type<Matrix44f>());

	// Calculate projection matrix
	RECT rc;
	GetClientRect((HWND)m_pHWND, &rc);
	Vec2i vp = Vec2i(rc.right - rc.left, rc.bottom - rc.top);
	RenderContext->CreatePerspectiveMatrix(f44ProjMat, Math::deg2Rad(60.f), (float)vp[0] / (float)vp[1], fZNear, fZFar);
	invertFull((Matrix44f&)f44InvProjMat, (Matrix44f&)f44ProjMat);

	// Calculate some composite matrices
	invertFull((Matrix44f&)f44InvViewMat, (Matrix44f&)f44ViewMat);
	f44ViewProjMat = f44ProjMat * f44ViewMat;
	f44InvViewProjMat = f44InvViewMat * f44InvProjMat;

	RenderContext->SetBackBufferSize(vp);

	POP_PROFILE_MARKER();
}

void GITechDemo::Draw()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	if (RenderContext->BeginFrame())
	{
		RenderScheme::Draw();
		RenderContext->EndFrame();
		RenderContext->SwapBuffers();
	}
}

// Allocate/load resources required for rendering the scene
void GITechDemo::AllocateRenderResources()
{
}
