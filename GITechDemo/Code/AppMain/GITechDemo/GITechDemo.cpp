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

#include "PerlinNoise.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window c
HWND hWnd;

// Matrices
Matrix44f worldMat;
Matrix44f viewMat;
Matrix44f projMat;
Matrix44f invProjMat;
Matrix44f invViewMat;
Matrix44f invViewProjMat;
Matrix44f worldViewMat;
Matrix44f viewProjMat;
Matrix44f worldViewProjMat;
//////////////////////////////

// Configurable options
const bool GBUFFER_Z_PREPASS = false;

#define MAX_NUM_CASCADES (9)
#define PCF_BLUR_SIZE (16)
const bool DEBUG_CASCADES = false;
const unsigned int NUM_CASCADES = 4;
const float CASCADE_SPLIT_FACTOR = 0.7f;
const float CASCADE_MAX_VIEW_DEPTH = 3000.f;
const float CASCADE_BLEND_SIZE = 50.f;

float Z_NEAR = 1.f;
float Z_FAR = 5000.f;
float SHADOW_MAP_DEPTH_BIAS = 0.005f;
Vec2i SHADOW_MAP_SIZE = Vec2i(2048, 2048);
///////////////////////////////

// Cascaded Shadow Maps (CSM) and directional light related variables
Vec2f cascadeBoundsMin[MAX_NUM_CASCADES];
Vec2f cascadeBoundsMax[MAX_NUM_CASCADES];
Matrix44f dirLightWorldViewProjMat[MAX_NUM_CASCADES];
Matrix44f dirLightWorldViewMat;
Matrix44f dirLightViewMat;
Matrix44f dirLightProjMat[MAX_NUM_CASCADES];
Matrix44f dirLightViewProjMat[MAX_NUM_CASCADES];
Vec3f directionalLightDir = Vec3f(0.f, -1.f, 0.f);
Matrix44f directionalLightRotMat;
/////////////////////////////////////////////

// The vertices corresponding to DX9's clip space cuboid
// used when partitioning the view frustum for CSM
const Vec4f cuboidVerts[8] =
{
	Vec4f(-1.f,	 1.f,	1.f,	1.f),
	Vec4f( 1.f,	 1.f,	1.f,	1.f),
	Vec4f(-1.f,	-1.f,	1.f,	1.f),
	Vec4f( 1.f,	-1.f,	1.f,	1.f),
	Vec4f(-1.f,	 1.f,	0.f,	1.f),
	Vec4f( 1.f,	 1.f,	0.f,	1.f),
	Vec4f(-1.f,	-1.f,	0.f,	1.f),
	Vec4f( 1.f,	-1.f,	0.f,	1.f)
};
////////////////////////////////////

// A lookup table for textures (faster than searching everytime by its file name when setting materials)
// Usage: TextureLUT[TextureType][MaterialIndex] = TextureIndex
std::vector<unsigned int> TextureLUT[Model::TextureDesc::TT_UNKNOWN];

Renderer*			RenderContext = nullptr;
ResourceManager*	ResourceMgr = nullptr;

// Shader responsible for rendering the sky
ShaderTemplate*		SkyBoxVP = nullptr;
ShaderTemplate*		SkyBoxFP = nullptr;
// Shader responsible for generating the GBuffer
ShaderTemplate*		GBufferGenerationVP = nullptr;
ShaderTemplate*		GBufferGenerationFP = nullptr;
// Shader responsible for calculating the ambient light contribution
ShaderTemplate*		DeferredLightAmbVP = nullptr;
ShaderTemplate*		DeferredLightAmbFP = nullptr;
// Shader responsible for calculating the directional light contribution
ShaderTemplate*		DeferredLightDirVP = nullptr;
ShaderTemplate*		DeferredLightDirFP = nullptr;
// Shader responsible for depth-only passes (shadow maps, depth prepass, etc.)
ShaderTemplate*		DepthPassVP = nullptr;
ShaderTemplate*		DepthPassFP = nullptr;
// Shader responsible for depth-resolving
ShaderTemplate*		DepthCopyVP = nullptr;
ShaderTemplate*		DepthCopyFP = nullptr;
// Shader responsible for applying postprocessing effects
ShaderTemplate*		PostProcessVP = nullptr;
ShaderTemplate*		PostProcessFP = nullptr;

ShaderInput*	SkyBoxVInput = nullptr;
ShaderInput*	SkyBoxFInput = nullptr;
ShaderInput*	GBufferGenerationVInput = nullptr;
ShaderInput*	GBufferGenerationFInput = nullptr;
ShaderInput*	DeferredLightAmbVInput = nullptr;
ShaderInput*	DeferredLightAmbFInput = nullptr;
ShaderInput*	DeferredLightDirVInput = nullptr;
ShaderInput*	DeferredLightDirFInput = nullptr;
ShaderInput*	DepthPassVInput = nullptr;
ShaderInput*	DepthPassFInput = nullptr;
ShaderInput*	DepthCopyVInput = nullptr;
ShaderInput*	DepthCopyFInput = nullptr;
ShaderInput*	PostProcessVInput = nullptr;
ShaderInput*	PostProcessFInput = nullptr;

// A lookup table for shader inputs (faster than searching everytime by input name)
enum ShaderName
{
	SkyBoxVS, SkyBoxPS,
	GBufferGenerationVS, GBufferGenerationPS,
	DeferredLightAmbVS, DeferredLightAmbPS,
	DeferredLightDirVS, DeferredLightDirPS,
	DepthPassVS, DepthPassPS,
	DepthCopyVS, DepthCopyPS,
	PostProcessVS, PostProcessPS,

	SHADER_COUNT
};

enum ShaderInputName
{
	f44WorldViewProjMat, f44WorldViewMat, texDiffuse, texNormal, bHasNormalMap,
	texSpec, bHasSpecMap, fSpecIntensity, f2HalfTexelOffset, texDepthBuffer,
	f44SkyViewProjMat, texSkyTex, texDiffuseBuffer, fAmbientFactor, f44InvProjMat,
	texNormalBuffer, texShadowMap, fShadowDepthBias, f2OneOverShadowMapSize, f44ViewMat,
	f44InvViewProjMat, f44ScreenToLightViewMat, f3LightDir, fDiffuseFactor, fSpecFactor,
	bDebugCascades, nCascadeCount, nCascadesPerRow, fCascadeNormSize, f2CascadeBoundsMin,
	f2CascadeBoundsMax, f44CascadeProjMat, fCascadeBlendSize, texLightAccumulationBuffer,

	SHADER_INPUT_COUNT
};

struct ShaderInputLUT
{
	unsigned int lut[SHADER_COUNT][SHADER_INPUT_COUNT];
	static const std::string inputName[SHADER_INPUT_COUNT];
	static const ShaderInput * const * const inputHandler[SHADER_COUNT];

	ShaderInputLUT()
	{
		for (unsigned int i = 0; i < SHADER_COUNT; i++)
			for (unsigned int j = 0; j < SHADER_INPUT_COUNT; j++)
				lut[i][j] = ~0;
	}

	unsigned int* operator[](unsigned int idx) { return lut[idx]; }
	const unsigned int* const operator[](unsigned int idx) const { return lut[idx]; }

	const char* const getName(ShaderInputName name) const { return inputName[name].c_str(); }
	const ShaderInput* const getInputHandler(ShaderName name) const { return *inputHandler[name]; }
} ShdInputLUT;

const std::string ShaderInputLUT::inputName[] = {
	"f44WorldViewProjMat", "f44WorldViewMat", "texDiffuse", "texNormal", "bHasNormalMap",
	"texSpec", "bHasSpecMap", "fSpecIntensity", "f2HalfTexelOffset", "texDepthBuffer",
	"f44SkyViewProjMat", "texSkyTex", "texDiffuseBuffer", "fAmbientFactor", "f44InvProjMat",
	"texNormalBuffer", "texShadowMap", "fShadowDepthBias", "f2OneOverShadowMapSize", "f44ViewMat",
	"f44InvViewProjMat", "f44ScreenToLightViewMat", "f3LightDir", "fDiffuseFactor", "fSpecFactor",
	"bDebugCascades", "nCascadeCount", "nCascadesPerRow", "fCascadeNormSize", "f2CascadeBoundsMin",
	"f2CascadeBoundsMax", "f44CascadeProjMat", "fCascadeBlendSize", "texLightAccumulationBuffer"
};

const ShaderInput * const * const ShaderInputLUT::inputHandler[] = {
	&SkyBoxVInput, &SkyBoxFInput,
	&GBufferGenerationVInput, &GBufferGenerationFInput,
	&DeferredLightAmbVInput, &DeferredLightAmbFInput,
	&DeferredLightDirVInput, &DeferredLightDirFInput,
	&DepthPassVInput, &DepthPassFInput,
	&DepthCopyVInput, &DepthCopyFInput,
	&PostProcessVInput, &PostProcessFInput
};

// The geometry buffer, holding information necessary for calculating light contribution
RenderTarget*	GBuffer = nullptr;
// The shadow map corresponding to the directional light
RenderTarget*	ShadowMapDir = nullptr;
// The render target in which we accumulate the light contribution from all lights
RenderTarget*	LightAccumulationBuffer = nullptr;

// A model consisting of several meshes and material information
Model*			SponzaScene = nullptr;

// Used for fullscreen effects, postprocessing, etc.
// NB: in order to avoid tearing between the seams of a quad's triangles (the diagonal of the screen)
// we use a single over-sized triangle to fill the screen (2 x screen height, 2 x screen width)
VertexBuffer*	FullScreenQuad = nullptr;
// a cube used to draw the sky
VertexBuffer*	SkyBoxCube = nullptr;

// Axis-aligned bounding box of the scene, to be used when constructing
// the projection matrix for the directional light shadow map
AABoxf SponzaAABB;

// Index of the cube map used to draw the sky
unsigned int skyTexIdx = -1;

// Variables used for user interaction
bool bLMB = false;
bool bRMB = false;
bool bMMB = false;
Vec3f vMove;
float fSpeedFactor = 1.f;

struct Camera
{
	Vec3f vPos;
	Matrix44f mRot;
} tCamera;
///////////////////////////////////////

// Perlin Noise for animation of lights
Perlin PerlinNoise(1, USHRT_MAX, 1, GetTickCount());

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void AllocateRenderResources();
void UpdateMatrices();
void GenerateDirectionalShadowMap();
void GenerateGBuffer();
void ResolveDepthBuffer();
void DrawSky();
void AccumulateAmbientLight();
void AccumulateDirectionalLight();
void AccumulateLight();
void ApplyPostProcessing();
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

		if (bMMB)
		{
			directionalLightRotMat = makeRot(EulerAngleXYZf(Math::deg2Rad(-(float)delta[1] / 10.f), Math::deg2Rad(-(float)delta[0] / 10.f), Math::deg2Rad((float)delta[0] / 10.f)), Type2Type<Matrix44f>()) * directionalLightRotMat;
			directionalLightDir = directionalLightRotMat * Vec3f(0.f, -1.f, 0.f);
			SetCursorPos(center[0], center[1]);
		}

		// If the message is WM_QUIT, exit the while loop
		if (msg.message == WM_QUIT)
			break;

		// Set the size of the backbuffer according to the size of the window
		RECT rc;
		GetClientRect(hWnd, &rc);
		Vec2i vp = Vec2i(rc.right - rc.left, rc.bottom - rc.top);
		RenderContext->SetBackBufferSize(vp);

		// Render a frame
		RenderScene();
	}

	Renderer::DestroyInstance();

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

	AllocateRenderResources();

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

// Allocate/load resources required for rendering the scene
void AllocateRenderResources()
{
	Renderer::CreateInstance(API_DX9);
	//Renderer::CreateInstance(API_NULL);
	RenderContext = Renderer::GetInstance();
	RenderContext->Initialize(hWnd);
	ResourceMgr = RenderContext->GetResourceManager();

	// First of all, load a model file
	const unsigned int modelIdx = ResourceMgr->CreateModel("models\\sponza\\sponza.lrm");
	SponzaScene = ResourceMgr->GetModel(modelIdx);

	// Calculate its AABB
	// this will be used later when calculating the cascade bounds for the CSM
	SponzaAABB.mMin = Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
	SponzaAABB.mMax = Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (unsigned int mesh = 0; mesh < SponzaScene->arrMesh.size(); mesh++)
	{
		const VertexBuffer* const vb = SponzaScene->arrMesh[mesh]->pVertexBuffer;
		for (unsigned int vert = 0; vert < vb->GetElementCount(); vert++)
		{
			Vec3f vertPos = vb->Position<Vec3f>(vert);
			
			if (vertPos[0] < SponzaAABB.mMin[0])
			{
				SponzaAABB.mMin[0] = vertPos[0];
			}
			if (vertPos[1] < SponzaAABB.mMin[1])
			{
				SponzaAABB.mMin[1] = vertPos[1];
			}
			if (vertPos[2] < SponzaAABB.mMin[2])
			{
				SponzaAABB.mMin[2] = vertPos[2];
			}

			if (vertPos[0] > SponzaAABB.mMax[0])
			{
				SponzaAABB.mMax[0] = vertPos[0];
			}
			if (vertPos[1] > SponzaAABB.mMax[1])
			{
				SponzaAABB.mMax[1] = vertPos[1];
			}
			if (vertPos[2] > SponzaAABB.mMax[2])
			{
				SponzaAABB.mMax[2] = vertPos[2];
			}
		}
	}

	// Now load all the textures referenced by the model and build a lookup table
	for (unsigned int tt = Model::TextureDesc::TT_NONE; tt < Model::TextureDesc::TT_UNKNOWN; tt++)
		TextureLUT[tt].resize(SponzaScene->arrMaterial.size(), -1);

	for (unsigned int i = 0; i < SponzaScene->arrMaterial.size(); i++)
	{
		for (unsigned int j = 0; j < SponzaScene->arrMaterial[i]->arrTexture.size(); j++)
		{
			std::string filePath = "models\\sponza\\" + SponzaScene->arrMaterial[i]->arrTexture[j]->szFilePath;

			const unsigned int offset = (unsigned int)filePath.rfind('.');
			if (offset != std::string::npos)
				filePath.replace(offset, UINT_MAX, ".lrt");

			unsigned int texIdx = -1;
			texIdx = ResourceMgr->FindTexture(filePath.c_str());
			if (texIdx == -1)
			{
				texIdx = ResourceMgr->CreateTexture(filePath.c_str());

				// Set the sampling filter to linearly interpolate between texels and mips
				// and set the maximum anisotropy level for maximum quality
				if (SponzaScene->arrMaterial[i]->arrTexture[j]->eTexType == Model::TextureDesc::TT_DIFFUSE)
				{
					Texture* tex = ResourceMgr->GetTexture(texIdx);
					tex->SetAnisotropy(/*MAX_ANISOTROPY*/ 1.f);
					tex->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
				}
				if (SponzaScene->arrMaterial[i]->arrTexture[j]->eTexType == Model::TextureDesc::TT_SPECULAR)
				{
					Texture* tex = ResourceMgr->GetTexture(texIdx);
					tex->SetAnisotropy(1.f);
					tex->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
				}
				if (SponzaScene->arrMaterial[i]->arrTexture[j]->eTexType == Model::TextureDesc::TT_HEIGHT)
				{
					Texture* tex = ResourceMgr->GetTexture(texIdx);
					tex->SetAnisotropy(1.f);
					tex->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
				}
			}

			assert(texIdx != -1);
			assert(TextureLUT[SponzaScene->arrMaterial[i]->arrTexture[j]->eTexType][i] == -1 ||
				TextureLUT[SponzaScene->arrMaterial[i]->arrTexture[j]->eTexType][i] == texIdx);

			if (TextureLUT[SponzaScene->arrMaterial[i]->arrTexture[j]->eTexType][i] == -1)
				TextureLUT[SponzaScene->arrMaterial[i]->arrTexture[j]->eTexType][i] = texIdx;
		}
	}

	// Load the sky cube map texture
	skyTexIdx = ResourceMgr->CreateTexture("models/sponza/textures/sky.lrt");

	// Load shaders
	unsigned int vspIdx = ResourceMgr->CreateShaderProgram("shaders/GBufferGeneration.hlsl", SPT_VERTEX);
	unsigned int pspIdx = ResourceMgr->CreateShaderProgram("shaders/GBufferGeneration.hlsl", SPT_PIXEL);
	unsigned int vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	GBufferGenerationVP = ResourceMgr->GetShaderTemplate(vstIdx);
	unsigned int pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	GBufferGenerationFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/DeferredLightAmb.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/DeferredLightAmb.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	DeferredLightAmbVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	DeferredLightAmbFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/DeferredLightDir.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/DeferredLightDir.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	DeferredLightDirVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	DeferredLightDirFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/DepthPass.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/DepthPass.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	DepthPassVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	DepthPassFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/DepthCopy.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/DepthCopy.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	DepthCopyVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	DepthCopyFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/PostProcess.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/PostProcess.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	PostProcessVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	PostProcessFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/Skybox.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/Skybox.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	SkyBoxVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	SkyBoxFP = ResourceMgr->GetShaderTemplate(pstIdx);

	// Create shader inputs
	unsigned int vsiIdx = ResourceMgr->CreateShaderInput(GBufferGenerationVP);
	GBufferGenerationVInput = ResourceMgr->GetShaderInput(vsiIdx);
	unsigned int psiIdx = ResourceMgr->CreateShaderInput(GBufferGenerationFP);
	GBufferGenerationFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(DeferredLightAmbVP);
	DeferredLightAmbVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(DeferredLightAmbFP);
	DeferredLightAmbFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(DeferredLightDirVP);
	DeferredLightDirVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(DeferredLightDirFP);
	DeferredLightDirFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(DepthPassVP);
	DepthPassVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(DepthPassFP);
	DepthPassFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(DepthCopyVP);
	DepthCopyVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(DepthCopyFP);
	DepthCopyFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(PostProcessVP);
	PostProcessVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(PostProcessFP);
	PostProcessFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(SkyBoxVP);
	SkyBoxVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(SkyBoxFP);
	SkyBoxFInput = ResourceMgr->GetShaderInput(psiIdx);

	// Build a lookup table of shader inputs
	// NB: because this searches for all inputs in all shaders,
	// regardless if the input belongs to another shader, this
	// will generate some harmless errors in the console output
	for (unsigned int shader = SkyBoxVS; shader <= PostProcessPS; shader++)
		for (unsigned int input = f44WorldViewProjMat; input <= texLightAccumulationBuffer; input++)
			ShdInputLUT.getInputHandler((ShaderName)shader)->GetInputHandleByName(ShdInputLUT.getName((ShaderInputName)input), ShdInputLUT[shader][input]);

	// Initialize the various render targets we will be using
	//
	// Geometry buffer (GBuffer)
	//	RT0:	A8R8G8B8 with diffuse albedo in RGB and specular power in A
	//	RT1:	G16R16F with compressed normals (stereographic projection)
	//	DS:		INTZ (for sampling as a regular texture later)
	unsigned int rtIdx = ResourceMgr->CreateRenderTarget(2, PF_A8R8G8B8, PF_G16R16F, PF_NONE, PF_NONE, /*1280, 720,*/ false, true, PF_INTZ);
	GBuffer = ResourceMgr->GetRenderTarget(rtIdx);

	// Shadow map for the directional light (the dummy color buffer is required because of DX9 limitations
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_R32F, SHADOW_MAP_SIZE[0], SHADOW_MAP_SIZE[1], false, true, PF_INTZ);
	ShadowMapDir = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(ShadowMapDir->GetDepthBuffer())->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(ShadowMapDir->GetDepthBuffer())->SetBorderColor(Vec4f(1.f, 1.f, 1.f, 1.f));

	// Render target in which we accumulate the light contribution from all light sources (floating point color components make it HDR-ready)
	// It contains a regular depth-stencil surface in which we will copy-resolve our INTZ depth texture from the GBuffer
	// (the depth is required for correctly rendering the sky and for future stencil optimizations)
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, false, true, PF_D24S8);
	LightAccumulationBuffer = ResourceMgr->GetRenderTarget(rtIdx);

	// Create a full screen quad (it's actually an over-sized triangle) for fullscreen effects and processing
	/*
	You can view the triangle as half of the quad required to fill a screen that's twice as
	high and twice as wide as our actual screen. The diagram below should help you visualize.
	Note that, as opposed to the traditional fullscreen quad composed of two triangles, this
	method avoids any artifact on the screen's diagonal, where the two triangles would line up.

	|-------------------------------------------------------------------------------------------------
	|||============================================||                                          /====/
	|||											   ||                                    /====/
	|||											   ||                              /====/
	|||											   ||                        /====/
	|||											   ||                  /====/
	|||											   ||            /====/
	|||											   ||      /====/
	|||============================================||/====/
	|                                         /====/
	|                                   /====/
	|                             /====/
	|                       /====/
	|                 /====/
	|           /====/
	|     /====/
	|====/
	*/
	unsigned int vfIdx = ResourceMgr->CreateVertexFormat(1);
	VertexFormat* vf = ResourceMgr->GetVertexFormat(vfIdx);
	vf->Initialize(VAU_POSITION, VAT_FLOAT4, 0);
	vf->SetStride(vf->CalculateStride());
	vf->Update();

	unsigned int ibIdx = ResourceMgr->CreateIndexBuffer(3);
	IndexBuffer* ib = ResourceMgr->GetIndexBuffer(ibIdx);
	const unsigned int fsqIndices[] = { 0, 1, 2 };
	ib->SetIndices(fsqIndices, 3);

	unsigned int vbIdx = ResourceMgr->CreateVertexBuffer(vf, 3, ib);
	FullScreenQuad = ResourceMgr->GetVertexBuffer(vbIdx);

	FullScreenQuad->Lock(BL_WRITE_ONLY);
	FullScreenQuad->Position<Vec4f>(0) = Vec4f(-1.f, 1.f, 1.f, 1.f);
	FullScreenQuad->Position<Vec4f>(1) = Vec4f(3.f, 1.f, 1.f, 1.f);
	FullScreenQuad->Position<Vec4f>(2) = Vec4f(-1.f, -3.f, 1.f, 1.f);
	FullScreenQuad->Update();
	FullScreenQuad->Unlock();

	// Create a cube VB for sky
	// The cube is aligned with world axes so that the camera can rotate freely,
	// giving the impressiong that the sky is stationary. Also, the center of the
	// cube is positioned at the camera's location so as not to break the illusion
	// that the sky "hemisphere" is actually a giant cube.
	// NB: in the pixel shader responsible for drawing the sky, the visible face(s)
	// are pushed back to the maximum depth (far plane) so as not to be drawn
	// over objects that are not inside the cube (bigger depth).
	vfIdx = ResourceMgr->CreateVertexFormat(1);
	vf = ResourceMgr->GetVertexFormat(vfIdx);
	vf->Initialize(VAU_POSITION, VAT_FLOAT4, 0);
	vf->SetStride(vf->CalculateStride());
	vf->Update();

	ibIdx = ResourceMgr->CreateIndexBuffer(36);
	ib = ResourceMgr->GetIndexBuffer(ibIdx);
	const unsigned int sbIndices[] = {
		0, 1, 2, 2, 1, 3,	// front
		5, 4, 7, 7, 4, 6,	// back
		4, 0, 6, 6, 0, 2,	// left
		1, 5, 3, 3, 5, 7,	// right
		4, 5, 0, 0, 5, 1,	// top
		2, 3, 6, 6, 3, 7	// bottom
	};
	ib->SetIndices(sbIndices, 36);

	vbIdx = ResourceMgr->CreateVertexBuffer(vf, 8, ib);
	SkyBoxCube = ResourceMgr->GetVertexBuffer(vbIdx);

	SkyBoxCube->Lock(BL_WRITE_ONLY);
	SkyBoxCube->Position<Vec4f>(0) = Vec4f(	-1.f,	1.f,	1.f,	1.f);
	SkyBoxCube->Position<Vec4f>(1) = Vec4f(	1.f,	1.f,	1.f,	1.f);
	SkyBoxCube->Position<Vec4f>(2) = Vec4f(	-1.f,	-1.f,	1.f,	1.f);
	SkyBoxCube->Position<Vec4f>(3) = Vec4f(	1.f,	-1.f,	1.f,	1.f);
	SkyBoxCube->Position<Vec4f>(4) = Vec4f(	-1.f,	1.f,	-1.f,	1.f);
	SkyBoxCube->Position<Vec4f>(5) = Vec4f(	1.f,	1.f,	-1.f,	1.f);
	SkyBoxCube->Position<Vec4f>(6) = Vec4f(	-1.f,	-1.f,	-1.f,	1.f);
	SkyBoxCube->Position<Vec4f>(7) = Vec4f(	1.f,	-1.f,	-1.f,	1.f);
	SkyBoxCube->Update();
	SkyBoxCube->Unlock();

	// Set an initial position and rotation for the camera
	tCamera.vPos = Vec3f(-840.f, -600.f, -195.f);
	tCamera.mRot.set(
		-0.440301329f,	0.00776965916f,	0.897806108f,	0.f,
		-0.142924204f,	0.986597657f,	-0.0786283761f,	0.f,
		-0.886387110f,	-0.162937075f,	-0.433295786f,	0.f,
		0.f,			0.f,			0.f,			1.f
		);
}

void UpdateMatrices()
{
	// Calculate world matrix
	worldMat = makeTrans(Vec3f(0, 0, 0), Type2Type<Matrix44f>());

	// Calculate view matrix
	normalize(vMove);
	vMove *= 5.f;
	tCamera.vPos -=
		Vec3f(tCamera.mRot[2][0] * vMove[2] * fSpeedFactor, tCamera.mRot[2][1] * vMove[2] * fSpeedFactor, tCamera.mRot[2][2] * vMove[2] * fSpeedFactor) +
		Vec3f(tCamera.mRot[0][0] * vMove[0] * fSpeedFactor, tCamera.mRot[0][1] * vMove[0] * fSpeedFactor, tCamera.mRot[0][2] * vMove[0] * fSpeedFactor);
	viewMat = tCamera.mRot * makeTrans(tCamera.vPos, Type2Type<Matrix44f>());
	invertFull(invViewMat, viewMat);

	// Calculate projection matrix
	RECT rc;
	GetClientRect(hWnd, &rc);
	Vec2i vp = Vec2i(rc.right - rc.left, rc.bottom - rc.top);
	RenderContext->CreatePerspectiveMatrix(projMat, Math::deg2Rad(60.f), (float)vp[0] / (float)vp[1], Z_NEAR, Z_FAR);
	invertFull(invProjMat, projMat);

	// Calculate composite matrices
	worldViewMat = viewMat * worldMat;
	viewProjMat = projMat * viewMat;
	invViewProjMat = invViewMat * invProjMat;
	worldViewProjMat = projMat * worldViewMat;

	// Animate directional light using Perlin Noise
	float noiseX = PerlinNoise.Get((float)GetTickCount() / (float)INT_MAX, 0);
	float noiseZ = PerlinNoise.Get(0, (float)GetTickCount() / (float)INT_MAX);
	directionalLightDir[0] = noiseX;
	directionalLightDir[2] = noiseZ;
	normalize(directionalLightDir);

	// Calculate directional light camera view matrix
	Vec3f zAxis = makeNormal(directionalLightDir);
	Vec3f upVec = Vec3f(0.f, 1.f, 0.f);
	if (zAxis == upVec || zAxis == Vec3f(upVec * -1.f))
		upVec = Vec3f(0.f, 0.f, 1.f);
	Vec3f xAxis = makeNormal(makeCross(upVec, zAxis));
	Vec3f yAxis = makeCross(zAxis, xAxis);
	dirLightViewMat.set(
		xAxis[0],	xAxis[1],	xAxis[2],	0.f,
		yAxis[0],	yAxis[1],	yAxis[2],	0.f,
		zAxis[0],	zAxis[1],	zAxis[2],	0.f,
		0.f,		0.f,		0.f,		1.f
		);
	dirLightWorldViewMat = dirLightViewMat * worldMat;

	// Calculate the projection matrices for all shadow map cascades
	// Start with converting the scene AABB to a light view space OBB
	// and then calculating its light view space AABB
	Vec3f aabbVerts[8];
	aabbVerts[0] = SponzaAABB.getMin();
	aabbVerts[1] = Vec3f(SponzaAABB.getMin()[0], SponzaAABB.getMin()[1], SponzaAABB.getMax()[2]);
	aabbVerts[2] = Vec3f(SponzaAABB.getMax()[0], SponzaAABB.getMin()[1], SponzaAABB.getMax()[2]);
	aabbVerts[3] = Vec3f(SponzaAABB.getMax()[0], SponzaAABB.getMin()[1], SponzaAABB.getMin()[2]);
	aabbVerts[4] = Vec3f(SponzaAABB.getMin()[0], SponzaAABB.getMax()[1], SponzaAABB.getMin()[2]);
	aabbVerts[5] = Vec3f(SponzaAABB.getMin()[0], SponzaAABB.getMax()[1], SponzaAABB.getMax()[2]);
	aabbVerts[6] = Vec3f(SponzaAABB.getMax()[0], SponzaAABB.getMax()[1], SponzaAABB.getMin()[2]);
	aabbVerts[7] = SponzaAABB.getMax();
	// This is the scene AABB in light view space (which is actually the view frustum of the
	// directional light camera) aproximated from the scene's light view space OBB
	AABoxf SponzaLightSpaceAABB(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX), Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));
	for (unsigned int i = 0; i < 8; i++)
	{
		// For each AABB vertex, calculate the corresponding light view space OBB vertex
		aabbVerts[i] = dirLightWorldViewMat * aabbVerts[i];

		// Calculate the light view space AABB using the minimum and maximum values
		// on each axis of the light view space OBB
		if (aabbVerts[i][0] < SponzaLightSpaceAABB.getMin()[0])
			SponzaLightSpaceAABB.mMin[0] = aabbVerts[i][0];
		if (aabbVerts[i][1] < SponzaLightSpaceAABB.getMin()[1])
			SponzaLightSpaceAABB.mMin[1] = aabbVerts[i][1];
		if (aabbVerts[i][2] < SponzaLightSpaceAABB.getMin()[2])
			SponzaLightSpaceAABB.mMin[2] = aabbVerts[i][2];

		if (aabbVerts[i][0] > SponzaLightSpaceAABB.getMax()[0])
			SponzaLightSpaceAABB.mMax[0] = aabbVerts[i][0];
		if (aabbVerts[i][1] > SponzaLightSpaceAABB.getMax()[1])
			SponzaLightSpaceAABB.mMax[1] = aabbVerts[i][1];
		if (aabbVerts[i][2] > SponzaLightSpaceAABB.getMax()[2])
			SponzaLightSpaceAABB.mMax[2] = aabbVerts[i][2];
	}

	// Calculate each cascade properties
	for (unsigned int cascade = 0; cascade < NUM_CASCADES; cascade++)
	{
		// This is the part of the viewer's view frustum corresponding to the view frustum of the current cascade
		AABoxf ViewFrustumPartitionLightSpaceAABB(Vec3f(FLT_MAX, FLT_MAX, FLT_MAX), Vec3f(-FLT_MAX, -FLT_MAX, -FLT_MAX));

		// Partition the viewer's view frustum (the can be viewed as Z slices into the view frustum)
		// This formula is derived from Nvidia's paper on Cascaded Shadow Maps:
		// http://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
		Vec4f partitionNear(0.f, 0.f, 0.f, 1.f);
		Vec4f partitionFar(0.f, 0.f, 0.f, 1.f);

		Math::lerp(partitionNear[2],
			CASCADE_SPLIT_FACTOR,
			Z_NEAR + ((float)cascade / NUM_CASCADES)*(CASCADE_MAX_VIEW_DEPTH - Z_NEAR),
			Z_NEAR * powf(CASCADE_MAX_VIEW_DEPTH / Z_NEAR, (float)cascade / NUM_CASCADES)
			);

		Math::lerp(partitionFar[2],
			CASCADE_SPLIT_FACTOR,
			Z_NEAR + (((float)cascade + 1.f) / NUM_CASCADES)*(CASCADE_MAX_VIEW_DEPTH - Z_NEAR),
			Z_NEAR * powf(CASCADE_MAX_VIEW_DEPTH / Z_NEAR, ((float)cascade + 1.f) / NUM_CASCADES)
			);
		
		// Calculate the partition's depth in projective space (viewer camera, i.e. perspective projection)
		partitionNear = projMat * partitionNear;
		partitionNear /= partitionNear[3]; // w-divide
		partitionFar = projMat * partitionFar;
		partitionFar /= partitionFar[3]; // w-divide

		for (unsigned int vert = 0; vert < 8; vert++)
		{
			// Calculate the vertices of each view frustum partition (in clip space)
			Vec4f partitionVert = cuboidVerts[vert];
			if (partitionVert[2] == 0.f)
				partitionVert[2] = partitionNear[2];
			else if(partitionVert[2] == 1.f)
				partitionVert[2] = partitionFar[2];

			// Calculate the current partition's vertices in world space coordinates,
			// then apply the directional light camera's view matrix in order to
			// obtain the light view space coordinates of the partitioned view frustum.
			// This is the light view space OBB of the current view frustum partition.
			Vec4f viewFrustumVertPreW = invViewProjMat * partitionVert;
			Vec4f viewFrustumVertPostW = viewFrustumVertPreW / viewFrustumVertPreW[3];
			Vec3f wsFrustumVert = Vec3f(viewFrustumVertPostW[0], viewFrustumVertPostW[1], viewFrustumVertPostW[2]);
			Vec3f lsFrustumVerts = dirLightViewMat * wsFrustumVert;

			// Calculate a light view space AABB from the light view space OBB of this view frustum partition.
			// In other words, this light view space AABB is the view frustum (light view space)
			// of the current cascade, encompassing the part of the world that the viewer
			// sees which is sufficiently far/near to the viewer's camera so as to
			// be considered valid for rendering into the current shadow map cascade.
			if (lsFrustumVerts[0] < ViewFrustumPartitionLightSpaceAABB.getMin()[0])
				ViewFrustumPartitionLightSpaceAABB.mMin[0] = lsFrustumVerts[0];
			if (lsFrustumVerts[0] > ViewFrustumPartitionLightSpaceAABB.getMax()[0])
				ViewFrustumPartitionLightSpaceAABB.mMax[0] = lsFrustumVerts[0];

			if (lsFrustumVerts[1] < ViewFrustumPartitionLightSpaceAABB.getMin()[1])
				ViewFrustumPartitionLightSpaceAABB.mMin[1] = lsFrustumVerts[1];
			if (lsFrustumVerts[1] > ViewFrustumPartitionLightSpaceAABB.getMax()[1])
				ViewFrustumPartitionLightSpaceAABB.mMax[1] = lsFrustumVerts[1];

			if (lsFrustumVerts[2] < ViewFrustumPartitionLightSpaceAABB.getMin()[2])
				ViewFrustumPartitionLightSpaceAABB.mMin[2] = lsFrustumVerts[2];
			if (lsFrustumVerts[2] > ViewFrustumPartitionLightSpaceAABB.getMax()[2])
				ViewFrustumPartitionLightSpaceAABB.mMax[2] = lsFrustumVerts[2];
		}

		// Enlarge the light view frustum in order to avoid PCF shadow sampling from
		// sampling outside of a shadow map cascade
		float pcfScale = (((float)PCF_BLUR_SIZE + 1.f) * 2.f ) / (float)SHADOW_MAP_SIZE[0];
		Vec3f aabbDiag = ViewFrustumPartitionLightSpaceAABB.mMax - ViewFrustumPartitionLightSpaceAABB.mMin;
		Vec2f offsetForPCF = Vec2f(aabbDiag[0], aabbDiag[1]) * 0.5f * pcfScale;

		// Snap the ortographic projection to texel-sized increments in order to prevent shadow edges from jittering.
		// However, because we're tightly fitting the cascade around the view frustum, jittering will still be
		// present when rotating the camera, but not when zooming or strafing.
		Vec2f worldUnitsPerTexel = Vec2f(
			ViewFrustumPartitionLightSpaceAABB.mMax[0] - ViewFrustumPartitionLightSpaceAABB.mMin[0] + 2.f * offsetForPCF[0],
			ViewFrustumPartitionLightSpaceAABB.mMax[1] - ViewFrustumPartitionLightSpaceAABB.mMin[1] + 2.f * offsetForPCF[1]) /
			Math::floor((float)SHADOW_MAP_SIZE[0] / Math::ceil(Math::sqrt((float)NUM_CASCADES))/*cascades per row*/);
		
		// Calculate the projection matrix for the current shadow map cascade
		RenderContext->CreateOrthographicMatrix(
			dirLightProjMat[cascade],
			Math::floor((ViewFrustumPartitionLightSpaceAABB.mMin[0] - offsetForPCF[0]) / worldUnitsPerTexel[0]) * worldUnitsPerTexel[0],
			Math::floor((ViewFrustumPartitionLightSpaceAABB.mMax[1] + offsetForPCF[1]) / worldUnitsPerTexel[1]) * worldUnitsPerTexel[1],
			Math::floor((ViewFrustumPartitionLightSpaceAABB.mMax[0] + offsetForPCF[0]) / worldUnitsPerTexel[0]) * worldUnitsPerTexel[0],
			Math::floor((ViewFrustumPartitionLightSpaceAABB.mMin[1] - offsetForPCF[1]) / worldUnitsPerTexel[1]) * worldUnitsPerTexel[1],
			SponzaLightSpaceAABB.mMin[2], SponzaLightSpaceAABB.mMax[2]);

		// Store the light space coordinates of the bounds of the current shadow map cascade
		cascadeBoundsMin[cascade] = Vec2f(ViewFrustumPartitionLightSpaceAABB.mMin[0], ViewFrustumPartitionLightSpaceAABB.mMin[1]);
		cascadeBoundsMax[cascade] = Vec2f(ViewFrustumPartitionLightSpaceAABB.mMax[0], ViewFrustumPartitionLightSpaceAABB.mMax[1]);

		// Calculate the current shadow map cascade's corresponding composite matrices
		dirLightViewProjMat[cascade] = dirLightProjMat[cascade] * dirLightViewMat;
		dirLightWorldViewProjMat[cascade] = dirLightViewProjMat[cascade] * worldMat;
	}

	//worldViewProjMat = dirLightProjMat[NUM_CASCADES-1] * dirLightViewMat * worldMat;
}

void GenerateDirectionalShadowMap()
{
	//unsigned int handle;

	bool red, blue, green, alpha;
	RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);
	bool scissorEnabled = RenderContext->GetRenderStateManager()->GetScissorEnable();
	RenderContext->GetRenderStateManager()->SetScissorEnable(true);

	ShadowMapDir->Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	assert(SHADOW_MAP_SIZE[0] == SHADOW_MAP_SIZE[1]);
	const unsigned int cascadesPerRow = (unsigned int)Math::ceil(Math::sqrt((float)NUM_CASCADES));
	const unsigned int cascadeSize = SHADOW_MAP_SIZE[0] / cascadesPerRow;
	for (unsigned int cascade = 0; cascade < NUM_CASCADES; cascade++)
	{
		Vec2i size = Vec2i(cascadeSize, cascadeSize);
		Vec2i offset = Vec2i(cascadeSize * (cascade % cascadesPerRow), cascadeSize * (cascade / cascadesPerRow));
		RenderContext->SetViewport(size, offset);
		RenderContext->GetRenderStateManager()->SetScissor(size, offset);

		//if (DepthPassVInput->GetInputHandleByName("f44WorldViewProjMat", handle))
		if(ShdInputLUT[DepthPassVS][f44WorldViewProjMat] != ~0)
			DepthPassVInput->SetMatrix4x4(ShdInputLUT[DepthPassVS][f44WorldViewProjMat], dirLightWorldViewProjMat[cascade]);

		DepthPassVP->Enable(DepthPassVInput);
		DepthPassFP->Enable(DepthPassFInput);

		// Normally, you would only render meshes whose AABB/OBB intersect with the cascade's
		// view frustum, but we don't have a big enough scene to care at the moment
		for (unsigned int mesh = 0; mesh < SponzaScene->arrMesh.size(); mesh++)
			RenderContext->DrawVertexBuffer(SponzaScene->arrMesh[mesh]->pVertexBuffer);

		DepthPassVP->Disable();
		DepthPassFP->Disable();
	}

	ShadowMapDir->Disable();

	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetScissorEnable(scissorEnabled);
}

void GenerateGBuffer()
{
	//unsigned int handle;

	GBuffer->Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);
	
	// A depth prepass is useful if we have expensive pixel shaders in our
	// GBuffer generation process. It allows us to avoid shading pixels
	// that eventually get overwritten by other pixels that have smaller
	// depth values. We are not fill-rate bound, so the depth prepass is disabled.
	if (GBUFFER_Z_PREPASS)
	{
		bool red, blue, green, alpha;
		RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);
		RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);

		//if (DepthPassVInput->GetInputHandleByName("f44WorldViewProjMat", handle))
		if (ShdInputLUT[DepthPassVS][f44WorldViewProjMat] != ~0)
			DepthPassVInput->SetMatrix4x4(ShdInputLUT[DepthPassVS][f44WorldViewProjMat], worldViewProjMat);

		DepthPassVP->Enable(DepthPassVInput);
		DepthPassFP->Enable(DepthPassFInput);

		for (unsigned int mesh = 0; mesh < SponzaScene->arrMesh.size(); mesh++)
			RenderContext->DrawVertexBuffer(SponzaScene->arrMesh[mesh]->pVertexBuffer);

		DepthPassVP->Disable();
		DepthPassFP->Disable();

		RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);

		RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
		RenderContext->GetRenderStateManager()->SetZFunc(CMP_EQUAL);
	}

	//if (GBufferGenerationVInput->GetInputHandleByName("f44WorldViewMat", handle))
	if (ShdInputLUT[GBufferGenerationVS][f44WorldViewMat] != ~0)
		GBufferGenerationVInput->SetMatrix4x4(ShdInputLUT[GBufferGenerationVS][f44WorldViewMat], worldViewMat);

	//if (GBufferGenerationVInput->GetInputHandleByName("f44WorldViewProjMat", handle))
	if (ShdInputLUT[GBufferGenerationVS][f44WorldViewProjMat] != ~0)
		GBufferGenerationVInput->SetMatrix4x4(ShdInputLUT[GBufferGenerationVS][f44WorldViewProjMat], worldViewProjMat);

	// A visibility test would be useful if we were CPU bound (or vertex bound).
	// However, there isn't a reason to do such an optimization for now, since the
	// scene isn't very big and we are mostly pixel bound.
	for (unsigned int mesh = 0; mesh < SponzaScene->arrMesh.size(); mesh++)
	{
		const unsigned int diffTexIdx = TextureLUT[Model::TextureDesc::TT_DIFFUSE][SponzaScene->arrMesh[mesh]->nMaterialIdx];
		const unsigned int normalTexIdx = TextureLUT[Model::TextureDesc::TT_HEIGHT][SponzaScene->arrMesh[mesh]->nMaterialIdx];
		const unsigned int specTexIdx = TextureLUT[Model::TextureDesc::TT_SPECULAR][SponzaScene->arrMesh[mesh]->nMaterialIdx];

		//if (GBufferGenerationFInput->GetInputHandleByName("texDiffuse", handle))
		if (ShdInputLUT[GBufferGenerationPS][texDiffuse] != ~0)
			GBufferGenerationFInput->SetTexture(ShdInputLUT[GBufferGenerationPS][texDiffuse], diffTexIdx);

		//if (GBufferGenerationFInput->GetInputHandleByName("texNormal", handle))
		if (ShdInputLUT[GBufferGenerationPS][texNormal] != ~0)
		{
			GBufferGenerationFInput->SetTexture(ShdInputLUT[GBufferGenerationPS][texNormal], normalTexIdx);

			//if (GBufferGenerationFInput->GetInputHandleByName("bHasNormalMap", handle))
			if (ShdInputLUT[GBufferGenerationPS][bHasNormalMap] != ~0)
				GBufferGenerationFInput->SetBool(ShdInputLUT[GBufferGenerationPS][bHasNormalMap], normalTexIdx != -1);
		}

		//if (GBufferGenerationFInput->GetInputHandleByName("texSpec", handle))
		if (ShdInputLUT[GBufferGenerationPS][texSpec] != ~0)
		{
			GBufferGenerationFInput->SetTexture(ShdInputLUT[GBufferGenerationPS][texSpec], specTexIdx);

			//if (GBufferGenerationFInput->GetInputHandleByName("bHasSpecMap", handle))
			if (ShdInputLUT[GBufferGenerationPS][bHasSpecMap] != ~0)
				GBufferGenerationFInput->SetBool(ShdInputLUT[GBufferGenerationPS][bHasSpecMap], specTexIdx != -1);
		}

		//if (GBufferGenerationFInput->GetInputHandleByName("fSpecIntensity", handle))
		if (ShdInputLUT[GBufferGenerationPS][fSpecIntensity] != ~0)
			GBufferGenerationFInput->SetFloat(ShdInputLUT[GBufferGenerationPS][fSpecIntensity], SponzaScene->arrMaterial[SponzaScene->arrMesh[mesh]->nMaterialIdx]->fShininessStrength);

		GBufferGenerationVP->Enable(GBufferGenerationVInput);
		GBufferGenerationFP->Enable(GBufferGenerationFInput);

		RenderContext->DrawVertexBuffer(SponzaScene->arrMesh[mesh]->pVertexBuffer);

		GBufferGenerationVP->Disable();
		GBufferGenerationFP->Disable();
	}

	GBuffer->Disable();

	if (GBUFFER_Z_PREPASS)
	{
		RenderContext->GetRenderStateManager()->SetZWriteEnabled(true);
		RenderContext->GetRenderStateManager()->SetZFunc(CMP_LESSEQUAL);
	}
}

// This function copy-resolves the INTZ depth texture we use when generating the
// GBuffer (since we can sample it like a regular texture) to the D24S8 depth
// surface of the light accumulation buffer for depth testing (e.g. when rendering the sky)
// Having the correct depth allows us to do some more optimizations which also make use of
// the stencil (e.g. rendering a cone corresponding to the spot light and marking the pixels
// that pass the depth test, using the stencil buffer, so that we only calculate the light
// for those pixels and thus reducing pointless shading of pixels which are not lit)
void ResolveDepthBuffer()
{
	//unsigned int handle;

	bool red, blue, green, alpha;
	RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);
	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(true);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	//if (DepthCopyVInput->GetInputHandleByName("f2HalfTexelOffset", handle))
	if (ShdInputLUT[DepthCopyVS][f2HalfTexelOffset] != ~0)
		DepthCopyVInput->SetFloat2(ShdInputLUT[DepthCopyVS][f2HalfTexelOffset], Vec2f(0.5f / GBuffer->GetWidth(), 0.5f / GBuffer->GetHeight()));

	//if (DepthCopyFInput->GetInputHandleByName("texDepthBuffer", handle))
	if (ShdInputLUT[DepthCopyPS][texDepthBuffer] != ~0)
		DepthCopyFInput->SetTexture(ShdInputLUT[DepthCopyPS][texDepthBuffer], GBuffer->GetDepthBuffer());

	DepthCopyVP->Enable(DepthCopyVInput);
	DepthCopyFP->Enable(DepthCopyFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DepthCopyVP->Disable();
	DepthCopyFP->Disable();

	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);
}

// Draw a world axis-aligned cube with its' center coinciding with the camera's position.
// The vertex shader sets the visible face(s) at max depth so as to not draw over
// other objects that aren't inside the cube (which, at 2x2x2, is quite small)
void DrawSky()
{
	//unsigned int handle;
	
	bool blendEnabled;
	Blend DstBlend, SrcBlend;
	blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnable();
	DstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	SrcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	RenderContext->GetRenderStateManager()->SetColorBlendEnable(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ZERO);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);

	bool zWriteEnable = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_LESSEQUAL);

	//if (SkyBoxVInput->GetInputHandleByName("f44SkyViewProjMat", handle))
	if (ShdInputLUT[SkyBoxVS][f44SkyViewProjMat] != ~0)
		SkyBoxVInput->SetMatrix4x4(ShdInputLUT[SkyBoxVS][f44SkyViewProjMat], viewProjMat* makeTrans(-tCamera.vPos, Type2Type<Matrix44f>()));

	//if (SkyBoxFInput->GetInputHandleByName("texSkyTex", handle))
	if (ShdInputLUT[SkyBoxPS][texSkyTex] != ~0)
		SkyBoxFInput->SetTexture(ShdInputLUT[SkyBoxPS][texSkyTex], skyTexIdx);
		
	SkyBoxVP->Enable(SkyBoxVInput);
	SkyBoxFP->Enable(SkyBoxFInput);

	RenderContext->DrawVertexBuffer(SkyBoxCube);

	SkyBoxVP->Disable();
	SkyBoxFP->Disable();

	RenderContext->GetRenderStateManager()->SetColorBlendEnable(blendEnabled);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(DstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(SrcBlend);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWriteEnable);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);
}

// Accumulate ambient light
void AccumulateAmbientLight()
{
	//unsigned int handle;

	//if (DeferredLightAmbVInput->GetInputHandleByName("f2HalfTexelOffset", handle))
	if (ShdInputLUT[DeferredLightAmbVS][f2HalfTexelOffset] != ~0)
		DeferredLightAmbVInput->SetFloat2(ShdInputLUT[DeferredLightAmbVS][f2HalfTexelOffset], Vec2f(0.5f / GBuffer->GetWidth(), 0.5f / GBuffer->GetHeight()));

	//if (DeferredLightAmbFInput->GetInputHandleByName("texDiffuseBuffer", handle))
	if (ShdInputLUT[DeferredLightAmbPS][texDiffuseBuffer] != ~0)
		DeferredLightAmbFInput->SetTexture(ShdInputLUT[DeferredLightAmbPS][texDiffuseBuffer], GBuffer->GetColorBuffer(0));

	//if (DeferredLightAmbFInput->GetInputHandleByName("fAmbientFactor", handle))
	if (ShdInputLUT[DeferredLightAmbPS][fAmbientFactor] != ~0)
		DeferredLightAmbFInput->SetFloat(ShdInputLUT[DeferredLightAmbPS][fAmbientFactor], 0.1f);

	DeferredLightAmbVP->Enable(DeferredLightAmbVInput);
	DeferredLightAmbFP->Enable(DeferredLightAmbFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DeferredLightAmbVP->Disable();
	DeferredLightAmbFP->Disable();
}

// Accumulate directional light
void AccumulateDirectionalLight()
{
	//unsigned int handle;

	//if (DeferredLightDirVInput->GetInputHandleByName("f44InvProjMat", handle))
	if (ShdInputLUT[DeferredLightDirVS][f44InvProjMat] != ~0)
		DeferredLightDirVInput->SetMatrix4x4(ShdInputLUT[DeferredLightDirVS][f44InvProjMat], invProjMat);

	//if (DeferredLightDirVInput->GetInputHandleByName("f2HalfTexelOffset", handle))
	if (ShdInputLUT[DeferredLightDirVS][f2HalfTexelOffset] != ~0)
		DeferredLightDirVInput->SetFloat2(ShdInputLUT[DeferredLightDirVS][f2HalfTexelOffset], Vec2f(0.5f / GBuffer->GetWidth(), 0.5f / GBuffer->GetHeight()));

	//if (DeferredLightDirFInput->GetInputHandleByName("texDiffuseBuffer", handle))
	if (ShdInputLUT[DeferredLightDirPS][texDiffuseBuffer] != ~0)
		DeferredLightDirFInput->SetTexture(ShdInputLUT[DeferredLightDirPS][texDiffuseBuffer], GBuffer->GetColorBuffer(0));

	//if (DeferredLightDirFInput->GetInputHandleByName("texNormalBuffer", handle))
	if (ShdInputLUT[DeferredLightDirPS][texNormalBuffer] != ~0)
		DeferredLightDirFInput->SetTexture(ShdInputLUT[DeferredLightDirPS][texNormalBuffer], GBuffer->GetColorBuffer(1));

	//if (DeferredLightDirFInput->GetInputHandleByName("texDepthBuffer", handle))
	if (ShdInputLUT[DeferredLightDirPS][texDepthBuffer] != ~0)
		DeferredLightDirFInput->SetTexture(ShdInputLUT[DeferredLightDirPS][texDepthBuffer], GBuffer->GetDepthBuffer());

	//if (DeferredLightDirFInput->GetInputHandleByName("texShadowMap", handle))
	if (ShdInputLUT[DeferredLightDirPS][texShadowMap] != ~0)
		DeferredLightDirFInput->SetTexture(ShdInputLUT[DeferredLightDirPS][texShadowMap], ShadowMapDir->GetDepthBuffer());

	//if (DeferredLightDirFInput->GetInputHandleByName("fShadowDepthBias", handle))
	if (ShdInputLUT[DeferredLightDirPS][fShadowDepthBias] != ~0)
		DeferredLightDirFInput->SetFloat(ShdInputLUT[DeferredLightDirPS][fShadowDepthBias], SHADOW_MAP_DEPTH_BIAS);

	//if (DeferredLightDirFInput->GetInputHandleByName("f2OneOverShadowMapSize", handle))
	if (ShdInputLUT[DeferredLightDirPS][f2OneOverShadowMapSize] != ~0)
		DeferredLightDirFInput->SetFloat2(ShdInputLUT[DeferredLightDirPS][f2OneOverShadowMapSize], Vec2f(1.f / (float)SHADOW_MAP_SIZE[0], 1.f / (float)SHADOW_MAP_SIZE[1]));

	//if (DeferredLightDirFInput->GetInputHandleByName("f44ViewMat", handle))
	if (ShdInputLUT[DeferredLightDirPS][f44ViewMat] != ~0)
		DeferredLightDirFInput->SetMatrix4x4(ShdInputLUT[DeferredLightDirPS][f44ViewMat], viewMat);

	//if (DeferredLightDirFInput->GetInputHandleByName("f44InvViewProjMat", handle))
	//if (ShdInputLUT[DeferredLightDirPS][f44InvViewProjMat] != ~0)
	//	DeferredLightDirFInput->SetMatrix4x4(ShdInputLUT[DeferredLightDirPS][f44InvViewProjMat], invViewProjMat);

	//if (DeferredLightDirFInput->GetInputHandleByName("f44ScreenToLightViewMat", handle))
	if (ShdInputLUT[DeferredLightDirPS][f44ScreenToLightViewMat] != ~0)
		DeferredLightDirFInput->SetMatrix4x4(ShdInputLUT[DeferredLightDirPS][f44ScreenToLightViewMat], dirLightViewMat * invViewProjMat);

	//if (DeferredLightDirFInput->GetInputHandleByName("f3LightDir", handle))
	if (ShdInputLUT[DeferredLightDirPS][f3LightDir] != ~0)
		DeferredLightDirFInput->SetFloat3(ShdInputLUT[DeferredLightDirPS][f3LightDir], directionalLightDir);

	//if (DeferredLightDirFInput->GetInputHandleByName("fDiffuseFactor", handle))
	if (ShdInputLUT[DeferredLightDirPS][fDiffuseFactor] != ~0)
		DeferredLightDirFInput->SetFloat(ShdInputLUT[DeferredLightDirPS][fDiffuseFactor], 1.f);

	//if (DeferredLightDirFInput->GetInputHandleByName("fSpecFactor", handle))
	if (ShdInputLUT[DeferredLightDirPS][fSpecFactor] != ~0)
		DeferredLightDirFInput->SetFloat(ShdInputLUT[DeferredLightDirPS][fSpecFactor], 40.f);

	//if (DeferredLightDirFInput->GetInputHandleByName("bDebugCascades", handle))
	if (ShdInputLUT[DeferredLightDirPS][bDebugCascades] != ~0)
		DeferredLightDirFInput->SetBool(ShdInputLUT[DeferredLightDirPS][bDebugCascades], DEBUG_CASCADES);

	//if (DeferredLightDirFInput->GetInputHandleByName("nCascadeCount", handle))
	//if (ShdInputLUT[DeferredLightDirPS][nCascadeCount] != ~0)
	//	DeferredLightDirFInput->SetInt(ShdInputLUT[DeferredLightDirPS][nCascadeCount], NUM_CASCADES);

	//if (DeferredLightDirFInput->GetInputHandleByName("nCascadesPerRow", handle))
	//if (ShdInputLUT[DeferredLightDirPS][nCascadesPerRow] != ~0)
	//	DeferredLightDirFInput->SetInt(ShdInputLUT[DeferredLightDirPS][nCascadesPerRow], (int)Math::ceil(Math::sqrt((float)NUM_CASCADES)));

	//if (DeferredLightDirFInput->GetInputHandleByName("fCascadeNormSize", handle))
	//if (ShdInputLUT[DeferredLightDirPS][fCascadeNormSize] != ~0)
	//	DeferredLightDirFInput->SetFloat(ShdInputLUT[DeferredLightDirPS][fCascadeNormSize], 1.f / Math::ceil(Math::sqrt((float)NUM_CASCADES)));

	//if (DeferredLightDirFInput->GetInputHandleByName("f2CascadeBoundsMin", handle))
	if (ShdInputLUT[DeferredLightDirPS][f2CascadeBoundsMin] != ~0)
		DeferredLightDirFInput->SetFloatArray(ShdInputLUT[DeferredLightDirPS][f2CascadeBoundsMin], cascadeBoundsMin);

	//if (DeferredLightDirFInput->GetInputHandleByName("f2CascadeBoundsMax", handle))
	if (ShdInputLUT[DeferredLightDirPS][f2CascadeBoundsMax] != ~0)
		DeferredLightDirFInput->SetFloatArray(ShdInputLUT[DeferredLightDirPS][f2CascadeBoundsMax], cascadeBoundsMax);

	//if (DeferredLightDirFInput->GetInputHandleByName("f44CascadeProjMat", handle))
	if (ShdInputLUT[DeferredLightDirPS][f44CascadeProjMat] != ~0)
		DeferredLightDirFInput->SetMatrixArray(ShdInputLUT[DeferredLightDirPS][f44CascadeProjMat], dirLightProjMat);

	//if (DeferredLightDirFInput->GetInputHandleByName("fCascadeBlendSize", handle))
	if (ShdInputLUT[DeferredLightDirPS][fCascadeBlendSize] != ~0)
		DeferredLightDirFInput->SetFloat(ShdInputLUT[DeferredLightDirPS][fCascadeBlendSize], CASCADE_BLEND_SIZE);

	DeferredLightDirVP->Enable(DeferredLightDirVInput);
	DeferredLightDirFP->Enable(DeferredLightDirFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DeferredLightDirVP->Disable();
	DeferredLightDirFP->Disable();
}

// Accumulate contributions from all light sources into the light accumulation buffer
void AccumulateLight()
{
	LightAccumulationBuffer->Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	// Copy-resolve the depth buffer for later usage
	ResolveDepthBuffer();

	// Disable Z writes, since we already have the correct depth buffer
	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	bool blendEnabled;
	Blend DstBlend, SrcBlend;
	blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnable();
	DstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	SrcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	// Additive color blending is required for accumulating light
	RenderContext->GetRenderStateManager()->SetColorBlendEnable(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ONE);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);

	// Accumulate the contributions from the various light sources
	AccumulateAmbientLight();
	AccumulateDirectionalLight();

	// Reset the render states
	RenderContext->GetRenderStateManager()->SetColorBlendEnable(blendEnabled);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(DstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(SrcBlend);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	// Draw the sky where the depth value is at 1 (no other object has been drawn there)
	DrawSky();

	LightAccumulationBuffer->Disable();
}

// Apply postprocessing effects. Afterwards, copy results to the backbuffer.
void ApplyPostProcessing()
{
	//unsigned int handle;

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	//if (PostProcessVInput->GetInputHandleByName("f2HalfTexelOffset", handle))
	if (ShdInputLUT[PostProcessVS][f2HalfTexelOffset] != ~0)
		PostProcessVInput->SetFloat2(ShdInputLUT[PostProcessVS][f2HalfTexelOffset], Vec2f(0.5f / LightAccumulationBuffer->GetWidth(), 0.5f / LightAccumulationBuffer->GetHeight()));

	//if (PostProcessFInput->GetInputHandleByName("texLightAccumulationBuffer", handle))
	if (ShdInputLUT[PostProcessPS][texLightAccumulationBuffer] != ~0)
		PostProcessFInput->SetTexture(ShdInputLUT[PostProcessPS][texLightAccumulationBuffer], LightAccumulationBuffer->GetColorBuffer(0));

	PostProcessVP->Enable(PostProcessVInput);
	PostProcessFP->Enable(PostProcessFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	PostProcessVP->Disable();
	PostProcessFP->Disable();

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);
}

// Render a frame
void RenderScene()
{
	if (RenderContext->BeginFrame())
	{
		UpdateMatrices();

		GenerateDirectionalShadowMap();

		GenerateGBuffer();

		AccumulateLight();

		ApplyPostProcessing();

		RenderContext->EndFrame();

		RenderContext->SwapBuffers();
	}
}