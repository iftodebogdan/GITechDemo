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