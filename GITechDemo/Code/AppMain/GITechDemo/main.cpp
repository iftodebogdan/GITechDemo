// GITechDemo.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <fstream>
#include <cstdlib>
#include <thread>

#include <Renderer.h>
#include <ShaderTemplate.h>
#include <SamplerState.h>
#include <RenderTarget.h>
#include <ResourceManager.h>
#include <VertexBuffer.h>
#include <Texture.h>
#include <ShaderInput.h>
#include <VertexFormat.h>
#include <IndexBuffer.h>
#include <RenderState.h>
#include <../Utility/ColorUtility.h>
using namespace LibRendererDll;

#include "PerlinNoise.h"
#include "Poisson.h"
#include "GaussianFilter.h"

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
/* GBuffer generation */
bool GBUFFER_Z_PREPASS = false;
float Z_NEAR = 1.f;
float Z_FAR = 5000.f;

bool AMBIENT_LIGHT_ENABLED = true;
bool DIRECTIONAL_LIGHT_ENABLED = true;
bool INDIRECT_LIGHT_ENABLED = true;
bool AMBIENT_OCCLUSION_ENABLED = true;

/* Ambient light parameters */
float AMBIENT_FACTOR = 0.1f;

/* Directional light parameters */
float DIRECTIONAL_DIFFUSE_FACTOR = 15.f;
float DIRECITONAL_SPECULAR_FACTOR = 75.f;

/* Sun parameters */
float SUN_RADIUS = 1000.f;
float SUN_BRIGHTNESS = 500.f;

/* Directional light shadow map parameters */
float SHADOW_MAP_DEPTH_BIAS = 0.01f;
const Vec<unsigned int, 2> SHADOW_MAP_SIZE = Vec<unsigned int, 2>(4096, 4096);
const unsigned int PCF_KERNEL_SIZE = 16;

/* Cascaded Shadow Map parameters */
bool DEBUG_CASCADES = false;
const unsigned int NUM_CASCADES = 4;
float CASCADE_SPLIT_FACTOR = 0.7f;
float CASCADE_MAX_VIEW_DEPTH = 3000.f;
float CASCADE_BLEND_SIZE = 25.f;

/* Reflective Shadow Map parameters */
const unsigned int RSM_SIZE = 512;
const unsigned int RSM_NUM_PASSES = 8;
const unsigned int RSM_SAMPLES_PER_PASS = 16;
const unsigned int RSM_NUM_SAMPLES = RSM_NUM_PASSES * RSM_SAMPLES_PER_PASS;
float RSM_INTENSITY = 150.f;
float RSM_KERNEL_SCALE = 0.015f;
bool USE_QUARTER_RES_INDIRECT_LIGHT_ACCUMULATION_BUFFER = true;

/* Screen-Space Ambient Occlusion */
float SSAO_SAMPLE_RADIUS = 10.f;
float SSAO_INTENSITY = 5.f;
float SSAO_SCALE = 0.05f;
float SSAO_BIAS = 0.25f;
const unsigned int SSAO_BLUR_KERNEL_COUNT = 3;
unsigned int SSAO_BLUR_KERNEL[SSAO_BLUR_KERNEL_COUNT] = { 0, 1, 2 };

/* Postprocessing parameters */
bool POST_PROCESSING_ENABLED = true;
// Tonemapping
bool HDR_TONEMAPPING_ENABLED = true;
float HDR_EXPOSURE_BIAS = 0.1f;
float HDR_AVG_LUMA_CLAMP_MIN = 0.00001f;
float HDR_AVG_LUMA_CLAMP_MAX = 0.25f;
float HDR_TONEMAPPING_SHOULDER_STRENGTH = 0.15f;
float HDR_TONEMAPPING_LINEAR_STRENGTH = 0.5f;
float HDR_TONEMAPPING_LINEAR_ANGLE = 0.07f;
float HDR_TONEMAPPING_TOE_STRENGTH = 0.75f;
float HDR_TONEMAPPING_TOE_NUMERATOR = 0.02f;
float HDR_TONEMAPPING_TOE_DENOMINATOR = 0.25f;
float HDR_TONEMAPPING_LINEAR_WHITE = 11.2f;
float HDR_LUMA_ADAPT_SPEED = 1.f;
// Bloom
bool BLOOM_ENABLED = true;
float BLOOM_BRIGHTNESS_THRESHOLD = 1.f;
float BLOOM_STRENGTH = 0.6f;
float BLOOM_POWER = 1.f;
const unsigned int BLOOM_BLUR_KERNEL_COUNT = 5;
unsigned int BLOOM_BLUR_KERNEL[BLOOM_BLUR_KERNEL_COUNT] = { 0, 1, 2, 2, 3 };
// FXAA
bool FXAA_ENABLED = true;
float FXAA_SUBPIX = 0.75f;
float FXAA_EDGE_THRESHOLD = 0.166f;
float FXAA_EDGE_THRESHOLD_MIN = 0.0833f;
// DoF
bool DOF_ENABLED = true;
bool DOF_USE_QUARTER_RESOLUTION_BUFFER = false;
float DOF_FOCAL_DEPTH = 100.f;
float DOF_FOCAL_LENGTH = 75.f;
float DOF_FSTOP = 3.5f;
float DOF_COC = 0.02f;
float DOF_NEAR_START = 1.f;
float DOF_NEAR_FALLOFF = 2.f;
float DOF_FAR_START = 1.f;
float DOF_FAR_FALLOFF = 3.f;
bool DOF_MANUAL = false;
bool DOF_DEBUG_FOCUS = false;
bool DOF_AUTOFOCUS = true;
Vec2f DOF_FOCUS_POINT = Vec2f(0.5f, 0.5f);
float DOF_MAX_BLUR = 1.f;
float DOF_HIGHLIGHT_THRESHOLD = 1.f;
float DOF_HIGHLIGHT_GAIN = 10.f;
float DOF_BOKEH_BIAS = 0.75f;
float DOF_BOKEH_FRINGE = 2.5f;
bool DOF_PENTAGON_BOKEH = false;
float DOF_PENTAGON_FEATHER = 0.4f;
bool DOF_USE_NOISE = false;
float DOF_NOISE_AMOUNT = 0.0001f;
bool DOF_BLUR_DEPTH = false;
float DOF_DEPTH_BLUR_SIZE = 0.001f;
bool DOF_VIGNETTING = true;
float DOF_VIGNETTING_OUTER_BORDER = 1.f;
float DOF_VIGNETTING_INNER_BORDER = 0.f;
float DOF_VIGNETTING_FADE = 22.f;

/* Debug options */
bool DEBUG_CSM_CAMERA = false;
bool DEBUG_RSM_CAMERA = false;
///////////////////////////////

// Cascaded Shadow Maps (CSM) and directional light related variables
#define MAX_NUM_CASCADES (9)
Vec2f cascadeBoundsMin[MAX_NUM_CASCADES];
Vec2f cascadeBoundsMax[MAX_NUM_CASCADES];
Matrix44f dirLightWorldViewProjMat[MAX_NUM_CASCADES];
Matrix44f dirLightWorldViewMat;
Matrix44f dirLightViewMat;
Matrix44f dirInvLightViewMat;
Matrix44f dirScreenToLightViewMat;
Matrix44f dirLightProjMat[MAX_NUM_CASCADES];
Matrix44f dirLightViewProjMat[MAX_NUM_CASCADES];
Vec3f directionalLightDir = Vec3f(0.f, -1.f, 0.f);
Matrix44f directionalLightRotMat;
Matrix44f skyViewProjMat;
/////////////////////////////////////////////

// Reflective Shadow Map (RSM) related variables
Matrix44f RSMProjMat;
Matrix44f RSMInvProjMat;
Matrix44f RSMWorldViewProjMat;
Matrix44f ViewToRSMViewMat;
Vec3f RSMKernel[RSM_NUM_SAMPLES];
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
// Shader responsible for copying the light accumulation buffer contents to the backbuffer
ShaderTemplate*		ColorCopyVP = nullptr;
ShaderTemplate*		ColorCopyFP = nullptr;
// Shader responsible for capturing RSM data
ShaderTemplate*		RSMCaptureVP = nullptr;
ShaderTemplate*		RSMCaptureFP = nullptr;
// Shader responsible for applying indirect lighting from the RSM
ShaderTemplate*		RSMApplyVP = nullptr;
ShaderTemplate*		RSMApplyFP = nullptr;
// Shader responsible for downsampling the HDR light accumulation buffer
ShaderTemplate*		DownsampleVP = nullptr;
ShaderTemplate*		DownsampleFP = nullptr;
// Shader responsible for calculating the average luminance
ShaderTemplate*		LumaCalcVP = nullptr;
ShaderTemplate*		LumaCalcFP = nullptr;
// Shader responsible for mapping from HDR image space to LDR image space
ShaderTemplate*		HDRToneMappingVP = nullptr;
ShaderTemplate*		HDRToneMappingFP = nullptr;
// Shader responsible for bloom effect
ShaderTemplate*		BloomVP = nullptr;
ShaderTemplate*		BloomFP = nullptr;
// Shader responsible for antialiasing (FXAA)
ShaderTemplate*		FxaaVP = nullptr;
ShaderTemplate*		FxaaFP = nullptr;
// Shader responsible for screen-space ambient occlusion (SSAO)
ShaderTemplate*		SsaoVP = nullptr;
ShaderTemplate*		SsaoFP = nullptr;
// Shader responsible for depth of field effect
ShaderTemplate*		BokehDofVP = nullptr;
ShaderTemplate*		BokehDofFP = nullptr;

// Shader input instances for setting shader resources
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
ShaderInput*	ColorCopyVInput = nullptr;
ShaderInput*	ColorCopyFInput = nullptr;
ShaderInput*	RSMCaptureVInput = nullptr;
ShaderInput*	RSMCaptureFInput = nullptr;
ShaderInput*	RSMApplyVInput = nullptr;
ShaderInput*	RSMApplyFInput = nullptr;
ShaderInput*	DownsampleVInput = nullptr;
ShaderInput*	DownsampleFInput = nullptr;
ShaderInput*	LumaCalcVInput = nullptr;
ShaderInput*	LumaCalcFInput = nullptr;
ShaderInput*	HDRToneMappingVInput = nullptr;
ShaderInput*	HDRToneMappingFInput = nullptr;
ShaderInput*	BloomVInput = nullptr;
ShaderInput*	BloomFInput = nullptr;
ShaderInput*	FxaaVInput = nullptr;
ShaderInput*	FxaaFInput = nullptr;
ShaderInput*	SsaoVInput = nullptr;
ShaderInput*	SsaoFInput = nullptr;
ShaderInput*	BokehDofVInput = nullptr;
ShaderInput*	BokehDofFInput = nullptr;

// A lookup table for shader inputs (faster than searching everytime by input name)
enum ShaderName
{
	SHADER_BEGIN = -1,

	SkyBoxVS, SkyBoxPS,
	GBufferGenerationVS, GBufferGenerationPS,
	DeferredLightAmbVS, DeferredLightAmbPS,
	DeferredLightDirVS, DeferredLightDirPS,
	DepthPassVS, DepthPassPS,
	DepthCopyVS, DepthCopyPS,
	ColorCopyVS, ColorCopyPS,
	RSMCaptureVS, RSMCapturePS,
	RSMApplyVS, RSMApplyPS,
	DownsampleVS, DownsamplePS,
	LumaCalcVS, LumaCalcPS,
	HDRToneMappingVS, HDRToneMappingPS,
	BloomVS, BloomPS,
	FxaaVS, FxaaPS,
	SsaoVS, SsaoPS,
	BokehDofVS, BokehDofPS,

	SHADER_COUNT
};

enum ShaderInputName
{
	SHADER_INPUT_BEGIN = -1,

	f44WorldViewProjMat, f44WorldViewMat, texDiffuse, texNormal, bHasNormalMap,
	texSpec, bHasSpecMap, fSpecIntensity, f2HalfTexelOffset, texDepthBuffer,
	f44SkyViewProjMat, texSkyTex, texDiffuseBuffer, fAmbientFactor, f44InvProjMat,
	texNormalBuffer, texShadowMap, fShadowDepthBias, f2OneOverShadowMapSize, f44ViewMat,
	f44InvViewProjMat, f44ScreenToLightViewMat, f3LightDir, fDiffuseFactor, fSpecFactor,
	bDebugCascades, nCascadeCount, nCascadesPerRow, fCascadeNormSize, f2CascadeBoundsMin,
	f2CascadeBoundsMax, f44CascadeProjMat, fCascadeBlendSize, texLightAccumulationBuffer,
	poissonDisk, f44LightWorldViewProjMat, f44LightWorldViewMat, texRSMFluxBuffer,
	texRSMNormalBuffer, texRSMDepthBuffer, f3RSMKernel, f44RSMProjMat, f44RSMInvProjMat,
	f44ViewToRSMViewMat, fRSMIntensity, fRSMKernelScale, fSunRadius, fSunBrightness,
	texIndirectLightAccumulationBuffer, bIsUpscalePass, texLumaCalcInput, bInitialLumaPass,
	bFinalLumaPass, texAvgLuma, fExposureBias, f2AvgLumaClamp, texSource, f2TexelSize,
	fShoulderStrength, fLinearStrength, fLinearAngle, fToeStrength, fToeNumerator,
	fToeDenominator, fLinearWhite, bLumaAdaptationPass, fLumaAdaptSpeed, fFrameTime,
	texLumaTarget, fBrightnessThreshold, bApplyBrightnessFilter, fBloomPower, nKernel,
	nDownsampleFactor, fBloomStrength, f2ChromaShiftScale, fGaussianKernel, fFxaaSubpix,
	fFxaaEdgeThreshold, fFxaaEdgeThresholdMin, fSSAOSampleRadius, fSSAOIntensity,
	fSSAOScale, fSSAOBias, bBlurPass, f2TexSourceSize, fFocalDepth, fFocalLength, fFStop,
	fCoC, fNearDofStart, fNearDofFalloff, fFarDofStart, fFarDofFalloff, bManualDof,
	bDebugFocus, bAutofocus, f2FocusPoint, fMaxBlur, fHighlightThreshold, fHighlightGain,
	fBokehBias, fBokehFringe, bPentagonBokeh, fPentagonFeather, bUseNoise, fNoiseAmount,
	bBlurDepth, fDepthBlurSize, bVignetting, fVignOut, fVignIn, fVignFade,

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
	"f2CascadeBoundsMax", "f44CascadeProjMat", "fCascadeBlendSize", "texLightAccumulationBuffer",
	"poissonDisk", "f44LightWorldViewProjMat", "f44LightWorldViewMat", "texRSMFluxBuffer",
	"texRSMNormalBuffer", "texRSMDepthBuffer", "f3RSMKernel", "f44RSMProjMat", "f44RSMInvProjMat",
	"f44ViewToRSMViewMat", "fRSMIntensity", "fRSMKernelScale", "fSunRadius", "fSunBrightness",
	"texIndirectLightAccumulationBuffer", "bIsUpscalePass", "texLumaCalcInput", "bInitialLumaPass",
	"bFinalLumaPass", "texAvgLuma", "fExposureBias", "f2AvgLumaClamp", "texSource", "f2TexelSize",
	"fShoulderStrength", "fLinearStrength", "fLinearAngle", "fToeStrength", "fToeNumerator",
	"fToeDenominator", "fLinearWhite", "bLumaAdaptationPass", "fLumaAdaptSpeed", "fFrameTime",
	"texLumaTarget", "fBrightnessThreshold", "bApplyBrightnessFilter", "fBloomPower", "nKernel",
	"nDownsampleFactor", "fBloomStrength", "f2ChromaShiftScale", "fGaussianKernel", "fFxaaSubpix",
	"fFxaaEdgeThreshold", "fFxaaEdgeThresholdMin", "fSSAOSampleRadius", "fSSAOIntensity",
	"fSSAOScale", "fSSAOBias", "bBlurPass", "f2TexSourceSize", "fFocalDepth", "fFocalLength", "fFStop",
	"fCoC", "fNearDofStart", "fNearDofFalloff", "fFarDofStart", "fFarDofFalloff", "bManualDof",
	"bDebugFocus", "bAutofocus", "f2FocusPoint", "fMaxBlur", "fHighlightThreshold", "fHighlightGain",
	"fBokehBias", "fBokehFringe", "bPentagonBokeh", "fPentagonFeather", "bUseNoise", "fNoiseAmount",
	"bBlurDepth", "fDepthBlurSize", "bVignetting", "fVignOut", "fVignIn", "fVignFade"
};

const ShaderInput * const * const ShaderInputLUT::inputHandler[] = {
	&SkyBoxVInput, &SkyBoxFInput,
	&GBufferGenerationVInput, &GBufferGenerationFInput,
	&DeferredLightAmbVInput, &DeferredLightAmbFInput,
	&DeferredLightDirVInput, &DeferredLightDirFInput,
	&DepthPassVInput, &DepthPassFInput,
	&DepthCopyVInput, &DepthCopyFInput,
	&ColorCopyVInput, &ColorCopyFInput,
	&RSMCaptureVInput, &RSMCaptureFInput,
	&RSMApplyVInput, &RSMApplyFInput,
	&DownsampleVInput, &DownsampleFInput,
	&LumaCalcVInput, &LumaCalcFInput,
	&HDRToneMappingVInput, &HDRToneMappingFInput,
	&BloomVInput, &BloomFInput,
	&FxaaVInput, &FxaaFInput,
	&SsaoVInput, &SsaoFInput,
	&BokehDofVInput, &BokehDofFInput
};

// The geometry buffer, holding information necessary for calculating light contribution
RenderTarget*	GBuffer = nullptr;
// The shadow map corresponding to the directional light
RenderTarget*	ShadowMapDir = nullptr;
// The render target in which we accumulate the light contribution from all lights
RenderTarget*	LightAccumulationBuffer = nullptr;
// The RSM render target
RenderTarget*	RSMBuffer = nullptr;
// The indirect light accumulation buffer (quarter resolution)
RenderTarget*	IndirectLightAccumulationBuffer = nullptr;
// The 1/16 resolution target in which the light accumulation buffer is downsampled
RenderTarget*	HDRDownsampleBuffer = nullptr;
// The 64x64, 16x16, 4x4 and 1x1 average luminance render targets
RenderTarget*	AverageLuminanceBuffer[4];
// The two 1x1 adapted luminance render targets (per frame swapped)
RenderTarget*	AdaptedLuminanceCurr;
RenderTarget*	AdaptedLuminancePrev;
// Two HDR render targets for bloom
RenderTarget*	HDRBloomBuffer[2];
// The LDR tone mapped image
RenderTarget*	LDRToneMappedImageBuffer;
// The LDR antialiased image
RenderTarget*	LDRFxaaImageBuffer;
// The ambient occlusion texture
RenderTarget*	AmbientOcclusionBuffer[2];
// The depth of field render target
RenderTarget*	DepthOfFieldFullBuffer;
RenderTarget*	DepthOfFieldQuarterBuffer;

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

// Poisson sampling for PCF
Vec2f PoissonDisk[PCF_KERNEL_SIZE];

// Gaussian kernel for blurring
float GaussianKernel[16];

// App states
enum AppState
{
	APP_STARTING,
	APP_LOADING_RESOURCES,
	APP_DONE_LOADING_RESOURCES,
	APP_RUNNING,
	APP_EXITING
};
AppState eAppState(APP_STARTING);

// Loading thread
std::thread LoadingThread;
std::string LoadingThreadBuffer;
ULONGLONG eventStart;

void PushLoadThreadEvent(const char* text)
{
	eventStart = GetTickCount64();
	char tmpBuffer[256];
	sprintf_s(tmpBuffer, "%s... ", text);
	LoadingThreadBuffer.append(tmpBuffer);
	RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_ERASE | RDW_INVALIDATE);
}

void PopLoadThreadEvent()
{
	char tmpBuffer[256];
	sprintf_s(tmpBuffer, "Done in %llu ms\n", GetTickCount64() - eventStart);
	LoadingThreadBuffer.append(tmpBuffer);
	RedrawWindow(hWnd, NULL, NULL, RDW_INTERNALPAINT | RDW_ERASE | RDW_INVALIDATE);
}

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void AllocateRenderResources();
void HandleUserInput(MSG msg, Vec2i& pos, Vec2i& center);
void UpdateMatrices();
void GenerateDirectionalShadowMap();
void GenerateRSM();
void GenerateGBuffer();
void CopyDepthBuffer();
void DrawSky();
void AccumulateAmbientOcclusion();
void AccumulateAmbientLight();
void AccumulateDirectionalLight();
void AccumulateIndirectLight();
void AccumulateLight();
void HDRDownsamplePass();
void LuminanceMeasurementPass();
void HDRToneMappingPass();
void FxaaPass();
void BloomPass();
void DepthOfFieldPass();
void ApplyPostProcessing();
void CopyResultToBackBuffer(RenderTarget* const rt);
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

			if(eAppState == APP_RUNNING)
				HandleUserInput(msg, pos, center);
		}

		// If the message is WM_QUIT, exit the while loop
		if (msg.message == WM_QUIT)
		{
			eAppState = APP_EXITING;
			break;
		}

		switch (eAppState)
		{
		case APP_DONE_LOADING_RESOURCES:
			LoadingThread.join();
			eAppState = APP_RUNNING;
			break;

		case APP_RUNNING:
		{
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

			// Set the size of the backbuffer according to the size of the window
			RECT rc;
			GetClientRect(hWnd, &rc);
			Vec2i vp = Vec2i(rc.right - rc.left, rc.bottom - rc.top);
			RenderContext->SetBackBufferSize(vp);

			// Render a frame
			RenderScene();
			break;
		}
		}
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

	// Renderer MUST be initialized on the SAME thread as the target window
	Renderer::CreateInstance(API_DX9);
	//Renderer::CreateInstance(API_NULL);
	RenderContext = Renderer::GetInstance();
	RenderContext->Initialize(hWnd);
	ResourceMgr = RenderContext->GetResourceManager();

	// Load resources on separate thread
	LoadingThread = std::thread(AllocateRenderResources);
	//AllocateRenderResources();

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
		DrawTextA(hdc, LoadingThreadBuffer.c_str(), (int)LoadingThreadBuffer.length(), &rc, DT_LEFT);
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

// Allocate/load resources required for rendering the scene
void AllocateRenderResources()
{
	PUSH_PROFILE_MARKER("AllocateRenderResources()");

	assert(eAppState == APP_STARTING);
	eAppState = APP_LOADING_RESOURCES;
	
	// First of all, load a model file
	PushLoadThreadEvent("Loading scene");
	const unsigned int modelIdx = ResourceMgr->CreateModel("models\\sponza\\sponza.lrm");
	SponzaScene = ResourceMgr->GetModel(modelIdx);
	PopLoadThreadEvent(); // scene

	// Calculate its AABB
	// this will be used later when calculating the cascade bounds for the CSM
	PushLoadThreadEvent("Processing scene");
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
	PopLoadThreadEvent(); // processing scene

	// Now load all the textures referenced by the model and build a lookup table
	PushLoadThreadEvent("Loading textures");
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
					tex->SetSRGBEnabled(true);
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
	ResourceMgr->GetTexture(skyTexIdx)->SetFilter(SF_MIN_MAG_LINEAR_MIP_LINEAR);
	PopLoadThreadEvent(); // textures

	// Load shaders
	PushLoadThreadEvent("Loading shaders");
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

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/ColorCopy.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/ColorCopy.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	ColorCopyVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	ColorCopyFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/Skybox.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/Skybox.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	SkyBoxVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	SkyBoxFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/RSMCapture.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/RSMCapture.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	RSMCaptureVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	RSMCaptureFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/RSMApply.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/RSMApply.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	RSMApplyVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	RSMApplyFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/Downsample.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/Downsample.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	DownsampleVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	DownsampleFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/LumaCalc.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/LumaCalc.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	LumaCalcVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	LumaCalcFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/HDRToneMapping.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/HDRToneMapping.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	HDRToneMappingVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	HDRToneMappingFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/Bloom.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/Bloom.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	BloomVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	BloomFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/FXAA.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/FXAA.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	FxaaVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	FxaaFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/SSAO.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/SSAO.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	SsaoVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	SsaoFP = ResourceMgr->GetShaderTemplate(pstIdx);

	vspIdx = ResourceMgr->CreateShaderProgram("shaders/BokehDoF.hlsl", SPT_VERTEX);
	pspIdx = ResourceMgr->CreateShaderProgram("shaders/BokehDoF.hlsl", SPT_PIXEL);
	vstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(vspIdx));
	BokehDofVP = ResourceMgr->GetShaderTemplate(vstIdx);
	pstIdx = ResourceMgr->CreateShaderTemplate(ResourceMgr->GetShaderProgram(pspIdx));
	BokehDofFP = ResourceMgr->GetShaderTemplate(pstIdx);
	PopLoadThreadEvent(); //shaders

	PushLoadThreadEvent("Creating shader resource table");
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

	vsiIdx = ResourceMgr->CreateShaderInput(ColorCopyVP);
	ColorCopyVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(ColorCopyFP);
	ColorCopyFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(SkyBoxVP);
	SkyBoxVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(SkyBoxFP);
	SkyBoxFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(RSMCaptureVP);
	RSMCaptureVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(RSMCaptureFP);
	RSMCaptureFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(RSMApplyVP);
	RSMApplyVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(RSMApplyFP);
	RSMApplyFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(DownsampleVP);
	DownsampleVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(DownsampleFP);
	DownsampleFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(LumaCalcVP);
	LumaCalcVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(LumaCalcFP);
	LumaCalcFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(HDRToneMappingVP);
	HDRToneMappingVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(HDRToneMappingFP);
	HDRToneMappingFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(BloomVP);
	BloomVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(BloomFP);
	BloomFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(FxaaVP);
	FxaaVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(FxaaFP);
	FxaaFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(SsaoVP);
	SsaoVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(SsaoFP);
	SsaoFInput = ResourceMgr->GetShaderInput(psiIdx);

	vsiIdx = ResourceMgr->CreateShaderInput(BokehDofVP);
	BokehDofVInput = ResourceMgr->GetShaderInput(vsiIdx);
	psiIdx = ResourceMgr->CreateShaderInput(BokehDofFP);
	BokehDofFInput = ResourceMgr->GetShaderInput(psiIdx);

	// Build a lookup table of shader inputs
	// NB: because this searches for all inputs in all shaders,
	// regardless if the input belongs to another shader, this
	// will generate some harmless errors in the console output
	for (unsigned int shader = SHADER_BEGIN + 1; shader < SHADER_COUNT; shader++)
		for (unsigned int input = SHADER_INPUT_BEGIN + 1; input < SHADER_INPUT_COUNT; input++)
			ShdInputLUT.getInputHandler((ShaderName)shader)->GetInputHandleByName(ShdInputLUT.getName((ShaderInputName)input), ShdInputLUT[shader][input]);
	PopLoadThreadEvent(); // shader resource table

	// Generate Poisson Disk samples
	PushLoadThreadEvent("Generating Poisson and Gaussian kernels");
	std::vector<sPoint> poisson;
	float minDist = sqrt((float)PCF_KERNEL_SIZE) / (float)PCF_KERNEL_SIZE * 0.8f;
	float oneOverMinDist = 1.f / minDist;
	const float sqrt2 = sqrt(2.f);
	do
	{
		 poisson =
			GeneratePoissonPoints(
				minDist,
				30,
				PCF_KERNEL_SIZE
				);
	} while (poisson.size() != PCF_KERNEL_SIZE);

	for (unsigned int i = 0; i < PCF_KERNEL_SIZE; i++)
	{
		PoissonDisk[i][0] = poisson[i].x * oneOverMinDist * sqrt2;
		PoissonDisk[i][1] = poisson[i].y * oneOverMinDist * sqrt2;
	}

	minDist = sqrt((float)RSM_NUM_SAMPLES) / (float)RSM_NUM_SAMPLES * 0.7f;
	do
	{
		poisson =
			GeneratePoissonPoints(
				minDist,
				30,
				RSM_NUM_SAMPLES
				);
	} while (poisson.size() != RSM_NUM_SAMPLES);

	for (unsigned int i = 0; i < RSM_NUM_SAMPLES; i++)
	{
		// Increase sample density towards the outside of the kernel
		RSMKernel[i][0] = sqrt(abs(poisson[i].x - 0.5f) * 2.f) * ((poisson[i].x < 0.5f) ? -1.f : 1.f);
		RSMKernel[i][1] = sqrt(abs(poisson[i].y - 0.5f) * 2.f) * ((poisson[i].y < 0.5f) ? -1.f : 1.f);
		// Linear weights combined with non-linear sample density has proven
		// to provide very good quality with very little jitter / noise
		RSMKernel[i][2] = length(Vec2f(poisson[i].x - 0.5f, poisson[i].y - 0.5f)) * 2.f;
	}

	CreateGaussianFilter(GaussianKernel, 16);
	PopLoadThreadEvent(); // kernels

	// Initialize the various render targets we will be using
	PushLoadThreadEvent("Allocating render targets");
	// Geometry buffer (GBuffer)
	//	RT0:	A8R8G8B8 with diffuse albedo in RGB and specular power in A
	//	RT1:	G16R16F with compressed normals (stereographic projection)
	//	DS:		INTZ (for sampling as a regular texture later)
	unsigned int rtIdx = ResourceMgr->CreateRenderTarget(2, PF_A8R8G8B8, PF_G16R16F, PF_NONE, PF_NONE, 1.f, 1.f, false, true, PF_INTZ);
	GBuffer = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(GBuffer->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(GBuffer->GetColorBuffer(1))->SetAddressingMode(SAM_CLAMP);
	ResourceMgr->GetTexture(GBuffer->GetDepthBuffer())->SetAddressingMode(SAM_CLAMP);

	// Shadow map for the directional light (the dummy color buffer is required because of DX9 limitations
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A8, SHADOW_MAP_SIZE[0], SHADOW_MAP_SIZE[1], false, true, PF_INTZ);
	ShadowMapDir = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(ShadowMapDir->GetDepthBuffer())->SetAddressingMode(SAM_BORDER);
	ResourceMgr->GetTexture(ShadowMapDir->GetDepthBuffer())->SetBorderColor(Vec4f(1.f, 1.f, 1.f, 1.f));

	// Render target in which we accumulate the light contribution from all light sources (floating point color components make it HDR-ready)
	// It contains a regular depth-stencil surface in which we will copy-resolve our INTZ depth texture from the GBuffer
	// (the depth is required for correctly rendering the sky and for future stencil optimizations)
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A16B16G16R16F, 1.f, 1.f, false, true, PF_D24S8);
	LightAccumulationBuffer = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(LightAccumulationBuffer->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(LightAccumulationBuffer->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);

	// Reflective Shadow Map render target
	rtIdx = ResourceMgr->CreateRenderTarget(2, PF_A8R8G8B8, PF_G16R16F, PF_NONE, PF_NONE, RSM_SIZE, RSM_SIZE, false, true, PF_INTZ);
	RSMBuffer = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(RSMBuffer->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(RSMBuffer->GetColorBuffer(0))->SetAddressingMode(SAM_BORDER);
	ResourceMgr->GetTexture(RSMBuffer->GetColorBuffer(0))->SetBorderColor(Vec4f(0.f, 0.f, 0.f, 0.f));
	ResourceMgr->GetTexture(RSMBuffer->GetColorBuffer(1))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(RSMBuffer->GetDepthBuffer())->SetAddressingMode(SAM_BORDER);
	ResourceMgr->GetTexture(RSMBuffer->GetDepthBuffer())->SetBorderColor(Vec4f(0.f, 0.f, 0.f, 0.f));

	// Indirect lighting accumulation buffer (quarter resolution)
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A16B16G16R16F, 0.5f, 0.5f, false, false, PF_NONE);
	IndirectLightAccumulationBuffer = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(IndirectLightAccumulationBuffer->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);

	// HDR downsampled buffer (1/16 resolution)
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A16B16G16R16F, 0.25f, 0.25f, false, false, PF_NONE);
	HDRDownsampleBuffer = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(HDRDownsampleBuffer->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	// Average luminance buffers (64x64, 16x16, 4x4, two 1x1, one target luma, one current luma)
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_R16F, 64u, 64u, false, false, PF_NONE);
	AverageLuminanceBuffer[0] = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(AverageLuminanceBuffer[0]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_R16F, 16u, 16u, false, false, PF_NONE);
	AverageLuminanceBuffer[1] = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(AverageLuminanceBuffer[1]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_R16F, 4u, 4u, false, false, PF_NONE);
	AverageLuminanceBuffer[2] = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(AverageLuminanceBuffer[2]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_R16F, 1u, 1u, false, false, PF_NONE);
	AverageLuminanceBuffer[3] = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(AverageLuminanceBuffer[3]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	// Adapted luminance for simulating light adaptation effect
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_R16F, 1u, 1u, false, false, PF_NONE);
	AdaptedLuminanceCurr = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(AdaptedLuminanceCurr->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_R16F, 1u, 1u, false, false, PF_NONE);
	AdaptedLuminancePrev = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(AdaptedLuminancePrev->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	// HDR bloom render targets
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A16B16G16R16F, 0.25f, 0.25f, false, false, PF_NONE);
	HDRBloomBuffer[0] = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(HDRBloomBuffer[0]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(HDRBloomBuffer[0]->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A16B16G16R16F, 0.25f, 0.25f, false, false, PF_NONE);
	HDRBloomBuffer[1] = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(HDRBloomBuffer[1]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(HDRBloomBuffer[1]->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);

	// LDR tone mapped render target
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A8R8G8B8, 1.f, 1.f, false, false, PF_NONE);
	LDRToneMappedImageBuffer = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(LDRToneMappedImageBuffer->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(LDRToneMappedImageBuffer->GetColorBuffer(0))->SetSRGBEnabled(true);

	// LDR tone mapped render target
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A8R8G8B8, 1.f, 1.f, false, false, PF_NONE);
	LDRFxaaImageBuffer = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(LDRFxaaImageBuffer->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	ResourceMgr->GetTexture(LDRFxaaImageBuffer->GetColorBuffer(0))->SetSRGBEnabled(true);

	// Ambient occlusion render target
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_L8, 1.f, 1.f, false, false, PF_NONE);
	AmbientOcclusionBuffer[0] = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(AmbientOcclusionBuffer[0]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_L8, 1.f, 1.f, false, false, PF_NONE);
	AmbientOcclusionBuffer[1] = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(AmbientOcclusionBuffer[1]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);

	// Depth of field render target
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A16B16G16R16F, 1.f, 1.f, false, false, PF_NONE);
	DepthOfFieldFullBuffer = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(DepthOfFieldFullBuffer->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(DepthOfFieldFullBuffer->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);
	rtIdx = ResourceMgr->CreateRenderTarget(1, PF_A16B16G16R16F, 0.5f, 0.5f, false, false, PF_NONE);
	DepthOfFieldQuarterBuffer = ResourceMgr->GetRenderTarget(rtIdx);
	ResourceMgr->GetTexture(DepthOfFieldQuarterBuffer->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
	ResourceMgr->GetTexture(DepthOfFieldQuarterBuffer->GetColorBuffer(0))->SetAddressingMode(SAM_CLAMP);
	PopLoadThreadEvent(); // render targets

	// Create a full screen quad (it's actually an over-sized triangle) for fullscreen effects and processing
	/*
	You can view the triangle as half of the quad required to fill a screen that's twice as
	high and twice as wide as our actual screen. The diagram below should help you visualize.
	Note that, as opposed to the traditional fullscreen quad composed of two triangles, this
	method avoids any artifact on the screen's diagonal, where the two triangles would line up.
	Also, it avoids having to shade twice along the screen's diagonal.

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

	unsigned int vfIdx = ResourceMgr->CreateVertexFormat(1, VAU_POSITION, VAT_FLOAT4, 0);
	VertexFormat* vf = ResourceMgr->GetVertexFormat(vfIdx);

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
	vfIdx = ResourceMgr->CreateVertexFormat(1, VAU_POSITION, VAT_FLOAT4, 0);
	vf = ResourceMgr->GetVertexFormat(vfIdx);

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

	tCamera.vPos = Vec3f(-840.f, -600.f, -195.f);
	tCamera.mRot.set(
		-0.440301329f,	0.00776965916f,	0.897806108f,	0.f,
		-0.142924204f,	0.986597657f,	-0.0786283761f,	0.f,
		-0.886387110f,	-0.162937075f,	-0.433295786f,	0.f,
		0.f,			0.f,			0.f,			1.f
		);

	Sleep(1000);

	assert(eAppState == APP_LOADING_RESOURCES);
	eAppState = APP_DONE_LOADING_RESOURCES;
	LoadingThreadBuffer.clear();

	POP_PROFILE_MARKER();
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

void UpdateMatrices()
{
	PUSH_PROFILE_MARKER("UpdateMatrices()");

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
	float noiseX = PerlinNoise.Get((float)GetTickCount64() / (float)INT_MAX, 0);
	float noiseZ = PerlinNoise.Get(0, (float)GetTickCount64() / (float)INT_MAX);
	directionalLightDir[0] = noiseX;//0.0f;//
	directionalLightDir[2] = noiseZ;//0.3f;//
	normalize(directionalLightDir);

	// Calculate directional light camera view matrix
	Vec3f zAxis = makeNormal(directionalLightDir);
	// Use the previous frame's up vector to avoid
	// the camera making sudden jumps when rolling over
	static Vec3f upVec = abs(zAxis[1]) == 1.f ? Vec3f(0.f, 0.f, 1.f) : Vec3f(0.f, 1.f, 0.f);
	Vec3f xAxis = makeNormal(makeCross(upVec, zAxis));
	Vec3f yAxis = makeCross(zAxis, xAxis);
	upVec = yAxis;
	dirLightViewMat.set(
		xAxis[0],	xAxis[1],	xAxis[2],	0.f,
		yAxis[0],	yAxis[1],	yAxis[2],	0.f,
		zAxis[0],	zAxis[1],	zAxis[2],	0.f,
		0.f,		0.f,		0.f,		1.f
		);
	dirLightWorldViewMat = dirLightViewMat * worldMat;
	invertFull(dirInvLightViewMat, dirLightViewMat);
	dirScreenToLightViewMat = dirLightViewMat * invViewProjMat;

	skyViewProjMat = viewProjMat * makeTrans(-tCamera.vPos, Type2Type<Matrix44f>());

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
		const unsigned int cascadesPerRow = (unsigned int)Math::ceil(Math::sqrt((float)NUM_CASCADES));
		const unsigned int cascadeSize = SHADOW_MAP_SIZE[0] / cascadesPerRow;
		//float pcfScale = ((float)PCF_KERNEL_SIZE * 2.f ) / (float)cascadeSize;
		//Vec3f aabbDiag = ViewFrustumPartitionLightSpaceAABB.mMax - ViewFrustumPartitionLightSpaceAABB.mMin;
		float offsetForPCF = 4.f * (float)PCF_KERNEL_SIZE * sqrt(2.f); // TODO: recheck the math on this one

		// Snap the ortographic projection to texel-sized increments in order to prevent shadow edges from jittering.
		// However, because we're tightly fitting the cascade around the view frustum, jittering will still be
		// present when rotating the camera, but not when zooming or strafing.
		Vec2f worldUnitsPerTexel = Vec2f(
			ViewFrustumPartitionLightSpaceAABB.mMax[0] - ViewFrustumPartitionLightSpaceAABB.mMin[0] + 2.f * offsetForPCF,
			ViewFrustumPartitionLightSpaceAABB.mMax[1] - ViewFrustumPartitionLightSpaceAABB.mMin[1] + 2.f * offsetForPCF) /
			Math::floor((float)SHADOW_MAP_SIZE[0] / Math::ceil(Math::sqrt((float)NUM_CASCADES))/*cascades per row*/);
		
		// Calculate the projection matrix for the current shadow map cascade
		RenderContext->CreateOrthographicMatrix(
			dirLightProjMat[cascade],
			Math::floor((ViewFrustumPartitionLightSpaceAABB.mMin[0] - offsetForPCF) / worldUnitsPerTexel[0]) * worldUnitsPerTexel[0],
			Math::ceil((ViewFrustumPartitionLightSpaceAABB.mMax[1] + offsetForPCF) / worldUnitsPerTexel[1]) * worldUnitsPerTexel[1],
			Math::ceil((ViewFrustumPartitionLightSpaceAABB.mMax[0] + offsetForPCF) / worldUnitsPerTexel[0]) * worldUnitsPerTexel[0],
			Math::floor((ViewFrustumPartitionLightSpaceAABB.mMin[1] - offsetForPCF) / worldUnitsPerTexel[1]) * worldUnitsPerTexel[1],
			SponzaLightSpaceAABB.mMin[2], SponzaLightSpaceAABB.mMax[2]);

		// Store the light space coordinates of the bounds of the current shadow map cascade
		cascadeBoundsMin[cascade] = Vec2f(ViewFrustumPartitionLightSpaceAABB.mMin[0], ViewFrustumPartitionLightSpaceAABB.mMin[1]);
		cascadeBoundsMax[cascade] = Vec2f(ViewFrustumPartitionLightSpaceAABB.mMax[0], ViewFrustumPartitionLightSpaceAABB.mMax[1]);

		// Calculate the current shadow map cascade's corresponding composite matrices
		dirLightViewProjMat[cascade] = dirLightProjMat[cascade] * dirLightViewMat;
		dirLightWorldViewProjMat[cascade] = dirLightViewProjMat[cascade] * worldMat;
	}
	
	// RSM matrices
	RenderContext->CreateOrthographicMatrix(RSMProjMat,
		SponzaLightSpaceAABB.mMin[0],
		SponzaLightSpaceAABB.mMax[1],
		SponzaLightSpaceAABB.mMax[0],
		SponzaLightSpaceAABB.mMin[1],
		SponzaLightSpaceAABB.mMin[2], SponzaLightSpaceAABB.mMax[2]);
	RSMWorldViewProjMat = RSMProjMat * dirLightWorldViewMat;
	invertFull(RSMInvProjMat, RSMProjMat);
	ViewToRSMViewMat = dirLightViewMat * invViewMat;

	// Debug sun camera
	if (DEBUG_CSM_CAMERA)
	{
		viewMat = dirLightViewMat;
		invertFull(invViewMat, viewMat);
		projMat = dirLightProjMat[NUM_CASCADES - 1];
		invertFull(invProjMat, projMat);
		worldViewMat = viewMat * worldMat;
		viewProjMat = projMat * viewMat;
		invViewProjMat = invViewMat * invProjMat;
		worldViewProjMat = projMat * worldViewMat;
		dirScreenToLightViewMat = dirLightViewMat * invViewProjMat;
		skyViewProjMat = viewProjMat * makeTrans(-tCamera.vPos, Type2Type<Matrix44f>());
	}

	// Debug RSM camera
	if (DEBUG_RSM_CAMERA)
	{
		viewMat = dirLightViewMat;
		invertFull(invViewMat, viewMat);
		projMat = RSMProjMat;
		invertFull(invProjMat, projMat);
		worldViewMat = viewMat * worldMat;
		viewProjMat = projMat * viewMat;
		invViewProjMat = invViewMat * invProjMat;
		worldViewProjMat = projMat * worldViewMat;
		dirScreenToLightViewMat = dirLightViewMat * invViewProjMat;
		skyViewProjMat = viewProjMat * makeTrans(-tCamera.vPos, Type2Type<Matrix44f>());
	}

	POP_PROFILE_MARKER();
}

void GenerateDirectionalShadowMap()
{
	PUSH_PROFILE_MARKER("GenerateDirectionalShadowMap()");

	bool red, blue, green, alpha;
	RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);
	bool scissorEnabled = RenderContext->GetRenderStateManager()->GetScissorEnabled();
	RenderContext->GetRenderStateManager()->SetScissorEnabled(true);

	ShadowMapDir->Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	assert(SHADOW_MAP_SIZE[0] == SHADOW_MAP_SIZE[1]);
	const unsigned int cascadesPerRow = (unsigned int)Math::ceil(Math::sqrt((float)NUM_CASCADES));
	const unsigned int cascadeSize = SHADOW_MAP_SIZE[0] / cascadesPerRow;
	for (unsigned int cascade = 0; cascade < NUM_CASCADES; cascade++)
	{
		char tmpBuf[16];
		sprintf_s(tmpBuf, "Cascade %d", cascade);
		PUSH_PROFILE_MARKER(tmpBuf);

		Vec2i size = Vec2i(cascadeSize, cascadeSize);
		Vec2i offset = Vec2i(cascadeSize * (cascade % cascadesPerRow), cascadeSize * (cascade / cascadesPerRow));
		RenderContext->SetViewport(size, offset);
		RenderContext->GetRenderStateManager()->SetScissor(size, offset);

		PUSH_PROFILE_MARKER("DepthPass.hlsl");
		if(ShdInputLUT[DepthPassVS][f44WorldViewProjMat] != ~0)
			DepthPassVInput->SetMatrix4x4(ShdInputLUT[DepthPassVS][f44WorldViewProjMat], dirLightWorldViewProjMat[cascade]);

		DepthPassVP->Enable(DepthPassVInput);
		DepthPassFP->Enable(DepthPassFInput);

		// Normally, you would only render meshes whose AABB/OBB intersect with the cascade's
		// view frustum, but we don't have a big enough scene to care at the moment
		for (unsigned int mesh = 0; mesh < SponzaScene->arrMesh.size(); mesh++)
		{
			PUSH_PROFILE_MARKER(SponzaScene->arrMaterial[SponzaScene->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
			RenderContext->DrawVertexBuffer(SponzaScene->arrMesh[mesh]->pVertexBuffer);
			POP_PROFILE_MARKER();
		}

		DepthPassVP->Disable();
		DepthPassFP->Disable();
		POP_PROFILE_MARKER();

		POP_PROFILE_MARKER();
	}

	ShadowMapDir->Disable();

	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetScissorEnabled(scissorEnabled);

	POP_PROFILE_MARKER();
}

void GenerateRSM()
{
	PUSH_PROFILE_MARKER("GenerateRSM()");

	RSMBuffer->Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	PUSH_PROFILE_MARKER("RSMCapture.hlsl");
	if (ShdInputLUT[RSMCaptureVS][f44LightWorldViewProjMat] != ~0)
		RSMCaptureVInput->SetMatrix4x4(ShdInputLUT[RSMCaptureVS][f44LightWorldViewProjMat], RSMWorldViewProjMat);

	if (ShdInputLUT[RSMCaptureVS][f44LightWorldViewMat] != ~0)
		RSMCaptureVInput->SetMatrix4x4(ShdInputLUT[RSMCaptureVS][f44LightWorldViewMat], dirLightWorldViewMat);

	RSMCaptureVP->Enable(RSMCaptureVInput);

	for (unsigned int mesh = 0; mesh < SponzaScene->arrMesh.size(); mesh++)
	{
		const unsigned int diffTexIdx = TextureLUT[Model::TextureDesc::TT_DIFFUSE][SponzaScene->arrMesh[mesh]->nMaterialIdx];
		const unsigned int normalTexIdx = TextureLUT[Model::TextureDesc::TT_HEIGHT][SponzaScene->arrMesh[mesh]->nMaterialIdx];

		if (ShdInputLUT[RSMCapturePS][texDiffuse] != ~0)
			RSMCaptureFInput->SetTexture(ShdInputLUT[RSMCapturePS][texDiffuse], diffTexIdx);

		RSMCaptureFP->Enable(RSMCaptureFInput);

		RenderContext->DrawVertexBuffer(SponzaScene->arrMesh[mesh]->pVertexBuffer);

		RSMCaptureFP->Disable();
	}

	RSMCaptureVP->Disable();
	POP_PROFILE_MARKER();

	RSMBuffer->Disable();

	POP_PROFILE_MARKER();
}

void GenerateGBuffer()
{
	PUSH_PROFILE_MARKER("GenerateGBuffer()");

	GBuffer->Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);
	
	// A depth prepass is useful if we have expensive pixel shaders in our
	// GBuffer generation process. It allows us to avoid shading pixels
	// that eventually get overwritten by other pixels that have smaller
	// depth values. We are not fill-rate bound, so the depth prepass is disabled.
	if (GBUFFER_Z_PREPASS)
	{
		PUSH_PROFILE_MARKER("G-Buffer Z prepass");

		bool red, blue, green, alpha;
		RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);
		RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);

		PUSH_PROFILE_MARKER("DepthPass.hlsl");
		if (ShdInputLUT[DepthPassVS][f44WorldViewProjMat] != ~0)
			DepthPassVInput->SetMatrix4x4(ShdInputLUT[DepthPassVS][f44WorldViewProjMat], worldViewProjMat);

		DepthPassVP->Enable(DepthPassVInput);
		DepthPassFP->Enable(DepthPassFInput);

		for (unsigned int mesh = 0; mesh < SponzaScene->arrMesh.size(); mesh++)
		{
			PUSH_PROFILE_MARKER(SponzaScene->arrMaterial[SponzaScene->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
			RenderContext->DrawVertexBuffer(SponzaScene->arrMesh[mesh]->pVertexBuffer);
			POP_PROFILE_MARKER();
		}

		DepthPassVP->Disable();
		DepthPassFP->Disable();
		POP_PROFILE_MARKER();

		RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);

		RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
		RenderContext->GetRenderStateManager()->SetZFunc(CMP_EQUAL);

		POP_PROFILE_MARKER();

		PUSH_PROFILE_MARKER("G-Buffer capture pass");
	}

	PUSH_PROFILE_MARKER("GBufferGeneration.hlsl");

	if (ShdInputLUT[GBufferGenerationVS][f44WorldViewMat] != ~0)
		GBufferGenerationVInput->SetMatrix4x4(ShdInputLUT[GBufferGenerationVS][f44WorldViewMat], worldViewMat);

	if (ShdInputLUT[GBufferGenerationVS][f44WorldViewProjMat] != ~0)
		GBufferGenerationVInput->SetMatrix4x4(ShdInputLUT[GBufferGenerationVS][f44WorldViewProjMat], worldViewProjMat);

	GBufferGenerationVP->Enable(GBufferGenerationVInput);

	// A visibility test would be useful if we were CPU bound (or vertex bound).
	// However, there isn't a reason to do such an optimization for now, since the
	// scene isn't very big and we are mostly pixel bound.
	for (unsigned int mesh = 0; mesh < SponzaScene->arrMesh.size(); mesh++)
	{
		PUSH_PROFILE_MARKER(SponzaScene->arrMaterial[SponzaScene->arrMesh[mesh]->nMaterialIdx]->szName.c_str());
		const unsigned int diffTexIdx = TextureLUT[Model::TextureDesc::TT_DIFFUSE][SponzaScene->arrMesh[mesh]->nMaterialIdx];
		const unsigned int normalTexIdx = TextureLUT[Model::TextureDesc::TT_HEIGHT][SponzaScene->arrMesh[mesh]->nMaterialIdx];
		const unsigned int specTexIdx = TextureLUT[Model::TextureDesc::TT_SPECULAR][SponzaScene->arrMesh[mesh]->nMaterialIdx];

		if (ShdInputLUT[GBufferGenerationPS][texDiffuse] != ~0)
			GBufferGenerationFInput->SetTexture(ShdInputLUT[GBufferGenerationPS][texDiffuse], diffTexIdx);

		if (ShdInputLUT[GBufferGenerationPS][texNormal] != ~0)
		{
			GBufferGenerationFInput->SetTexture(ShdInputLUT[GBufferGenerationPS][texNormal], normalTexIdx);

			if (ShdInputLUT[GBufferGenerationPS][bHasNormalMap] != ~0)
				GBufferGenerationFInput->SetBool(ShdInputLUT[GBufferGenerationPS][bHasNormalMap], normalTexIdx != -1);
		}

		if (ShdInputLUT[GBufferGenerationPS][texSpec] != ~0)
		{
			GBufferGenerationFInput->SetTexture(ShdInputLUT[GBufferGenerationPS][texSpec], specTexIdx);

			if (ShdInputLUT[GBufferGenerationPS][bHasSpecMap] != ~0)
				GBufferGenerationFInput->SetBool(ShdInputLUT[GBufferGenerationPS][bHasSpecMap], specTexIdx != -1);
		}

		if (ShdInputLUT[GBufferGenerationPS][fSpecIntensity] != ~0)
			GBufferGenerationFInput->SetFloat(ShdInputLUT[GBufferGenerationPS][fSpecIntensity], SponzaScene->arrMaterial[SponzaScene->arrMesh[mesh]->nMaterialIdx]->fShininessStrength);

		GBufferGenerationFP->Enable(GBufferGenerationFInput);

		RenderContext->DrawVertexBuffer(SponzaScene->arrMesh[mesh]->pVertexBuffer);

		GBufferGenerationFP->Disable();
		POP_PROFILE_MARKER();
	}

	GBufferGenerationVP->Disable();

	POP_PROFILE_MARKER();

	GBuffer->Disable();

	if (GBUFFER_Z_PREPASS)
	{
		RenderContext->GetRenderStateManager()->SetZWriteEnabled(true);
		RenderContext->GetRenderStateManager()->SetZFunc(CMP_LESSEQUAL);
		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

// This function copy-resolves the INTZ depth texture we use when generating the
// GBuffer (since we can sample it like a regular texture) to the D24S8 depth
// surface of the light accumulation buffer for depth testing (e.g. when rendering the sky)
// Having the correct depth allows us to do some more optimizations which also make use of
// the stencil (e.g. rendering a cone corresponding to the spot light and marking the pixels
// that pass the depth test, using the stencil buffer, so that we only calculate the light
// for those pixels and thus reducing pointless shading of pixels which are not lit)
void CopyDepthBuffer()
{
	PUSH_PROFILE_MARKER("CopyDepthBuffer()");

	bool red, blue, green, alpha;
	RenderContext->GetRenderStateManager()->GetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(false, false, false, false);
	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(true);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	PUSH_PROFILE_MARKER("DepthCopy.hlsl");
	if (ShdInputLUT[DepthCopyVS][f2HalfTexelOffset] != ~0)
		DepthCopyVInput->SetFloat2(ShdInputLUT[DepthCopyVS][f2HalfTexelOffset], Vec2f(0.5f / GBuffer->GetWidth(), 0.5f / GBuffer->GetHeight()));

	if (ShdInputLUT[DepthCopyPS][texDepthBuffer] != ~0)
		DepthCopyFInput->SetTexture(ShdInputLUT[DepthCopyPS][texDepthBuffer], GBuffer->GetDepthBuffer());

	DepthCopyVP->Enable(DepthCopyVInput);
	DepthCopyFP->Enable(DepthCopyFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DepthCopyVP->Disable();
	DepthCopyFP->Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorWriteEnabled(red, green, blue, alpha);
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	POP_PROFILE_MARKER();
}

// Draw a world axis-aligned cube with its' center coinciding with the camera's position.
// The vertex shader sets the visible face(s) at max depth so as to not draw over
// other objects that aren't inside the cube (which, at 2x2x2, is quite small)
void DrawSky()
{
	PUSH_PROFILE_MARKER("DrawSky()");

	bool blendEnabled;
	Blend DstBlend, SrcBlend;
	blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	DstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	SrcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ZERO);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);

	bool zWriteEnable = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_LESSEQUAL);

	PUSH_PROFILE_MARKER("SkyBox.hlsl");
	if (ShdInputLUT[SkyBoxVS][f44SkyViewProjMat] != ~0)
		SkyBoxVInput->SetMatrix4x4(ShdInputLUT[SkyBoxVS][f44SkyViewProjMat], skyViewProjMat);

	if (ShdInputLUT[SkyBoxPS][texSkyTex] != ~0)
		SkyBoxFInput->SetTexture(ShdInputLUT[SkyBoxPS][texSkyTex], skyTexIdx);

	if (ShdInputLUT[SkyBoxPS][f3LightDir] != ~0)
		SkyBoxFInput->SetFloat3(ShdInputLUT[SkyBoxPS][f3LightDir], directionalLightDir);

	if (ShdInputLUT[SkyBoxPS][fSunRadius] != ~0)
		SkyBoxFInput->SetFloat(ShdInputLUT[SkyBoxPS][fSunRadius], SUN_RADIUS);

	if (ShdInputLUT[SkyBoxPS][fSunBrightness] != ~0)
		SkyBoxFInput->SetFloat(ShdInputLUT[SkyBoxPS][fSunBrightness], SUN_BRIGHTNESS);

	SkyBoxVP->Enable(SkyBoxVInput);
	SkyBoxFP->Enable(SkyBoxFInput);

	RenderContext->DrawVertexBuffer(SkyBoxCube);

	SkyBoxVP->Disable();
	SkyBoxFP->Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(DstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(SrcBlend);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWriteEnable);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	POP_PROFILE_MARKER();
}

// Generate ambient occlusion buffer
void CalculateAmbientOcclusion()
{
	PUSH_PROFILE_MARKER("CalculateAmbientOcclusion()");
	AmbientOcclusionBuffer[0]->Enable();

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	PUSH_PROFILE_MARKER("SSAO.hlsl");
	if (ShdInputLUT[SsaoVS][f2HalfTexelOffset] != ~0)
		SsaoVInput->SetFloat2(ShdInputLUT[SsaoVS][f2HalfTexelOffset], Vec2f(0.5f / GBuffer->GetWidth(), 0.5f / GBuffer->GetHeight()));

	if (ShdInputLUT[SsaoPS][texNormalBuffer] != ~0)
		SsaoFInput->SetTexture(ShdInputLUT[SsaoPS][texNormalBuffer], GBuffer->GetColorBuffer(1));

	if (ShdInputLUT[SsaoPS][texDepthBuffer] != ~0)
		SsaoFInput->SetTexture(ShdInputLUT[SsaoPS][texDepthBuffer], GBuffer->GetDepthBuffer());

	if (ShdInputLUT[SsaoPS][f44InvProjMat] != ~0)
		SsaoFInput->SetMatrix4x4(ShdInputLUT[SsaoPS][f44InvProjMat], invProjMat);

	if (ShdInputLUT[SsaoPS][fSSAOSampleRadius] != ~0)
		SsaoFInput->SetFloat(ShdInputLUT[SsaoPS][fSSAOSampleRadius], SSAO_SAMPLE_RADIUS);

	if (ShdInputLUT[SsaoPS][fSSAOIntensity] != ~0)
		SsaoFInput->SetFloat(ShdInputLUT[SsaoPS][fSSAOIntensity], SSAO_INTENSITY);

	if (ShdInputLUT[SsaoPS][fSSAOScale] != ~0)
		SsaoFInput->SetFloat(ShdInputLUT[SsaoPS][fSSAOScale], SSAO_SCALE);

	if (ShdInputLUT[SsaoPS][fSSAOBias] != ~0)
		SsaoFInput->SetFloat(ShdInputLUT[SsaoPS][fSSAOBias], SSAO_BIAS);

	if (ShdInputLUT[SsaoPS][bBlurPass] != ~0)
		SsaoFInput->SetBool(ShdInputLUT[SsaoPS][bBlurPass], false);

	SsaoVP->Enable(SsaoVInput);
	SsaoFP->Enable(SsaoFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	SsaoVP->Disable();
	SsaoFP->Disable();
	POP_PROFILE_MARKER();

	AmbientOcclusionBuffer[0]->Disable();
	POP_PROFILE_MARKER();
}

// Blur ambient occlusion buffer
void BlurAmbientOcclusion()
{
	PUSH_PROFILE_MARKER("BlurAmbientOcclusion()");

	for (unsigned int i = 0; i < SSAO_BLUR_KERNEL_COUNT; i++)
	{
		char label[10];
		sprintf_s(label, "Kernel %d", SSAO_BLUR_KERNEL[i]);
		PUSH_PROFILE_MARKER(label);
		AmbientOcclusionBuffer[(i + 1) % 2]->Enable();

		// Not necesarry
		//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

		PUSH_PROFILE_MARKER("SSAO.hlsl");
		if (ShdInputLUT[SsaoVS][f2HalfTexelOffset] != ~0)
			SsaoVInput->SetFloat2(ShdInputLUT[SsaoVS][f2HalfTexelOffset], Vec2f(0.5f / AmbientOcclusionBuffer[i % 2]->GetWidth(), 0.5f / AmbientOcclusionBuffer[i % 2]->GetHeight()));

		if (ShdInputLUT[SsaoPS][bBlurPass] != ~0)
			SsaoFInput->SetBool(ShdInputLUT[SsaoPS][bBlurPass], true);

		if (ShdInputLUT[SsaoPS][texSource] != ~0)
		{
			ResourceMgr->GetTexture(AmbientOcclusionBuffer[i % 2]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
			SsaoFInput->SetTexture(ShdInputLUT[SsaoPS][texSource], AmbientOcclusionBuffer[i % 2]->GetColorBuffer(0));
		}

		if (ShdInputLUT[SsaoPS][f2TexelSize] != ~0)
			SsaoFInput->SetFloat2(ShdInputLUT[SsaoPS][f2TexelSize], Vec2f(1.f / AmbientOcclusionBuffer[i % 2]->GetWidth(), 1.f / AmbientOcclusionBuffer[i % 2]->GetHeight()));

		if (ShdInputLUT[SsaoPS][nKernel] != ~0)
			SsaoFInput->SetInt(ShdInputLUT[SsaoPS][nKernel], SSAO_BLUR_KERNEL[i]);

		SsaoVP->Enable(SsaoVInput);
		SsaoFP->Enable(SsaoFInput);

		RenderContext->DrawVertexBuffer(FullScreenQuad);

		SsaoFP->Disable();
		SsaoVP->Disable();
		POP_PROFILE_MARKER();

		AmbientOcclusionBuffer[(i + 1) % 2]->Disable();
		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

// Apply ambient occlusion to the light accumulation buffer
void ApplyAmbientOcclusion()
{
	PUSH_PROFILE_MARKER("ApplyAmbientOcclusion()");
	LightAccumulationBuffer->Enable();

	const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	const Blend dstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	const Blend srcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_INVSRCCOLOR);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ZERO);

	PUSH_PROFILE_MARKER("ColorCopy.hlsl");
	if (ShdInputLUT[ColorCopyVS][f2HalfTexelOffset] != ~0)
		ColorCopyVInput->SetFloat2(ShdInputLUT[ColorCopyVS][f2HalfTexelOffset], Vec2f(0.5f / AmbientOcclusionBuffer[SSAO_BLUR_KERNEL_COUNT % 2]->GetWidth(), 0.5f / AmbientOcclusionBuffer[SSAO_BLUR_KERNEL_COUNT % 2]->GetHeight()));

	if (ShdInputLUT[ColorCopyPS][texSource] != ~0)
	{
		ResourceMgr->GetTexture(AmbientOcclusionBuffer[SSAO_BLUR_KERNEL_COUNT % 2]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
		ColorCopyFInput->SetTexture(ShdInputLUT[ColorCopyPS][texSource], AmbientOcclusionBuffer[SSAO_BLUR_KERNEL_COUNT % 2]->GetColorBuffer(0));
	}

	ColorCopyVP->Enable(ColorCopyVInput);
	ColorCopyFP->Enable(ColorCopyFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	ColorCopyFP->Disable();
	ColorCopyVP->Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);

	LightAccumulationBuffer->Disable();
	POP_PROFILE_MARKER();
}

// Accumulate ambient occlusion
void AccumulateAmbientOcclusion()
{
	PUSH_PROFILE_MARKER("AccumulateAmbientOcclusion()");

	const bool blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

	RenderTarget* const rtBkp = RenderTarget::GetActiveRenderTarget();
	rtBkp->Disable();

	CalculateAmbientOcclusion();
	BlurAmbientOcclusion();
	ApplyAmbientOcclusion();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);

	rtBkp->Enable();
	POP_PROFILE_MARKER();
}

// Accumulate ambient light
void AccumulateAmbientLight()
{
	PUSH_PROFILE_MARKER("AccumulateAmbientLight()");

	PUSH_PROFILE_MARKER("DeferredLightAmb.hlsl");
	if (ShdInputLUT[DeferredLightAmbVS][f2HalfTexelOffset] != ~0)
		DeferredLightAmbVInput->SetFloat2(ShdInputLUT[DeferredLightAmbVS][f2HalfTexelOffset], Vec2f(0.5f / GBuffer->GetWidth(), 0.5f / GBuffer->GetHeight()));

	if (ShdInputLUT[DeferredLightAmbPS][texDiffuseBuffer] != ~0)
		DeferredLightAmbFInput->SetTexture(ShdInputLUT[DeferredLightAmbPS][texDiffuseBuffer], GBuffer->GetColorBuffer(0));

	if (ShdInputLUT[DeferredLightAmbPS][fAmbientFactor] != ~0)
		DeferredLightAmbFInput->SetFloat(ShdInputLUT[DeferredLightAmbPS][fAmbientFactor], AMBIENT_FACTOR);

	DeferredLightAmbVP->Enable(DeferredLightAmbVInput);
	DeferredLightAmbFP->Enable(DeferredLightAmbFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DeferredLightAmbVP->Disable();
	DeferredLightAmbFP->Disable();
	POP_PROFILE_MARKER();

	if (AMBIENT_OCCLUSION_ENABLED)
		AccumulateAmbientOcclusion();

	POP_PROFILE_MARKER();
}

// Accumulate directional light
void AccumulateDirectionalLight()
{
	PUSH_PROFILE_MARKER("AccumulateDirectionalLight()");

	PUSH_PROFILE_MARKER("DeferredLightDir.hlsl");
	if (ShdInputLUT[DeferredLightDirVS][f44InvProjMat] != ~0)
		DeferredLightDirVInput->SetMatrix4x4(ShdInputLUT[DeferredLightDirVS][f44InvProjMat], invProjMat);

	if (ShdInputLUT[DeferredLightDirVS][f2HalfTexelOffset] != ~0)
		DeferredLightDirVInput->SetFloat2(ShdInputLUT[DeferredLightDirVS][f2HalfTexelOffset], Vec2f(0.5f / GBuffer->GetWidth(), 0.5f / GBuffer->GetHeight()));

	if (ShdInputLUT[DeferredLightDirPS][texDiffuseBuffer] != ~0)
		DeferredLightDirFInput->SetTexture(ShdInputLUT[DeferredLightDirPS][texDiffuseBuffer], GBuffer->GetColorBuffer(0));

	if (ShdInputLUT[DeferredLightDirPS][texNormalBuffer] != ~0)
		DeferredLightDirFInput->SetTexture(ShdInputLUT[DeferredLightDirPS][texNormalBuffer], GBuffer->GetColorBuffer(1));

	if (ShdInputLUT[DeferredLightDirPS][texDepthBuffer] != ~0)
		DeferredLightDirFInput->SetTexture(ShdInputLUT[DeferredLightDirPS][texDepthBuffer], GBuffer->GetDepthBuffer());

	if (ShdInputLUT[DeferredLightDirPS][texShadowMap] != ~0)
		DeferredLightDirFInput->SetTexture(ShdInputLUT[DeferredLightDirPS][texShadowMap], ShadowMapDir->GetDepthBuffer());

	if (ShdInputLUT[DeferredLightDirPS][fShadowDepthBias] != ~0)
		DeferredLightDirFInput->SetFloat(ShdInputLUT[DeferredLightDirPS][fShadowDepthBias], SHADOW_MAP_DEPTH_BIAS);

	if (ShdInputLUT[DeferredLightDirPS][f2OneOverShadowMapSize] != ~0)
		DeferredLightDirFInput->SetFloat2(ShdInputLUT[DeferredLightDirPS][f2OneOverShadowMapSize], Vec2f(1.f / (float)SHADOW_MAP_SIZE[0], 1.f / (float)SHADOW_MAP_SIZE[1]));

	if (ShdInputLUT[DeferredLightDirPS][f44ViewMat] != ~0)
		DeferredLightDirFInput->SetMatrix4x4(ShdInputLUT[DeferredLightDirPS][f44ViewMat], viewMat);

	if (ShdInputLUT[DeferredLightDirPS][f44ScreenToLightViewMat] != ~0)
		DeferredLightDirFInput->SetMatrix4x4(ShdInputLUT[DeferredLightDirPS][f44ScreenToLightViewMat], dirScreenToLightViewMat);

	if (ShdInputLUT[DeferredLightDirPS][f3LightDir] != ~0)
		DeferredLightDirFInput->SetFloat3(ShdInputLUT[DeferredLightDirPS][f3LightDir], directionalLightDir);

	if (ShdInputLUT[DeferredLightDirPS][fDiffuseFactor] != ~0)
		DeferredLightDirFInput->SetFloat(ShdInputLUT[DeferredLightDirPS][fDiffuseFactor], DIRECTIONAL_DIFFUSE_FACTOR);

	if (ShdInputLUT[DeferredLightDirPS][fSpecFactor] != ~0)
		DeferredLightDirFInput->SetFloat(ShdInputLUT[DeferredLightDirPS][fSpecFactor], DIRECITONAL_SPECULAR_FACTOR);

	if (ShdInputLUT[DeferredLightDirPS][bDebugCascades] != ~0)
		DeferredLightDirFInput->SetBool(ShdInputLUT[DeferredLightDirPS][bDebugCascades], DEBUG_CASCADES);

	if (ShdInputLUT[DeferredLightDirPS][f2CascadeBoundsMin] != ~0)
		DeferredLightDirFInput->SetFloatArray(ShdInputLUT[DeferredLightDirPS][f2CascadeBoundsMin], cascadeBoundsMin);

	if (ShdInputLUT[DeferredLightDirPS][f2CascadeBoundsMax] != ~0)
		DeferredLightDirFInput->SetFloatArray(ShdInputLUT[DeferredLightDirPS][f2CascadeBoundsMax], cascadeBoundsMax);

	if (ShdInputLUT[DeferredLightDirPS][f44CascadeProjMat] != ~0)
		DeferredLightDirFInput->SetMatrixArray(ShdInputLUT[DeferredLightDirPS][f44CascadeProjMat], dirLightProjMat);

	if (ShdInputLUT[DeferredLightDirPS][fCascadeBlendSize] != ~0)
		DeferredLightDirFInput->SetFloat(ShdInputLUT[DeferredLightDirPS][fCascadeBlendSize], CASCADE_BLEND_SIZE);

	if (ShdInputLUT[DeferredLightDirPS][poissonDisk] != ~0)
		DeferredLightDirFInput->SetFloatArray(ShdInputLUT[DeferredLightDirPS][poissonDisk], PoissonDisk);

	DeferredLightDirVP->Enable(DeferredLightDirVInput);
	DeferredLightDirFP->Enable(DeferredLightDirFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DeferredLightDirVP->Disable();
	DeferredLightDirFP->Disable();
	POP_PROFILE_MARKER();

	POP_PROFILE_MARKER();
}

// Accumulate indirect lighting (RSM)
void AccumulateIndirectLight()
{
	PUSH_PROFILE_MARKER("AccumulateIndirectLight()");

	RenderTarget* const rtBkp = RenderTarget::GetActiveRenderTarget();
	if (USE_QUARTER_RES_INDIRECT_LIGHT_ACCUMULATION_BUFFER)
	{
		rtBkp->Disable();
		IndirectLightAccumulationBuffer->Enable();

		RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);
	}

	PUSH_PROFILE_MARKER("RSMApply.hlsl");
	if (ShdInputLUT[RSMApplyVS][f2HalfTexelOffset] != ~0)
		RSMApplyVInput->SetFloat2(ShdInputLUT[RSMApplyVS][f2HalfTexelOffset], Vec2f(0.5f / GBuffer->GetWidth(), 0.5f / GBuffer->GetHeight()));

	if (ShdInputLUT[RSMApplyPS][f2HalfTexelOffset] != ~0)
		RSMApplyFInput->SetFloat2(ShdInputLUT[RSMApplyPS][f2HalfTexelOffset], Vec2f(0.5f / GBuffer->GetWidth(), 0.5f / GBuffer->GetHeight()));

	if (ShdInputLUT[RSMApplyPS][texRSMFluxBuffer] != ~0)
		RSMApplyFInput->SetTexture(ShdInputLUT[RSMApplyPS][texRSMFluxBuffer], RSMBuffer->GetColorBuffer(0));

	if (ShdInputLUT[RSMApplyPS][texRSMNormalBuffer] != ~0)
		RSMApplyFInput->SetTexture(ShdInputLUT[RSMApplyPS][texRSMNormalBuffer], RSMBuffer->GetColorBuffer(1));

	if (ShdInputLUT[RSMApplyPS][texRSMDepthBuffer] != ~0)
		RSMApplyFInput->SetTexture(ShdInputLUT[RSMApplyPS][texRSMDepthBuffer], RSMBuffer->GetDepthBuffer());

	if (ShdInputLUT[RSMApplyPS][texNormalBuffer] != ~0)
		RSMApplyFInput->SetTexture(ShdInputLUT[RSMApplyPS][texNormalBuffer], GBuffer->GetColorBuffer(1));

	if (ShdInputLUT[RSMApplyPS][texDepthBuffer] != ~0)
		RSMApplyFInput->SetTexture(ShdInputLUT[RSMApplyPS][texDepthBuffer], GBuffer->GetDepthBuffer());

	if (ShdInputLUT[RSMApplyPS][f44ScreenToLightViewMat] != ~0)
		RSMApplyFInput->SetMatrix4x4(ShdInputLUT[RSMApplyPS][f44ScreenToLightViewMat], dirScreenToLightViewMat);

	if (ShdInputLUT[RSMApplyPS][f44RSMProjMat] != ~0)
		RSMApplyFInput->SetMatrix4x4(ShdInputLUT[RSMApplyPS][f44RSMProjMat], RSMProjMat);

	if (ShdInputLUT[RSMApplyPS][f44RSMInvProjMat] != ~0)
		RSMApplyFInput->SetMatrix4x4(ShdInputLUT[RSMApplyPS][f44RSMInvProjMat], RSMInvProjMat);

	if (ShdInputLUT[RSMApplyPS][f44ViewToRSMViewMat] != ~0)
		RSMApplyFInput->SetMatrix4x4(ShdInputLUT[RSMApplyPS][f44ViewToRSMViewMat], ViewToRSMViewMat);

	if (ShdInputLUT[RSMApplyPS][fRSMIntensity] != ~0)
		RSMApplyFInput->SetFloat(ShdInputLUT[RSMApplyPS][fRSMIntensity], RSM_INTENSITY);

	if (ShdInputLUT[RSMApplyPS][fRSMKernelScale] != ~0)
		RSMApplyFInput->SetFloat(ShdInputLUT[RSMApplyPS][fRSMKernelScale], RSM_KERNEL_SCALE);

	if (ShdInputLUT[RSMApplyPS][texIndirectLightAccumulationBuffer] != ~0)
		RSMApplyFInput->SetTexture(ShdInputLUT[RSMApplyPS][texIndirectLightAccumulationBuffer], IndirectLightAccumulationBuffer->GetColorBuffer(0));

	if (ShdInputLUT[RSMApplyPS][bIsUpscalePass] != ~0)
		RSMApplyFInput->SetBool(ShdInputLUT[RSMApplyPS][bIsUpscalePass], false);

	for (unsigned int i = 0; i < RSM_NUM_PASSES; i++)
	{
		if (ShdInputLUT[RSMApplyPS][f3RSMKernel] != ~0)
			RSMApplyFInput->SetFloatArray(ShdInputLUT[RSMApplyPS][f3RSMKernel], RSMKernel + i * RSM_SAMPLES_PER_PASS);

		char marker[16];
		sprintf_s(marker, "Pass %d", i);
		PUSH_PROFILE_MARKER(marker);

		RSMApplyVP->Enable(RSMApplyVInput);
		RSMApplyFP->Enable(RSMApplyFInput);

		RenderContext->DrawVertexBuffer(FullScreenQuad);

		RSMApplyVP->Disable();
		RSMApplyFP->Disable();

		POP_PROFILE_MARKER();
	}
	POP_PROFILE_MARKER();

	if (USE_QUARTER_RES_INDIRECT_LIGHT_ACCUMULATION_BUFFER)
	{
		IndirectLightAccumulationBuffer->Disable();
		rtBkp->Enable();

		PUSH_PROFILE_MARKER("RSMApply.hlsl (upsample)");

		//if (ShdInputLUT[RSMApplyVS][f2HalfTexelOffset] != ~0)
		//	RSMApplyVInput->SetFloat2(ShdInputLUT[RSMApplyVS][f2HalfTexelOffset], Vec2f(0.5f / IndirectLightAccumulationBuffer->GetWidth(), 0.5f / IndirectLightAccumulationBuffer->GetHeight()));

		if (ShdInputLUT[RSMApplyPS][f3RSMKernel] != ~0)
			RSMApplyFInput->SetFloatArray(ShdInputLUT[RSMApplyPS][f3RSMKernel], RSMKernel);

		if (ShdInputLUT[RSMApplyPS][bIsUpscalePass] != ~0)
			RSMApplyFInput->SetBool(ShdInputLUT[RSMApplyPS][bIsUpscalePass], true);

		RSMApplyVP->Enable(RSMApplyVInput);
		RSMApplyFP->Enable(RSMApplyFInput);

		RenderContext->DrawVertexBuffer(FullScreenQuad);

		RSMApplyVP->Disable();
		RSMApplyFP->Disable();

		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

// Accumulate contributions from all light sources into the light accumulation buffer
void AccumulateLight()
{
	PUSH_PROFILE_MARKER("AccumulateLight()");

	LightAccumulationBuffer->Enable();

	RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	// Copy-resolve the depth buffer for later usage
	CopyDepthBuffer();

	// Disable Z writes, since we already have the correct depth buffer
	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	bool blendEnabled;
	Blend DstBlend, SrcBlend;
	blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	DstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	SrcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	// Additive color blending is required for accumulating light
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ONE);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);

	// Accumulate the contributions from the various light sources
	if(AMBIENT_LIGHT_ENABLED)
		AccumulateAmbientLight();
		
	if(DIRECTIONAL_LIGHT_ENABLED)
		AccumulateDirectionalLight();

	if(INDIRECT_LIGHT_ENABLED)
		AccumulateIndirectLight();

	// Reset the render states
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(DstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(SrcBlend);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	// Draw the sky where the depth value is at 1 (no other object has been drawn there)
	DrawSky();

	LightAccumulationBuffer->Disable();

	POP_PROFILE_MARKER();
}

// Downscale the HDR light accumulation texture
void HDRDownsamplePass()
{
	PUSH_PROFILE_MARKER("HDRDownsamplePass()");

	HDRDownsampleBuffer->Enable();

	// Not necessary
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	PUSH_PROFILE_MARKER("Downsample.hlsl");
	if (ShdInputLUT[DownsampleVS][f2HalfTexelOffset] != ~0)
		DownsampleVInput->SetFloat2(ShdInputLUT[DownsampleVS][f2HalfTexelOffset], Vec2f(0.5f / HDRDownsampleBuffer->GetWidth(), 0.5f / HDRDownsampleBuffer->GetHeight()));

	if (ShdInputLUT[DownsamplePS][f2TexelSize] != ~0)
		DownsampleFInput->SetFloat2(ShdInputLUT[DownsamplePS][f2TexelSize], Vec2f(1.f / LightAccumulationBuffer->GetWidth(), 1.f / LightAccumulationBuffer->GetHeight()));

	if (ShdInputLUT[DownsamplePS][texSource] != ~0)
		DownsampleFInput->SetTexture(ShdInputLUT[DownsamplePS][texSource], LightAccumulationBuffer->GetColorBuffer(0));

	if (ShdInputLUT[DownsamplePS][nDownsampleFactor] != ~0)
		DownsampleFInput->SetInt(ShdInputLUT[DownsamplePS][nDownsampleFactor], 16);

	if (ShdInputLUT[DownsamplePS][bApplyBrightnessFilter] != ~0)
		DownsampleFInput->SetBool(ShdInputLUT[DownsamplePS][bApplyBrightnessFilter], false);

	DownsampleVP->Enable(DownsampleVInput);
	DownsampleFP->Enable(DownsampleFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DownsampleVP->Disable();
	DownsampleFP->Disable();
	POP_PROFILE_MARKER();

	HDRDownsampleBuffer->Disable();
	
	POP_PROFILE_MARKER();
}

// Measure average luminance level of scene
void LuminanceMeasurementPass()
{
	PUSH_PROFILE_MARKER("LuminanceMeasurementPass()");

	for (unsigned int i = 0; i < 4; i++)
	{
		switch (i)
		{
		case 0:
			PUSH_PROFILE_MARKER("64x64");
			break;
		case 1:
			PUSH_PROFILE_MARKER("16x16");
			break;
		case 2:
			PUSH_PROFILE_MARKER("4x4");
			break;
		case 3:
			PUSH_PROFILE_MARKER("1x1");
		}
		AverageLuminanceBuffer[i]->Enable();
		
		// Not necesarry
		//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

		PUSH_PROFILE_MARKER("LumaCalc.hlsl");
		if (ShdInputLUT[LumaCalcVS][f2HalfTexelOffset] != ~0)
			LumaCalcVInput->SetFloat2(ShdInputLUT[LumaCalcVS][f2HalfTexelOffset], Vec2f(0.5f / AverageLuminanceBuffer[i]->GetWidth(), 0.5f / AverageLuminanceBuffer[i]->GetHeight()));

		if (i == 0)
		{
			if (ShdInputLUT[LumaCalcPS][f2HalfTexelOffset] != ~0)
				LumaCalcFInput->SetFloat2(ShdInputLUT[LumaCalcPS][f2HalfTexelOffset], Vec2f(0.5f / HDRDownsampleBuffer->GetWidth(), 0.5f / HDRDownsampleBuffer->GetHeight()));

			if (ShdInputLUT[LumaCalcPS][texLumaCalcInput] != ~0)
				LumaCalcFInput->SetTexture(ShdInputLUT[LumaCalcPS][texLumaCalcInput], HDRDownsampleBuffer->GetColorBuffer(0));

			if (ShdInputLUT[LumaCalcPS][bInitialLumaPass] != ~0)
				LumaCalcFInput->SetBool(ShdInputLUT[LumaCalcPS][bInitialLumaPass], true);

			if (ShdInputLUT[LumaCalcPS][bFinalLumaPass] != ~0)
				LumaCalcFInput->SetBool(ShdInputLUT[LumaCalcPS][bFinalLumaPass], false);

			if (ShdInputLUT[LumaCalcPS][bLumaAdaptationPass] != ~0)
				LumaCalcFInput->SetBool(ShdInputLUT[LumaCalcPS][bLumaAdaptationPass], false);
		}
		else
		{
			if (ShdInputLUT[LumaCalcPS][f2HalfTexelOffset] != ~0)
				LumaCalcFInput->SetFloat2(ShdInputLUT[LumaCalcPS][f2HalfTexelOffset], Vec2f(0.5f / AverageLuminanceBuffer[i - 1]->GetWidth(), 0.5f / AverageLuminanceBuffer[i - 1]->GetHeight()));

			if (ShdInputLUT[LumaCalcPS][texLumaCalcInput] != ~0)
				LumaCalcFInput->SetTexture(ShdInputLUT[LumaCalcPS][texLumaCalcInput], AverageLuminanceBuffer[i - 1]->GetColorBuffer(0));

			if (ShdInputLUT[LumaCalcPS][bInitialLumaPass] != ~0)
				LumaCalcFInput->SetBool(ShdInputLUT[LumaCalcPS][bInitialLumaPass], false);

			if (ShdInputLUT[LumaCalcPS][bLumaAdaptationPass] != ~0)
				LumaCalcFInput->SetBool(ShdInputLUT[LumaCalcPS][bLumaAdaptationPass], false);

			if (i == 3)
			{
				if (ShdInputLUT[LumaCalcPS][bFinalLumaPass] != ~0)
					LumaCalcFInput->SetBool(ShdInputLUT[LumaCalcPS][bFinalLumaPass], true);

				if (ShdInputLUT[LumaCalcPS][f2AvgLumaClamp] != ~0)
					LumaCalcFInput->SetFloat2(ShdInputLUT[LumaCalcPS][f2AvgLumaClamp], Vec2f(HDR_AVG_LUMA_CLAMP_MIN, HDR_AVG_LUMA_CLAMP_MAX));
			}
			else
			{
				if (ShdInputLUT[LumaCalcPS][bFinalLumaPass] != ~0)
					LumaCalcFInput->SetBool(ShdInputLUT[LumaCalcPS][bFinalLumaPass], false);
			}
		}

		LumaCalcVP->Enable(LumaCalcVInput);
		LumaCalcFP->Enable(LumaCalcFInput);

		RenderContext->DrawVertexBuffer(FullScreenQuad);

		LumaCalcFP->Disable();
		LumaCalcVP->Disable();
		POP_PROFILE_MARKER();

		AverageLuminanceBuffer[i]->Disable();
		POP_PROFILE_MARKER();
	}

	PUSH_PROFILE_MARKER("Luminance adaptation");

	static unsigned long long lastTick = GetTickCount64();
	const float frameTime = gmtl::Math::clamp((float)(GetTickCount64() - lastTick) / 1000.f, 0.f, 1.f / HDR_LUMA_ADAPT_SPEED);
	lastTick = GetTickCount64();

	RenderTarget* const rtBkp = AdaptedLuminanceCurr;
	AdaptedLuminanceCurr = AdaptedLuminancePrev;
	AdaptedLuminancePrev = rtBkp;

	AdaptedLuminanceCurr->Enable();

	PUSH_PROFILE_MARKER("LumaCalc.hlsl");
	if (ShdInputLUT[LumaCalcVS][f2HalfTexelOffset] != ~0)
		LumaCalcVInput->SetFloat2(ShdInputLUT[LumaCalcVS][f2HalfTexelOffset], Vec2f(0.5f / AdaptedLuminancePrev->GetWidth(), 0.5f / AdaptedLuminancePrev->GetHeight()));

	if (ShdInputLUT[LumaCalcPS][texLumaCalcInput] != ~0)
		LumaCalcFInput->SetTexture(ShdInputLUT[LumaCalcPS][texLumaCalcInput], AdaptedLuminancePrev->GetColorBuffer(0));

	if (ShdInputLUT[LumaCalcPS][texLumaTarget] != ~0)
		LumaCalcFInput->SetTexture(ShdInputLUT[LumaCalcPS][texLumaTarget], AverageLuminanceBuffer[3]->GetColorBuffer(0));

	if (ShdInputLUT[LumaCalcPS][bInitialLumaPass] != ~0)
		LumaCalcFInput->SetBool(ShdInputLUT[LumaCalcPS][bInitialLumaPass], false);

	if (ShdInputLUT[LumaCalcPS][bFinalLumaPass] != ~0)
		LumaCalcFInput->SetBool(ShdInputLUT[LumaCalcPS][bFinalLumaPass], false);

	if (ShdInputLUT[LumaCalcPS][bLumaAdaptationPass] != ~0)
		LumaCalcFInput->SetBool(ShdInputLUT[LumaCalcPS][bLumaAdaptationPass], true);

	if (ShdInputLUT[LumaCalcPS][fLumaAdaptSpeed] != ~0)
		LumaCalcFInput->SetFloat(ShdInputLUT[LumaCalcPS][fLumaAdaptSpeed], HDR_LUMA_ADAPT_SPEED);

	if (ShdInputLUT[LumaCalcPS][fFrameTime] != ~0)
		LumaCalcFInput->SetFloat(ShdInputLUT[LumaCalcPS][fFrameTime], frameTime);

	LumaCalcVP->Enable(LumaCalcVInput);
	LumaCalcFP->Enable(LumaCalcFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	LumaCalcFP->Disable();
	LumaCalcVP->Disable();
	POP_PROFILE_MARKER();

	AdaptedLuminanceCurr->Disable();
	POP_PROFILE_MARKER();

	POP_PROFILE_MARKER();
}

// Tone map the HDR image onto the LDR target
void HDRToneMappingPass()
{
	PUSH_PROFILE_MARKER("HDRToneMappingPass");

	LuminanceMeasurementPass();

	LDRToneMappedImageBuffer->Enable();

	const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	PUSH_PROFILE_MARKER("HDRToneMapping.hlsl");
	if (ShdInputLUT[HDRToneMappingVS][f2HalfTexelOffset] != ~0)
		HDRToneMappingVInput->SetFloat2(ShdInputLUT[HDRToneMappingVS][f2HalfTexelOffset], Vec2f(0.5f / LightAccumulationBuffer->GetWidth(), 0.5f / LightAccumulationBuffer->GetHeight()));

	if (ShdInputLUT[HDRToneMappingPS][texLightAccumulationBuffer] != ~0)
		HDRToneMappingFInput->SetTexture(ShdInputLUT[HDRToneMappingPS][texLightAccumulationBuffer], LightAccumulationBuffer->GetColorBuffer(0));

	if (ShdInputLUT[HDRToneMappingPS][texAvgLuma] != ~0)
		HDRToneMappingFInput->SetTexture(ShdInputLUT[HDRToneMappingPS][texAvgLuma], AdaptedLuminanceCurr->GetColorBuffer(0));

	if (ShdInputLUT[HDRToneMappingPS][fExposureBias] != ~0)
		HDRToneMappingFInput->SetFloat(ShdInputLUT[HDRToneMappingPS][fExposureBias], HDR_EXPOSURE_BIAS);

	if (ShdInputLUT[HDRToneMappingPS][fShoulderStrength] != ~0)
		HDRToneMappingFInput->SetFloat(ShdInputLUT[HDRToneMappingPS][fShoulderStrength], HDR_TONEMAPPING_SHOULDER_STRENGTH);

	if (ShdInputLUT[HDRToneMappingPS][fLinearStrength] != ~0)
		HDRToneMappingFInput->SetFloat(ShdInputLUT[HDRToneMappingPS][fLinearStrength], HDR_TONEMAPPING_LINEAR_STRENGTH);

	if (ShdInputLUT[HDRToneMappingPS][fLinearAngle] != ~0)
		HDRToneMappingFInput->SetFloat(ShdInputLUT[HDRToneMappingPS][fLinearAngle], HDR_TONEMAPPING_LINEAR_ANGLE);

	if (ShdInputLUT[HDRToneMappingPS][fToeStrength] != ~0)
		HDRToneMappingFInput->SetFloat(ShdInputLUT[HDRToneMappingPS][fToeStrength], HDR_TONEMAPPING_TOE_STRENGTH);

	if (ShdInputLUT[HDRToneMappingPS][fToeNumerator] != ~0)
		HDRToneMappingFInput->SetFloat(ShdInputLUT[HDRToneMappingPS][fToeNumerator], HDR_TONEMAPPING_TOE_NUMERATOR);

	if (ShdInputLUT[HDRToneMappingPS][fToeDenominator] != ~0)
		HDRToneMappingFInput->SetFloat(ShdInputLUT[HDRToneMappingPS][fToeDenominator], HDR_TONEMAPPING_TOE_DENOMINATOR);

	if (ShdInputLUT[HDRToneMappingPS][fLinearWhite] != ~0)
		HDRToneMappingFInput->SetFloat(ShdInputLUT[HDRToneMappingPS][fLinearWhite], HDR_TONEMAPPING_LINEAR_WHITE);

	HDRToneMappingVP->Enable(HDRToneMappingVInput);
	HDRToneMappingFP->Enable(HDRToneMappingFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	HDRToneMappingVP->Disable();
	HDRToneMappingFP->Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);

	LDRToneMappedImageBuffer->Disable();

	POP_PROFILE_MARKER();
}

void BloomDownsample()
{
	PUSH_PROFILE_MARKER("Downsample + brightness filter");
	HDRBloomBuffer[0]->Enable();

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	PUSH_PROFILE_MARKER("Downsample.hlsl (4x4)");
	if (ShdInputLUT[DownsampleVS][f2HalfTexelOffset] != ~0)
		DownsampleVInput->SetFloat2(ShdInputLUT[DownsampleVS][f2HalfTexelOffset], Vec2f(0.5f / HDRBloomBuffer[0]->GetWidth(), 0.5f / HDRBloomBuffer[0]->GetHeight()));
	/*
	if (ShdInputLUT[DownsamplePS][f2TexelSize] != ~0)
		DownsampleFInput->SetFloat2(ShdInputLUT[DownsamplePS][f2TexelSize], Vec2f(1.f / LightAccumulationBuffer->GetWidth(), 1.f / LightAccumulationBuffer->GetHeight()));

	if (ShdInputLUT[DownsamplePS][texSource] != ~0)
		DownsampleFInput->SetTexture(ShdInputLUT[DownsamplePS][texSource], LightAccumulationBuffer->GetColorBuffer(0));

	if (ShdInputLUT[DownsamplePS][nDownsampleFactor] != ~0)
		DownsampleFInput->SetInt(ShdInputLUT[DownsamplePS][nDownsampleFactor], 16);
	*/
	if (ShdInputLUT[DownsamplePS][f2TexelSize] != ~0)
		DownsampleFInput->SetFloat2(ShdInputLUT[DownsamplePS][f2TexelSize], Vec2f(1.f / HDRDownsampleBuffer->GetWidth(), 1.f / HDRDownsampleBuffer->GetHeight()));

	if (ShdInputLUT[DownsamplePS][texSource] != ~0)
		DownsampleFInput->SetTexture(ShdInputLUT[DownsamplePS][texSource], HDRDownsampleBuffer->GetColorBuffer(0));

	if (ShdInputLUT[DownsamplePS][nDownsampleFactor] != ~0)
		DownsampleFInput->SetInt(ShdInputLUT[DownsamplePS][nDownsampleFactor], 1);

	if (ShdInputLUT[DownsamplePS][bApplyBrightnessFilter] != ~0)
		DownsampleFInput->SetBool(ShdInputLUT[DownsamplePS][bApplyBrightnessFilter], true);

	if (ShdInputLUT[DownsamplePS][fBrightnessThreshold] != ~0)
		DownsampleFInput->SetFloat(ShdInputLUT[DownsamplePS][fBrightnessThreshold], BLOOM_BRIGHTNESS_THRESHOLD);

	DownsampleVP->Enable(DownsampleVInput);
	DownsampleFP->Enable(DownsampleFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	DownsampleVP->Disable();
	DownsampleFP->Disable();
	POP_PROFILE_MARKER();

	HDRBloomBuffer[0]->Disable();
	POP_PROFILE_MARKER();
}

void BloomBlur()
{
	PUSH_PROFILE_MARKER("Bloom blur pass");

	for (unsigned int i = 0; i < BLOOM_BLUR_KERNEL_COUNT; i++)
	{
		char label[10];
		sprintf_s(label, "Kernel %d", BLOOM_BLUR_KERNEL[i]);
		PUSH_PROFILE_MARKER(label);
		HDRBloomBuffer[(i + 1) % 2]->Enable();

		// Not necesarry
		//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

		PUSH_PROFILE_MARKER("Bloom.hlsl");
		if (ShdInputLUT[BloomVS][f2HalfTexelOffset] != ~0)
			BloomVInput->SetFloat2(ShdInputLUT[BloomVS][f2HalfTexelOffset], Vec2f(0.5f / HDRBloomBuffer[i % 2]->GetWidth(), 0.5f / HDRBloomBuffer[i % 2]->GetHeight()));

		if (ShdInputLUT[BloomPS][texSource] != ~0)
		{
			ResourceMgr->GetTexture(HDRBloomBuffer[i % 2]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_POINT_MIP_NONE);
			BloomFInput->SetTexture(ShdInputLUT[BloomPS][texSource], HDRBloomBuffer[i % 2]->GetColorBuffer(0));
		}

		if (ShdInputLUT[BloomPS][fBloomStrength] != ~0)
			BloomFInput->SetFloat(ShdInputLUT[BloomPS][fBloomStrength], BLOOM_STRENGTH);

		if (ShdInputLUT[BloomPS][fBloomPower] != ~0)
			BloomFInput->SetFloat(ShdInputLUT[BloomPS][fBloomPower], BLOOM_POWER);

		if (ShdInputLUT[BloomPS][f2TexelSize] != ~0)
			BloomFInput->SetFloat2(ShdInputLUT[BloomPS][f2TexelSize], Vec2f(1.f / HDRBloomBuffer[i % 2]->GetWidth(), 1.f / HDRBloomBuffer[i % 2]->GetHeight()));

		if (ShdInputLUT[BloomPS][nKernel] != ~0)
			BloomFInput->SetInt(ShdInputLUT[BloomPS][nKernel], BLOOM_BLUR_KERNEL[i]);

		BloomVP->Enable(BloomVInput);
		BloomFP->Enable(BloomFInput);

		RenderContext->DrawVertexBuffer(FullScreenQuad);

		BloomFP->Disable();
		BloomVP->Disable();
		POP_PROFILE_MARKER();

		HDRBloomBuffer[(i + 1) % 2]->Disable();
		POP_PROFILE_MARKER();
	}

	POP_PROFILE_MARKER();
}

void BloomApply()
{
	PUSH_PROFILE_MARKER("Bloom apply");
	LightAccumulationBuffer->Enable();

	const bool blendEnable = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	const Blend dstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	const Blend srcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_ONE);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_ONE);

	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	PUSH_PROFILE_MARKER("ColorCopy.hlsl");
	if (ShdInputLUT[ColorCopyVS][f2HalfTexelOffset] != ~0)
		ColorCopyVInput->SetFloat2(ShdInputLUT[ColorCopyVS][f2HalfTexelOffset], Vec2f(0.5f / HDRBloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetWidth(), 0.5f / HDRBloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetHeight()));

	if (ShdInputLUT[ColorCopyPS][texSource] != ~0)
	{
		ResourceMgr->GetTexture(HDRBloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetColorBuffer(0))->SetFilter(SF_MIN_MAG_LINEAR_MIP_NONE);
		ColorCopyFInput->SetTexture(ShdInputLUT[ColorCopyPS][texSource], HDRBloomBuffer[BLOOM_BLUR_KERNEL_COUNT % 2]->GetColorBuffer(0));
	}

	ColorCopyVP->Enable(ColorCopyVInput);
	ColorCopyFP->Enable(ColorCopyFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	ColorCopyFP->Disable();
	ColorCopyVP->Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnable);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	LightAccumulationBuffer->Disable();
	POP_PROFILE_MARKER();
}

// Apply bloom effect
void BloomPass()
{
	PUSH_PROFILE_MARKER("BloomPass()");

	BloomDownsample();
	BloomBlur();
	BloomApply();

	POP_PROFILE_MARKER();
}

void FxaaPass()
{
	PUSH_PROFILE_MARKER("FxaaPass()");
	LDRFxaaImageBuffer->Enable();

	const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	RenderTarget* const srcRT = HDR_TONEMAPPING_ENABLED ? LDRToneMappedImageBuffer : LightAccumulationBuffer;

	PUSH_PROFILE_MARKER("FXAA.hlsl");
	if (ShdInputLUT[FxaaVS][f2HalfTexelOffset] != ~0)
		FxaaVInput->SetFloat2(ShdInputLUT[FxaaVS][f2HalfTexelOffset], Vec2f(0.5f / srcRT->GetWidth(), 0.5f / srcRT->GetHeight()));

	if (ShdInputLUT[FxaaPS][f2HalfTexelOffset] != ~0)
		FxaaFInput->SetFloat2(ShdInputLUT[FxaaPS][f2HalfTexelOffset], Vec2f(0.5f / srcRT->GetWidth(), 0.5f / srcRT->GetHeight()));

	if (ShdInputLUT[FxaaPS][texSource] != ~0)
		FxaaFInput->SetTexture(ShdInputLUT[FxaaPS][texSource], srcRT->GetColorBuffer(0));

	if (ShdInputLUT[FxaaPS][f2TexelSize] != ~0)
		FxaaFInput->SetFloat2(ShdInputLUT[FxaaPS][f2TexelSize], Vec2f(1.f / srcRT->GetWidth(), 1.f / srcRT->GetHeight()));

	if (ShdInputLUT[FxaaPS][fFxaaSubpix] != ~0)
		FxaaFInput->SetFloat(ShdInputLUT[FxaaPS][fFxaaSubpix], FXAA_SUBPIX);

	if (ShdInputLUT[FxaaPS][fFxaaEdgeThreshold] != ~0)
		FxaaFInput->SetFloat(ShdInputLUT[FxaaPS][fFxaaEdgeThreshold], FXAA_EDGE_THRESHOLD);

	if (ShdInputLUT[FxaaPS][fFxaaEdgeThresholdMin] != ~0)
		FxaaFInput->SetFloat(ShdInputLUT[FxaaPS][fFxaaEdgeThresholdMin], FXAA_EDGE_THRESHOLD_MIN);

	FxaaVP->Enable(FxaaVInput);
	FxaaFP->Enable(FxaaFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	FxaaFP->Disable();
	FxaaVP->Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);

	LDRFxaaImageBuffer->Disable();
	POP_PROFILE_MARKER();
}

void CalculateDoF()
{
	PUSH_PROFILE_MARKER("CalculateDoF()");
	if (DOF_USE_QUARTER_RESOLUTION_BUFFER)
		DepthOfFieldQuarterBuffer->Enable();
	else
		DepthOfFieldFullBuffer->Enable();

	const bool colorBlendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(false);

	PUSH_PROFILE_MARKER("BokehDof.hlsl");
	if (ShdInputLUT[BokehDofVS][f2HalfTexelOffset] != ~0)
		BokehDofVInput->SetFloat2(ShdInputLUT[BokehDofVS][f2HalfTexelOffset], Vec2f(0.5f / LightAccumulationBuffer->GetWidth(), 0.5f / LightAccumulationBuffer->GetHeight()));

	if (ShdInputLUT[BokehDofPS][texSource] != ~0)
		BokehDofFInput->SetTexture(ShdInputLUT[BokehDofPS][texSource], LightAccumulationBuffer->GetColorBuffer(0));

	if (ShdInputLUT[BokehDofPS][texDepthBuffer] != ~0)
		BokehDofFInput->SetTexture(ShdInputLUT[BokehDofPS][texDepthBuffer], GBuffer->GetDepthBuffer());

	if (ShdInputLUT[BokehDofPS][f2TexSourceSize] != ~0)
		BokehDofFInput->SetFloat2(ShdInputLUT[BokehDofPS][f2TexSourceSize], Vec2f((float)LightAccumulationBuffer->GetWidth(), (float)LightAccumulationBuffer->GetHeight()));

	if (ShdInputLUT[BokehDofPS][f2TexelSize] != ~0)
		BokehDofFInput->SetFloat2(ShdInputLUT[BokehDofPS][f2TexelSize], Vec2f(1.f / LightAccumulationBuffer->GetWidth(), 1.f / LightAccumulationBuffer->GetHeight()));

	if (ShdInputLUT[BokehDofPS][fFocalDepth] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fFocalDepth], DOF_FOCAL_DEPTH);

	if (ShdInputLUT[BokehDofPS][fFocalLength] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fFocalLength], DOF_FOCAL_LENGTH);

	if (ShdInputLUT[BokehDofPS][fFStop] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fFStop], DOF_FSTOP);

	if (ShdInputLUT[BokehDofPS][fCoC] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fCoC], DOF_COC);

	if (ShdInputLUT[BokehDofPS][fNearDofStart] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fNearDofStart], DOF_NEAR_START);

	if (ShdInputLUT[BokehDofPS][fNearDofFalloff] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fNearDofFalloff], DOF_NEAR_FALLOFF);

	if (ShdInputLUT[BokehDofPS][fFarDofStart] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fFarDofStart], DOF_FAR_START);

	if (ShdInputLUT[BokehDofPS][fFarDofFalloff] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fFarDofFalloff], DOF_FAR_FALLOFF);

	if (ShdInputLUT[BokehDofPS][bManualDof] != ~0)
		BokehDofFInput->SetBool(ShdInputLUT[BokehDofPS][bManualDof], DOF_MANUAL);

	if (ShdInputLUT[BokehDofPS][bDebugFocus] != ~0)
		BokehDofFInput->SetBool(ShdInputLUT[BokehDofPS][bDebugFocus], DOF_DEBUG_FOCUS);

	if (ShdInputLUT[BokehDofPS][bAutofocus] != ~0)
		BokehDofFInput->SetBool(ShdInputLUT[BokehDofPS][bAutofocus], DOF_AUTOFOCUS);

	if (ShdInputLUT[BokehDofPS][f2FocusPoint] != ~0)
		BokehDofFInput->SetFloat2(ShdInputLUT[BokehDofPS][f2FocusPoint], DOF_FOCUS_POINT);

	if (ShdInputLUT[BokehDofPS][fMaxBlur] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fMaxBlur], DOF_MAX_BLUR);

	if (ShdInputLUT[BokehDofPS][fHighlightThreshold] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fHighlightThreshold], DOF_HIGHLIGHT_THRESHOLD);

	if (ShdInputLUT[BokehDofPS][fHighlightGain] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fHighlightGain], DOF_HIGHLIGHT_GAIN);

	if (ShdInputLUT[BokehDofPS][fBokehBias] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fBokehBias], DOF_BOKEH_BIAS);

	if (ShdInputLUT[BokehDofPS][fBokehFringe] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fBokehFringe], DOF_BOKEH_FRINGE);

	if (ShdInputLUT[BokehDofPS][bPentagonBokeh] != ~0)
		BokehDofFInput->SetBool(ShdInputLUT[BokehDofPS][bPentagonBokeh], DOF_PENTAGON_BOKEH);

	if (ShdInputLUT[BokehDofPS][fPentagonFeather] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fPentagonFeather], DOF_PENTAGON_FEATHER);

	if (ShdInputLUT[BokehDofPS][bUseNoise] != ~0)
		BokehDofFInput->SetBool(ShdInputLUT[BokehDofPS][bUseNoise], DOF_USE_NOISE);

	if (ShdInputLUT[BokehDofPS][fNoiseAmount] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fNoiseAmount], DOF_NOISE_AMOUNT);

	if (ShdInputLUT[BokehDofPS][bBlurDepth] != ~0)
		BokehDofFInput->SetBool(ShdInputLUT[BokehDofPS][bBlurDepth], DOF_BLUR_DEPTH);

	if (ShdInputLUT[BokehDofPS][fDepthBlurSize] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fDepthBlurSize], DOF_DEPTH_BLUR_SIZE);

	if (ShdInputLUT[BokehDofPS][bVignetting] != ~0)
		BokehDofFInput->SetBool(ShdInputLUT[BokehDofPS][bVignetting], DOF_VIGNETTING);

	if (ShdInputLUT[BokehDofPS][fVignOut] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fVignOut], DOF_VIGNETTING_OUTER_BORDER);

	if (ShdInputLUT[BokehDofPS][fVignIn] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fVignIn], DOF_VIGNETTING_INNER_BORDER);

	if (ShdInputLUT[BokehDofPS][fVignFade] != ~0)
		BokehDofFInput->SetFloat(ShdInputLUT[BokehDofPS][fVignFade], DOF_VIGNETTING_FADE);

	BokehDofVP->Enable(BokehDofVInput);
	BokehDofFP->Enable(BokehDofFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	BokehDofFP->Disable();
	BokehDofVP->Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(colorBlendEnabled);

	if (DOF_USE_QUARTER_RESOLUTION_BUFFER)
		DepthOfFieldQuarterBuffer->Disable();
	else
		DepthOfFieldFullBuffer->Disable();
	POP_PROFILE_MARKER();
}

void ApplyDoF()
{
	PUSH_PROFILE_MARKER("ApplyDoF()");
	LightAccumulationBuffer->Enable();

	const bool blendEnabled = RenderContext->GetRenderStateManager()->GetColorBlendEnabled();
	const Blend dstBlend = RenderContext->GetRenderStateManager()->GetColorDstBlend();
	const Blend srcBlend = RenderContext->GetRenderStateManager()->GetColorSrcBlend();
	
	if (DOF_USE_QUARTER_RESOLUTION_BUFFER)
	{
		RenderContext->GetRenderStateManager()->SetColorBlendEnabled(true);
		RenderContext->GetRenderStateManager()->SetColorDstBlend(BLEND_INVSRCALPHA);
		RenderContext->GetRenderStateManager()->SetColorSrcBlend(BLEND_SRCALPHA);
	}

	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);
	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	PUSH_PROFILE_MARKER("ColorCopy.hlsl");
	if (ShdInputLUT[ColorCopyVS][f2HalfTexelOffset] != ~0)
		ColorCopyVInput->SetFloat2(ShdInputLUT[ColorCopyVS][f2HalfTexelOffset], Vec2f(0.5f / LightAccumulationBuffer->GetWidth(), 0.5f / LightAccumulationBuffer->GetHeight()));

	if (ShdInputLUT[ColorCopyPS][texSource] != ~0)
		ColorCopyFInput->SetTexture(ShdInputLUT[ColorCopyPS][texSource], (DOF_USE_QUARTER_RESOLUTION_BUFFER ? DepthOfFieldQuarterBuffer : DepthOfFieldFullBuffer)->GetColorBuffer(0));

	ColorCopyVP->Enable(ColorCopyVInput);
	ColorCopyFP->Enable(ColorCopyFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	ColorCopyFP->Disable();
	ColorCopyVP->Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetColorBlendEnabled(blendEnabled);
	RenderContext->GetRenderStateManager()->SetColorDstBlend(dstBlend);
	RenderContext->GetRenderStateManager()->SetColorSrcBlend(srcBlend);

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	LightAccumulationBuffer->Disable();
	POP_PROFILE_MARKER();
}

// Apply DoF effect
void DepthOfFieldPass()
{
	PUSH_PROFILE_MARKER("DepthOfFieldPass()");

	CalculateDoF();
	ApplyDoF();

	POP_PROFILE_MARKER();
}

// Apply postprocessing effects. Afterwards, copy results to the backbuffer.
void ApplyPostProcessing()
{
	PUSH_PROFILE_MARKER("ApplyPostProcessing()");
	
	// Step 1: Downsampling
	if(HDR_TONEMAPPING_ENABLED || BLOOM_ENABLED)
		HDRDownsamplePass();

	// Step 2: Depth of Field
	if(DOF_ENABLED)
		DepthOfFieldPass();

	// Step 3: Bloom
	if(BLOOM_ENABLED)
		BloomPass();

	// Step 4: Tone mapping + gamma correction
	if(HDR_TONEMAPPING_ENABLED)
		HDRToneMappingPass();

	// Step 5: Antialiasing
	if(FXAA_ENABLED)
		FxaaPass();

	POP_PROFILE_MARKER();
}

// Copy texture to the back buffer
void CopyResultToBackBuffer(RenderTarget* const rt)
{
	PUSH_PROFILE_MARKER("CopyResultToBackBuffer()");

	const bool sRGBEnabled = RenderContext->GetRenderStateManager()->GetSRGBWriteEnabled();
	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(true);

	// Not necesarry
	//RenderContext->Clear(Vec4f(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	bool zWrite = RenderContext->GetRenderStateManager()->GetZWriteEnabled();
	RenderContext->GetRenderStateManager()->SetZWriteEnabled(false);

	Cmp zFunc = RenderContext->GetRenderStateManager()->GetZFunc();
	RenderContext->GetRenderStateManager()->SetZFunc(CMP_ALWAYS);

	PUSH_PROFILE_MARKER("ColorCopy.hlsl");
	if (ShdInputLUT[ColorCopyVS][f2HalfTexelOffset] != ~0)
		ColorCopyVInput->SetFloat2(ShdInputLUT[ColorCopyVS][f2HalfTexelOffset], Vec2f(0.5f / rt->GetWidth(), 0.5f / rt->GetHeight()));

	if (ShdInputLUT[ColorCopyPS][texSource] != ~0)
		ColorCopyFInput->SetTexture(ShdInputLUT[ColorCopyPS][texSource], rt->GetColorBuffer(0));

	ColorCopyVP->Enable(ColorCopyVInput);
	ColorCopyFP->Enable(ColorCopyFInput);

	RenderContext->DrawVertexBuffer(FullScreenQuad);

	ColorCopyFP->Disable();
	ColorCopyVP->Disable();
	POP_PROFILE_MARKER();

	RenderContext->GetRenderStateManager()->SetZWriteEnabled(zWrite);
	RenderContext->GetRenderStateManager()->SetZFunc(zFunc);

	RenderContext->GetRenderStateManager()->SetSRGBWriteEnabled(sRGBEnabled);

	POP_PROFILE_MARKER();
}

// Render a frame
void RenderScene()
{
	if (RenderContext->BeginFrame())
	{
		UpdateMatrices();

		GenerateDirectionalShadowMap();

		GenerateRSM();

		GenerateGBuffer();

		AccumulateLight();

		if (POST_PROCESSING_ENABLED)
			ApplyPostProcessing();
		
		RenderTarget* FinalImageBuffer = nullptr;
		if (POST_PROCESSING_ENABLED)
		{
			FinalImageBuffer = LightAccumulationBuffer;

			if (DOF_ENABLED)
				FinalImageBuffer = LightAccumulationBuffer;

			if (BLOOM_ENABLED)
				FinalImageBuffer = LightAccumulationBuffer;

			if (HDR_TONEMAPPING_ENABLED)
				FinalImageBuffer = LDRToneMappedImageBuffer;

			if (FXAA_ENABLED)
				FinalImageBuffer = LDRFxaaImageBuffer;
		}
		else
			FinalImageBuffer = LightAccumulationBuffer;

		if(FinalImageBuffer)
			CopyResultToBackBuffer(FinalImageBuffer);

		RenderContext->EndFrame();

		RenderContext->SwapBuffers();
	}
}
