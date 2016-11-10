/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   App.h
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

#ifndef APP_H_
#define APP_H_

#include <gainput/gainput.h>

#include <gmtl\gmtl.h>
using namespace gmtl;

#define CREATE_APP(CLASS) \
    CLASS AppMainRef; \
    AppFramework::App* AppFramework::AppMain = &AppMainRef;

#define IMPLEMENT_APP(CLASS) \
    CLASS (); \
    ~ CLASS (); \
    bool Init(void* hWnd); \
    void Release(); \
    void LoadResources(unsigned int thId, unsigned int thCount); \
    void Update(const float fDeltaTime); \
    void Draw();

namespace gainput
{
    class InputManager;
}

namespace AppFramework
{
    class App
    {
    public:
        App() { m_pInputManager = new gainput::InputManager(); }
        virtual ~App() { if (m_pInputManager) delete m_pInputManager; }

        virtual bool Init(void* hWnd) = 0;
        virtual void Release() = 0;
        virtual void LoadResources(unsigned int thId, unsigned int thCount) = 0;
        virtual void Update(const float fDeltaTime) = 0;
        virtual void Draw() = 0;

        gainput::InputManager* GetInputManager() { return m_pInputManager; }

    protected:
        gainput::InputManager* m_pInputManager;
    };

    extern App* AppMain;
}

#endif