#include "stdafx.h"

#include <gainput/gainput.h>

#include "ArtistParameter.h"
#include "RenderResourcesDef.h"
#include "HUDPass.h"
using namespace GITechDemoApp;

#define MAX_STEP_MULTIPLIER (10.f)
#define MIN_STEP_MULTIPLIER (0.1f)

vector<ArtistParameter*> ArtistParameterManager::ms_arrParams;
ArtistParameterManager* ArtistParameterManager::ms_pInstance = nullptr;

namespace GITechDemoApp
{
	extern HUDPass HUD_PASS;
}

ArtistParameterManager::ArtistParameterManager()
	: m_nCurrParam(-1)
	, m_pInputMap(nullptr)
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
		m_nCurrParam < (int)ms_arrParams.size() - 1 ? m_nCurrParam++ : m_nCurrParam  = -1;
	else if(cmd & APM_CMD_PREV)
		m_nCurrParam > -1 ? m_nCurrParam-- : m_nCurrParam = (int)ms_arrParams.size() - 1;

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

		char szTempBuffer[2048];
		if (pCurrAP->nTypeHash == typeid(float).hash_code())
		{
			sprintf_s(szTempBuffer, "Current: %g", *(float*)pCurrAP->pParam);
			HUD_PASS.PrintLn(szTempBuffer);
			sprintf_s(szTempBuffer, "Step:     %g", fStep);
			HUD_PASS.PrintLn(szTempBuffer);

			if (cmd & APM_CMD_STEP_UP)
				*(float*)pCurrAP->pParam += fStep;
			else if (cmd & APM_CMD_STEP_DOWN)
				*(float*)pCurrAP->pParam -= fStep;
		}
		else if (pCurrAP->nTypeHash == typeid(int).hash_code())
		{
			sprintf_s(szTempBuffer, "Current: %d", *(int*)pCurrAP->pParam);
			HUD_PASS.PrintLn(szTempBuffer);
			sprintf_s(szTempBuffer, "Step:     %d", (int)fStep);
			HUD_PASS.PrintLn(szTempBuffer);

			if (cmd & APM_CMD_STEP_UP)
				*(int*)pCurrAP->pParam += (int)fStep;
			else if (cmd & APM_CMD_STEP_DOWN)
				*(int*)pCurrAP->pParam -= (int)fStep;
		}
		else if (pCurrAP->nTypeHash == typeid(unsigned int).hash_code())
		{
			sprintf_s(szTempBuffer, "Current: %u", *(unsigned int*)pCurrAP->pParam);
			HUD_PASS.PrintLn(szTempBuffer);
			sprintf_s(szTempBuffer, "Step:     %u", (unsigned int)fStep);
			HUD_PASS.PrintLn(szTempBuffer);

			if (cmd & APM_CMD_STEP_UP)
				*(unsigned int*)pCurrAP->pParam += (unsigned int)fStep;
			else if (cmd & APM_CMD_STEP_DOWN)
				*(unsigned int*)pCurrAP->pParam -= (unsigned int)fStep;
		}
		else if (pCurrAP->nTypeHash == typeid(bool).hash_code())
		{
			sprintf_s(szTempBuffer, "Current: %s", *(bool*)pCurrAP->pParam ? "True" : "False");
			HUD_PASS.PrintLn(szTempBuffer);
			sprintf_s(szTempBuffer, "Step:     Negate", fStep);
			HUD_PASS.PrintLn(szTempBuffer);

			if (cmd & (APM_CMD_STEP_UP | APM_CMD_STEP_DOWN))
				*(bool*)pCurrAP->pParam = !(*(bool*)pCurrAP->pParam);
		}
		else
			assert(0);

		HUD_PASS.PrintLn("");
		HUD_PASS.PrintLn("Hold RCtrl or RShift to scale step value.");
	}
}

void ArtistParameterManager::SetupInput(gainput::InputManager* pInputManager)
{
	if (m_pInputMap || !pInputManager)
		return;

	gainput::DeviceId keyboardId = pInputManager->CreateDevice<gainput::InputDeviceKeyboard>(gainput::InputDevice::AutoIndex, gainput::InputDevice::DV_RAW);

	m_pInputMap = new gainput::InputMap(*pInputManager);

	m_pInputMap->MapBool(APM_CMD_NEXT,		keyboardId,	gainput::KeyDown);
	m_pInputMap->MapBool(APM_CMD_PREV,		keyboardId,	gainput::KeyUp);
	m_pInputMap->MapBool(APM_CMD_STEP_UP,	keyboardId,	gainput::KeyRight);
	m_pInputMap->MapBool(APM_CMD_STEP_DOWN,	keyboardId,	gainput::KeyLeft);
	m_pInputMap->MapBool(APM_CMD_MIN_STEP,	keyboardId,	gainput::KeyCtrlR);
	m_pInputMap->MapBool(APM_CMD_MAX_STEP,	keyboardId,	gainput::KeyShiftR);
}