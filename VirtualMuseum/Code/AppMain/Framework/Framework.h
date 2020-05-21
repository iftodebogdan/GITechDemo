/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Framework.h
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

#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

#include <string>

namespace AppFramework
{
    class Framework
    {
    public:
        Framework()
            : m_bQuit(false)
            , m_bPauseRendering(false)
            , m_bPauseUpdate(false)
            , m_fDeltaTime(0.f)
            , m_eWindowMode(WM_WINDOWED)
            , m_szTitle("PLACEHOLDER")
        { m_pInstance = this; };
        virtual ~Framework() { m_pInstance = nullptr; };

        virtual int Run() = 0;
        static Framework* const GetInstance() { return m_pInstance; }

        const bool  IsRenderingPaused() const { return m_bPauseRendering; }
        const bool  IsUpdatePaused() const { return m_bPauseUpdate; }

        const bool  IsFullscreen() const { return m_eWindowMode == WM_FULLSCREEN; }
        const bool  IsWindowed() const { return m_eWindowMode == WM_WINDOWED; }
        const bool  IsBorderlessWindow() const { return m_eWindowMode == WM_BORDERLESS; }

        // Low level, platform specific functionality required by the application
        virtual void ShowCursor(const bool bShow) = 0;
        virtual bool IsCursorHidden() = 0;
        virtual void SetCursorAtPos(const int x, const int y) = 0;
        virtual void GetClientArea(int& left, int& top, int& right, int& bottom) = 0;
        virtual void GetWindowArea(int& left, int& top, int& right, int& bottom) = 0;
        virtual void CreateMessageBox(const char* const title, const char* const body) = 0;

        virtual unsigned int    GetTicks() = 0; // in microseconds
        virtual void            Sleep(const unsigned int miliseconds) = 0;

        virtual void OnSwitchToFullscreenMode() { m_eWindowMode = WM_FULLSCREEN; }
        virtual void OnSwitchToWindowedMode() { m_eWindowMode = WM_WINDOWED; }
        virtual void OnSwitchToBorderlessWindowedMode() { m_eWindowMode = WM_BORDERLESS; }

        void Quit() { m_bQuit = true; }
        const std::string& GetWindowTitle() { return m_szTitle; }

        void PauseRendering(const bool pauseEnable) { m_bPauseRendering = pauseEnable; }
        void PauseUpdate(const bool pauseEnable) { m_bPauseUpdate = pauseEnable; }

        float GetDeltaTime() const { return m_fDeltaTime; } // in seconds

    protected:
        enum WindowMode
        {
            WM_WINDOWED,
            WM_BORDERLESS,
            WM_FULLSCREEN,
            WM_MAX
        } m_eWindowMode;

        virtual float CalculateDeltaTime() = 0; // in seconds

        bool m_bQuit;
        bool m_bPauseRendering; // Pause rendering when not in focus
        bool m_bPauseUpdate;
        float m_fDeltaTime; // in seconds
        std::string m_szTitle;
        static Framework* m_pInstance;
    };
}

#endif // FRAMEWORK_H_
