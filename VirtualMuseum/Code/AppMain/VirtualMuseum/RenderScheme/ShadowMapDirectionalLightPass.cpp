/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   ShadowMapDirectionalLightPass.cpp
 *      Author: Bogdan Iftode
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
#include "SceneGeometryPass.h"
using namespace VirtualMuseumApp;

#include "AppResources.h"
#include "PBRMaterialTestPass.h"

namespace VirtualMuseumApp
{
    // The vertices corresponding to DX9's clip space cuboid
    // used when partitioning the view frustum for CSM
    const Vec4f cuboidVerts[] =
    {
        Vec4f(-1.f,  1.f,   1.f,    1.f),
        Vec4f( 1.f,  1.f,   1.f,    1.f),
        Vec4f(-1.f, -1.f,   1.f,    1.f),
        Vec4f( 1.f, -1.f,   1.f,    1.f),
        Vec4f(-1.f,  1.f,   0.f,    1.f),
        Vec4f( 1.f,  1.f,   0.f,    1.f),
        Vec4f(-1.f, -1.f,   0.f,    1.f),
        Vec4f( 1.f, -1.f,   0.f,    1.f)
    };
    ////////////////////////////////////

    extern SceneGeometryPass SCENE_GEOMETRY_PASS;
}

ShadowMapDirectionalLightPass::ShadowMapDirectionalLightPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

ShadowMapDirectionalLightPass::~ShadowMapDirectionalLightPass()
{}

void ShadowMapDirectionalLightPass::UpdateSceneAABB()
{
    // Calculate the scene's AABB
    // This will be used later when calculating the cascade bounds for the CSM
    RenderConfig::Scene::WorldSpaceAABB.mMin = Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
    RenderConfig::Scene::WorldSpaceAABB.mMax = Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (unsigned int mesh = 0; mesh < SponzaScene.GetModel()->arrMesh.size(); mesh++)
    {
        const VertexBuffer* const vb = SponzaScene.GetModel()->arrMesh[mesh]->pVertexBuffer;
        for (unsigned int vert = 0; vert < vb->GetElementCount(); vert++)
        {
            Vec3f vertPos = vb->Position<Vec3f>(vert);

            if (vertPos[0] < RenderConfig::Scene::WorldSpaceAABB.mMin[0])
            {
                RenderConfig::Scene::WorldSpaceAABB.mMin[0] = vertPos[0];
            }
            if (vertPos[1] < RenderConfig::Scene::WorldSpaceAABB.mMin[1])
            {
                RenderConfig::Scene::WorldSpaceAABB.mMin[1] = vertPos[1];
            }
            if (vertPos[2] < RenderConfig::Scene::WorldSpaceAABB.mMin[2])
            {
                RenderConfig::Scene::WorldSpaceAABB.mMin[2] = vertPos[2];
            }

            if (vertPos[0] > RenderConfig::Scene::WorldSpaceAABB.mMax[0])
            {
                RenderConfig::Scene::WorldSpaceAABB.mMax[0] = vertPos[0];
            }
            if (vertPos[1] > RenderConfig::Scene::WorldSpaceAABB.mMax[1])
            {
                RenderConfig::Scene::WorldSpaceAABB.mMax[1] = vertPos[1];
            }
            if (vertPos[2] > RenderConfig::Scene::WorldSpaceAABB.mMax[2])
            {
                RenderConfig::Scene::WorldSpaceAABB.mMax[2] = vertPos[2];
            }
        }
    }

    RenderConfig::Scene::WorldSpaceAABB.setInitialized();
}

void ShadowMapDirectionalLightPass::Update(const float fDeltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return;

    ResourceMgr->GetTexture(ShadowMapDir.GetRenderTarget()->GetDepthBuffer())->SetAddressingMode(SAM_BORDER);
    ResourceMgr->GetTexture(ShadowMapDir.GetRenderTarget()->GetDepthBuffer())->SetBorderColor(Vec4f(1.f, 1.f, 1.f, 1.f));

    // Calculate directional light camera view matrix
    Vec3f zAxis = makeNormal(RenderConfig::DirectionalLight::LightDir);
    // Use the previous frame's up vector to avoid
    // the camera making sudden jumps when rolling over
    static Vec3f upVec = abs(zAxis[1]) == 1.f ? Vec3f(0.f, 0.f, 1.f) : Vec3f(0.f, 1.f, 0.f);
    Vec3f xAxis = makeNormal(makeCross(upVec, zAxis));
    Vec3f yAxis = makeCross(zAxis, xAxis);
    upVec = yAxis;
    HLSL::FrameParams->DirectionalLightViewMat.set(
        xAxis[0], xAxis[1], xAxis[2], 0.f,
        yAxis[0], yAxis[1], yAxis[2], 0.f,
        zAxis[0], zAxis[1], zAxis[2], 0.f,
        0.f, 0.f, 0.f, 1.f
        );
    HLSL::FrameParams->LightWorldViewMat = HLSL::FrameParams->DirectionalLightViewMat * HLSL::FrameParams->WorldMat;
    invertFull(HLSL::FrameParams->InvLightViewMat, HLSL::FrameParams->DirectionalLightViewMat);
    HLSL::FrameParams->ScreenToLightViewMat = HLSL::FrameParams->DirectionalLightViewMat * HLSL::FrameParams->InvViewProjMat;

    // Calculate the projection matrices for all shadow map cascades
    // Start with converting the scene AABB to a light view space OBB
    // and then calculating its light view space AABB
    Vec3f aabbVerts[8];
    aabbVerts[0] = RenderConfig::Scene::WorldSpaceAABB.getMin();
    aabbVerts[1] = Vec3f(RenderConfig::Scene::WorldSpaceAABB.getMin()[0], RenderConfig::Scene::WorldSpaceAABB.getMin()[1], RenderConfig::Scene::WorldSpaceAABB.getMax()[2]);
    aabbVerts[2] = Vec3f(RenderConfig::Scene::WorldSpaceAABB.getMax()[0], RenderConfig::Scene::WorldSpaceAABB.getMin()[1], RenderConfig::Scene::WorldSpaceAABB.getMax()[2]);
    aabbVerts[3] = Vec3f(RenderConfig::Scene::WorldSpaceAABB.getMax()[0], RenderConfig::Scene::WorldSpaceAABB.getMin()[1], RenderConfig::Scene::WorldSpaceAABB.getMin()[2]);
    aabbVerts[4] = Vec3f(RenderConfig::Scene::WorldSpaceAABB.getMin()[0], RenderConfig::Scene::WorldSpaceAABB.getMax()[1], RenderConfig::Scene::WorldSpaceAABB.getMin()[2]);
    aabbVerts[5] = Vec3f(RenderConfig::Scene::WorldSpaceAABB.getMin()[0], RenderConfig::Scene::WorldSpaceAABB.getMax()[1], RenderConfig::Scene::WorldSpaceAABB.getMax()[2]);
    aabbVerts[6] = Vec3f(RenderConfig::Scene::WorldSpaceAABB.getMax()[0], RenderConfig::Scene::WorldSpaceAABB.getMax()[1], RenderConfig::Scene::WorldSpaceAABB.getMin()[2]);
    aabbVerts[7] = RenderConfig::Scene::WorldSpaceAABB.getMax();
    // This is the scene AABB in light view space (which is actually the view frustum of the
    // directional light camera) aproximated from the scene's light view space OBB
    RenderConfig::Scene::LightSpaceAABB = AABoxf(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX), Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));
    for (unsigned int i = 0; i < 8; i++)
    {
        // For each AABB vertex, calculate the corresponding light view space OBB vertex
        aabbVerts[i] = HLSL::FrameParams->LightWorldViewMat * aabbVerts[i];

        // Calculate the light view space AABB using the minimum and maximum values
        // on each axis of the light view space OBB
        if (aabbVerts[i][0] < RenderConfig::Scene::LightSpaceAABB.getMin()[0])
            RenderConfig::Scene::LightSpaceAABB.mMin[0] = aabbVerts[i][0];
        if (aabbVerts[i][1] < RenderConfig::Scene::LightSpaceAABB.getMin()[1])
            RenderConfig::Scene::LightSpaceAABB.mMin[1] = aabbVerts[i][1];
        if (aabbVerts[i][2] < RenderConfig::Scene::LightSpaceAABB.getMin()[2])
            RenderConfig::Scene::LightSpaceAABB.mMin[2] = aabbVerts[i][2];

        if (aabbVerts[i][0] > RenderConfig::Scene::LightSpaceAABB.getMax()[0])
            RenderConfig::Scene::LightSpaceAABB.mMax[0] = aabbVerts[i][0];
        if (aabbVerts[i][1] > RenderConfig::Scene::LightSpaceAABB.getMax()[1])
            RenderConfig::Scene::LightSpaceAABB.mMax[1] = aabbVerts[i][1];
        if (aabbVerts[i][2] > RenderConfig::Scene::LightSpaceAABB.getMax()[2])
            RenderConfig::Scene::LightSpaceAABB.mMax[2] = aabbVerts[i][2];
    }

    // Calculate each cascade properties
    for (unsigned int cascade = 0; cascade < HLSL::CSM::CascadeCount; cascade++)
    {
        // This is the part of the viewer's view frustum corresponding to the view frustum of the current cascade
        AABoxf ViewFrustumPartitionLightSpaceAABB(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX), Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));

        // Partition the viewer's view frustum (the can be viewed as Z slices into the view frustum)
        // This formula is derived from Nvidia's paper on Cascaded Shadow Maps:
        // http://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
        Vec4f partitionNear(0.f, 0.f, 0.f, 1.f);
        Vec4f partitionFar(0.f, 0.f, 0.f, 1.f);

        Math::lerp(partitionNear[2],
            RenderConfig::CascadedShadowMaps::SplitFactor,
            RenderConfig::Camera::ZNear + ((float)cascade / HLSL::CSM::CascadeCount)*(RenderConfig::CascadedShadowMaps::MaxViewDepth - (float)HLSL::PostProcessingParams->ZNear),
            RenderConfig::Camera::ZNear * powf(RenderConfig::CascadedShadowMaps::MaxViewDepth / (float)HLSL::PostProcessingParams->ZNear, (float)cascade / HLSL::CSM::CascadeCount)
            );

        Math::lerp(partitionFar[2],
            RenderConfig::CascadedShadowMaps::SplitFactor,
            RenderConfig::Camera::ZNear + (((float)cascade + 1.f) / HLSL::CSM::CascadeCount)*(RenderConfig::CascadedShadowMaps::MaxViewDepth - (float)HLSL::PostProcessingParams->ZNear),
            RenderConfig::Camera::ZNear * powf(RenderConfig::CascadedShadowMaps::MaxViewDepth / (float)HLSL::PostProcessingParams->ZNear, ((float)cascade + 1.f) / HLSL::CSM::CascadeCount)
            );

        // Calculate the partition's depth in projective space (viewer camera, i.e. perspective projection)
        partitionNear = HLSL::FrameParams->ProjMat * partitionNear;
        partitionNear /= partitionNear[3]; // w-divide
        partitionFar = HLSL::FrameParams->ProjMat * partitionFar;
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
            Vec4f viewFrustumVertPreW = HLSL::FrameParams->InvViewProjMat * partitionVert;
            Vec4f viewFrustumVertPostW = viewFrustumVertPreW / viewFrustumVertPreW[3];
            Vec3f wsFrustumVert = Vec3f(viewFrustumVertPostW[0], viewFrustumVertPostW[1], viewFrustumVertPostW[2]);
            Vec3f lsFrustumVerts = HLSL::FrameParams->DirectionalLightViewMat * wsFrustumVert;

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
        const unsigned int cascadesPerRow = (unsigned int)Math::ceil(Math::sqrt((float)HLSL::CSM::CascadeCount));
        const unsigned int cascadeSize = RenderConfig::CascadedShadowMaps::ShadowMapSize[0] / cascadesPerRow;
        float pcfScale = (float)RenderConfig::CascadedShadowMaps::PCFMaxSampleCount * 0.5f * sqrt(2.f) / (float)cascadeSize;
        Vec3f aabbDiag = ViewFrustumPartitionLightSpaceAABB.mMax - ViewFrustumPartitionLightSpaceAABB.mMin;
        Vec2f offsetForPCF = Vec2f(aabbDiag[0], aabbDiag[1]) * pcfScale;

        // Snap the ortographic projection to texel-sized increments in order to prevent shadow edges from jittering.
        // However, because we're tightly fitting the cascade around the view frustum, jittering will still be
        // present when rotating the camera, but not when zooming or strafing.
        Vec2f worldUnitsPerTexel = Vec2f(
            ViewFrustumPartitionLightSpaceAABB.mMax[0] - ViewFrustumPartitionLightSpaceAABB.mMin[0] + 2.f * offsetForPCF[0],
            ViewFrustumPartitionLightSpaceAABB.mMax[1] - ViewFrustumPartitionLightSpaceAABB.mMin[1] + 2.f * offsetForPCF[1]) /
            Math::floor((float)RenderConfig::CascadedShadowMaps::ShadowMapSize[0] / Math::ceil(Math::sqrt((float)HLSL::CSM::CascadeCount))/*cascades per row*/);

        // Calculate the projection matrix for the current shadow map cascade
        RenderContext->CreateOrthographicMatrix(
            HLSL::CSMParams->CascadeProjMat[cascade],
            Math::floor((ViewFrustumPartitionLightSpaceAABB.mMin[0] - offsetForPCF[0]) / worldUnitsPerTexel[0]) * worldUnitsPerTexel[0],
            Math::ceil((ViewFrustumPartitionLightSpaceAABB.mMax[1]  + offsetForPCF[1]) / worldUnitsPerTexel[1]) * worldUnitsPerTexel[1],
            Math::ceil((ViewFrustumPartitionLightSpaceAABB.mMax[0]  + offsetForPCF[0]) / worldUnitsPerTexel[0]) * worldUnitsPerTexel[0],
            Math::floor((ViewFrustumPartitionLightSpaceAABB.mMin[1] - offsetForPCF[1]) / worldUnitsPerTexel[1]) * worldUnitsPerTexel[1],
            RenderConfig::Scene::LightSpaceAABB.mMin[2], RenderConfig::Scene::LightSpaceAABB.mMax[2]);

        HLSL::CSMParams->CascadeBlendSize = RenderConfig::CascadedShadowMaps::CascadeBlendSize;

        // Store the light space coordinates of the bounds of the current shadow map cascade
        HLSL::CSMParams->CascadeBoundsMin[cascade] = Vec2f(ViewFrustumPartitionLightSpaceAABB.mMin[0], ViewFrustumPartitionLightSpaceAABB.mMin[1]);
        HLSL::CSMParams->CascadeBoundsMax[cascade] = Vec2f(ViewFrustumPartitionLightSpaceAABB.mMax[0], ViewFrustumPartitionLightSpaceAABB.mMax[1]);

        // Calculate the current shadow map cascade's corresponding composite matrices
        HLSL::FrameParams->DirectionalLightViewProjMat[cascade] = HLSL::CSMParams->CascadeProjMat[cascade] * HLSL::FrameParams->DirectionalLightViewMat;
        HLSL::FrameParams->DirectionalLightWorldViewProjMat[cascade] = HLSL::FrameParams->DirectionalLightViewProjMat[cascade] * HLSL::FrameParams->WorldMat;
    }
}

void ShadowMapDirectionalLightPass::Draw()
{
    if (!RenderConfig::DirectionalLight::Enabled && !RenderConfig::DirectionalLightVolume::Enabled)
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

    assert(RenderConfig::CascadedShadowMaps::ShadowMapSize[0] == RenderConfig::CascadedShadowMaps::ShadowMapSize[1]);
    const unsigned int cascadesPerRow = (unsigned int)Math::ceil(Math::sqrt((float)HLSL::CSM::CascadeCount));
    const unsigned int cascadeSize = RenderConfig::CascadedShadowMaps::ShadowMapSize[0] / cascadesPerRow;
    for (unsigned int cascade = 0; cascade < HLSL::CSM::CascadeCount; cascade++)
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

        RenderContext->GetRenderStateManager()->SetDepthBias(RenderConfig::CascadedShadowMaps::DepthBias[cascade]);
        RenderContext->GetRenderStateManager()->SetSlopeScaledDepthBias(RenderConfig::CascadedShadowMaps::SlopeScaledDepthBias[cascade]);

        HLSL::DepthPassParams->WorldViewProjMat = HLSL::FrameParams->DirectionalLightWorldViewProjMat[cascade];

        // Normally, you would only render meshes whose AABB/OBB intersect with the cascade's
        // view frustum, but we don't have a big enough scene to care at the moment
        //SCENE_GEOMETRY_PASS.DrawModel(SponzaScene, SceneGeometryPass::DM_DEPTH_ONLY);
        SCENE_GEOMETRY_PASS.DrawAllDoors(SceneGeometryPass::DrawMode::SHADOW, cascade);

        const vector<RenderResource*>& arrRenderResourceList = RenderResource::GetResourceList();
        const unsigned int pbrMaterialCount = RenderResource::GetResourceCountByType(RenderResource::RES_PBR_MATERIAL);

        for (unsigned int resIdx = 0, pbrMatIdx = 0; resIdx < arrRenderResourceList.size(); resIdx++)
        {
            if (arrRenderResourceList[resIdx] && arrRenderResourceList[resIdx]->GetResourceType() == RenderResource::RES_PBR_MATERIAL)
            {
                const PBRMaterial* const pbrMaterial = (PBRMaterial*)arrRenderResourceList[resIdx];

                PUSH_PROFILE_MARKER(pbrMaterial->GetDesc());

                HLSL::DepthPassParams->WorldViewProjMat = HLSL::FrameParams->DirectionalLightViewProjMat[cascade] * PBRMaterialTestPass::CalculateWorldMatrixForSphereIdx(pbrMatIdx++, pbrMaterialCount);

                DepthPassShader.Enable(); // Set the shader again in order to update f44WorldViewProjMat

                // It should have only one mesh, but in case we ever change that...
                for (unsigned int mesh = 0; mesh < SphereModel.GetModel()->arrMesh.size(); mesh++)
                    RenderContext->DrawVertexBuffer(SphereModel.GetModel()->arrMesh[mesh]->pVertexBuffer);

                DepthPassShader.Disable();

                POP_PROFILE_MARKER();
            }
        }

        POP_PROFILE_MARKER();
    }

    ShadowMapDir.Disable();

    RenderContext->GetRenderStateManager()->SetDepthBias(0.f);
    RenderContext->GetRenderStateManager()->SetSlopeScaledDepthBias(0.f);

    RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
    RenderContext->GetRenderStateManager()->SetScissorEnabled(scissorEnabled);
}

void ShadowMapDirectionalLightPass::AllocateResources()
{
    // RenderPass::AllocateResources() is called from multiple threads along with the
    // various RenderResource::Init() calls. As such, any RenderResource type that is
    // being modified here has to be locked for modification to avoid race conditions.
    SponzaScene.LockRes();
    UpdateSceneAABB();
    SponzaScene.UnlockRes();
}

void ShadowMapDirectionalLightPass::ReleaseResources()
{
}
