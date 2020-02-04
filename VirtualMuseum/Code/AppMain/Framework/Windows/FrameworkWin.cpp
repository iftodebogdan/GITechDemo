/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   FrameworkWin.cpp
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

#include "Resource.h"

#include "App.h"
#include "FrameworkWin.h"
#include "Timestamp.h"
using namespace AppFramework;

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <mmsystem.h>
#include <strsafe.h>
using namespace std;

namespace AppFramework
{
    LRESULT CALLBACK WndProc_wrapper(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ((FrameworkWin*)FrameworkWin::m_pInstance)->WndProc(hWnd, message, wParam, lParam);
    }

    INT_PTR CALLBACK About_wrapper(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ((FrameworkWin*)FrameworkWin::m_pInstance)->About(hDlg, message, wParam, lParam);
    }

    DWORD WINAPI AppMainLoadResources_wrapper(LPVOID args)
    {
        // 'args' is of type unsigned int[2] where:
        // args[0] = Thread ID
        // args[1] = Thread count
        AppMain->LoadResources(((unsigned int*)args)[0], ((unsigned int*)args)[1]);
        return 0;
    }
}

void FrameworkWin::Init(HINSTANCE& hInstance, int& nCmdShow)
{
    m_hInstance = hInstance;
    m_nCmdShow = nCmdShow;
}

int FrameworkWin::Run()
{
    // TODO: Place code here.
    MSG msg; memset(&msg, 0, sizeof(msg));
    HACCEL hAccelTable;

    char szTitle[MAX_LOADSTRING];

    // Initialize global strings
    LoadString(m_hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    m_szTitle = string(szTitle) + " (" + _CONFIGURATION  + "|" + _PLATFORM + " - " + GetBuildDate() + " " + GetBuildTime() + ")";// szTitleSuffix;

    LoadString(m_hInstance, IDC_FRAMEWORK, m_szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(m_hInstance);

    // Perform application initialization:
    if (!InitInstance(m_hInstance, m_nCmdShow))
    {
        return FALSE;
    }

#if (1) //_DEBUG
    // Allocate the console
    AllocConsole();
    FILE* streamOut = nullptr;
    FILE* streamIn = nullptr;
    freopen_s(&streamOut, "CONOUT$", "w", stdout);
    freopen_s(&streamIn, "CONIN$", "r", stdin);
#endif

    hAccelTable = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDC_FRAMEWORK));

    // Force display and system to be in a working state by resetting
    // the display and the system idle timer, respectively
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);

    bool bAppRdy = false;               // App can be updated
    PHANDLE hThread = nullptr;          // Array of thread handles
    unsigned int* nThArgs = nullptr;    // Array of thread arguments
    unsigned int startLoadingTicks = 0; // Timestamp for timing resource loading

    // Acquire system information
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    // Create one thread per CPU
    DWORD dwCPUCount = sysinfo.dwNumberOfProcessors;
    if (dwCPUCount < 1)
        dwCPUCount = 1;

    hThread = new HANDLE[dwCPUCount];
    memset(hThread, 0, sizeof(HANDLE) * dwCPUCount);

    nThArgs = new unsigned int[dwCPUCount * 2];
    memset(nThArgs, 0, sizeof(unsigned int) * dwCPUCount * 2);

    // Initialize app
    if (AppMain->Init(m_hWnd))
    {
        cout << "Starting " << dwCPUCount << " threads for loading resources." << endl << endl;
        startLoadingTicks = GetTicks();

        // Create some threads for loading data. Applications can decide how to schedule
        // their tasks based on provided thread index and total thread count.
        DWORD(WINAPI* pThread)(LPVOID) = &AppMainLoadResources_wrapper;
        for (unsigned int i = 0; i < dwCPUCount; i++)
        {
            // Set thread arguments and create thread
            nThArgs[i * 2] = i;
            nThArgs[i * 2 + 1] = dwCPUCount;
            hThread[i] = CreateThread(NULL, 0, pThread, nThArgs + i * 2, 0, NULL);

            if (!hThread[i])
            {
                ErrorExit(TEXT("CreateThread()"));
                m_bQuit = true;
            }
            else
            {
                // Set the loading threads' priorities to above normal
                //if (!SetThreadPriority(hThread[i], THREAD_PRIORITY_ABOVE_NORMAL))
                //{
                //    ErrorExit(TEXT("SetThreadPriority()"));
                //    m_bQuit = true;
                //}
            }
        }

        // Reduce this thread's priority since it isn't doing anything important right now
        //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    }
    else
    {
        ErrorExit(TEXT("AppMain->Init()"));
        m_bQuit = true;
    }

    while (!m_bQuit)
    {
        gainput::InputManager* pInputMgr = nullptr;
        if (AppMain && AppMain->GetInputManager())
            pInputMgr = AppMain->GetInputManager();

        // Main message loop:
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

            if (pInputMgr && bAppRdy)
                pInputMgr->HandleMessage(msg);

            // If the message is WM_QUIT, exit the while loop
            if (msg.message == WM_QUIT)
                m_bQuit = true;
        }

        // Update input
        if (pInputMgr && bAppRdy)
        {
            RECT rc;
            RECT wRect;
            GetWindowRect(m_hWnd, &wRect);
            GetClientRect(m_hWnd, &rc);
            const Vec2i viewportSize = Vec2i(rc.right - rc.left - 1, rc.bottom - rc.top - 1);
            pInputMgr->SetDisplaySize(viewportSize[0], viewportSize[1]);
            pInputMgr->Update();
        }

        if (bAppRdy)
        {
            CalculateDeltaTime();

            // Calculate delta time and update app
            AppMain->Update(IsUpdatePaused() ? 0.f : GetDeltaTime());

            // Draw scene
            if (!IsRenderingPaused())
                AppMain->Draw();
            else
                Sleep(1);
        }
        else
        {
            // Check if loading threads are done
            if (hThread)
            {
                bAppRdy = true;
                for (unsigned int i = 0; i < dwCPUCount; i++)
                {
                    if (hThread[i])
                    {
                        DWORD lpExitCode = 0;
                        if (GetExitCodeThread(hThread[i], &lpExitCode))
                        {
                            if (lpExitCode == STILL_ACTIVE)
                            {
                                bAppRdy = false;
                                break;
                            }
                        }
                        else
                        {
                            ErrorExit(TEXT("GetExitCodeThread()"));
                            m_bQuit = true;
                            break;
                        }
                    }
                }

                // Restore thread priority after resources are loaded
                if (bAppRdy)
                {
                    cout << endl << "Resources successfully loaded in " << (float)(GetTicks() - startLoadingTicks) / 1000000.f << " seconds." << endl;
                    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
                #ifndef _DEBUG
                    Sleep(2000);
                    // Free the console
                    FreeConsole();
                    if (streamIn)
                        fclose(streamIn);
                    if (streamOut)
                        fclose(streamOut);
                #endif
                    //ShowWindow(m_hWnd, SW_MAXIMIZE);
                    OnSwitchToWindowedMode();
                    BringWindowToTop(m_hWnd);
                    SetFocus(m_hWnd);
                    SetActiveWindow(m_hWnd);
                    SetForegroundWindow(m_hWnd);
                }

                // Give some CPU time to other threads
                Sleep(1);
            }
            else
                m_bQuit = true;
        }
    }

    WaitForMultipleObjects(dwCPUCount, hThread, TRUE, INFINITE);
    for (unsigned int i = 0; i < dwCPUCount; i++)
    {
        if (hThread[i])
            CloseHandle(hThread[i]);
        hThread[i] = nullptr;
    }

    // Reset execution state
    SetThreadExecutionState(ES_CONTINUOUS);

    // Release app resources
    AppMain->Release();

    // Release previously allocated memory
    delete[] nThArgs;
    delete[] hThread;

    DestroyWindow(m_hWnd);

#if _DEBUG
    // Free the console
    FreeConsole();
    if (streamIn)
        fclose(streamIn);
    if (streamOut)
        fclose(streamOut);
#endif

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM FrameworkWin::MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc_wrapper;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FRAMEWORK));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL; //MAKEINTRESOURCE(IDC_FRAMEWORK);
    wcex.lpszClassName = m_szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
BOOL FrameworkWin::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    m_hWnd = CreateWindow(m_szWindowClass, m_szTitle.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!m_hWnd)
    {
        return FALSE;
    }

    //ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK FrameworkWin::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    RECT rc;
    GetClientRect(hWnd, &rc);

    switch (message)
    {
    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About_wrapper);
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
    case WM_KILLFOCUS:
    case WM_SETFOCUS:
    case WM_ACTIVATE:
    {
        bool bOutOfFocus = (message == WM_KILLFOCUS || (message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE));
#if !defined(_DEBUG) && !defined(_PROFILE)
        PauseRendering(bOutOfFocus);
#endif
        if (IsFullscreen() && bOutOfFocus)
            OnSetWindowedCursor();
        else if (IsFullscreen() && !bOutOfFocus)
            OnSetFullscreenCursor();

        // Pass focus messages to input handler so that it resets input state
        if (AppMain && AppMain->GetInputManager())
        {
            MSG msg; memset(&msg, 0, sizeof(msg));
            msg.hwnd = hWnd;
            msg.message = message;
            msg.wParam = wParam;
            msg.lParam = lParam;
            AppMain->GetInputManager()->HandleMessage(msg);
        }

        break;
    }
    //case WM_NCACTIVATE:   // Causes a focus issue with the assert dialog
    case WM_MOUSEACTIVATE:
    case WM_ACTIVATEAPP:
        if (IsCursorHidden())
            ShowCursor(true);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK FrameworkWin::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

void FrameworkWin::ShowCursor(const bool bShow)
{
    //if (IsFullscreen() && bShow)
    //    return;

    // Loop until counter is reset
    while((::ShowCursor(bShow) < 0) == bShow);
}

bool FrameworkWin::IsCursorHidden()
{
    CURSORINFO ci;
    ci.cbSize = sizeof(CURSORINFO);
    ::GetCursorInfo(&ci);
    return ci.flags != CURSOR_SHOWING;
}

void FrameworkWin::SetCursorAtPos(const int x, const int y)
{
    ::SetCursorPos(x, y);
}

void FrameworkWin::GetClientArea(int& left, int& top, int& right, int& bottom)
{
    RECT rc;
    ::GetClientRect(m_hWnd, &rc);
    left    = rc.left;
    top     = rc.top;
    right   = rc.right;
    bottom  = rc.bottom;
}

void FrameworkWin::GetWindowArea(int& left, int& top, int& right, int& bottom)
{
    RECT rc;
    ::GetWindowRect(m_hWnd, &rc);
    left    = rc.left;
    top     = rc.top;
    right   = rc.right;
    bottom  = rc.bottom;
}

unsigned int FrameworkWin::GetTicks()
{
    DWORD now = 0;
    LARGE_INTEGER hiresNow;

    if (!m_bTicksStarted)
    {
        m_bTicksStarted = true;

        /* Set first ticks value */
        /* QueryPerformanceCounter has had problems in the past, but lots of games
        use it, so we'll rely on it here.
        */
        if (QueryPerformanceFrequency(&m_bHiresTicksPerSecond) == TRUE)
        {
            m_bHiresTimerAvailable = TRUE;
            QueryPerformanceCounter(&m_bHiresStartTicks);
        }
        else
        {
            m_bHiresTimerAvailable = FALSE;
            m_nStartTicks = timeGetTime();
        }
    }

    if (m_bHiresTimerAvailable)
    {
        QueryPerformanceCounter(&hiresNow);

        hiresNow.QuadPart -= m_bHiresStartTicks.QuadPart;
        hiresNow.QuadPart *= 1000000;
        hiresNow.QuadPart /= m_bHiresTicksPerSecond.QuadPart;

        return (DWORD)hiresNow.QuadPart;
    }
    else
    {
        now = timeGetTime();
    }

    return (now - m_nStartTicks) * 1000;
}

float FrameworkWin::CalculateDeltaTime()
{
    const unsigned int ticksNow = GetTicks();
    if (m_nTicksPrev == 0) m_nTicksPrev = ticksNow;
    const unsigned int deltaTicks = ticksNow - m_nTicksPrev;
    m_nTicksPrev = ticksNow;
    m_fDeltaTime = (float)deltaTicks / 1000000.f;
    return m_fDeltaTime;
}

void FrameworkWin::Sleep(const unsigned int miliseconds)
{
    ::Sleep(miliseconds);
}

void FrameworkWin::ErrorExit(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf = NULL;
    LPVOID lpDisplayBuf = NULL;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and exit the process
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));

    if (lpMsgBuf && lpDisplayBuf)
    {
        StringCchPrintf((LPTSTR)lpDisplayBuf,
            LocalSize(lpDisplayBuf) / sizeof(TCHAR),
            TEXT("%s failed with error %d: %s"),
            lpszFunction, dw, (LPCTSTR)lpMsgBuf);
        MessageBox(m_hWnd, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
    }

    if(lpMsgBuf)
        LocalFree(lpMsgBuf);
    if(lpDisplayBuf)
        LocalFree(lpDisplayBuf);

    ExitProcess(dw);
}

void FrameworkWin::OnSwitchToFullscreenMode()
{
    Framework::OnSwitchToFullscreenMode();

    OutputDebugString("[FrameworkWin] Setting fullscreen mode window style\n");
    SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    OnSetFullscreenCursor();
}

void FrameworkWin::OnSwitchToWindowedMode()
{
    Framework::OnSwitchToWindowedMode();

    OutputDebugString("[FrameworkWin] Setting windowed mode window style\n");
    SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_CLIPSIBLINGS | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_GROUP | WS_TABSTOP);
    ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
    SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    OnSetWindowedCursor();
}

void FrameworkWin::OnSwitchToBorderlessWindowedMode()
{
    Framework::OnSwitchToBorderlessWindowedMode();

    OutputDebugString("[FrameworkWin] Setting windowed mode window style\n");
    SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
    SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    OnSetWindowedCursor();
}

void FrameworkWin::OnSetFullscreenCursor()
{
    const POINT pt = { 0, 0 };
    MONITORINFO mi = { sizeof(MONITORINFO) };
    GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &mi);

    ShowCursor(true);
    ClipCursor(&mi.rcMonitor);
}

void FrameworkWin::OnSetWindowedCursor()
{
    ShowCursor(true);
    ClipCursor(NULL);
}
