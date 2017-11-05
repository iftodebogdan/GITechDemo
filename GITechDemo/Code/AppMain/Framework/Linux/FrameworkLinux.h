/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   FrameworkLinux.h
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

#ifndef FRAMEWORK_WIN_H_
#define FRAMEWORK_WIN_H_

#include "Framework.h"

#define MAX_LOADSTRING 100

namespace AppFramework
{
    class FrameworkLinux : public Framework
    {
    public:
        FrameworkLinux() {};
        ~FrameworkLinux() {};

        int Run();

        void ShowCursor(const bool bShow) {}
        bool IsCursorHidden() { return false; }
        void SetCursorAtPos(const int x, const int y) {}
        void GetClientArea(int& left, int& top, int& right, int& bottom) {}
        void GetWindowArea(int& left, int& top, int& right, int& bottom) {}

        unsigned int    GetTicks() { return 0u; } // in microseconds
        void            Sleep(const unsigned int miliseconds) {}

        void OnSwitchToFullscreenMode() {}
        void OnSwitchToWindowedMode() {}
        void OnSwitchToBorderlessWindowedMode() {}

    private:
        float       CalculateDeltaTime() { return 0.f; } // in miliseconds

        void OnSetFullscreenCursor() {}
        void OnSetWindowedCursor() {}

    };
}

#endif // FRAMEWORK_WIN_H_
