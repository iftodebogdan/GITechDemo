// GITechDemo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "GITechDemo.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <fstream>
#include <cstdlib>

#include <Renderer.h>
#include <../Utility/ColorUtility.h>
using namespace LibRendererDll;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window c
HWND hWnd;

VertexBuffer*	VBuf	= nullptr;
Texture*		Tex		= nullptr;
ShaderTemplate*	VShd	= nullptr;
ShaderTemplate*	PShd	= nullptr;
ShaderInput*	VSInput	= nullptr;
ShaderInput*	PSInput	= nullptr;
Model*			model	= nullptr;

bool bLMB = false;
bool bRMB = false;
Vec3f vMove;
float fSpeedFactor = 1.f;
struct Camera
{
	Vec3f vPos;
	Matrix44f mRot;
} tCamera;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void RenderScene();

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
	LoadString(hInstance, IDC_GITECHDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GITECHDEMO));

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

			case WM_MOUSEMOVE:
				if (bLMB || bRMB)
				{
					WINDOWINFO wi;
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
					vMove[2] = 1.f;
					break;
				case 'S':
					vMove[2] = -1.f;
					break;
				case 'A':
					vMove[0] = -1.f;
					break;
				case 'D':
					vMove[0] = 1.f;
					break;
				case VK_SHIFT:
					fSpeedFactor = 5.f;
					break;
				case VK_CONTROL:
					fSpeedFactor = 0.1f;
				}
				break;
				
			case WM_KEYUP:
				switch (msg.wParam)
				{
				case 'W':
					vMove[2] = 0.f;
					break;
				case 'S':
					vMove[2] = 0.f;
					break;
				case 'A':
					vMove[0] = 0.f;
					break;
				case 'D':
					vMove[0] = 0.f;
					break;
				case VK_SHIFT:
					fSpeedFactor = 1.f;
					break;
				case VK_CONTROL:
					fSpeedFactor = 1.0f;
				}
				break;
			}
		}

		delta = pos - center;
		center += Vec2i(wRect.left, wRect.top);

		if (bLMB)
		{
			tCamera.mRot = makeRot(EulerAngleXYZf(Math::deg2Rad(-(float)delta[1] / 10.f), Math::deg2Rad(-(float)delta[0] / 10.f), 0), Type2Type<Matrix44f>()) * tCamera.mRot;
			SetCursorPos(center[0], center[1]);
		}

		if (bRMB)
		{
			tCamera.mRot = makeRot(EulerAngleXYZf(0.f, 0.f, Math::deg2Rad((float)delta[0] / 10.f)), Type2Type<Matrix44f>()) * tCamera.mRot;
			SetCursorPos(center[0], center[1]);
		}

		// If the message is WM_QUIT, exit the while loop
		if (msg.message == WM_QUIT)
			break;

		RenderScene();
	}

	Renderer::GetInstance()->DestroyInstance();

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GITECHDEMO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GITECHDEMO);
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

	// Create an instance of the renderer
	Renderer::CreateInstance(API_DX9);
	//Renderer::CreateInstance(API_NULL);
	Renderer *renderer = Renderer::GetInstance();
	renderer->Initialize(hWnd);
	
	// The resource manager is the only way to
	// allocate resources to be used by the renderer
	// NB: when creating a resource, the resource manager
	// will return an index, with which you can retrieve
	// a pointer to the actual resource
	ResourceManager* resMan = renderer->GetResourceManager();
	
	// First of all, load a model file
	const unsigned int modelIdx = resMan->CreateModel("sponza\\sponza.lrm");
	model = resMan->GetModel(modelIdx);

	// Read the contents of the file
	std::ifstream t("simple.hlsl");
	//std::ifstream t("normal.hlsl");
	//std::ifstream t("normal_scale.hlsl");
	//std::ifstream t("pos_only.hlsl");
	int length;
	t.seekg(0, std::ios::end);			// go to the end
	length = (int)t.tellg();			// report location (this is the length)
	t.seekg(0, std::ios::beg);			// go back to the beginning
	char* buffer = new char[length];    // allocate memory for a buffer of appropriate dimension
	t.read(buffer, length);				// read the whole file into the buffer
	t.close();							// close file handle
	buffer[t.gcount()] = '\0';			// maybe there where less characters than expected (CRLF converted to LF)

	// Compile and create the vertex program
	const unsigned int vspIdx = resMan->CreateShaderProgram(SPT_VERTEX, buffer);
	// Compile and create the pixel program
	const unsigned int pspIdx = resMan->CreateShaderProgram(SPT_PIXEL, buffer);
	// Create a shader template for our vertex program
	const unsigned int vstIdx = resMan->CreateShaderTemplate(resMan->GetShaderProgram(vspIdx));
	VShd = resMan->GetShaderTemplate(vstIdx);
	// Create a shader template for our pixel program
	const unsigned int pstIdx = resMan->CreateShaderTemplate(resMan->GetShaderProgram(pspIdx));
	PShd = resMan->GetShaderTemplate(pstIdx);

	delete[] buffer;

	// Shader inputs are separated from templates so that one could use the same
	// shader with different inputs, without having to create multiple copies of
	// the same template. A ShaderTemplate is a descriptor for what the shader expects
	// as input, whereas a ShaderInput is the instance of those inputs.
	const unsigned int vsiIdx = resMan->CreateShaderInput(VShd);
	VSInput = resMan->GetShaderInput(vsiIdx);
	const unsigned int psiIdx = resMan->CreateShaderInput(PShd);
	PSInput = resMan->GetShaderInput(psiIdx);

	const unsigned int texIdx = resMan->CreateTexture("sponza\\textures\\sponza_curtain_diff.lrt");
	Tex = resMan->GetTexture(texIdx);

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

void RenderScene()
{
	// After creating the resources required for compositing the scene, let's build our frame
	Renderer *renderer = Renderer::GetInstance();
	// First off, set our viewport to the size of the client area containing our renderer
	RECT rc;
	GetClientRect(hWnd, &rc);
	Vec2i vp = Vec2i(rc.right - rc.left, rc.bottom - rc.top);
	renderer->SetViewport(vp);

	unsigned int handle;
	// Obtain a handle for our shader inputs
	if (VSInput->GetInputHandleByName("worldViewProjMat", handle))
	{
		// Here we will build 3 matrices:
		// *the world matrix which positions/rotates/scales our object in the world
		// *the view matrix which determines the position and rotation of the camera (i.e. transforms world coordinates into local camera coordinates)
		// *the projection matrix which determines the kind of projection (orthographic or perspective)
		Matrix44f matWVP[3];

		// First, we build a world matrix by composing a rotation matrix with a translation matrix.
		// The order in which we compose the matrices matter, so here, since GMTL matrices are column-major,
		// the final world matrix will FIRST rotate the object by 90 degrees on the X axis and then
		// translate it 200 units on the Z axis.
		matWVP[0] = makeTrans(Vec3f(0, 0, 0), Type2Type<Matrix44f>());// *makeRot(AxisAnglef(Math::PI_OVER_2, Vec3f(1.f, 0.f, 0.f)), Type2Type<Matrix44f>());

		// Now create our view matrix, which actually is the INVERSE world matrix of the camera.
		// So we could either build a world matrix for the camera, then invert it, or, as we do here,
		// we build a matrix with inverse translation and rotation from the get-go. In other words,
		// if we want our camera to be at position x, y, z then we need to build a matrix that
		// translates to -x, -y, -z
		normalize(vMove);
		tCamera.vPos -=
			Vec3f(tCamera.mRot[2][0] * vMove[2] * fSpeedFactor, tCamera.mRot[2][1] * vMove[2] * fSpeedFactor, tCamera.mRot[2][2] * vMove[2] * fSpeedFactor) +
			Vec3f(tCamera.mRot[0][0] * vMove[0] * fSpeedFactor, tCamera.mRot[0][1] * vMove[0] * fSpeedFactor, tCamera.mRot[0][2] * vMove[0] * fSpeedFactor);
		matWVP[1] = tCamera.mRot * makeTrans(tCamera.vPos, Type2Type<Matrix44f>());

		// Create the projection matrix according to the convention of the rendering API used.
		// A projection matrix transforms view-space coordinates into perspective-correct (if
		// using a perspective matrix) clip-space coordinates. What it does is transform a
		// frustum into a cuboid (cube in OpenGL) with edges ranging between (-1, 1)
		// for X and Y coordinates and (0, 1) for Z coordinate. Any geometry that falls
		// outside of this range after application of the projection matrix gets clipped 
		// (this is why it's called clip-space) before rasterization.
		renderer->CreateProjectionMatrix(matWVP[2], Math::deg2Rad(60.f), (float)vp[0] / (float)vp[1], 1.0f, 10000.f);

		// Set our matrices as input to the vertex shader, which will then transform
		// each vertex from the vertex buffer
		VSInput->SetMatrix4x4(handle, matWVP[2] * matWVP[1] * matWVP[0]);
	}

	if (PSInput->GetInputHandleByName("Tex", handle))
	{
		// Set the texture as an input to the pixel shader
		PSInput->SetTexture(handle, Tex);
		// Also set the sampling filter to linearly interpolate between texels and mips
		renderer->GetSamplerStateManager()->SetFilter(PSInput->GetInputDesc(handle).nRegisterIndex, SF_MIN_MAG_LINEAR_MIP_LINEAR);
	}
	
	// Clear our backbuffer so that we have a nice black background
	// and a depth value that's as far away as possible (at our Z far, which is 2000.f)
	renderer->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	// If we couldn't begin the frame, something might have gone wrong.
	// Usually everything goes smooth as butter, but sometimes, in DX9,
	// the device can get lost (Alt+Tab, Win+L, Ctrl+Alt+Delete, etc.).
	// When this happens, all video memory allocated by the renderer
	// has to be freed, then the device reset. The renderer takes care
	// of that for you. All you have to do is remember not to try and
	// render anything while the device is in limbo, so check the
	// return value of BeginFrame() before submiting draw calls!
	if (renderer->BeginFrame())
	{
		// Set the vertex and pixel shaders used to render the scene
		VShd->Enable(VSInput);
		PShd->Enable(PSInput);
		
		// Draw the contents of the vertex buffers
		for (unsigned int mesh = 0; mesh < model->arrMesh.size(); mesh++)
			renderer->DrawVertexBuffer(model->arrMesh[mesh]->pVertexBuffer);

		// Disable shaders
		VShd->Disable();
		PShd->Disable();

		// End the frame
		renderer->EndFrame();

		// I'm sure you are aware of the fact that that pixels on your screen are progressively
		// refreshed and not all at once. Imagine that during that refreshing time, we render into
		// the backbuffer. The image on the screen will contain a part of the old backbuffer and
		// a part of the new one, resulting in what is known as screen tearing. To avoid this,
		// we use a double buffered system in which we allocate two buffers: a backbuffer in
		// which we render to, and a frontbuffer which is presented on the screen. When we finish
		// rendering our scene into our backbuffer, we swap them at a time when the screen is not
		// refreshing. By doing this, we avoid screen tearing.
		renderer->SwapBuffers();
	}
}