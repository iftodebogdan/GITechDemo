#ifndef FRAMEWORK_WIN_H_
#define FRAMEWORK_WIN_H_

#include "Framework.h"

#define MAX_LOADSTRING 100

namespace AppFramework
{
	class FrameworkWin : public Framework
	{
	public:
		FrameworkWin()
			: m_nTicksPrev(0)
			, m_bTicksStarted(false)
			, m_bHiresTimerAvailable(false)
		{};
		~FrameworkWin() {};

		void Init(HINSTANCE& hInstance, int& nCmdShow);
		int Run();

		void ShowCursor(const bool bShow);
		bool IsCursorHidden();
		void SetCursorAtPos(const int x, const int y);
		void GetClientArea(int& left, int& top, int& right, int& bottom);
		void GetWindowArea(int& left, int& top, int& right, int& bottom);

		unsigned int	GetTicks(); // in microseconds
		void			Sleep(const unsigned int miliseconds);

	private:
		// Windows specific stuff
		ATOM				MyRegisterClass(HINSTANCE hInstance);
		BOOL				InitInstance(HINSTANCE hInstance, int nCmdShow);
		LRESULT CALLBACK	WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		INT_PTR CALLBACK	About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

		HINSTANCE	m_hInstance;
		TCHAR		m_szTitle[MAX_LOADSTRING];
		TCHAR		m_szWindowClass[MAX_LOADSTRING];
		HWND		m_hWnd;
		int			m_nCmdShow;

		float		CalculateDeltaTime(); // in miliseconds

		// Error handling
		void ErrorExit(LPTSTR lpszFunction);

		// High resolution timer, adapted from SDL
		// The first (low-resolution) ticks value of the application
		unsigned int	m_nStartTicks;
		bool			m_bTicksStarted;
		// Store if a high-resolution performance counter exists on the system
		bool			m_bHiresTimerAvailable;
		// The first high-resolution ticks value of the application
		LARGE_INTEGER	m_bHiresStartTicks;
		// The number of ticks per second of the high-resolution performance counter
		LARGE_INTEGER	m_bHiresTicksPerSecond;
		unsigned int	m_nTicksPrev;

		friend LRESULT CALLBACK	WndProc_wrapper(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		friend INT_PTR CALLBACK	About_wrapper(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	};
}

#endif // FRAMEWORK_WIN_H_
