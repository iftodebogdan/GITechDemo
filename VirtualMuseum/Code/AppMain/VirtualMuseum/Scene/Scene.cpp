/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Scene.cpp
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

#include <gmtl/gmtl.h>
using namespace gmtl;

#include <RenderState.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "Scene.h"

#include "RenderResource.h"
#include "AppResources.h"
#include "Audio.h"
#include "VirtualMuseum.h"
using namespace VirtualMuseumApp;

namespace VirtualMuseumApp
{
    class Door : public Scene::Actor
    {
    public:
        Door();
        ~Door();

        void Update(const float deltaTime);
        void Draw(DrawMode drawMode, unsigned int cascade = ~0u);

    protected:
        void DrawModel(Model* model, DrawMode drawMode, Matrix44f* worldMat = nullptr,
            Matrix44f* viewMat = nullptr, Matrix44f* projMat = nullptr);
        void DrawDoor(Model* model, DrawMode drawMode, Vec3f pos, float rotDeg, float openDoor, unsigned int cascade = ~0u);

        Model* m_pModel;
        Audio::SoundSource* m_pOpenSound;
        float m_fDoorAnimation;
    };
}

Door::Door()
    : Actor()
    , m_pModel(nullptr)
    , m_pOpenSound(nullptr)
    , m_fDoorAnimation(0.f)
{
    m_pModel = &DoorModel;
    m_pOpenSound = Audio::GetInstance()->CreateSoundSource();
    m_pOpenSound->SetSoundFile("sounds/open_door.wav");
    m_pOpenSound->Play(true);
}

Door::~Door()
{
    Audio::GetInstance()->RemoveSoundSource(m_pOpenSound);
}

void Door::Update(const float deltaTime)
{
    m_fDoorAnimation += deltaTime;
    m_vPosition[0] = 100.f * sin(m_fDoorAnimation);
    Vec4f position = m_vPosition;
    position[3] = 1.f;
    position = HLSL::BRDFParams->ViewMat * position;
    position /= 20.f;
    m_pOpenSound->SetPosition(Vec3f(position[0], position[1], -position[2]));
}

void Door::Draw(DrawMode drawMode, unsigned int cascade)
{
    assert(m_pModel);
    if (m_pModel)
    {
        DrawDoor(m_pModel, drawMode, m_vPosition, m_fOrientation, (gmtl::Math::sin(m_fDoorAnimation) + 1.f) * 0.25f, cascade);
    }
}

void Door::DrawModel(Model* model, DrawMode drawMode, Matrix44f* worldMat, Matrix44f* viewMat, Matrix44f* projMat)
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

        for (unsigned int mesh = 0; mesh < model->GetModel()->arrMesh.size(); mesh++)
        {
            if (overrideTransformMatrices)
            {
                HLSL::DepthPassParams->WorldViewProjMat = projMat[mesh] * viewMat[mesh] * worldMat[mesh];
                DepthPassShader.CommitShaderInputs();
            }

            PUSH_PROFILE_MARKER(model->GetModel()->arrMaterial[model->GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
            RenderContext->DrawVertexBuffer(model->GetModel()->arrMesh[mesh]->pVertexBuffer);
            POP_PROFILE_MARKER();
        }

        DepthPassShader.Disable();

        break;
    }

    case DrawMode::DEPTH_ONLY_ALPHA_TEST:
    {
        DepthPassShader.Enable();

        for (unsigned int mesh = 0; mesh < model->GetModel()->arrMesh.size(); mesh++)
        {
            const unsigned int matIdx = model->GetModel()->arrMesh[mesh]->nMaterialIdx;
            const s3dSampler2D diffuseTexIdx = model->GetTexture(Synesthesia3D::Model::TextureDesc::TT_DIFFUSE, matIdx);
            Synesthesia3D::Texture* const diffuseTex = ResMgr->GetTexture(diffuseTexIdx);
            const s3dByte* const texDiffuseData = diffuseTex->GetMipData();

            assert(diffuseTex->GetPixelFormat() == PF_X8R8G8B8 || diffuseTex->GetPixelFormat() == PF_A8R8G8B8);

            if (model->GetModel()->arrMaterial[matIdx]->fOpacity >= 1.f)
            {
                if (overrideTransformMatrices)
                {
                    HLSL::DepthPassParams->WorldViewProjMat = projMat[mesh] * viewMat[mesh] * worldMat[mesh];
                    DepthPassShader.CommitShaderInputs();
                }

                PUSH_PROFILE_MARKER(model->GetModel()->arrMaterial[model->GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
                RenderContext->DrawVertexBuffer(model->GetModel()->arrMesh[mesh]->pVertexBuffer);
                POP_PROFILE_MARKER();
            }
        }

        DepthPassShader.Disable();

        if (RenderConfig::GBuffer::DrawAlphaTestGeometry)
        {
            DepthPassAlphaTestShader.Enable();

            for (unsigned int mesh = 0; mesh < model->GetModel()->arrMesh.size(); mesh++)
            {
                const unsigned int matIdx = model->GetModel()->arrMesh[mesh]->nMaterialIdx;
                const s3dSampler2D diffuseTexIdx = model->GetTexture(Synesthesia3D::Model::TextureDesc::TT_DIFFUSE, matIdx);
                Synesthesia3D::Texture* const diffuseTex = ResMgr->GetTexture(diffuseTexIdx);
                const s3dByte* const texDiffuseData = diffuseTex->GetMipData();

                if (model->GetModel()->arrMaterial[matIdx]->fOpacity < 1.f)
                {
                    if (overrideTransformMatrices)
                    {
                        HLSL::DepthPassAlphaTestParams->WorldViewProjMat = projMat[mesh] * viewMat[mesh] * worldMat[mesh];
                        //DepthPassAlphaTestShader.CommitShaderInputs();
                    }

                    HLSL::DepthPassAlphaTest_Diffuse = diffuseTexIdx;
                    DepthPassAlphaTestShader.CommitShaderInputs();

                    PUSH_PROFILE_MARKER(model->GetModel()->arrMaterial[model->GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
                    RenderContext->DrawVertexBuffer(model->GetModel()->arrMesh[mesh]->pVertexBuffer);
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

        for (unsigned int mesh = 0; mesh < model->GetModel()->arrMesh.size(); mesh++)
        {
            PUSH_PROFILE_MARKER(model->GetModel()->arrMaterial[model->GetModel()->arrMesh[mesh]->nMaterialIdx]->szName.c_str());

            const unsigned int diffuseTexIdx = model->GetTexture(Synesthesia3D::Model::TextureDesc::TT_DIFFUSE, model->GetModel()->arrMesh[mesh]->nMaterialIdx);
            const unsigned int normalTexIdx = model->GetTexture(Synesthesia3D::Model::TextureDesc::TT_HEIGHT, model->GetModel()->arrMesh[mesh]->nMaterialIdx);
            const unsigned int specTexIdx = model->GetTexture(Synesthesia3D::Model::TextureDesc::TT_SPECULAR, model->GetModel()->arrMesh[mesh]->nMaterialIdx);
            const unsigned int matTexIdx = model->GetTexture(Synesthesia3D::Model::TextureDesc::TT_AMBIENT, model->GetModel()->arrMesh[mesh]->nMaterialIdx);
            const unsigned int roughnessTexIdx = model->GetTexture(Synesthesia3D::Model::TextureDesc::TT_SHININESS, model->GetModel()->arrMesh[mesh]->nMaterialIdx);

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
                HLSL::GBufferGenerationParams->SpecIntensity = model->GetModel()->arrMaterial[model->GetModel()->arrMesh[mesh]->nMaterialIdx]->fShininessStrength;

                // For Cook-Torrance BRDF
                HLSL::GBufferGeneration_MatType = matTexIdx;
                HLSL::GBufferGeneration_Roughness = roughnessTexIdx;

                GBufferGenerationShader.CommitShaderInputs();

                RenderContext->DrawVertexBuffer(model->GetModel()->arrMesh[mesh]->pVertexBuffer);
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

void Door::DrawDoor(Model* model, DrawMode drawMode, Vec3f pos, float rotDeg, float openDoor, unsigned int cascade)
{
    const unsigned int meshCount = (unsigned int)model->GetModel()->arrMesh.size();
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

    DrawModel(model, drawMode, worldMat, viewMat, projMat);

    delete[] worldMat;
    delete[] viewMat;
    delete[] projMat;
}

void Scene::SetupScene()
{
    // Door 1
    m_pActors.push_back(new Door());
    m_pActors.back()->SetPosition(Vec3f(0.f, 0.f, 0.f));
    m_pActors.back()->SetOrientation(0.f);

    // Door 2
    //m_pActors.push_back(new Door());
    //m_pActors.back()->SetPosition(Vec3f(0.f, 0.f, 100.f));
    //m_pActors.back()->SetOrientation(45.f);

    // Door 3
    //m_pActors.push_back(new Door());
    //m_pActors.back()->SetPosition(Vec3f(100.f, 0.f, 0.f));
    //m_pActors.back()->SetOrientation(90.f);
}

void Scene::Update(const float deltaTime)
{
    for (unsigned int i = 0; i < m_pActors.size(); i++)
    {
        m_pActors[i]->Update(deltaTime);
    }
}

void Scene::Draw(Actor::DrawMode drawMode, unsigned int cascade)
{
    for (unsigned int i = 0; i < m_pActors.size(); i++)
    {
        m_pActors[i]->Draw(drawMode, cascade);
    }
}

Scene::~Scene()
{
    while (m_pActors.size())
    {
        delete m_pActors.back();
        m_pActors.pop_back();
    }
}

Scene::Actor::Actor()
    : m_vPosition(0.f, 0.f, 0.f)
    , m_fOrientation(0.f)
{}
