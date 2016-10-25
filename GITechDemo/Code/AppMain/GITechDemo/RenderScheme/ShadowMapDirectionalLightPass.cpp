/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	ShadowMapDirectionalLightPass.cpp
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "stdafx.h"

#include "PerlinNoise.h"

#include <Renderer.h>
#include <RenderState.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <VertexBuffer.h>
#include <VertexFormat.h>
#include <RenderTarget.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "ShadowMapDirectionalLightPass.h"
using namespace GITechDemoApp;

#include "AppResources.h"

namespace GITechDemoApp
{
	bool DEBUG_CSM_CAMERA = false;

	extern bool DIRECTIONAL_LIGHT_ENABLED;
	extern bool DIR_LIGHT_VOLUME_ENABLE;

	// Cascaded Shadow Maps (CSM) and directional light related variables
	float CASCADE_SPLIT_FACTOR = 0.7f;
	float CASCADE_MAX_VIEW_DEPTH = 3000.f;

	extern const unsigned int PCF_MAX_SAMPLE_COUNT = 16;
	const unsigned int NUM_CASCADES = 4;

	extern const Vec<unsigned int, 2> SHADOW_MAP_SIZE = Vec<unsigned int, 2>(4096, 4096);

	float DEPTH_BIAS[NUM_CASCADES]				= {	0.002f,		0.002f,		0.0015f,	0.001f	};
	float SLOPE_SCALED_DEPTH_BIAS[NUM_CASCADES]	= {	2.f,		2.5f,		2.5f,		1.5f	};

	AABoxf SceneAABB;
	AABoxf SceneLightSpaceAABB;

	// The vertices corresponding to DX9's clip space cuboid
	// used when partitioning the view frustum for CSM
	const Vec4f cuboidVerts[] =
	{
		Vec4f(-1.f,	 1.f,	1.f,	1.f),
		Vec4f( 1.f,	 1.f,	1.f,	1.f),
		Vec4f(-1.f,	-1.f,	1.f,	1.f),
		Vec4f( 1.f,	-1.f,	1.f,	1.f),
		Vec4f(-1.f,	 1.f,	0.f,	1.f),
		Vec4f( 1.f,	 1.f,	0.f,	1.f),
		Vec4f(-1.f,	-1.f,	0.f,	1.f),
		Vec4f( 1.f,	-1.f,	0.f,	1.f)
	};
	////////////////////////////////////
}

ShadowMapDirectionalLightPass::ShadowMapDirectionalLightPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
{
	f44CascadeProjMat = new Matrix44f[NUM_CASCADES];
	f44LightViewProjMat = new Matrix44f[NUM_CASCADES];
	f44LightWorldViewProjMat = new Matrix44f[NUM_CASCADES];

	f2CascadeBoundsMin = new Vec2f[NUM_CASCADES];
	f2CascadeBoundsMax = new Vec2f[NUM_CASCADES];
}

ShadowMapDirectionalLightPass::~ShadowMapDirectionalLightPass()
{
	delete[] f44CascadeProjMat;
	delete[] f44LightViewProjMat;
	delete[] f44LightWorldViewProjMat;

	delete[] f2CascadeBoundsMin;
	delete[] f2CascadeBoundsMax;
}

void ShadowMapDirectionalLightPass::UpdateSceneAABB()
{
	// Calculate the scene's AABB
	// This will be used later when calculating the cascade bounds for the CSM
	SceneAABB.mMin = Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
	SceneAABB.mMax = Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
	{
		const VertexBuffer* const vb = SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer;
		for (unsigned int vert = 0; vert < vb->GetElementCount(); vert++)
		{
			Vec3f vertPos = vb->Position<Vec3f>(vert);

			if (vertPos[0] < SceneAABB.mMin[0])
			{
				SceneAABB.mMin[0] = vertPos[0];
			}
			if (vertPos[1] < SceneAABB.mMin[1])
			{
				SceneAABB.mMin[1] = vertPos[1];
			}
			if (vertPos[2] < SceneAABB.mMin[2])
			{
				SceneAABB.mMin[2] = vertPos[2];
			}

			if (vertPos[0] > SceneAABB.mMax[0])
			{
				SceneAABB.mMax[0] = vertPos[0];
			}
			if (vertPos[1] > SceneAABB.mMax[1])
			{
				SceneAABB.mMax[1] = vertPos[1];
			}
			if (vertPos[2] > SceneAABB.mMax[2])
			{
				SceneAABB.mMax[2] = vertPos[2];
			}
		}
	}

	SceneAABB.setInitialized();
}

void ShadowMapDirectionalLightPass::Update(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	if (!SceneAABB.isInitialized())
		UpdateSceneAABB();

	ResourceMgr->GetTexture(ShadowMapDir.GetRenderTarget()->GetDepthBuffer())->SetAddressingMode(SAM_BORDER);
	ResourceMgr->GetTexture(ShadowMapDir.GetRenderTarget()->GetDepthBuffer())->SetBorderColor(Vec4f(1.f, 1.f, 1.f, 1.f));

	// Calculate directional light camera view matrix
	Vec3f zAxis = makeNormal((Vec3f&)f3LightDir);
	// Use the previous frame's up vector to avoid
	// the camera making sudden jumps when rolling over
	static Vec3f upVec = abs(zAxis[1]) == 1.f ? Vec3f(0.f, 0.f, 1.f) : Vec3f(0.f, 1.f, 0.f);
	Vec3f xAxis = makeNormal(makeCross(upVec, zAxis));
	Vec3f yAxis = makeCross(zAxis, xAxis);
	upVec = yAxis;
	((Matrix44f&)f44LightViewMat).set(
		xAxis[0], xAxis[1], xAxis[2], 0.f,
		yAxis[0], yAxis[1], yAxis[2], 0.f,
		zAxis[0], zAxis[1], zAxis[2], 0.f,
		0.f, 0.f, 0.f, 1.f
		);
	f44LightWorldViewMat = f44LightViewMat * f44WorldMat;
	invertFull((Matrix44f&)f44InvLightViewMat, (Matrix44f&)f44LightViewMat);
	f44ScreenToLightViewMat = f44LightViewMat * f44InvViewProjMat;

	// Calculate the projection matrices for all shadow map cascades
	// Start with converting the scene AABB to a light view space OBB
	// and then calculating its light view space AABB
	Vec3f aabbVerts[8];
	aabbVerts[0] = SceneAABB.getMin();
	aabbVerts[1] = Vec3f(SceneAABB.getMin()[0], SceneAABB.getMin()[1], SceneAABB.getMax()[2]);
	aabbVerts[2] = Vec3f(SceneAABB.getMax()[0], SceneAABB.getMin()[1], SceneAABB.getMax()[2]);
	aabbVerts[3] = Vec3f(SceneAABB.getMax()[0], SceneAABB.getMin()[1], SceneAABB.getMin()[2]);
	aabbVerts[4] = Vec3f(SceneAABB.getMin()[0], SceneAABB.getMax()[1], SceneAABB.getMin()[2]);
	aabbVerts[5] = Vec3f(SceneAABB.getMin()[0], SceneAABB.getMax()[1], SceneAABB.getMax()[2]);
	aabbVerts[6] = Vec3f(SceneAABB.getMax()[0], SceneAABB.getMax()[1], SceneAABB.getMin()[2]);
	aabbVerts[7] = SceneAABB.getMax();
	// This is the scene AABB in light view space (which is actually the view frustum of the
	// directional light camera) aproximated from the scene's light view space OBB
	SceneLightSpaceAABB = AABoxf(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX), Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	for (unsigned int i = 0; i < 8; i++)
	{
		// For each AABB vertex, calculate the corresponding light view space OBB vertex
		aabbVerts[i] = f44LightWorldViewMat * aabbVerts[i];

		// Calculate the light view space AABB using the minimum and maximum values
		// on each axis of the light view space OBB
		if (aabbVerts[i][0] < SceneLightSpaceAABB.getMin()[0])
			SceneLightSpaceAABB.mMin[0] = aabbVerts[i][0];
		if (aabbVerts[i][1] < SceneLightSpaceAABB.getMin()[1])
			SceneLightSpaceAABB.mMin[1] = aabbVerts[i][1];
		if (aabbVerts[i][2] < SceneLightSpaceAABB.getMin()[2])
			SceneLightSpaceAABB.mMin[2] = aabbVerts[i][2];

		if (aabbVerts[i][0] > SceneLightSpaceAABB.getMax()[0])
			SceneLightSpaceAABB.mMax[0] = aabbVerts[i][0];
		if (aabbVerts[i][1] > SceneLightSpaceAABB.getMax()[1])
			SceneLightSpaceAABB.mMax[1] = aabbVerts[i][1];
		if (aabbVerts[i][2] > SceneLightSpaceAABB.getMax()[2])
			SceneLightSpaceAABB.mMax[2] = aabbVerts[i][2];
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
			fZNear + ((float)cascade / NUM_CASCADES)*(CASCADE_MAX_VIEW_DEPTH - fZNear),
			fZNear * powf(CASCADE_MAX_VIEW_DEPTH / fZNear, (float)cascade / NUM_CASCADES)
			);

		Math::lerp(partitionFar[2],
			CASCADE_SPLIT_FACTOR,
			fZNear + (((float)cascade + 1.f) / NUM_CASCADES)*(CASCADE_MAX_VIEW_DEPTH - fZNear),
			fZNear * powf(CASCADE_MAX_VIEW_DEPTH / fZNear, ((float)cascade + 1.f) / NUM_CASCADES)
			);

		// Calculate the partition's depth in projective space (viewer camera, i.e. perspective projection)
		partitionNear = f44ProjMat * partitionNear;
		partitionNear /= partitionNear[3]; // w-divide
		partitionFar = f44ProjMat * partitionFar;
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
			Vec4f viewFrustumVertPreW = f44InvViewProjMat * partitionVert;
			Vec4f viewFrustumVertPostW = viewFrustumVertPreW / viewFrustumVertPreW[3];
			Vec3f wsFrustumVert = Vec3f(viewFrustumVertPostW[0], viewFrustumVertPostW[1], viewFrustumVertPostW[2]);
			Vec3f lsFrustumVerts = f44LightViewMat * wsFrustumVert;

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

		// Enlarge the light view frustum in order to avoid PCF shadow sampling from
		// sampling outside of a shadow map cascade
		const unsigned int cascadesPerRow = (unsigned int)Math::ceil(Math::sqrt((float)NUM_CASCADES));
		const unsigned int cascadeSize = SHADOW_MAP_SIZE[0] / cascadesPerRow;
		float pcfScale = (float)PCF_MAX_SAMPLE_COUNT * 0.5f * sqrt(2.f) / (float)cascadeSize;
		Vec3f aabbDiag = ViewFrustumPartitionLightSpaceAABB.mMax - ViewFrustumPartitionLightSpaceAABB.mMin;
		Vec2f offsetForPCF = Vec2f(aabbDiag[0], aabbDiag[1]) * pcfScale;

		// Snap the ortographic projection to texel-sized increments in order to prevent shadow edges from jittering.
		// However, because we're tightly fitting the cascade around the view frustum, jittering will still be
		// present when rotating the camera, but not when zooming or strafing.
		Vec2f worldUnitsPerTexel = Vec2f(
			ViewFrustumPartitionLightSpaceAABB.mMax[0] - ViewFrustumPartitionLightSpaceAABB.mMin[0] + 2.f * offsetForPCF[0],
			ViewFrustumPartitionLightSpaceAABB.mMax[1] - ViewFrustumPartitionLightSpaceAABB.mMin[1] + 2.f * offsetForPCF[1]) /
			Math::floor((float)SHADOW_MAP_SIZE[0] / Math::ceil(Math::sqrt((float)NUM_CASCADES))/*cascades per row*/);

		// Calculate the projection matrix for the current shadow map cascade
		RenderContext->CreateOrthographicMatrix(
			f44CascadeProjMat[cascade],
			Math::floor((ViewFrustumPartitionLightSpaceAABB.mMin[0]	- offsetForPCF[0]) / worldUnitsPerTexel[0]) * worldUnitsPerTexel[0],
			Math::ceil((ViewFrustumPartitionLightSpaceAABB.mMax[1]	+ offsetForPCF[1]) / worldUnitsPerTexel[1]) * worldUnitsPerTexel[1],
			Math::ceil((ViewFrustumPartitionLightSpaceAABB.mMax[0]	+ offsetForPCF[0]) / worldUnitsPerTexel[0]) * worldUnitsPerTexel[0],
			Math::floor((ViewFrustumPartitionLightSpaceAABB.mMin[1]	- offsetForPCF[1]) / worldUnitsPerTexel[1]) * worldUnitsPerTexel[1],
			SceneLightSpaceAABB.mMin[2], SceneLightSpaceAABB.mMax[2]);

		// Store the light space coordinates of the bounds of the current shadow map cascade
		f2CascadeBoundsMin[cascade] = Vec2f(ViewFrustumPartitionLightSpaceAABB.mMin[0], ViewFrustumPartitionLightSpaceAABB.mMin[1]);
		f2CascadeBoundsMax[cascade] = Vec2f(ViewFrustumPartitionLightSpaceAABB.mMax[0], ViewFrustumPartitionLightSpaceAABB.mMax[1]);

		// Calculate the current shadow map cascade's corresponding composite matrices
		f44LightViewProjMat[cascade] = f44CascadeProjMat[cascade] * f44LightViewMat;
		f44LightWorldViewProjMat[cascade] = f44LightViewProjMat[cascade] * f44WorldMat;
	}

	// Debug sun camera
	if (DEBUG_CSM_CAMERA)
	{
		f44ViewMat = f44LightViewMat;
		invertFull((Matrix44f&)f44InvViewMat, (Matrix44f&)f44ViewMat);
		f44ProjMat = f44CascadeProjMat[NUM_CASCADES - 1];
		invertFull((Matrix44f&)f44InvProjMat, (Matrix44f&)f44ProjMat);
		f44WorldViewMat = f44ViewMat * f44WorldMat;
		f44ViewProjMat = f44ProjMat * f44ViewMat;
		f44InvViewProjMat = f44InvViewMat * f44InvProjMat;
		f44WorldViewProjMat = f44ProjMat * f44WorldViewMat;
		f44ScreenToLightViewMat = f44LightViewMat * f44InvViewProjMat;
	}
}

void ShadowMapDirectionalLightPass::Draw()
{
	if (!DIRECTIONAL_LIGHT_ENABLED && !DIR_LIGHT_VOLUME_ENABLE)
		return;

	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	bool red, blue, green, alpha;
	const bool scissorEnabled = RenderContext->GetRenderStateManager()->GetScissorEnabled();
	RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);

	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);
	RenderContext->GetRenderStateManager()->SetScissorEnabled(true);

	ShadowMapDir.Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	assert(SHADOW_MAP_SIZE[0] == SHADOW_MAP_SIZE[1]);
	const unsigned int cascadesPerRow = (unsigned int)Math::ceil(Math::sqrt((float)NUM_CASCADES));
	const unsigned int cascadeSize = SHADOW_MAP_SIZE[0] / cascadesPerRow;
	for (unsigned int cascade = 0; cascade < NUM_CASCADES; cascade++)
	{
#if ENABLE_PROFILE_MARKERS
		char tmpBuf[16];
		sprintf_s(tmpBuf, "Cascade %d", cascade);
#endif
		PUSH_PROFILE_MARKER(tmpBuf);

		const Vec2i size(cascadeSize, cascadeSize);
		const Vec2i offset(cascadeSize * (cascade % cascadesPerRow), cascadeSize * (cascade / cascadesPerRow));
		RenderContext->SetViewport(size, offset);
		RenderContext->GetRenderStateManager()->SetScissor(size, offset);

		RenderContext->GetRenderStateManager()->SetDepthBias(DEPTH_BIAS[cascade]);
		RenderContext->GetRenderStateManager()->SetSlopeScaledDepthBias(SLOPE_SCALED_DEPTH_BIAS[cascade]);

		f44WorldViewProjMat = f44LightWorldViewProjMat[cascade];

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
	}

	ShadowMapDir.Disable();

	RenderContext->GetRenderStateManager()->SetDepthBias(0.f);
	RenderContext->GetRenderStateManager()->SetSlopeScaledDepthBias(0.f);

	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetScissorEnabled(scissorEnabled);
}
