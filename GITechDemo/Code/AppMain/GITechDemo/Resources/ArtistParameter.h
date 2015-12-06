/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	ArtistParameter.h
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#ifndef ARTIST_PARAMETER_H_
#define ARTIST_PARAMETER_H_

#include <string>
#include <vector>
using namespace std;

namespace gainput
{
	class InputManager;
	class InputMap;
}

namespace GITechDemoApp
{
	class ArtistParameter;

	class ArtistParameterManager
	{
	public:
		ArtistParameterManager();
		~ArtistParameterManager();

		void Update();
		void SetupInput(gainput::InputManager* pInputManager);

		bool IsDrawingOnHUD() { return m_nCurrParam != -1; }
		unsigned int GetParameterCount() { return (unsigned int)ms_arrParams.size(); }

		static void CreateInstance() { ms_pInstance = new ArtistParameterManager(); }
		static void DestroyInstance() { if (ms_pInstance) delete ms_pInstance; ms_pInstance = nullptr; }
		static ArtistParameterManager* GetArtistParameterManager() { return ms_pInstance; }

	protected:
		static vector<ArtistParameter*>	ms_arrParams;
		static ArtistParameterManager*	ms_pInstance;

		gainput::InputMap*	m_pInputMap;

		int		m_nCurrParam;
		
		enum Command
		{
			APM_CMD_NONE		= 0,

			APM_CMD_NEXT		= 1,
			APM_CMD_PREV		= APM_CMD_NEXT << 1,
			APM_CMD_STEP_UP		= APM_CMD_PREV << 1,
			APM_CMD_STEP_DOWN	= APM_CMD_STEP_UP << 1,
			APM_CMD_MIN_STEP	= APM_CMD_STEP_DOWN << 1,
			APM_CMD_MAX_STEP	= APM_CMD_MIN_STEP << 1
		};

		friend class ArtistParameter;
	};

	class ArtistParameter
	{
	public:
		string				szName;
		string				szDesc;
		string				szCategory;

		void*				pParam;
		float				pStepValue;

		unsigned long long	nTypeHash;

		ArtistParameter(
			const char* const name,
			const char* const desc,
			const char* const category,
			void* const param,
			const float step,
			const unsigned long long typeHash)
			: szName(name)
			, szDesc(desc)
			, szCategory(category)
			, pParam(param)
			, pStepValue(step)
			, nTypeHash(typeHash)
		{
			ArtistParameterManager::ms_arrParams.push_back(this);
		}

		~ArtistParameter()
		{
			for (unsigned int i = 0; i < ArtistParameterManager::ms_arrParams.size(); i++)
				if (ArtistParameterManager::ms_arrParams[i] == this)
					ArtistParameterManager::ms_arrParams.erase(ArtistParameterManager::ms_arrParams.begin() + i);
		}
	};
}

#endif //ARTIST_PARAMETER_MANAGER_H_