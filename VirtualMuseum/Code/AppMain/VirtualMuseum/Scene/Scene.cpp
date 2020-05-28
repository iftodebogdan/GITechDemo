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
#include "SceneData.h"

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
        Door(const unsigned int doorId);
        ~Door();

        void Update(const float deltaTime);
        void Draw(DrawMode drawMode, unsigned int cascade = ~0u);

        void Interact();

    protected:
        void DrawModel(Model* model, DrawMode drawMode, Matrix44f* worldMat = nullptr,
            Matrix44f* viewMat = nullptr, Matrix44f* projMat = nullptr);
        void DrawDoor(Model* model, DrawMode drawMode, Vec3f pos, float rotDeg, float openDoor, unsigned int cascade = ~0u);
        void DrawExhibits(DrawMode drawMode, Vec3f pos, float rotDeg);
        void DrawExhibit(DrawMode drawMode, Vec3f pos, float rotDeg, Vec3f localOffset, unsigned int id);

        enum InteractState
        {
            IS_ADJUST_PLAYER,
            IS_OPEN_DOOR,
            IS_FADE_FROM_WHITE
        };

        Model* m_pModel;
        Audio::SoundSource* m_pOpenSound;
        Audio::SoundSource* m_pHallSound;
        float m_fDoorAnimation;
        float m_fInteractAccum;
        Vec3f m_vPlayerInitialPos;
        Vec3f m_vPlayerInitialRot;
        float m_fInitialFoV;
        float m_fInitialLight;
        InteractState m_eInteractState;
        bool m_bIsInRoom;
        float m_fHallSoundGain;
        unsigned int m_nSceneDataId;
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

    Vec3f pos = -((VirtualMuseum*)AppMain)->GetCamera().vPos;

    if (!((VirtualMuseum*)AppMain)->GetScene()->IsInteracting())
    {
        if (input->GetBool(VirtualMuseum::Command::APP_CMD_FORWARD) ||
            input->GetBool(VirtualMuseum::Command::APP_CMD_BACKWARD) ||
            input->GetBool(VirtualMuseum::Command::APP_CMD_LEFT) ||
            input->GetBool(VirtualMuseum::Command::APP_CMD_RIGHT))
        {
            if (m_pFootstepsSound->GetStatus() != Audio::SoundSource::PLAYING)
            {
                m_pFootstepsSound->Play(true);
            }

            if (input->GetBool(VirtualMuseum::Command::APP_CMD_SPEED_UP))
            {
                m_pFootstepsSound->SetPitch(2.f);
            }
            else
            {
                m_pFootstepsSound->SetPitch(1.f);
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

    // Leash
    const float leashDist = 10.f;
    if (lengthSquared(Vec3f(pos[0], 0.f, pos[2])) > leashDist * leashDist)
    {
        Vec3f leashVec = makeNormal(Vec3f(pos[0], 0.f, pos[2])) * leashDist;
        ((VirtualMuseum*)AppMain)->GetCamera().vPos[0] = -leashVec[0];
        ((VirtualMuseum*)AppMain)->GetCamera().vPos[2] = -leashVec[2];
    }

    // Listener pos and rot
    pos = -((VirtualMuseum*)AppMain)->GetCamera().vPos;
    Vec4f lookAt(0.f, 0.f, 1.f, 0.f);
    Vec4f up(0.f, 1.f, 0.f, 0.f);

    lookAt = HLSL::BRDFParams->InvViewMat * lookAt;
    up = HLSL::BRDFParams->InvViewMat * up;

    Audio::GetInstance()->SetListenerPosition(Vec3f(pos[0], pos[1], -pos[2]));
    Audio::GetInstance()->SetListenerOrientation(
        makeNormal(Vec3f(lookAt[0], lookAt[1], -lookAt[2])),
        makeNormal(Vec3f(up[0], up[1], -up[2]))
    );

    // "Collision"
    for (unsigned int i = 0; i < actors.size(); i++)
    {
        if (actors[i]->IsActive())
        {
            Vec3f posDiff = Vec3f(pos[0], actors[i]->GetPosition()[1], pos[2]) - actors[i]->GetPosition();
            if (lengthSquared(posDiff) < 1)
            {
                normalize(posDiff);
                ((VirtualMuseum*)AppMain)->GetCamera().vPos[0] = -(actors[i]->GetPosition() + posDiff)[0];
                ((VirtualMuseum*)AppMain)->GetCamera().vPos[2] = -(actors[i]->GetPosition() + posDiff)[2];
                m_pFootstepsSound->Stop();
            }
        }
    }

    // Interact
    if (!((VirtualMuseum*)AppMain)->GetScene()->IsInteracting())
    {
        for (unsigned int i = 0; i < actors.size(); i++)
        {
            if (actors[i]->IsActive())
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

Door::Door(const unsigned int sceneDataId)
    : Actor()
    , m_pModel(nullptr)
    , m_pOpenSound(nullptr)
    , m_pHallSound(nullptr)
    , m_fDoorAnimation(0.f)
    , m_fInteractAccum(0.f)
    , m_vPlayerInitialPos(0.f, 0.f, 0.f)
    , m_vPlayerInitialRot(0.f, 0.f, 0.f)
    , m_fInitialFoV(0.f)
    , m_fInitialLight(0.f)
    , m_eInteractState(IS_ADJUST_PLAYER)
    , m_bIsInRoom(false)
    , m_fHallSoundGain(0.f)
    , m_nSceneDataId(sceneDataId)
{
    m_pModel = &DoorModel;

    m_pOpenSound = Audio::GetInstance()->CreateSoundSource();
    m_pOpenSound->SetSoundFile("sounds/open_door.wav");
    m_pOpenSound->SetPosition(Vec3f(m_vPosition[0], m_vPosition[1], -m_vPosition[2]));

    m_pHallSound = Audio::GetInstance()->CreateSoundSource();
    m_pHallSound->SetSoundFile(SceneData::GetSceneData()[sceneDataId].hallSnd.c_str());
    m_pHallSound->SetPosition(Vec3f(m_vPosition[0], m_vPosition[1], -m_vPosition[2]));
    m_pHallSound->SetFilterGain(0.f, 0.f);
}

Door::~Door()
{
    Audio::GetInstance()->RemoveSoundSource(m_pOpenSound);
    Audio::GetInstance()->RemoveSoundSource(m_pHallSound);
}

void Door::Update(const float deltaTime)
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    if(IsActive())
    {
        m_pOpenSound->SetPosition(Vec3f(m_vPosition[0], m_vPosition[1], -m_vPosition[2]));

        if (!m_bIsInRoom)
        {
            m_pHallSound->SetPosition(Vec3f(m_vPosition[0], m_vPosition[1], -m_vPosition[2]));
        }
        else
        {
            Vec3f camPos = -((VirtualMuseum*)AppMain)->GetCamera().vPos;
            m_pHallSound->SetPosition(Vec3f(camPos[0], m_vPosition[1], -camPos[2]));
        }

        if (m_fHallSoundGain < 1.f)
        {
            m_fHallSoundGain += deltaTime / 3.f;
            m_fHallSoundGain = gmtl::Math::clamp(m_fHallSoundGain, 0.f, 1.f);
            m_pHallSound->SetFilterGain(m_fHallSoundGain, 0.f);
        }

        if (m_pHallSound->GetStatus() != Audio::SoundSource::PLAYING)
        {
            m_pHallSound->Play(true);
        }

        if (IsInteracting())
        {
            m_fInteractAccum += deltaTime;

            switch (m_eInteractState)
            {
                case IS_ADJUST_PLAYER:
                {
                    gmtl::Math::lerp(RenderConfig::Camera::FoV, gmtl::Math::clamp(m_fInteractAccum / 4.4f, 0.f, 1.f), m_fInitialFoV, 1.f);

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

                    if (m_fInteractAccum >= 1.4f)
                    {
                        m_eInteractState = IS_OPEN_DOOR;

                        if (!m_bIsInRoom)
                        {
                            auto actors = ((VirtualMuseum*)AppMain)->GetScene()->GetActors();
                            for (unsigned int i = 0; i < actors.size(); i++)
                            {
                                if (actors[i] != this)
                                {
                                    actors[i]->MakeActive(false);
                                }
                            }
                        }
                    }

                    break;
                }

                case IS_OPEN_DOOR:
                {
                    gmtl::Math::lerp(RenderConfig::Camera::FoV, gmtl::Math::clamp(m_fInteractAccum / 4.4f, 0.f, 1.f), m_fInitialFoV, 1.f);

                    m_fDoorAnimation += deltaTime / 3.f;

                    float lerpVal = gmtl::Math::clamp(m_fDoorAnimation, 0.f, 1.f);
                    gmtl::Math::lerp(RenderConfig::DirectionalLightVolume::MultScatterIntensity, Math::pow(lerpVal, 5.f), m_fInitialLight, 10000.f);

                    Vec3f hallSoundPos;
                    lerpVal = m_bIsInRoom ? gmtl::Math::pow(1.f - lerpVal, 2.f) : lerpVal;
                    gmtl::Math::lerp(hallSoundPos[0], lerpVal, m_vPosition[0], -((VirtualMuseum*)AppMain)->GetCamera().vPos[0]);
                    //gmtl::Math::lerp(hallSoundPos[1], lerpVal, m_vPosition[1], -((VirtualMuseum*)AppMain)->GetCamera().vPos[1]);
                    hallSoundPos[1] = m_vPosition[1];
                    gmtl::Math::lerp(hallSoundPos[2], lerpVal, -m_vPosition[2], ((VirtualMuseum*)AppMain)->GetCamera().vPos[2]);
                    m_pHallSound->SetPosition(hallSoundPos);
                    m_pHallSound->SetFilterGain(m_fHallSoundGain, lerpVal);

                    if (m_fDoorAnimation >= 1.f)
                    {
                        if (m_bIsInRoom)
                        {
                            Vec3f currRot = ((VirtualMuseum*)AppMain)->GetCamera().vRot;
                            currRot[1] += 180.f;
                            ((VirtualMuseum*)AppMain)->GetCamera().vRot = currRot;
                        }
                        else
                        {
                            Vec3f currPos = ((VirtualMuseum*)AppMain)->GetCamera().vPos;
                            ((VirtualMuseum*)AppMain)->GetCamera().vPos = Vec3f(-currPos[0], currPos[1], -currPos[2]);
                        }

                        RenderConfig::Camera::FoV = m_fInitialFoV;

                        m_eInteractState = IS_FADE_FROM_WHITE;

                        if (m_bIsInRoom)
                        {
                            auto actors = ((VirtualMuseum*)AppMain)->GetScene()->GetActors();
                            for (unsigned int i = 0; i < actors.size(); i++)
                            {
                                if (actors[i] != this)
                                {
                                    actors[i]->MakeActive(true);
                                }
                            }
                        }

                        m_bIsInRoom = !m_bIsInRoom;
                    }

                    break;
                }

                case IS_FADE_FROM_WHITE:
                {
                    m_fDoorAnimation -= deltaTime / 3.f;

                    const float lerpVal = gmtl::Math::clamp(m_fDoorAnimation, 0.f, 1.f);
                    gmtl::Math::lerp(RenderConfig::DirectionalLightVolume::MultScatterIntensity, Math::pow(lerpVal, 5.f), m_fInitialLight, 10000.f);

                    if (m_fDoorAnimation <= 0.f)
                    {
                        RenderConfig::Camera::FoV = m_fInitialFoV;
                        RenderConfig::DirectionalLightVolume::MultScatterIntensity = m_fInitialLight;
                        m_bIsInteracting = false;
                        m_fDoorAnimation = 0.f;
                        m_eInteractState = Door::IS_ADJUST_PLAYER;
                    }
                }
            }
        }

        if (m_bIsInRoom)
        {
            const float corridorWidth = 2.f;
            const Vec3f corridorDir = makeNormal(m_vPosition);
            const Vec3f camPos = -((VirtualMuseum*)AppMain)->GetCamera().vPos;
            const Vec3f playerPos = Vec3f(camPos[0], 0.f, camPos[2]);
            const Vec3f corridorCenterLine = corridorDir * dot(corridorDir, playerPos);
            const Vec3f centerLineToPlayer = playerPos - corridorCenterLine;
            const float playerDistFromCorridor = length(centerLineToPlayer);
            const Vec3f adjustPlayerPos = corridorCenterLine + makeNormal(centerLineToPlayer) * Math::Min(playerDistFromCorridor, corridorWidth * 0.5f);
            ((VirtualMuseum*)AppMain)->GetCamera().vPos = Vec3f(-adjustPlayerPos[0], -camPos[1], -adjustPlayerPos[2]);

            UI_PASS.AddTooltip(SceneData::GetSceneData()[m_nSceneDataId].roomName.c_str(),
                Vec2f(0.5f * (float)RenderContext->GetDisplayResolution()[0], 0.95f * (float)RenderContext->GetDisplayResolution()[1]));
        }
    }
    else // if (!IsActive())
    {
        if (m_fHallSoundGain > 0.f)
        {
            m_fHallSoundGain -= deltaTime / 3.f;
            m_fHallSoundGain = gmtl::Math::clamp(m_fHallSoundGain, 0.f, 1.f);
            m_pHallSound->SetFilterGain(m_fHallSoundGain, 0.f);
        }

        if (m_pHallSound->GetStatus() == Audio::SoundSource::PLAYING && m_fHallSoundGain <= 0.f)
        {
            m_pHallSound->Pause();
        }

        m_bIsInteracting = false;
        m_fDoorAnimation = 0.f;
        m_eInteractState = Door::IS_ADJUST_PLAYER;
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
    if (!IsActive())
    {
        return;
    }

    assert(m_pModel);
    if (m_pModel)
    {
        DrawDoor(m_pModel, drawMode, m_vPosition, m_fOrientation, m_fDoorAnimation, cascade);
    }

    if (m_bIsInRoom)
    {
        DrawExhibits(drawMode, m_vPosition, m_fOrientation);
    }
}

void Door::DrawExhibits(DrawMode drawMode, Vec3f pos, float rotDeg)
{
    const unsigned int exhibitCount = SceneData::GetSceneData()[m_nSceneDataId].exhibitCount;
    for (unsigned int id = 0; id < exhibitCount; id++)
    {
        const float corridorLength = length(pos) * 2.f;
        const Vec3f corridorDir = makeNormal(pos);
        float dist = 5.f;
        const float exhbitDist = 3.f;
        for (unsigned int i = 0; dist <= corridorLength; i++)
        {
            DrawExhibit(drawMode,
                pos - corridorDir * dist -
                makeCross(corridorDir, Vec3f(0.f, 1.f, 0.f)) * exhbitDist +
                Vec3f(0.f, 1.5f, 0.f),
                rotDeg, Vec3f(0.f, 0.f, 0.f), i % exhibitCount);

            DrawExhibit(drawMode,
                pos - corridorDir * dist +
                makeCross(corridorDir, Vec3f(0.f, 1.f, 0.f)) * exhbitDist +
                Vec3f(0.f, 1.5f, 0.f),
                rotDeg - 180.f, Vec3f(0.f, 0.f, 0.f), (exhibitCount / 2 + i) % exhibitCount);

            dist += 4.f;
        }
    }
}

void Door::DrawExhibit(DrawMode drawMode, Vec3f pos, float rotDeg, Vec3f localOffset, unsigned int id)
{
    Renderer* RenderContext = Renderer::GetInstance();
    RenderState* RSMgr = RenderContext ? RenderContext->GetRenderStateManager() : nullptr;
    if (!RenderContext || !RSMgr)
        return;

    switch (drawMode)
    {
        case UI_3D:
        {
            // Setup render states
            const bool sRGBEnabled = RSMgr->GetSRGBWriteEnabled();
            const bool zWriteEnable = RSMgr->GetZWriteEnabled();
            const Cmp zFunc = RSMgr->GetZFunc();
            const ZBuffer zEnabled = RSMgr->GetZEnabled();
            const bool blendEnabled = RSMgr->GetColorBlendEnabled();
            const Blend dstBlend = RSMgr->GetColorDstBlend();
            const Blend srcBlend = RSMgr->GetColorSrcBlend();
            const Cull cullMode = RSMgr->GetCullMode();

            RSMgr->SetSRGBWriteEnabled(false);
            RSMgr->SetZWriteEnabled(true);
            RSMgr->SetZFunc(CMP_LESSEQUAL);
            RSMgr->SetZEnabled(ZB_ENABLED);
            RSMgr->SetColorBlendEnabled(true);
            RSMgr->SetColorDstBlend(BLEND_ZERO);
            RSMgr->SetColorSrcBlend(BLEND_ONE);
            RSMgr->SetCullMode(CULL_NONE);

            SceneData::GetSceneData()[m_nSceneDataId].exhibits[id]->GetTexture()->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
            SceneData::GetSceneData()[m_nSceneDataId].exhibits[id]->GetTexture()->SetSRGBEnabled(true);

            const float texDimRatio = 
                (float)SceneData::GetSceneData()[m_nSceneDataId].exhibits[id]->GetTexture()->GetHeight() /
                (float)SceneData::GetSceneData()[m_nSceneDataId].exhibits[id]->GetTexture()->GetWidth();
            Matrix44f worldMat =
                makeTrans(pos, Type2Type<Matrix44f>()) *
                makeRot(EulerAngleXYZf(0.f, Math::deg2Rad(rotDeg), 0.f), Type2Type<Matrix44f>()) *
                makeTrans(localOffset, Type2Type<Matrix44f>()) *
                makeScale(Vec3f(1.f, texDimRatio, 1.f), Type2Type<Matrix44f>());

            /*
            HLSL::DepthPassParams->WorldViewProjMat =
                HLSL::FrameParams->ProjMat *
                HLSL::BRDFParams->ViewMat *
                makeTrans(pos, Type2Type<Matrix44f>()) *
                makeRot(EulerAngleXYZf(0.f, Math::deg2Rad(rotDeg), 0.f), Type2Type<Matrix44f>());

            RSMgr->SetColorWriteEnabled(false, false, false, false);

            GBuffer.Enable();
            DepthPassShader.Enable();

            PUSH_PROFILE_MARKER(SceneData::GetSceneData()[m_nSceneDataId].exhibits[i]->GetFilePath());
            RenderContext->DrawVertexBuffer(SimpleQuad);
            POP_PROFILE_MARKER();

            DepthPassShader.Disable();
            GBuffer.Disable();
            */

            GBuffer.Enable();

            HLSL::GBufferGenerationParams->WorldViewProjMat =
                HLSL::FrameParams->ProjMat *
                HLSL::BRDFParams->ViewMat *
                worldMat;
            HLSL::GBufferGenerationParams->WorldViewMat = 
                HLSL::BRDFParams->ViewMat *
                worldMat;

            HLSL::GBufferGeneration_Diffuse = SceneData::GetSceneData()[m_nSceneDataId].exhibits[id]->GetTextureIndex();
            HLSL::GBufferGeneration_Normal = ~0u;
            HLSL::GBufferGenerationParams->HasNormalMap = false;

            // For Blinn-Phong BRDF
            HLSL::GBufferGeneration_Spec = ~0u;
            HLSL::GBufferGenerationParams->HasSpecMap = false;
            HLSL::GBufferGenerationParams->SpecIntensity = 0;

            // For Cook-Torrance BRDF
            HLSL::GBufferGeneration_MatType = ~0u;
            HLSL::GBufferGeneration_Roughness = ~0u;

            GBufferGenerationShader.Enable();

            PUSH_PROFILE_MARKER(SceneData::GetSceneData()[m_nSceneDataId].exhibits[id]->GetFilePath());
            RenderContext->DrawVertexBuffer(SimpleQuad);
            POP_PROFILE_MARKER();

            GBufferGenerationShader.Disable();

            GBuffer.Disable();

            HLSL::UIParams->TextureSwitch = 2;
            HLSL::UIParams->MipLevel = 0;
            HLSL::UIParams->FaceIdx = 0;
            HLSL::UIParams->DepthSlice = 0;
            HLSL::UIParams->ColorMul = Vec4f(
                RenderConfig::PostProcessing::LensFlare::BrightnessThreshold * 0.9f,
                RenderConfig::PostProcessing::LensFlare::BrightnessThreshold * 0.9f,
                RenderConfig::PostProcessing::LensFlare::BrightnessThreshold * 0.9f,
                1.f);

            HLSL::UIParams->UIProjMat =
                HLSL::FrameParams->ProjMat *
                HLSL::BRDFParams->ViewMat *
                worldMat;

            HLSL::UI_Texture2D = SceneData::GetSceneData()[m_nSceneDataId].exhibits[id]->GetTextureIndex();

            UIShader.Enable();

            PUSH_PROFILE_MARKER(SceneData::GetSceneData()[m_nSceneDataId].exhibits[id]->GetFilePath());
            RenderContext->DrawVertexBuffer(SimpleQuad);
            POP_PROFILE_MARKER();

            UIShader.Disable();

            // Revert render states
            RSMgr->SetSRGBWriteEnabled(sRGBEnabled);
            RSMgr->SetZWriteEnabled(zWriteEnable);
            RSMgr->SetZFunc(zFunc);
            RSMgr->SetZEnabled(zEnabled);
            RSMgr->SetColorBlendEnabled(blendEnabled);
            RSMgr->SetColorDstBlend(dstBlend);
            RSMgr->SetColorSrcBlend(srcBlend);
            RSMgr->SetCullMode(cullMode);

            break;
        }

        default:
            break;
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
        break;
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
    m_pPlayer = new Player();

    // Doors
    const unsigned int numDoors = 5;
    const float doorDist = 10.f;
    const float doorRotRad = Math::PI;
    for (unsigned int i = 0; i < numDoors; i++)
    {
        m_pActors.push_back(new Door(i));
        m_pActors.back()->SetPosition(doorDist * Vec3f(
            Math::sin(doorRotRad * ((float)i / (float)(numDoors - 1)) - doorRotRad * 0.5f),
            0.f,
            Math::cos(doorRotRad * ((float)i / (float)(numDoors - 1)) - doorRotRad * 0.5f)));
        m_pActors.back()->SetOrientation(Math::rad2Deg(doorRotRad) * ((float)i / (float)(numDoors - 1)));
    }
}

void Scene::Update(const float deltaTime)
{
    if (m_pPlayer)
    {
        m_pPlayer->Update(deltaTime);
    }

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
    delete m_pPlayer;
    m_pPlayer = nullptr;

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
    , m_bIsActive(true)
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
