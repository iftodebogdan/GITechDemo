/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   GITechDemo.cpp
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

#include <time.h>
#include <sstream>

#include <imgui.h>
#include <imgui_internal.h>

#include <Renderer.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <VertexFormat.h>
#include <Profiler.h>
using namespace Synesthesia3D;

#include "Framework.h"
using namespace AppFramework;

#include "GITechDemo.h"
#include "AppResources.h"
#include "RenderScheme.h"
#include "PerlinNoise.h"
#include "ArtistParameter.h"
#include "SkyPass.h"
#include "UIPass.h"
using namespace GITechDemoApp;

CREATE_APP(GITechDemo)

namespace GITechDemoApp
{
    static bool bExtraResInit = false;

    extern SkyPass SKY_PASS;
    extern UIPass UI_PASS;
    extern const char* const ResourceTypeMap[RenderResource::RES_MAX];

    bool FULLSCREEN_ENABLED = false;
    bool BORDERLESS_ENABLED = true;
    int FULLSCREEN_RESOLUTION_X = 0;
    int FULLSCREEN_RESOLUTION_Y = 0;
    int FULLSCREEN_REFRESH_RATE = 0;
    bool VSYNC_ENABLED = false;
}

namespace GITechDemoApp
{
    bool CAMERA_ANIMATION_ENABLED = true;
    int CAMERA_ANIMATION_TIMEOUT_SECONDS = 30;

    bool DIRECTIONAL_LIGHT_ANIMATION_ENABLED = true;

    Perlin PerlinNoise(1, 0.05f, 1.f, (int)time(NULL));

    extern AABoxf SceneAABB;

    bool CAMERA_INFINITE_PROJ = true;
    float CAMERA_FOV = 60.f;
    float CAMERA_MOVE_SPEED = 250.f;
    float CAMERA_SPEED_UP_FACTOR = 5.f;
    float CAMERA_SLOW_DOWN_FACTOR = 0.1f;
    float CAMERA_ROTATE_SPEED = 75.f;
}

template <typename T> string tostr(const T& t) {
    ostringstream os;
    os << t;
    return os.str();
}

GITechDemo::GITechDemo()
    : App()
    , m_fDeltaTime(0.f)
    , m_pInputMap(nullptr)
    , m_bUIHasFocus(false)
{
    MUTEX_INIT(mResInitMutex);
}

GITechDemo::~GITechDemo()
{
    MUTEX_DESTROY(mResInitMutex);
}

bool GITechDemo::Init(void* hWnd)
{
    Framework* const pFW = Framework::GetInstance();

    // Renderer MUST be initialized on the SAME thread as the target window
    Renderer::CreateInstance(API_DX9);
    //Renderer::CreateInstance(API_NULL);

    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return false;

    RenderContext->Initialize(hWnd);

    ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
    if (!ResourceMgr)
        return false;

    // Set initial camera position
    m_tCamera.vPos = Vec3f(-828.031738f, -651.508972f, -100.693771f);
    m_tCamera.mRot.set(
         0.524057686f,   0.0577781796f, 0.849711359f,   0.f,
        -0.155855492f,   0.987355292f,  0.0289840195f,  0.f,
        -0.837296307f,  -0.147619948f,  0.526430249f,   0.f,
         0.f,            0.f,           0.f,            1.f
    );

    // Setup Gainput
    if (m_pInputManager)
    {
        gainput::DeviceId mouseId = m_pInputManager->CreateDevice<gainput::InputDeviceMouse>();
        gainput::DeviceId keyboardId = m_pInputManager->CreateDevice<gainput::InputDeviceKeyboard>();

        m_pInputMap = new gainput::InputMap(*m_pInputManager);
        if (m_pInputMap && mouseId != gainput::InvalidDeviceId && keyboardId != gainput::InvalidDeviceId)
        {
            // Key bindings
            m_pInputMap->MapBool(APP_CMD_CTRL_YAW, mouseId, gainput::MouseButtonLeft);
            m_pInputMap->MapBool(APP_CMD_CTRL_PITCH, mouseId, gainput::MouseButtonLeft);
            m_pInputMap->MapBool(APP_CMD_CTRL_ROLL, mouseId, gainput::MouseButtonRight);
            m_pInputMap->MapFloat(APP_CMD_YAW_AXIS, mouseId, gainput::MouseAxisX);
            m_pInputMap->MapFloat(APP_CMD_PITCH_AXIS, mouseId, gainput::MouseAxisY);
            m_pInputMap->MapFloat(APP_CMD_ROLL_AXIS, mouseId, gainput::MouseAxisX);
            m_pInputMap->MapBool(APP_CMD_FORWARD, keyboardId, gainput::KeyW);
            m_pInputMap->MapBool(APP_CMD_BACKWARD, keyboardId, gainput::KeyS);
            m_pInputMap->MapBool(APP_CMD_LEFT, keyboardId, gainput::KeyA);
            m_pInputMap->MapBool(APP_CMD_RIGHT, keyboardId, gainput::KeyD);
            m_pInputMap->MapBool(APP_CMD_SPEED_UP, keyboardId, gainput::KeyShiftL);
            m_pInputMap->MapBool(APP_CMD_SLOW_DOWN, keyboardId, gainput::KeyCtrlL);
        }

        // Setup UI key bindings
        UI_PASS.SetupInput(m_pInputManager);
    }

    const std::vector<Synesthesia3D::DeviceCaps::SupportedScreenFormat>& arrSupportedScreenFormats = RenderContext->GetDeviceCaps().arrSupportedScreenFormats;

    // Set the highest available fullscreen resolution
    for (unsigned int i = 0; i < arrSupportedScreenFormats.size(); i++)
    {
        if (RenderContext->GetBackBufferFormat() == arrSupportedScreenFormats[i].ePixelFormat)
        {
            if ((int)arrSupportedScreenFormats[i].nWidth > FULLSCREEN_RESOLUTION_X ||
                (int)arrSupportedScreenFormats[i].nHeight > FULLSCREEN_RESOLUTION_Y)
            {
                FULLSCREEN_RESOLUTION_X = arrSupportedScreenFormats[i].nWidth;
                FULLSCREEN_RESOLUTION_Y = arrSupportedScreenFormats[i].nHeight;
                FULLSCREEN_REFRESH_RATE = arrSupportedScreenFormats[i].nRefreshRate;
            }

            if ((int)arrSupportedScreenFormats[i].nWidth == FULLSCREEN_RESOLUTION_X ||
                (int)arrSupportedScreenFormats[i].nHeight == FULLSCREEN_RESOLUTION_Y ||
                (int)arrSupportedScreenFormats[i].nRefreshRate > FULLSCREEN_REFRESH_RATE)
            {
                FULLSCREEN_REFRESH_RATE = arrSupportedScreenFormats[i].nRefreshRate;
            }
        }
    }

    m_nLastFrameResX = -1;
    m_nLastFrameResY = -1;
    m_vLastFrameViewport = Vec2i(-1, -1);
    m_nLastFrameRefreshRate = -1;
    m_bLastFrameFullscreen = pFW->IsFullscreen();
    m_bLastFrameBorderless = pFW->IsBorderlessWindow();
    m_bLastFrameVSync = RenderContext->GetVSyncStatus();

    return true;
}

void GITechDemo::Release()
{
    if (m_pInputMap)
    {
        delete m_pInputMap;
        m_pInputMap = nullptr;
    }

    bExtraResInit = false;

    RenderScheme::ReleaseResources();

    RenderResource::FreeAll();
    Renderer::DestroyInstance();
}

void GITechDemo::LoadResources(unsigned int thId, unsigned int thCount)
{
    Framework* const pFW = Framework::GetInstance();
    if (!pFW)
        return;

    bool bAllInitialized = false;
    do
    {
        bAllInitialized = true;
        const vector<RenderResource*>& resList = RenderResource::GetResourceList();
        for (unsigned int i = 0; i < resList.size(); i++)
        {
            if (!resList[i]->IsInitialized())
            {
                bAllInitialized = false;
                if (resList[i]->TryLockRes())
                {
                    std::stringstream msg;
                    msg << "Thread " << thId << " - ";
                    msg << ResourceTypeMap[resList[i]->GetResourceType()] << ": \"" << resList[i]->GetDesc() << "\"";
                    cout << msg.str() + " start\n";
                    const unsigned startTicks = pFW->GetTicks();
                    resList[i]->Init();
                    cout << msg.str() + " finished in " + tostr((float)(pFW->GetTicks() - startTicks) / 1000.f) + "ms\n";
                    resList[i]->UnlockRes();
                }
            }
        }
        pFW->Sleep(1); // sleep 1 ms so as not to hog CPU time
    } while (!bAllInitialized);

    // Allow only the first thread to get here to initialize the rest of the resources
    if (MUTEX_TRYLOCK(mResInitMutex))
    {
        if (!bExtraResInit)
        {
            std::stringstream msg;
            msg << "Thread " << thId << " - ";
            msg << "RenderScheme::AllocateResources()";
            cout << msg.str() + " start\n";

            const unsigned startTicks = pFW->GetTicks();

            // Misc. resources
            RenderScheme::AllocateResources();

            bExtraResInit = true;

            cout << msg.str() + " finished in " + tostr((float)(pFW->GetTicks() - startTicks) / 1000.f) + "ms\n";
        }
        MUTEX_UNLOCK(mResInitMutex);
    }
}

void GITechDemo::Update(const float fDeltaTime)
{
    m_fDeltaTime = fDeltaTime;
    
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    int cLeft, cTop, cRight, cBottom;
    Framework* const pFW = Framework::GetInstance();
    pFW->GetClientArea(cLeft, cTop, cRight, cBottom);
    const Vec2i viewportSize = Vec2i(cRight - cLeft, cBottom - cTop);

    const std::vector<Synesthesia3D::DeviceCaps::SupportedScreenFormat>& arrSupportedScreenFormats = RenderContext->GetDeviceCaps().arrSupportedScreenFormats;

    // Update fullscreen resolution setting
    if (m_nLastFrameResX != -1 ||
        m_nLastFrameResY != -1 ||
        m_vLastFrameViewport != Vec2i(-1, -1) ||
        m_nLastFrameRefreshRate != -1)
    {
        if (FULLSCREEN_RESOLUTION_X > m_nLastFrameResX)
        {
            // X resolution increased
            FULLSCREEN_RESOLUTION_X = INT_MAX;
            FULLSCREEN_RESOLUTION_Y = INT_MAX;
            for (unsigned int i = 0; i < arrSupportedScreenFormats.size(); i++)
            {
                if (RenderContext->GetBackBufferFormat() == arrSupportedScreenFormats[i].ePixelFormat)
                {
                    if ((int)arrSupportedScreenFormats[i].nWidth > m_nLastFrameResX &&
                        (int)arrSupportedScreenFormats[i].nWidth <= FULLSCREEN_RESOLUTION_X)
                    {
                        FULLSCREEN_RESOLUTION_X = (int)arrSupportedScreenFormats[i].nWidth;
                        FULLSCREEN_RESOLUTION_Y = (int)arrSupportedScreenFormats[i].nHeight;
                    }
                }
            }
            if (FULLSCREEN_RESOLUTION_X == INT_MAX && FULLSCREEN_RESOLUTION_Y == INT_MAX)
            {
                FULLSCREEN_RESOLUTION_X = m_nLastFrameResX; // Reset resolution setting in case
                FULLSCREEN_RESOLUTION_Y = m_nLastFrameResY; // we didn't find another suitable one
            }
        }
        else if (FULLSCREEN_RESOLUTION_X < m_nLastFrameResX)
        {
            // X resolution decreased
            FULLSCREEN_RESOLUTION_X = 0;
            FULLSCREEN_RESOLUTION_Y = 0;
            for (unsigned int i = 0; i < arrSupportedScreenFormats.size(); i++)
            {
                if (RenderContext->GetBackBufferFormat() == arrSupportedScreenFormats[i].ePixelFormat)
                {
                    if ((int)arrSupportedScreenFormats[i].nWidth < m_nLastFrameResX &&
                        (int)arrSupportedScreenFormats[i].nWidth >= FULLSCREEN_RESOLUTION_X)
                    {
                        FULLSCREEN_RESOLUTION_X = (int)arrSupportedScreenFormats[i].nWidth;
                        FULLSCREEN_RESOLUTION_Y = (int)arrSupportedScreenFormats[i].nHeight;
                    }
                }
            }
            if (FULLSCREEN_RESOLUTION_X == 0 && FULLSCREEN_RESOLUTION_Y == 0)
            {
                FULLSCREEN_RESOLUTION_X = m_nLastFrameResX; // Reset resolution setting in case
                FULLSCREEN_RESOLUTION_Y = m_nLastFrameResY; // we didn't find another suitable one
            }
        }
        else if (FULLSCREEN_RESOLUTION_Y > m_nLastFrameResY)
        {
            // Y resolution increased
            FULLSCREEN_RESOLUTION_X = INT_MAX;
            FULLSCREEN_RESOLUTION_Y = INT_MAX;
            for (unsigned int i = 0; i < arrSupportedScreenFormats.size(); i++)
            {
                if (RenderContext->GetBackBufferFormat() == arrSupportedScreenFormats[i].ePixelFormat)
                {
                    if ((int)arrSupportedScreenFormats[i].nHeight > m_nLastFrameResY &&
                        (int)arrSupportedScreenFormats[i].nHeight <= FULLSCREEN_RESOLUTION_Y)
                    {
                        FULLSCREEN_RESOLUTION_X = (int)arrSupportedScreenFormats[i].nWidth;
                        FULLSCREEN_RESOLUTION_Y = (int)arrSupportedScreenFormats[i].nHeight;
                    }
                }
            }
            if (FULLSCREEN_RESOLUTION_X == INT_MAX && FULLSCREEN_RESOLUTION_Y == INT_MAX)
            {
                FULLSCREEN_RESOLUTION_X = m_nLastFrameResX; // Reset resolution setting in case
                FULLSCREEN_RESOLUTION_Y = m_nLastFrameResY; // we didn't find another suitable one
            }
        }
        else if (FULLSCREEN_RESOLUTION_Y < m_nLastFrameResY)
        {
            // Y resolution decreased
            FULLSCREEN_RESOLUTION_X = 0;
            FULLSCREEN_RESOLUTION_Y = 0;
            for (unsigned int i = 0; i < arrSupportedScreenFormats.size(); i++)
            {
                if (RenderContext->GetBackBufferFormat() == arrSupportedScreenFormats[i].ePixelFormat)
                {
                    if ((int)arrSupportedScreenFormats[i].nHeight < m_nLastFrameResY &&
                        (int)arrSupportedScreenFormats[i].nHeight >= FULLSCREEN_RESOLUTION_Y)
                    {
                        FULLSCREEN_RESOLUTION_X = (int)arrSupportedScreenFormats[i].nWidth;
                        FULLSCREEN_RESOLUTION_Y = (int)arrSupportedScreenFormats[i].nHeight;
                    }
                }
            }
            if (FULLSCREEN_RESOLUTION_X == 0 && FULLSCREEN_RESOLUTION_Y == 0)
            {
                FULLSCREEN_RESOLUTION_X = m_nLastFrameResX; // Reset resolution setting in case
                FULLSCREEN_RESOLUTION_Y = m_nLastFrameResY; // we didn't find another suitable one
            }
        }
        else if (FULLSCREEN_REFRESH_RATE > m_nLastFrameRefreshRate)
        {
            // Refresh rate increased
            FULLSCREEN_REFRESH_RATE = INT_MAX;
            for (unsigned int i = 0; i < arrSupportedScreenFormats.size(); i++)
            {
                if (RenderContext->GetBackBufferFormat() == arrSupportedScreenFormats[i].ePixelFormat &&
                    (int)arrSupportedScreenFormats[i].nWidth == FULLSCREEN_RESOLUTION_X &&
                    (int)arrSupportedScreenFormats[i].nHeight == FULLSCREEN_RESOLUTION_Y)
                {
                    if ((int)arrSupportedScreenFormats[i].nRefreshRate > m_nLastFrameRefreshRate &&
                        (int)arrSupportedScreenFormats[i].nRefreshRate <= FULLSCREEN_REFRESH_RATE)
                    {
                        FULLSCREEN_REFRESH_RATE = (int)arrSupportedScreenFormats[i].nRefreshRate;
                    }
                }
            }
            if (FULLSCREEN_REFRESH_RATE == INT_MAX)
            {
                // Reset resolution setting in case we didn't find another suitable one
                FULLSCREEN_REFRESH_RATE = m_nLastFrameRefreshRate;
            }
        }
        else if (FULLSCREEN_REFRESH_RATE < m_nLastFrameRefreshRate)
        {
            // Refresh rate decreased
            FULLSCREEN_REFRESH_RATE = 0;
            for (unsigned int i = 0; i < arrSupportedScreenFormats.size(); i++)
            {
                if (RenderContext->GetBackBufferFormat() == arrSupportedScreenFormats[i].ePixelFormat &&
                    (int)arrSupportedScreenFormats[i].nWidth == FULLSCREEN_RESOLUTION_X &&
                    (int)arrSupportedScreenFormats[i].nHeight == FULLSCREEN_RESOLUTION_Y)
                {
                    if ((int)arrSupportedScreenFormats[i].nRefreshRate < m_nLastFrameRefreshRate &&
                        (int)arrSupportedScreenFormats[i].nRefreshRate >= FULLSCREEN_REFRESH_RATE)
                    {
                        FULLSCREEN_REFRESH_RATE = (int)arrSupportedScreenFormats[i].nRefreshRate;
                    }
                }
            }
            if (FULLSCREEN_REFRESH_RATE == 0)
            {
                // Reset resolution setting in case we didn't find another suitable one
                FULLSCREEN_REFRESH_RATE = m_nLastFrameRefreshRate;
            }
        }
    }

    // Set the size of the backbuffer accordingly
    if (!pFW->IsRenderingPaused() &&
        (m_nLastFrameResX != FULLSCREEN_RESOLUTION_X ||
        m_nLastFrameResY != FULLSCREEN_RESOLUTION_Y ||
        m_nLastFrameRefreshRate != FULLSCREEN_REFRESH_RATE ||
        (m_vLastFrameViewport != viewportSize && !FULLSCREEN_ENABLED) ||
        m_bLastFrameFullscreen != FULLSCREEN_ENABLED ||
        m_bLastFrameBorderless != BORDERLESS_ENABLED ||
        m_bLastFrameVSync != VSYNC_ENABLED))
    {
        if (!m_bLastFrameFullscreen && FULLSCREEN_ENABLED)
            pFW->OnSwitchToFullscreenMode();
        else if (m_bLastFrameFullscreen && !FULLSCREEN_ENABLED)
        {
            if (BORDERLESS_ENABLED)
                pFW->OnSwitchToBorderlessWindowedMode();
            else
                pFW->OnSwitchToWindowedMode();
        }
        else if (!m_bLastFrameBorderless && BORDERLESS_ENABLED && !FULLSCREEN_ENABLED)
            pFW->OnSwitchToBorderlessWindowedMode();
        else if (m_bLastFrameBorderless && !BORDERLESS_ENABLED && !FULLSCREEN_ENABLED)
            pFW->OnSwitchToWindowedMode();

        RenderContext->SetDisplayResolution(
            FULLSCREEN_ENABLED ? Vec2i(FULLSCREEN_RESOLUTION_X, FULLSCREEN_RESOLUTION_Y) : viewportSize,
            Vec2i(0, 0),
            FULLSCREEN_ENABLED,
            FULLSCREEN_REFRESH_RATE,
            VSYNC_ENABLED);

        m_nLastFrameResX = FULLSCREEN_RESOLUTION_X;
        m_nLastFrameResY = FULLSCREEN_RESOLUTION_Y;
        m_nLastFrameRefreshRate = FULLSCREEN_REFRESH_RATE;
        m_vLastFrameViewport = viewportSize;
        m_bLastFrameFullscreen = FULLSCREEN_ENABLED;
        m_bLastFrameBorderless = BORDERLESS_ENABLED;
        m_bLastFrameVSync = VSYNC_ENABLED;
    }

    // Update focus context
    UpdateUIFocus();

    // Handle user input
    unsigned int cmd = APP_CMD_NONE;
    if (m_pInputManager && m_pInputMap && !IsUIInFocus())
    {
        // Handle user input for camera movement
        if (m_pInputMap->GetBool(APP_CMD_FORWARD))
            cmd |= APP_CMD_FORWARD;
        if (m_pInputMap->GetBool(APP_CMD_BACKWARD))
            cmd |= APP_CMD_BACKWARD;
        if (m_pInputMap->GetBool(APP_CMD_LEFT))
            cmd |= APP_CMD_LEFT;
        if (m_pInputMap->GetBool(APP_CMD_RIGHT))
            cmd |= APP_CMD_RIGHT;
        if (m_pInputMap->GetBool(APP_CMD_SPEED_UP))
            cmd |= APP_CMD_SPEED_UP;
        if (m_pInputMap->GetBool(APP_CMD_SLOW_DOWN))
            cmd |= APP_CMD_SLOW_DOWN;

        // Handle user input for camera rotation
        if (m_pInputMap->GetBool(APP_CMD_CTRL_YAW))
            cmd |= APP_CMD_CTRL_YAW;
        if (m_pInputMap->GetBool(APP_CMD_CTRL_PITCH))
            cmd |= APP_CMD_CTRL_PITCH;
        if (m_pInputMap->GetBool(APP_CMD_CTRL_ROLL))
            cmd |= APP_CMD_CTRL_ROLL;
    }

    // If any mouse button was just pressed, the mouse is unlikely to be in
    // the center of the screen and this might result in a "jump" in the
    // rotation of the camera. Ignore this frame's mouse movement.
    // This will introduce a one frame lag.
    Vec3f rotateDelta;
    if (m_pInputMap->GetBoolIsNew(APP_CMD_CTRL_YAW) ||
        m_pInputMap->GetBoolIsNew(APP_CMD_CTRL_PITCH) ||
        m_pInputMap->GetBoolIsNew(APP_CMD_CTRL_ROLL))
    {
        rotateDelta *= 0.f;
    }
    else
    {
        // When we have an even number of pixels on one axis, the center is between
        // two adjacent pixels. Because of this, the normalized value we obtain
        // below can never be exactly 0.5f, so we apply a half-pixel offset.
        const Vec2f halfPixel(
            0.5f / (float)(viewportSize[0] - 1) * (float)(viewportSize[0] % 2 == 0),
            0.5f / (float)(viewportSize[1] - 1) * (float)(viewportSize[1] % 2 == 0)
            );
        rotateDelta.set(
            m_pInputMap->GetFloat(APP_CMD_PITCH_AXIS),
            m_pInputMap->GetFloat(APP_CMD_YAW_AXIS),
            m_pInputMap->GetFloat(APP_CMD_ROLL_AXIS)
            );
        rotateDelta -= Vec3f(0.5f - halfPixel[1], 0.5f - halfPixel[0], 0.5f - halfPixel[0]);
        rotateDelta *= CAMERA_ROTATE_SPEED;
    }

    // Update camera rotation matrix
    if (cmd & APP_CMD_CTRL_PITCH)
        m_tCamera.mRot = makeRot(EulerAngleXYZf(Math::deg2Rad(-rotateDelta[0]), 0.f, 0.f), Type2Type<Matrix44f>()) * m_tCamera.mRot;

    if (cmd & APP_CMD_CTRL_YAW)
        m_tCamera.mRot = makeRot(EulerAngleXYZf(0.f, Math::deg2Rad(-rotateDelta[1]), 0.f), Type2Type<Matrix44f>()) * m_tCamera.mRot;

    if (cmd & APP_CMD_CTRL_ROLL)
        m_tCamera.mRot = makeRot(EulerAngleXYZf(0.f, 0.f, Math::deg2Rad(rotateDelta[2])), Type2Type<Matrix44f>()) * m_tCamera.mRot;

    // Update camera position vector
    if (cmd & APP_CMD_FORWARD)
        m_tCamera.vMoveVec[2] = 1.f;
    else if (cmd & APP_CMD_BACKWARD)
        m_tCamera.vMoveVec[2] = -1.f;
    else
        m_tCamera.vMoveVec[2] = 0.f;

    if (cmd & APP_CMD_LEFT)
        m_tCamera.vMoveVec[0] = -1.f;
    else if (cmd & APP_CMD_RIGHT)
        m_tCamera.vMoveVec[0] = 1.f;
    else
        m_tCamera.vMoveVec[0] = 0.f;

    if (cmd & APP_CMD_SPEED_UP)
        m_tCamera.fSpeedFactor = CAMERA_SPEED_UP_FACTOR;
    else if (cmd & APP_CMD_SLOW_DOWN)
        m_tCamera.fSpeedFactor = CAMERA_SLOW_DOWN_FACTOR;
    else
        m_tCamera.fSpeedFactor = 1.f;

    gmtl::normalize(m_tCamera.vMoveVec);
    m_tCamera.vMoveVec *= CAMERA_MOVE_SPEED * m_tCamera.fSpeedFactor * pFW->GetDeltaTime();
    m_tCamera.vPos -=
        Vec3f(m_tCamera.mRot[2][0] * m_tCamera.vMoveVec[2], m_tCamera.mRot[2][1] * m_tCamera.vMoveVec[2], m_tCamera.mRot[2][2] * m_tCamera.vMoveVec[2]) +
        Vec3f(m_tCamera.mRot[0][0] * m_tCamera.vMoveVec[0], m_tCamera.mRot[0][1] * m_tCamera.vMoveVec[0], m_tCamera.mRot[0][2] * m_tCamera.vMoveVec[0]);

    // Update mouse cursor
    if (cmd & (APP_CMD_CTRL_PITCH | APP_CMD_CTRL_YAW | APP_CMD_CTRL_ROLL))
    {
        int wLeft, wTop, wRight, wBottom;
        pFW->GetWindowArea(wLeft, wTop, wRight, wBottom);
        const int sideBorderSize = (wRight - wLeft) - viewportSize[0];
        const int topBorderSize = (wBottom - wTop) - viewportSize[1] - sideBorderSize;
        pFW->SetCursorAtPos((wRight + wLeft - 1) / 2, (wBottom + wTop + topBorderSize - 1) / 2);
        if (!pFW->IsCursorHidden())
            pFW->ShowCursor(false);
    }
    else
    {
        if (pFW->IsCursorHidden())
            pFW->ShowCursor(true);
    }

    // Animate camera
    if (CAMERA_ANIMATION_ENABLED)
    {
        static float lastInput = 0.f;
        if (cmd == APP_CMD_NONE)
        {
            lastInput += fDeltaTime;
            if (lastInput > CAMERA_ANIMATION_TIMEOUT_SECONDS)
            {
                static float time = 0.f;
                time += fDeltaTime;
                Vec3f perlinPos(
                    PerlinNoise.Get(time, 0.f),
                    PerlinNoise.Get(0.f, time),
                    PerlinNoise.Get(time, time));

                perlinPos[0] = (perlinPos[0] + 1.f) / 2.f;
                perlinPos[1] = (perlinPos[1] + 1.f) / 2.f;
                perlinPos[2] = (perlinPos[2] + 1.f) / 2.f;

                // Wrap around to 0.f when reaching 1.f
                perlinPos[0] = perlinPos[0] - floor(perlinPos[0]);
                perlinPos[1] = perlinPos[1] - floor(perlinPos[1]);
                perlinPos[2] = perlinPos[2] - floor(perlinPos[2]);

                m_tCamera.vPos = -Vec3f(
                    ((SceneAABB.getMax()[0] - SceneAABB.getMin()[0]) * perlinPos[0] + SceneAABB.getMin()[0]) * 1.5f,
                    ((SceneAABB.getMax()[1] - SceneAABB.getMin()[1]) * (perlinPos[1] - 0.15f) + SceneAABB.getMin()[1]),
                    ((SceneAABB.getMax()[2] - SceneAABB.getMin()[2]) * perlinPos[2] + SceneAABB.getMin()[2]) * 0.9f
                    );

                Vec3f lookAtPos = f3LightDir.GetCurrentValue() * 1500.f;
                lookAtPos[1] = gmtl::Math::Max(((SceneAABB.getMax()[1] - SceneAABB.getMin()[1]) / 1.75f + SceneAABB.getMin()[1]) / 1.75f, -m_tCamera.vPos[1]);

                Vec3f zAxis = makeNormal(Vec3f(lookAtPos + m_tCamera.vPos));
                Vec3f upVec = Vec3f(0.f, 1.f, 0.f);
                Vec3f xAxis = makeNormal(makeCross(upVec, zAxis));
                Vec3f yAxis = makeCross(zAxis, xAxis);
                upVec = yAxis;
                m_tCamera.mRot.set(
                    xAxis[0], xAxis[1], xAxis[2], 0.f,
                    yAxis[0], yAxis[1], yAxis[2], 0.f,
                    zAxis[0], zAxis[1], zAxis[2], 0.f,
                    0.f, 0.f, 0.f, 1.f
                    );
            }
        }
        else
            lastInput = 0.f;
    }

    // Animate directional light using Perlin Noise
    if (DIRECTIONAL_LIGHT_ANIMATION_ENABLED)
    {
        static float time = 0.f;
        time += fDeltaTime;
        float noiseX = PerlinNoise.Get(time, 0);
        float noiseZ = PerlinNoise.Get(0, time);
        ((Vec3f&)f3LightDir)[0] = noiseX;
        ((Vec3f&)f3LightDir)[2] = noiseZ;
        ((Vec3f&)f3LightDir)[1] = -1.f;
    }
    gmtl::normalize((Vec3f&)f3LightDir);

#if 0
    // NaN at this position!
    f3LightDir.GetCurrentValue()[0] = 9.41706021e-05;
    f3LightDir.GetCurrentValue()[1] = -1.00000000;
    f3LightDir.GetCurrentValue()[2] = -0.000278121064;

    m_tCamera.vPos[0] = 545.252686;
    m_tCamera.vPos[1] = -1660.33801;
    m_tCamera.vPos[2] = -81.6245651;

    m_tCamera.mRot.mData[0] = -0.0230895355;
    m_tCamera.mRot.mData[1] = 0.260565788;
    m_tCamera.mRot.mData[2] = 0.965138495;
    m_tCamera.mRot.mData[3] = 0.000000000;
    m_tCamera.mRot.mData[4] = 0.0582327023;
    m_tCamera.mRot.mData[5] = 0.964119077;
    m_tCamera.mRot.mData[6] = -0.258897275;
    m_tCamera.mRot.mData[7] = 0.000000000;
    m_tCamera.mRot.mData[8] = -0.998014688;
    m_tCamera.mRot.mData[9] = 0.0502296351;
    m_tCamera.mRot.mData[10] = -0.0374352485;
    m_tCamera.mRot.mData[11] = 0.000000000;
    m_tCamera.mRot.mData[12] = 0.000000000;
    m_tCamera.mRot.mData[13] = 0.000000000;
    m_tCamera.mRot.mData[14] = 0.000000000;
    m_tCamera.mRot.mData[15] = 1.00000000;
#endif

    // Precalculate some parts of the equation for reconstructing
    // linear depth from hyperbolic depth
    f2LinearDepthEquation = Vec2f(fZNear * fZFar / (fZNear - fZFar), fZFar / (fZFar - fZNear));

    // Calculate world matrix
    f44WorldMat = makeTrans(Vec3f(0, 0, 0), Type2Type<Matrix44f>());

    // Calculate view matrix
    f44ViewMat = m_tCamera.mRot * makeTrans(m_tCamera.vPos, Type2Type<Matrix44f>());

    // Calculate projection matrix
    if(CAMERA_INFINITE_PROJ)
        RenderContext->CreateInfinitePerspectiveMatrix(f44ProjMat, Math::deg2Rad(CAMERA_FOV), (float)viewportSize[0] / (float)viewportSize[1], fZNear);
    else
        RenderContext->CreatePerspectiveMatrix(f44ProjMat, Math::deg2Rad(CAMERA_FOV), (float)viewportSize[0] / (float)viewportSize[1], fZNear, fZFar);
    gmtl::invertFull((Matrix44f&)f44InvProjMat, (Matrix44f&)f44ProjMat);

    // Calculate some composite matrices
    gmtl::invertFull((Matrix44f&)f44InvViewMat, (Matrix44f&)f44ViewMat);
    f44PrevViewProjMat = f44ViewProjMat; // View-projection matrix from last frame
    f44ViewProjMat = f44ProjMat * f44ViewMat;
    f44InvViewProjMat = f44InvViewMat * f44InvProjMat;

    // For the first frame, set the last frame's view-projection matrix
    // to the current frame's view-projection matrix
    if (f44PrevViewProjMat.GetCurrentValue() == MAT_IDENTITY44F)
        f44PrevViewProjMat = f44ViewProjMat;
}

void GITechDemo::UpdateUIFocus()
{
    if (m_pInputManager && m_pInputMap)
    {
        const unsigned int maxButtonsDown = 32;
        gainput::DeviceButtonSpec buttonsDown[maxButtonsDown];
        const unsigned int reportedNumButtonsDown = (unsigned int)m_pInputManager->GetAnyButtonDown(buttonsDown, maxButtonsDown);

        // Ignore mouse axes, they're always active
        unsigned int numButtonsDown = reportedNumButtonsDown;
        for (unsigned int i = 0; i < reportedNumButtonsDown; i++)
        {
            if (m_pInputManager->GetDevice(buttonsDown[i].deviceId)->GetType() == gainput::InputDevice::DT_MOUSE)
            {
                if (buttonsDown[i].buttonId == gainput::MouseAxisX || buttonsDown[i].buttonId == gainput::MouseAxisY)
                {
                    numButtonsDown--;
                }
            }
        }

        if (numButtonsDown == 0)
        {
            if (IsUIInFocus())
            {
                if (!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemActive())
                {
                    m_bUIHasFocus = false;
                }
            }
            else
            {
                if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemActive())
                {
                    m_bUIHasFocus = true;
                }
            }
        }
    }

    if (const ImGuiWindow* const navWindow = ImGui::GetCurrentContext()->NavWindow)
    {
        if (const ImGuiWindow* const focusedRootWindow = navWindow->RootWindow)
        {
            if (focusedRootWindow->Flags & ImGuiWindowFlags_Modal)
            {
                m_bUIHasFocus = true;
            }
        }
    }
}

void GITechDemo::Draw()
{
    Renderer* RenderContext = Renderer::GetInstance();
    if (!RenderContext)
        return;

    // For maximum parallelism, swap buffers at the beginning of the next CPU frame,
    // so as the GPU has as much time as possible to process the last frame.
    if (RenderContext->GetDeviceState() == DS_PRESENTING)
    {
        RenderContext->SwapBuffers();
    }

    if (RenderContext->BeginFrame())
    {
        RenderScheme::Draw();
        RenderContext->EndFrame();
    }
}
