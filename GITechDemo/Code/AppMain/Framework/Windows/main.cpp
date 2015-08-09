#include "stdafx.h"
#include "resource.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "App.h"
#include <gmtl\gmtl.h>
using namespace gmtl;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window c
HWND hWnd;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// Variables used for user interaction
bool bLMB = false;
bool bRMB = false;
bool bMMB = false;

void HandleUserInput(MSG msg, Vec2i& pos, Vec2i& center);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	// Allocate the console
	AllocConsole();

	// Connect I/O streams
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((intptr_t)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
#endif

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FRAMEWORK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FRAMEWORK));

	int xPos = 0;
	int yPos = 0;
	Vec2i delta;
	Vec2i pos;

	while (true)
	{
		RECT wRect;
		GetWindowRect(hWnd, &wRect);
		Vec2i center = Vec2i((wRect.right - wRect.left) / 2, (wRect.bottom - wRect.top) / 2);

		// Main message loop:
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			
			HandleUserInput(msg, pos, center);
		}

		// If the message is WM_QUIT, exit the while loop
		if (msg.message == WM_QUIT)
			break;

		// Update camera
		delta = pos - center;
		center += Vec2i(wRect.left, wRect.top);

		if (bLMB)
		{
			AppMain->GetCamera().mRot = makeRot(EulerAngleXYZf(Math::deg2Rad(-(float)delta[1] / 10.f), Math::deg2Rad(-(float)delta[0] / 10.f), 0), Type2Type<Matrix44f>()) * AppMain->GetCamera().mRot;
			SetCursorPos(center[0], center[1]);
		}

		if (bRMB)
		{
			AppMain->GetCamera().mRot = makeRot(EulerAngleXYZf(0.f, 0.f, Math::deg2Rad((float)delta[0] / 10.f)), Type2Type<Matrix44f>()) * AppMain->GetCamera().mRot;
			SetCursorPos(center[0], center[1]);
		}

		// Calculate delta time and update app
		static unsigned long long lastTick = GetTickCount64();
		const unsigned long long currTick = GetTickCount64();
		AppMain->Update((float)(currTick - lastTick) / 1000.f);
		lastTick = currTick;

		// Draw scene
		AppMain->Draw();
	}

	delete AppMain;

#ifdef _DEBUG
	// Free the console
	FreeConsole();
#endif

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FRAMEWORK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FRAMEWORK);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	AppMain->Init(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	
	RECT rc;
	GetClientRect(hWnd, &rc);

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_ERASEBKGND:
		return 1;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void HandleUserInput(MSG msg, Vec2i& pos, Vec2i& center)
{
	POINT p;
	switch (msg.message)
	{
	case WM_LBUTTONDOWN:
		pos = center;
		bLMB = true;
		ShowCursor(false);
		break;
	case WM_LBUTTONUP:
		ShowCursor(true);
		bLMB = false;
		break;

	case WM_RBUTTONDOWN:
		pos = center;
		bRMB = true;
		ShowCursor(false);
		break;
	case WM_RBUTTONUP:
		ShowCursor(true);
		bRMB = false;
		break;

	case WM_MBUTTONDOWN:
		pos = center;
		bMMB = true;
		ShowCursor(false);
		break;
	case WM_MBUTTONUP:
		ShowCursor(true);
		bMMB = false;
		break;

	case WM_MOUSEMOVE:
		if (bLMB || bRMB || bMMB)
		{
			WINDOWINFO wi;
			memset(&wi, 0, sizeof(WINDOWINFO));
			GetWindowInfo(hWnd, &wi);

			GetCursorPos(&p);
			ScreenToClient(hWnd, &p);

			pos = Vec2i(p.x, p.y) + Vec2i(wi.rcClient.left - wi.rcWindow.left, wi.rcClient.top - wi.rcWindow.top);
		}
		break;

	case WM_KEYDOWN:
		switch (msg.wParam)
		{
		case 'W':
			AppMain->GetCamera().vMoveVec[2] = 1.f;
			break;
		case 'S':
			AppMain->GetCamera().vMoveVec[2] = -1.f;
			break;
		case 'A':
			AppMain->GetCamera().vMoveVec[0] = -1.f;
			break;
		case 'D':
			AppMain->GetCamera().vMoveVec[0] = 1.f;
			break;
		case VK_SHIFT:
			AppMain->GetCamera().fSpeedFactor = 5.f;
			break;
		case VK_CONTROL:
			AppMain->GetCamera().fSpeedFactor = 0.1f;
		}
		break;

	case WM_KEYUP:
		switch (msg.wParam)
		{
		case 'W':
			AppMain->GetCamera().vMoveVec[2] = 0.f;
			break;
		case 'S':
			AppMain->GetCamera().vMoveVec[2] = 0.f;
			break;
		case 'A':
			AppMain->GetCamera().vMoveVec[0] = 0.f;
			break;
		case 'D':
			AppMain->GetCamera().vMoveVec[0] = 0.f;
			break;
		case VK_SHIFT:
			AppMain->GetCamera().fSpeedFactor = 1.f;
			break;
		case VK_CONTROL:
			AppMain->GetCamera().fSpeedFactor = 1.f;
		}
		break;
	}
}
