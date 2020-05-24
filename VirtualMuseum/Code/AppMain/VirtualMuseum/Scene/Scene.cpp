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
#include "UIPass.h"
using namespace VirtualMuseumApp;

namespace VirtualMuseumApp
{
    extern UIPass UI_PASS;

    class Door : public Scene::Actor
    {
    public:
        Door();
        ~Door();

        void Update(const float deltaTime);
        void Draw(DrawMode drawMode, unsigned int cascade = ~0u);

        void Interact();

    protected:
        void DrawModel(Model* model, DrawMode drawMode, Matrix44f* worldMat = nullptr,
            Matrix44f* viewMat = nullptr, Matrix44f* projMat = nullptr);
        void DrawDoor(Model* model, DrawMode drawMode, Vec3f pos, float rotDeg, float openDoor, unsigned int cascade = ~0u);

        Model* m_pModel;
        Audio::SoundSource* m_pOpenSound;
        float m_fDoorAnimation;
        float m_fInteractAccum;
        Vec3f m_vPlayerInitialPos;
        Vec3f m_vPlayerInitialRot;
        float m_fInitialFoV;
        float m_fInitialLight;
    };

    class Player : public Scene::Actor
    {
    public:
        Player();
        ~Player();

        void Update(const float deltaTime);
        void Draw(DrawMode drawMode, unsigned int cascade = ~0u) {}

        void Interact() {}

    protected:
        Audio::SoundSource* m_pFootstepsSound;
    };
}

Player::Player()
    : Actor()
    , m_pFootstepsSound(nullptr)
{
    m_pFootstepsSound = Audio::GetInstance()->CreateSoundSource();
    m_pFootstepsSound->SetSoundFile("sounds/footsteps.wav");
}

Player::~Player()
{
    Audio::GetInstance()->RemoveSoundSource(m_pFootstepsSound);
}

void Player::Update(const float deltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    gainput::InputMap* input = ((VirtualMuseum*)AppMain)->GetInput();
    auto actors = ((VirtualMuseum*)AppMain)->GetScene()->GetActors();

    Vec4f pos(0.f, 0.f, 0.f, 1.f);
    Vec4f lookAt(0.f, 0.f, 1.f, 0.f);
    Vec4f up(0.f, 1.f, 0.f, 0.f);

    pos = HLSL::BRDFParams->InvViewMat * pos;
    lookAt = HLSL::BRDFParams->InvViewMat * lookAt;
    up = HLSL::BRDFParams->InvViewMat * up;

    Audio::GetInstance()->SetListenerPosition(Vec3f(pos[0], pos[1], -pos[2]));
    Audio::GetInstance()->SetListenerOrientation(
        makeNormal(Vec3f(lookAt[0], lookAt[1], -lookAt[2])),
        makeNormal(Vec3f(up[0], up[1], -up[2]))
    );

    if (!((VirtualMuseum*)AppMain)->GetScene()->IsInteracting())
    {
        if (input->GetBoolIsNew(VirtualMuseum::Command::APP_CMD_FORWARD) ||
            input->GetBoolIsNew(VirtualMuseum::Command::APP_CMD_BACKWARD) ||
            input->GetBoolIsNew(VirtualMuseum::Command::APP_CMD_LEFT) ||
            input->GetBoolIsNew(VirtualMuseum::Command::APP_CMD_RIGHT))
        {
            if (m_pFootstepsSound->GetStatus() != Audio::SoundSource::PLAYING)
            {
                m_pFootstepsSound->Play(true);
            }
        }

        if (!input->GetBool(VirtualMuseum::Command::APP_CMD_FORWARD) &&
            !input->GetBool(VirtualMuseum::Command::APP_CMD_BACKWARD) &&
            !input->GetBool(VirtualMuseum::Command::APP_CMD_LEFT) &&
            !input->GetBool(VirtualMuseum::Command::APP_CMD_RIGHT))
        {
            m_pFootstepsSound->Stop();
        }
    }
    else
    {
        m_pFootstepsSound->Stop();
    }

    m_pFootstepsSound->SetPosition(Vec3f(pos[0], 0.f, -pos[2]));

    // "Collision"
    for (unsigned int i = 0; i < actors.size(); i++)
    {
        if (actors[i] != this)
        {
            Vec3f posDiff = Vec3f(pos[0], actors[i]->GetPosition()[1], pos[2]) - actors[i]->GetPosition();
            if (lengthSquared(posDiff) < 1)
            {
                normalize(posDiff);
                ((VirtualMuseum*)AppMain)->GetCamera().vPos[0] = -(actors[i]->GetPosition() + posDiff)[0];
                ((VirtualMuseum*)AppMain)->GetCamera().vPos[2] = -(actors[i]->GetPosition() + posDiff)[2];
            }
        }
    }

    // Leash
    const float leashDist = 50.f;
    if (lengthSquared(Vec3f(pos[0], 0.f, pos[2])) > leashDist * leashDist)
    {
        Vec3f leashVec = makeNormal(Vec3f(pos[0], 0.f, pos[2])) * leashDist;
        ((VirtualMuseum*)AppMain)->GetCamera().vPos[0] = -leashVec[0];
        ((VirtualMuseum*)AppMain)->GetCamera().vPos[2] = -leashVec[2];
    }

    // Interact
    if (!((VirtualMuseum*)AppMain)->GetScene()->IsInteracting())
    {
        for (unsigned int i = 0; i < actors.size(); i++)
        {
            if (actors[i] != this)
            {
                Vec4f actorLookAtWS = makeRot(EulerAngleXYZf(0.f, Math::deg2Rad(actors[i]->GetOrientation() - 90.f), 0.f), Type2Type<Matrix44f>()) * Vec4f(0.f, 0.f, 1.f, 0.f);
                Vec4f actorLookAtVS = HLSL::BRDFParams->ViewMat * actorLookAtWS;
                float lookAtDot = dot(Vec3f(0.f, 0.f, 1.f), Vec3f(actorLookAtVS[0], actorLookAtVS[1], actorLookAtVS[2]));
                float actorDot = dot(
                    Vec3f(actorLookAtWS[0], actorLookAtWS[1], actorLookAtWS[2]),
                    makeNormal(Vec3f(actors[i]->GetPosition() - Vec3f(pos[0], actors[i]->GetPosition()[1], pos[2]))));

                Vec3f posDiff = Vec3f(pos[0], actors[i]->GetPosition()[1], pos[2]) - actors[i]->GetPosition();
                if (lengthSquared(posDiff) <= 1.1f && lookAtDot > 0.5f && actorDot > 0.5f)
                {
                    Vec4f tooltipPos =
                        HLSL::FrameParams->ViewProjMat *
                        Vec4f(actors[i]->GetPosition()[0], pos[1], actors[i]->GetPosition()[2], 1.f);
                    tooltipPos = tooltipPos / tooltipPos[3];
                    if (tooltipPos[0] > -1.f && tooltipPos[0] < 1.f &&
                        tooltipPos[1] > -1.f && tooltipPos[1] < 1.f &&
                        tooltipPos[2] > 0.f && tooltipPos[2] < 1.f)
                    {
                        tooltipPos *= 0.5f;
                        tooltipPos[0] += 0.5f;
                        tooltipPos[1] += 0.5f;
                        Vec2f finalPos = Vec2f(
                            tooltipPos[0] * (float)RenderContext->GetDisplayResolution()[0],
                            (1.f - tooltipPos[1]) * (float)RenderContext->GetDisplayResolution()[1]);
                        UI_PASS.AddTooltip("Press E to interact", finalPos);

                        if (input->GetBoolIsNew(VirtualMuseum::Command::APP_CMD_INTERACT))
                        {
                            actors[i]->Interact();
                        }
                    }
                }
            }
        }
    }
}

Door::Door()
    : Actor()
    , m_pModel(nullptr)
    , m_pOpenSound(nullptr)
    , m_fDoorAnimation(0.f)
    , m_fInteractAccum(0.f)
    , m_vPlayerInitialPos(0.f, 0.f, 0.f)
    , m_vPlayerInitialRot(0.f, 0.f, 0.f)
    , m_fInitialFoV(0.f)
    , m_fInitialLight(0.f)
{
    m_pModel = &DoorModel;
    m_pOpenSound = Audio::GetInstance()->CreateSoundSource();
    m_pOpenSound->SetSoundFile("sounds/open_door.wav");
}

Door::~Door()
{
    Audio::GetInstance()->RemoveSoundSource(m_pOpenSound);
}

void Door::Update(const float deltaTime)
{
    m_pOpenSound->SetPosition(Vec3f(m_vPosition[0], m_vPosition[1], -m_vPosition[2]));

    if (IsInteracting())
    {
        m_fInteractAccum += deltaTime;

        const float lerpVal = gmtl::Math::clamp(m_fInteractAccum / 4.4f, 0.f, 1.f);
        gmtl::Math::lerp(RenderConfig::Camera::FoV, lerpVal, m_fInitialFoV, 1.f);

        if (m_fInteractAccum >= 1.4f)
        {
            m_fDoorAnimation += deltaTime / 3.f;

            const float lerpVal = gmtl::Math::clamp(Math::pow(m_fDoorAnimation, 5.f), 0.f, 1.f);
            gmtl::Math::lerp(RenderConfig::DirectionalLightVolume::MultScatterIntensity, lerpVal, m_fInitialLight, 10000.f);
        }
        else
        {
            const float lerpVal = gmtl::Math::clamp(m_fInteractAccum, 0.f, 1.f);

            // Adjust position
            Vec3f targetPos;
            Vec4f lookAt = makeRot(EulerAngleXYZf(0.f, Math::deg2Rad(GetOrientation() + 90.f), 0.f), Type2Type<Matrix44f>()) * Vec4f(0.f, 0.f, 1.f, 0.f);
            Vec3f finalPos = GetPosition() + Vec3f(lookAt[0], lookAt[1], lookAt[2]);
            targetPos[1] = m_vPlayerInitialPos[1];
            gmtl::Math::lerp(targetPos[0], lerpVal, m_vPlayerInitialPos[0], finalPos[0]);
            gmtl::Math::lerp(targetPos[2], lerpVal, m_vPlayerInitialPos[2], finalPos[2]);
            ((VirtualMuseum*)AppMain)->GetCamera().vPos = -targetPos;

            // Adjust rotation
            Vec3f targetRot;
            gmtl::Math::lerp(targetRot[0], lerpVal, m_vPlayerInitialRot[0], 0.f);
            gmtl::Math::lerp(targetRot[1], lerpVal, m_vPlayerInitialRot[1], m_fOrientation >= -90.f ? m_fOrientation - 90.f : m_fOrientation + 270.f);
            ((VirtualMuseum*)AppMain)->GetCamera().vRot = Vec3f(targetRot[0], targetRot[1], 0.f);
        }

        if (m_fDoorAnimation > 1.f)
        {
            m_fDoorAnimation = 0.f;
            m_bIsInteracting = false;
            RenderConfig::Camera::FoV = m_fInitialFoV;
            RenderConfig::DirectionalLightVolume::MultScatterIntensity = m_fInitialLight;
        }
    }
}

void Door::Interact()
{
    Actor::Interact();

    m_fInteractAccum = 0.f;
    m_fDoorAnimation = 0.f;
    m_pOpenSound->Play(false);

    m_vPlayerInitialPos = -((VirtualMuseum*)AppMain)->GetCamera().vPos;
    m_vPlayerInitialRot = ((VirtualMuseum*)AppMain)->GetCamera().vRot;
    m_fInitialFoV = RenderConfig::Camera::FoV;
    m_fInitialLight = RenderConfig::DirectionalLightVolume::MultScatterIntensity;
}

void Door::Draw(DrawMode drawMode, unsigned int cascade)
{
    assert(m_pModel);
    if (m_pModel)
    {
        DrawDoor(m_pModel, drawMode, m_vPosition, m_fOrientation, m_fDoorAnimation, cascade);
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
                makeScale(1.f / 45.f, Type2Type<Matrix44f>()) * // scale to realistic size
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
                makeScale(1.f / 45.f, Type2Type<Matrix44f>()) * // scale to realistic size
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
    // Player
    m_pActors.push_back(new Player());

    // Doors
    const unsigned int numDoors = 5;
    const float doorDist = 15.f;
    const float doorRotRad = Math::PI;
    for (unsigned int i = 0; i < numDoors; i++)
    {
        m_pActors.push_back(new Door());
        m_pActors.back()->SetPosition(doorDist * Vec3f(
            Math::sin(doorRotRad * ((float)i / (float)(numDoors - 1)) - doorRotRad * 0.5f),
            0.f,
            Math::cos(doorRotRad * ((float)i / (float)(numDoors - 1)) - doorRotRad * 0.5f)));
        m_pActors.back()->SetOrientation(Math::rad2Deg(doorRotRad) * ((float)i / (float)(numDoors - 1)));
    }
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
    , m_bIsInteracting(false)
{}

void Scene::Actor::SetOrientation(float angle)
{
    while (angle >= 180.f)
    {
        angle -= 360.f;
    }

    while (angle < -180.f)
    {
        angle += 360.f;
    }

    m_fOrientation = angle;
}

const bool Scene::IsInteracting() const
{
    auto actors = ((VirtualMuseum*)AppMain)->GetScene()->GetActors();
    for (unsigned int i = 0; i < actors.size(); i++)
    {
        if (actors[i]->IsInteracting())
        {
            return true;
        }
    }

    return false;
}
