/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   GITechDemo.h
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

#ifndef GITECHDEMO_H_
#define GITECHDEMO_H_

#include <gmtl\gmtl.h>

#include "App.h"
using namespace AppFramework;

#include <Utility/Mutex.h>

namespace gainput
{
    class InputMap;
}

namespace GITechDemoApp
{
    class RenderPass;

    class GITechDemo : public App
    {
    public:
        IMPLEMENT_APP(GITechDemo)

        struct Camera
        {
            Camera() : fSpeedFactor(1.f) {}

            Vec3f vPos;
            Matrix44f mRot;
            Vec3f vMoveVec;
            float fSpeedFactor;
        };

        Camera& GetCamera() { return m_tCamera; }
        const float GetDeltaTime() const { return m_fDeltaTime; }
        const float GetDeltaTimeModifier() const { return m_fDeltaTimeModifier; }
        float& GetDeltaTimeModifier() { return m_fDeltaTimeModifier; }
        void SetDeltaTimeModifier(const float factor) { m_fDeltaTimeModifier = factor; }
        const bool IsUIInFocus() const { return m_bUIHasFocus; }
        void ForceUpdateResolution() { m_bForceResolutionUpdate = true; }

    private:
        void UpdateUIFocus();

        Camera m_tCamera;
        float m_fDeltaTime;
        float m_fDeltaTimeModifier;
        gainput::InputMap* m_pInputMap;
        int m_nLastFrameResX, m_nLastFrameResY, m_nLastFrameRefreshRate, m_nLastFrameVSyncInterval;
        bool m_bForceResolutionUpdate;
        Vec2i m_vLastFrameViewport;
        bool m_bLastFrameFullscreen, m_bLastFrameBorderless, m_bLastFrameVSync;
        bool m_bUIHasFocus;

        MUTEX mResInitMutex;

        enum Command
        {
            APP_CMD_NONE        =           0,                  // No input
            APP_CMD_CTRL_YAW    =           1,                  // Enable yaw control 
            APP_CMD_CTRL_PITCH  = APP_CMD_CTRL_YAW      << 1,   // Enable pitch control
            APP_CMD_CTRL_ROLL   = APP_CMD_CTRL_PITCH    << 1,   // Enable roll control
            APP_CMD_YAW_AXIS    = APP_CMD_CTRL_ROLL     << 1,   // Analog yaw input
            APP_CMD_PITCH_AXIS  = APP_CMD_YAW_AXIS      << 1,   // Analog pitch input
            APP_CMD_ROLL_AXIS   = APP_CMD_PITCH_AXIS    << 1,   // Analog roll input
            APP_CMD_FORWARD     = APP_CMD_ROLL_AXIS     << 1,   // Camera move forward
            APP_CMD_BACKWARD    = APP_CMD_FORWARD       << 1,   // Camera move backward
            APP_CMD_LEFT        = APP_CMD_BACKWARD      << 1,   // Camera move left
            APP_CMD_RIGHT       = APP_CMD_LEFT          << 1,   // Camera move right
            APP_CMD_SPEED_UP    = APP_CMD_RIGHT         << 1,   // Enable faster moving camera
            APP_CMD_SLOW_DOWN   = APP_CMD_SPEED_UP      << 1    // Enable slower moving camera
        };
    };
}
#endif