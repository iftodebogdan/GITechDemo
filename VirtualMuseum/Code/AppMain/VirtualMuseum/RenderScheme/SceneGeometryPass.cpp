/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   SceneGeometryPass.cpp
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

#include <Renderer.h>
#include <RenderState.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "SceneGeometryPass.h"
using namespace VirtualMuseumApp;

#include "AppResources.h"

SceneGeometryPass::SceneGeometryPass(const char* const passName, RenderPass* const parentPass)
    : RenderPass(passName, parentPass)
{}

SceneGeometryPass::~SceneGeometryPass()
{}

void SceneGeometryPass::Update(const float fDeltaTime)
{}

void SceneGeometryPass::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    ResourceManager* const ResMgr = RenderContext ? RenderContext->GetResourceManager() : nullptr;
    if (!RenderContext || !ResMgr)
        return;

    const bool zWriteEnabled = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
    const Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
    bool red, blue, green, alpha;
    RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);

    // A depth prepass is useful if we have expensive pixel shaders in our
    // G-Buffer generation process. It allows us to avoid shading pixels
    // that eventually get overwritten by other pixels that have smaller
    // depth values. We are not fill-rate bound, so the depth prepass is disabled.
    if (RenderConfig::GBuffer::ZPrepass)
    {
        PUSH_PROFILE_MARKER("Z prepass");

        RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);

        //DrawModel(SponzaScene, DrawMode::DEPTH_ONLY_ALPHA_TEST);
        DrawAllDoors(DrawMode::DEPTH_ONLY_ALPHA_TEST);

        RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
        RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
        RenderContext->GetRenderStateManager()->SetZFunc(CMP_EQUAL);

        POP_PROFILE_MARKER();

        PUSH_PROFILE_MARKER("Capture");
    }

    // A visibility test would be useful if we were CPU bound (or vertex bound).
    // However, there isn't a reason to do such an optimization for now, since the
    // scene isn't very big and we are mostly pixel bound.
    //DrawModel(SponzaScene, DrawMode::COLOR);
    DrawAllDoors(DrawMode::COLOR);

    if (RenderConfig::GBuffer::ZPrepass)
    {
        RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWriteEnabled);
        RenderContext->GetRenderStateManager()->SetZFunc(zFunc);
        POP_PROFILE_MARKER();
    }
}

void SceneGeometryPass::AllocateResources()
{

}

void SceneGeometryPass::ReleaseResources()
{

}

void SceneGeometryPass::DrawModel(Model& model, DrawMode drawMode, Matrix44f* worldMat, Matrix44f* viewMat, Matrix44f* projMat)
{
    Renderer* RenderContext = Renderer::GetInstance();
    ResourceManager* const ResMgr = RenderContext ? RenderContext->GetResourceManager() : nullptr;
    RenderState* renderStateMgr = RenderContext ? RenderContext->GetRenderStateManager() : nullptr;
    if (!RenderContext || !ResMgr || !renderStateMgr)
        return;

    const bool overrideTransformMatrices = (worldMat && viewMat && projMat);

    Cull cullMode = renderStateMgr->GetCullMode();
    renderStateMgr->SetCullMode(CULL_NONE);

    switch (drawMode)
    {
    case DrawMode::SHADOW:
    case DrawMode::DEPTH_ONLY:
    {
        DepthPassShader.Enable();

        for (unsigned int mesh = 0; mesh < model.GetModel()->arrMesh.size(); mesh++)
        {
            if (overrideTransformMatrices)
            {
                HLSL::DepthPassParams->WorldViewProjMat = projMat[mesh] * viewMat[mesh] * worldMat[mesh];
                DepthPassShader.CommitShaderInputs();
            }

            PUSH_PROFILE_MARKER(model.GetModel()->arrMaterial[model.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
            RenderContext->DrawVertexBuffer(model.GetModel()->arrMesh[mesh]->pVertexBuffer);
            POP_PROFILE_MARKER();
        }

        DepthPassShader.Disable();

        break;
    }

    case DrawMode::DEPTH_ONLY_ALPHA_TEST:
    {
        DepthPassShader.Enable();

        for (unsigned int mesh = 0; mesh < model.GetModel()->arrMesh.size(); mesh++)
        {
            const unsigned int matIdx = model.GetModel()->arrMesh[mesh]->nMaterialIdx;
            const s3dSampler2D diffuseTexIdx = model.GetTexture(Synesthesia3D::Model::TextureDesc::TT_DIFFUSE, matIdx);
            Synesthesia3D::Texture* const diffuseTex = ResMgr->GetTexture(diffuseTexIdx);
            const s3dByte* const texDiffuseData = diffuseTex->GetMipData();

            assert(diffuseTex->GetPixelFormat() == PF_X8R8G8B8 || diffuseTex->GetPixelFormat() == PF_A8R8G8B8);

            if (model.GetModel()->arrMaterial[matIdx]->fOpacity >= 1.f)
            {
                if (overrideTransformMatrices)
                {
                    HLSL::DepthPassParams->WorldViewProjMat = projMat[mesh] * viewMat[mesh] * worldMat[mesh];
                    DepthPassShader.CommitShaderInputs();
                }

                PUSH_PROFILE_MARKER(model.GetModel()->arrMaterial[model.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
                RenderContext->DrawVertexBuffer(model.GetModel()->arrMesh[mesh]->pVertexBuffer);
                POP_PROFILE_MARKER();
            }
        }

        DepthPassShader.Disable();

        if (RenderConfig::GBuffer::DrawAlphaTestGeometry)
        {
            DepthPassAlphaTestShader.Enable();

            for (unsigned int mesh = 0; mesh < model.GetModel()->arrMesh.size(); mesh++)
            {
                const unsigned int matIdx = model.GetModel()->arrMesh[mesh]->nMaterialIdx;
                const s3dSampler2D diffuseTexIdx = model.GetTexture(Synesthesia3D::Model::TextureDesc::TT_DIFFUSE, matIdx);
                Synesthesia3D::Texture* const diffuseTex = ResMgr->GetTexture(diffuseTexIdx);
                const s3dByte* const texDiffuseData = diffuseTex->GetMipData();

                if (model.GetModel()->arrMaterial[matIdx]->fOpacity < 1.f)
                {
                    if (overrideTransformMatrices)
                    {
                        HLSL::DepthPassAlphaTestParams->WorldViewProjMat = projMat[mesh] * viewMat[mesh] * worldMat[mesh];
                        //DepthPassAlphaTestShader.CommitShaderInputs();
                    }

                    HLSL::DepthPassAlphaTest_Diffuse = diffuseTexIdx;
                    DepthPassAlphaTestShader.CommitShaderInputs();

                    PUSH_PROFILE_MARKER(model.GetModel()->arrMaterial[model.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
                    RenderContext->DrawVertexBuffer(model.GetModel()->arrMesh[mesh]->pVertexBuffer);
                    POP_PROFILE_MARKER();
                }
            }

            DepthPassAlphaTestShader.Disable();
        }

        break;
    }

    case DrawMode::COLOR:
    {
        GBufferGenerationShader.Enable();

        Matrix44f backup = HLSL::GBufferGenerationParams->WorldViewProjMat;

        for (unsigned int mesh = 0; mesh < model.GetModel()->arrMesh.size(); mesh++)
        {
            PUSH_PROFILE_MARKER(model.GetModel()->arrMaterial[model.GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());

            const unsigned int diffuseTexIdx = model.GetTexture(Synesthesia3D::Model::TextureDesc::TT_DIFFUSE, model.GetModel()->arrMesh[mesh]->nMaterialIdx);
            const unsigned int normalTexIdx = model.GetTexture(Synesthesia3D::Model::TextureDesc::TT_HEIGHT, model.GetModel()->arrMesh[mesh]->nMaterialIdx);
            const unsigned int specTexIdx = model.GetTexture(Synesthesia3D::Model::TextureDesc::TT_SPECULAR, model.GetModel()->arrMesh[mesh]->nMaterialIdx);
            const unsigned int matTexIdx = model.GetTexture(Synesthesia3D::Model::TextureDesc::TT_AMBIENT, model.GetModel()->arrMesh[mesh]->nMaterialIdx);
            const unsigned int roughnessTexIdx = model.GetTexture(Synesthesia3D::Model::TextureDesc::TT_SHININESS, model.GetModel()->arrMesh[mesh]->nMaterialIdx);

            if (diffuseTexIdx != ~0u && ((matTexIdx != ~0u && roughnessTexIdx != ~0u) || RenderConfig::DirectionalLight::BRDFModel == HLSL::BRDF::BlinnPhong))
            {
                if (overrideTransformMatrices)
                {
                    HLSL::GBufferGenerationParams->WorldViewProjMat = projMat[mesh] * viewMat[mesh] * worldMat[mesh];
                    HLSL::GBufferGenerationParams->WorldViewMat = viewMat[mesh] * worldMat[mesh];
                    //GBufferGenerationShader.CommitShaderInputs();
                }

                RenderContext->GetResourceManager()->GetTexture(diffuseTexIdx)->SetAnisotropy((unsigned int)RenderConfig::GBuffer::DiffuseAnisotropy);

                HLSL::GBufferGeneration_Diffuse = diffuseTexIdx;
                HLSL::GBufferGeneration_Normal = normalTexIdx;
                HLSL::GBufferGenerationParams->HasNormalMap = (HLSL::GBufferGeneration_Normal != -1) && RenderConfig::GBuffer::UseNormalMaps;

                // For Blinn-Phong BRDF
                HLSL::GBufferGeneration_Spec = specTexIdx;
                HLSL::GBufferGenerationParams->HasSpecMap = (HLSL::GBufferGeneration_Spec != -1);
                HLSL::GBufferGenerationParams->SpecIntensity = model.GetModel()->arrMaterial[model.GetModel()->arrMesh[mesh]->nMaterialIdx]->fShininessStrength;

                // For Cook-Torrance BRDF
                HLSL::GBufferGeneration_MatType = matTexIdx;
                HLSL::GBufferGeneration_Roughness = roughnessTexIdx;

                GBufferGenerationShader.CommitShaderInputs();

                RenderContext->DrawVertexBuffer(model.GetModel()->arrMesh[mesh]->pVertexBuffer);
            }

            POP_PROFILE_MARKER();
        }

        GBufferGenerationShader.Disable();

        break;
    }

    default:
        assert(0);
    }

    renderStateMgr->SetCullMode(cullMode);
}

void SceneGeometryPass::DrawDoor(DrawMode drawMode, Vec3f pos, float rotDeg, float openDoor, unsigned int cascade)
{
    const unsigned int meshCount = (unsigned int)DoorModel.GetModel()->arrMesh.size();
    assert(meshCount >= 2); // just in case
    assert(drawMode != DrawMode::SHADOW || cascade < HLSL::CSM::CascadeCount);

    Matrix44f currViewMat = (drawMode == DrawMode::SHADOW ? HLSL::FrameParams->DirectionalLightViewMat : HLSL::BRDFParams->ViewMat);
    Matrix44f currProjMat = (drawMode == DrawMode::SHADOW ? HLSL::CSMParams->CascadeProjMat[cascade] : HLSL::FrameParams->ProjMat);

    Matrix44f* worldMat = new Matrix44f[meshCount];
    Matrix44f* viewMat = new Matrix44f[meshCount];
    Matrix44f* projMat = new Matrix44f[meshCount];

    for (unsigned int mesh = 0; mesh < meshCount; mesh++)
    {
        switch (mesh)
        {
        // Door + knob
        case 0:
        case 1:
            worldMat[mesh] =
                makeTrans(pos, Type2Type<Matrix44f>()) * // position door
                makeRot(EulerAngleXYZf(0.f, Math::deg2Rad(rotDeg), 0.f), Type2Type<Matrix44f>()) * // rotate door
                makeTrans(Vec3f(0, 0, 18.413f), Type2Type<Matrix44f>()) * // revert origin
                makeRot(EulerAngleXYZf(0.f, Math::deg2Rad(90.f * openDoor), 0.f), Type2Type<Matrix44f>()) * // rotate around hinge
                makeTrans(Vec3f(0, 0, -18.413f), Type2Type<Matrix44f>()); // set door hinge as origin
            viewMat[mesh] = currViewMat;
            projMat[mesh] = currProjMat;
            break;

        // Door frame
        default:
            worldMat[mesh] =
                makeTrans(pos, Type2Type<Matrix44f>()) * // position door
                makeRot(EulerAngleXYZf(0.f, Math::deg2Rad(rotDeg), 0.f), Type2Type<Matrix44f>()); // rotate door
            viewMat[mesh] = currViewMat;
            projMat[mesh] = currProjMat;
        }
    }

    DrawModel(DoorModel, drawMode, worldMat, viewMat, projMat);

    delete[] worldMat;
    delete[] viewMat;
    delete[] projMat;
}

void SceneGeometryPass::DrawAllDoors(DrawMode drawMode, unsigned int cascade)
{
    DrawDoor(drawMode, Vec3f(0.f, 0.f, 0.f), 0.f, 0.f, cascade);
    DrawDoor(drawMode, Vec3f(0.f, 0.f, 100.f), 45.f, 0.3f, cascade);
    DrawDoor(drawMode, Vec3f(100.f, 0.f, 0.f), 90.f, 0.6f, cascade);
}
