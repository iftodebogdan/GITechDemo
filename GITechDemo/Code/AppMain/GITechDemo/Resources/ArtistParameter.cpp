/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   ArtistParameter.cpp
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

#include <gainput/gainput.h>

#include "ArtistParameter.h"
#include "HUDPass.h"
using namespace GITechDemoApp;

#define MAX_STEP_MULTIPLIER (10.f)
#define MIN_STEP_MULTIPLIER (0.1f)

// Moved to AppResources.cpp until the issue with the static initialization fiasco is resolved
//vector<ArtistParameter*> ArtistParameterManager::ms_arrParams;
ArtistParameterManager* ArtistParameterManager::ms_pInstance = nullptr;

namespace GITechDemoApp
{
    extern HUDPass HUD_PASS;

    const unsigned long long g_TypeHash[4] =
    {
        typeid(float).hash_code(),
        typeid(int).hash_code(),
        typeid(unsigned int).hash_code(),
        typeid(bool).hash_code()
    };

#define IS_FLOAT(ArtistParamPtr)    (ArtistParamPtr->nTypeHash == g_TypeHash[0])
#define IS_INT(ArtistParamPtr)      (ArtistParamPtr->nTypeHash == g_TypeHash[1])
#define IS_UINT(ArtistParamPtr)     (ArtistParamPtr->nTypeHash == g_TypeHash[2])
#define IS_BOOL(ArtistParamPtr)     (ArtistParamPtr->nTypeHash == g_TypeHash[3])
}

ArtistParameterManager::ArtistParameterManager()
    : m_pInputMap(nullptr)
    , m_nCurrParam(-1)
{
    if (ms_pInstance)
        delete ms_pInstance;

    ms_pInstance = this;
}

ArtistParameterManager::~ArtistParameterManager()
{
    if (m_pInputMap)
    {
        delete m_pInputMap;
        m_pInputMap = nullptr;
    }
}

void ArtistParameterManager::Update()
{
    if (ms_arrParams.size() == 0 || !m_pInputMap)
        return;

    // Update input
    unsigned int cmd = ArtistParameterManager::APM_CMD_NONE;
    if (m_pInputMap->GetBoolWasDown(ArtistParameterManager::APM_CMD_NEXT))
        cmd |= ArtistParameterManager::APM_CMD_NEXT;
    if (m_pInputMap->GetBoolWasDown(ArtistParameterManager::APM_CMD_PREV))
        cmd |= ArtistParameterManager::APM_CMD_PREV;
    if (m_pInputMap->GetBoolWasDown(ArtistParameterManager::APM_CMD_STEP_UP))
        cmd |= ArtistParameterManager::APM_CMD_STEP_UP;
    if (m_pInputMap->GetBoolWasDown(ArtistParameterManager::APM_CMD_STEP_DOWN))
        cmd |= ArtistParameterManager::APM_CMD_STEP_DOWN;
    if (m_pInputMap->GetBool(ArtistParameterManager::APM_CMD_MIN_STEP))
        cmd |= ArtistParameterManager::APM_CMD_MIN_STEP;
    if (m_pInputMap->GetBool(ArtistParameterManager::APM_CMD_MAX_STEP))
        cmd |= ArtistParameterManager::APM_CMD_MAX_STEP;

    // Handle input
    if(cmd & APM_CMD_NEXT)
	{
        if (cmd & APM_CMD_MAX_STEP)
        {
            string szCategory = m_nCurrParam != -1 ? ms_arrParams[m_nCurrParam]->szCategory : "";
            for (unsigned int i = 0; i < ms_arrParams.size() + 1; i++)
            {
                m_nCurrParam < (int)ms_arrParams.size() - 1 ? m_nCurrParam++ : m_nCurrParam = -1;
                if (m_nCurrParam == -1 || ms_arrParams[m_nCurrParam]->szCategory != szCategory)
                    break;
            }
        }
        else
        {
            m_nCurrParam < (int)ms_arrParams.size() - 1 ? m_nCurrParam++ : m_nCurrParam = -1;
        }
	}
    else
	{
        if(cmd & APM_CMD_PREV)
		{
            if (cmd & APM_CMD_MAX_STEP)
            {
                string szCategory = m_nCurrParam != -1 ? ms_arrParams[m_nCurrParam]->szCategory : "";
                string szCategory2;
                for (unsigned int i = 0; i < ms_arrParams.size(); i++)
                {
                    m_nCurrParam > -1 ? m_nCurrParam-- : m_nCurrParam = (int)ms_arrParams.size() - 1;
                    if (m_nCurrParam == -1 && szCategory2.empty())
                    {
                        break;
                    }
                    else
                    {
                        if (m_nCurrParam == -1 || ms_arrParams[m_nCurrParam]->szCategory != szCategory)
                        {
                            if (szCategory2.empty() && m_nCurrParam != -1)
                            {
                                szCategory2 = ms_arrParams[m_nCurrParam]->szCategory;
                                continue;
                            }

                            if (m_nCurrParam == -1 || ms_arrParams[m_nCurrParam]->szCategory != szCategory2)
                            {
                                m_nCurrParam < (int)ms_arrParams.size() - 1 ? m_nCurrParam++ : m_nCurrParam = -1;
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                m_nCurrParam > -1 ? m_nCurrParam-- : m_nCurrParam = (int)ms_arrParams.size() - 1;
            }
		}
	}

    ArtistParameter* const pCurrAP = m_nCurrParam != -1 ? ms_arrParams[m_nCurrParam] : nullptr;

    if (pCurrAP)
    {
        HUD_PASS.PrintLn(("Category:    " + pCurrAP->szCategory).c_str());
        HUD_PASS.PrintLn(("Name:         " + pCurrAP->szName).c_str());
        HUD_PASS.PrintLn(("Description: " + pCurrAP->szDesc).c_str());
        HUD_PASS.PrintLn("");

        float fStep = pCurrAP->pStepValue;
        if (cmd & APM_CMD_MAX_STEP)
            fStep *= MAX_STEP_MULTIPLIER;
        else if (cmd & APM_CMD_MIN_STEP)
            fStep *= MIN_STEP_MULTIPLIER;

        const unsigned int bufferSize = 2048;
        char szTempBuffer[bufferSize];
        if (IS_FLOAT(pCurrAP))
        {
            snprintf(szTempBuffer, bufferSize, "Current: %g", *(float*)pCurrAP->pParam);
            HUD_PASS.PrintLn(szTempBuffer);
            snprintf(szTempBuffer, bufferSize, "Step:     %g", fStep);
            HUD_PASS.PrintLn(szTempBuffer);

            if (cmd & APM_CMD_STEP_UP)
                *(float*)pCurrAP->pParam += fStep;
            else if (cmd & APM_CMD_STEP_DOWN)
                *(float*)pCurrAP->pParam -= fStep;
        }
        else if (IS_INT(pCurrAP))
        {
            snprintf(szTempBuffer, bufferSize, "Current: %d", *(int*)pCurrAP->pParam);
            HUD_PASS.PrintLn(szTempBuffer);
            snprintf(szTempBuffer, bufferSize, "Step:     %d", (int)fStep);
            HUD_PASS.PrintLn(szTempBuffer);

            if (cmd & APM_CMD_STEP_UP)
                *(int*)pCurrAP->pParam += (int)fStep;
            else if (cmd & APM_CMD_STEP_DOWN)
                *(int*)pCurrAP->pParam -= (int)fStep;
        }
        else if (IS_UINT(pCurrAP))
        {
            snprintf(szTempBuffer, bufferSize, "Current: %u", *(unsigned int*)pCurrAP->pParam);
            HUD_PASS.PrintLn(szTempBuffer);
            snprintf(szTempBuffer, bufferSize, "Step:     %u", (unsigned int)fStep);
            HUD_PASS.PrintLn(szTempBuffer);

            if (cmd & APM_CMD_STEP_UP)
                *(unsigned int*)pCurrAP->pParam += (unsigned int)fStep;
            else if (cmd & APM_CMD_STEP_DOWN)
                *(unsigned int*)pCurrAP->pParam -= (unsigned int)fStep;
        }
        else if (IS_BOOL(pCurrAP))
        {
            snprintf(szTempBuffer, bufferSize, "Current: %s", *(bool*)pCurrAP->pParam ? "True" : "False");
            HUD_PASS.PrintLn(szTempBuffer);
            snprintf(szTempBuffer, bufferSize, "Step:     Negate");
            HUD_PASS.PrintLn(szTempBuffer);

            if (cmd & (APM_CMD_STEP_UP | APM_CMD_STEP_DOWN))
                *(bool*)pCurrAP->pParam = !(*(bool*)pCurrAP->pParam);
        }
        else
            assert(0); // Support for data type hasn't been implemented!
        
        if (pCurrAP->nTypeHash != typeid(bool).hash_code())
        {
            HUD_PASS.PrintLn("");
            HUD_PASS.PrintLn("Hold RCtrl (x0.1) or RShift (x10) to scale step value.");
        }
    }
}

void ArtistParameterManager::SetupInput(gainput::InputManager* pInputManager)
{
    if (m_pInputMap || !pInputManager)
        return;

    gainput::DeviceId keyboardId = pInputManager->CreateDevice<gainput::InputDeviceKeyboard>(gainput::InputDevice::AutoIndex, gainput::InputDevice::DV_RAW);

    m_pInputMap = new gainput::InputMap(*pInputManager);

    m_pInputMap->MapBool(APM_CMD_NEXT,      keyboardId, gainput::KeyDown);
    m_pInputMap->MapBool(APM_CMD_PREV,      keyboardId, gainput::KeyUp);
    m_pInputMap->MapBool(APM_CMD_STEP_UP,   keyboardId, gainput::KeyRight);
    m_pInputMap->MapBool(APM_CMD_STEP_DOWN, keyboardId, gainput::KeyLeft);
    m_pInputMap->MapBool(APM_CMD_MIN_STEP,  keyboardId, gainput::KeyCtrlR);
    m_pInputMap->MapBool(APM_CMD_MAX_STEP,  keyboardId, gainput::KeyShiftR);
}
