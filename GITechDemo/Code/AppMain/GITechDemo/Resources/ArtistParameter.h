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