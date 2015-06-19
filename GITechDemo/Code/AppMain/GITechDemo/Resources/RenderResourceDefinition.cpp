#include "stdafx.h"

#include "RenderResourceDeclaration.h"
using namespace GITechDemoApp;

#define DEFINE_MODEL(NAME, FILEPATH) \
	Model NAME(FILEPATH);

#define DEFINE_TEXTURE(NAME, FILEPATH) \
	Texture NAME(FILEPATH);

#define DEFINE_SHADER(NAME, FILEPATH) \
	Shader NAME(FILEPATH);

#define DEFINE_ARTIST_SHADER_CONSTANT(NAME, TYPE, DEFAULT_VALUE) \
	ShaderConstantTemplate<TYPE> NAME(#NAME, DEFAULT_VALUE);

#define DEFINE_UTILITY_SHADER_CONSTANT(NAME, TYPE) \
	ShaderConstantTemplate<TYPE> NAME(#NAME);

#define DEFINE_RENDER_TARGET(NAME, TARGETCOUNT, RT0, RT1, RT2, RT3, W, H, DEPTH) \
	RenderTarget NAME(TARGETCOUNT, RT0, RT1, RT2, RT3, (unsigned int)W, (unsigned int)H, DEPTH);

#define DEFINE_DYNAMIC_RENDER_TARGET(NAME, TARGETCOUNT, RT0, RT1, RT2, RT3, W, H, DEPTH) \
	RenderTarget NAME(TARGETCOUNT, RT0, RT1, RT2, RT3, (float)W, (float)H, DEPTH);

namespace GITechDemoApp
{
	//////////////
	// Models	//
	//////////////
	DEFINE_MODEL(SponzaScene, "models\\sponza\\sponza.lrm")
	//------------------------------------------------------

	//////////////
	// Textures	//
	//////////////
	DEFINE_TEXTURE(SkyTexture, "models/sponza/textures/sky.lrt")
	//------------------------------------------------------

	//////////////////////
	// Render targets	//
	//////////////////////
	const Vec<unsigned int, 2> SHADOW_MAP_SIZE = Vec<unsigned int, 2>(4096, 4096);
	const unsigned int RSM_SIZE = 512;
	// Geometry buffer (GBuffer)
	//	RT0:	A8R8G8B8 with diffuse albedo in RGB and specular power in A
	//	RT1:	G16R16F with compressed normals (stereographic projection)
	//	DS:		INTZ (for sampling as a regular texture later)
	DEFINE_DYNAMIC_RENDER_TARGET(GBuffer, 2, LibRendererDll::PF_A8R8G8B8, LibRendererDll::PF_G16R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 1.f, 1.f, LibRendererDll::PF_INTZ)
	// Shadow map for the directional light (the dummy color buffer is required because of DX9 limitations
	DEFINE_RENDER_TARGET(ShadowMapDir, 1, LibRendererDll::PF_A8, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, SHADOW_MAP_SIZE[0], SHADOW_MAP_SIZE[1], LibRendererDll::PF_INTZ)
	// Render target in which we accumulate the light contribution from all light sources (floating point color components make it HDR-ready)
	// It contains a regular depth-stencil surface in which we will copy-resolve our INTZ depth texture from the GBuffer
	// (the depth is required for correctly rendering the sky and for future stencil optimizations)
	DEFINE_DYNAMIC_RENDER_TARGET(LightAccumulationBuffer, 1, LibRendererDll::PF_A16B16G16R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 1.f, 1.f, LibRendererDll::PF_D24S8)
	// Reflective Shadow Map render target
	DEFINE_RENDER_TARGET(RSMBuffer, 2, LibRendererDll::PF_A8R8G8B8, LibRendererDll::PF_G16R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, RSM_SIZE, RSM_SIZE, LibRendererDll::PF_INTZ)
	// Indirect lighting accumulation buffer (quarter resolution)
	DEFINE_DYNAMIC_RENDER_TARGET(IndirectLightAccumulationBuffer, 1, LibRendererDll::PF_A16B16G16R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 0.5f, 0.5f, LibRendererDll::PF_NONE)
	// HDR downsampled buffer (1/16 resolution)
	DEFINE_DYNAMIC_RENDER_TARGET(HDRDownsampleBuffer, 1, LibRendererDll::PF_A16B16G16R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 0.25f, 0.25f, LibRendererDll::PF_NONE)
	// Average luminance buffers (64x64, 16x16, 4x4, two 1x1, one target luma, one current luma)
	DEFINE_RENDER_TARGET(AverageLuminanceBuffer0, 1, LibRendererDll::PF_R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 64u, 64u, LibRendererDll::PF_NONE)
	DEFINE_RENDER_TARGET(AverageLuminanceBuffer1, 1, LibRendererDll::PF_R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 16u, 16u, LibRendererDll::PF_NONE)
	DEFINE_RENDER_TARGET(AverageLuminanceBuffer2, 1, LibRendererDll::PF_R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 4u, 4u, LibRendererDll::PF_NONE)
	DEFINE_RENDER_TARGET(AverageLuminanceBuffer3, 1, LibRendererDll::PF_R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 1u, 1u, LibRendererDll::PF_NONE)
	// Adapted luminance for simulating light adaptation effect
	DEFINE_RENDER_TARGET(AdaptedLuminance0, 1, LibRendererDll::PF_R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 1u, 1u, LibRendererDll::PF_NONE)
	DEFINE_RENDER_TARGET(AdaptedLuminance1, 1, LibRendererDll::PF_R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 1u, 1u, LibRendererDll::PF_NONE)
	// HDR bloom render targets
	DEFINE_DYNAMIC_RENDER_TARGET(HDRBloomBuffer0, 1, LibRendererDll::PF_A16B16G16R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 0.25f, 0.25f, LibRendererDll::PF_NONE)
	DEFINE_DYNAMIC_RENDER_TARGET(HDRBloomBuffer1, 1, LibRendererDll::PF_A16B16G16R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 0.25f, 0.25f, LibRendererDll::PF_NONE)
	// LDR tone mapped render target
	DEFINE_DYNAMIC_RENDER_TARGET(LDRToneMappedImageBuffer, 1, LibRendererDll::PF_A8R8G8B8, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 1.f, 1.f, LibRendererDll::PF_NONE)
	// LDR FXAA render target
	DEFINE_DYNAMIC_RENDER_TARGET(LDRFxaaImageBuffer, 1, LibRendererDll::PF_A8R8G8B8, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 1.f, 1.f, LibRendererDll::PF_NONE)
	// Ambient occlusion render targets
	DEFINE_DYNAMIC_RENDER_TARGET(AmbientOcclusionBuffer0, 1, LibRendererDll::PF_L8, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 1.f, 1.f, LibRendererDll::PF_NONE)
	DEFINE_DYNAMIC_RENDER_TARGET(AmbientOcclusionBuffer1, 1, LibRendererDll::PF_L8, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 1.f, 1.f, LibRendererDll::PF_NONE)
	// Depth of field render targets
	DEFINE_DYNAMIC_RENDER_TARGET(DepthOfFieldFullBuffer, 1, LibRendererDll::PF_A16B16G16R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 1.f, 1.f, LibRendererDll::PF_NONE)
	DEFINE_DYNAMIC_RENDER_TARGET(DepthOfFieldQuarterBuffer, 1, LibRendererDll::PF_A16B16G16R16F, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, LibRendererDll::PF_NONE, 0.5f, 0.5f, LibRendererDll::PF_NONE)

	RenderTarget* AverageLuminanceBuffer[4] = {
		&GITechDemoApp::AverageLuminanceBuffer0,
		&GITechDemoApp::AverageLuminanceBuffer1,
		&GITechDemoApp::AverageLuminanceBuffer2,
		&GITechDemoApp::AverageLuminanceBuffer3
	};
	RenderTarget* HDRBloomBuffer[2] = {
		&GITechDemoApp::HDRBloomBuffer0,
		&GITechDemoApp::HDRBloomBuffer1
	};
	RenderTarget* AmbientOcclusionBuffer[2] = {
		&GITechDemoApp::AmbientOcclusionBuffer0,
		&GITechDemoApp::AmbientOcclusionBuffer1
	};
	RenderTarget* AdaptedLuminance[2] = {
		&GITechDemoApp::AdaptedLuminance0,
		&GITechDemoApp::AdaptedLuminance1
	};
	//------------------------------------------------------

	//////////////
	// Shaders	//
	//////////////
	DEFINE_SHADER(SkyBoxShader, "shaders/SkyBox.hlsl")
	DEFINE_SHADER(GBufferGenerationShader, "shaders/GBufferGeneration.hlsl")
	DEFINE_SHADER(DeferredLightAmbShader, "shaders/DeferredLightAmb.hlsl")
	DEFINE_SHADER(DeferredLightDirShader, "shaders/DeferredLightDir.hlsl")
	DEFINE_SHADER(DepthPassShader, "shaders/DepthPass.hlsl")
	DEFINE_SHADER(DepthCopyShader, "shaders/DepthCopy.hlsl")
	DEFINE_SHADER(ColorCopyShader, "shaders/ColorCopy.hlsl")
	DEFINE_SHADER(RSMCaptureShader, "shaders/RSMCapture.hlsl")
	DEFINE_SHADER(RSMApplyShader, "shaders/RSMApply.hlsl")
	DEFINE_SHADER(DownsampleShader, "shaders/Downsample.hlsl")
	DEFINE_SHADER(LumaCalcShader, "shaders/LumaCalc.hlsl")
	DEFINE_SHADER(HDRToneMappingShader, "shaders/HDRToneMapping.hlsl")
	DEFINE_SHADER(BloomShader, "shaders/Bloom.hlsl")
	DEFINE_SHADER(FxaaShader, "shaders/FXAA.hlsl")
	DEFINE_SHADER(SsaoShader, "shaders/SSAO.hlsl")
	DEFINE_SHADER(BokehDofShader, "shaders/BokehDoF.hlsl")
	//------------------------------------------------------

	//////////////////////////////////////
	// Shader constants - artist driven	//
	//////////////////////////////////////
	/* Ambient light parameters */
	DEFINE_ARTIST_SHADER_CONSTANT(fAmbientFactor, float, 0.1f)

	/* Directional light parameters */
	DEFINE_ARTIST_SHADER_CONSTANT(fDiffuseFactor, float, 20.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fSpecFactor, float, 75.f)

	/* Sun parameters */
	DEFINE_ARTIST_SHADER_CONSTANT(fSunRadius, float, 1000.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fSunBrightness, float, 500.f)

	/* Directional light shadow map parameters */
	DEFINE_ARTIST_SHADER_CONSTANT(fShadowDepthBias, float, 0.01f)

	/* Cascaded Shadow Map parameters */
	DEFINE_ARTIST_SHADER_CONSTANT(bDebugCascades, bool, false)
	DEFINE_ARTIST_SHADER_CONSTANT(fCascadeBlendSize, float, 25.f)

	/* Reflective Shadow Map parameters */
	DEFINE_ARTIST_SHADER_CONSTANT(fRSMIntensity, float, 150.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fRSMKernelScale, float, 0.015f)

	/* Screen-Space Ambient Occlusion */
	DEFINE_ARTIST_SHADER_CONSTANT(fSSAOSampleRadius, float, 10.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fSSAOIntensity, float, 5.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fSSAOScale, float, 0.05f)
	DEFINE_ARTIST_SHADER_CONSTANT(fSSAOBias, float, 0.25f)

	/* Postprocessing parameters */
	// Tonemapping
	DEFINE_ARTIST_SHADER_CONSTANT(fExposureBias, float, 0.1f)
	DEFINE_ARTIST_SHADER_CONSTANT(f2AvgLumaClamp, Vec2f, Vec2f(0.00001f, 0.25f))
	DEFINE_ARTIST_SHADER_CONSTANT(fShoulderStrength, float, 0.15f)
	DEFINE_ARTIST_SHADER_CONSTANT(fLinearStrength, float, 0.5f)
	DEFINE_ARTIST_SHADER_CONSTANT(fLinearAngle, float, 0.07f)
	DEFINE_ARTIST_SHADER_CONSTANT(fToeStrength, float, 3.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fToeNumerator, float, 0.02f)
	DEFINE_ARTIST_SHADER_CONSTANT(fToeDenominator, float, 0.25f)
	DEFINE_ARTIST_SHADER_CONSTANT(fLinearWhite, float, 11.2f)
	DEFINE_ARTIST_SHADER_CONSTANT(fLumaAdaptSpeed, float, 1.f)
	// Bloom
	DEFINE_ARTIST_SHADER_CONSTANT(fBrightnessThreshold, float, 1.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fBloomPower, float, 1.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fBloomStrength, float, 0.6f)
	// FXAA
	DEFINE_ARTIST_SHADER_CONSTANT(fFxaaSubpix, float, 0.75f)
	DEFINE_ARTIST_SHADER_CONSTANT(fFxaaEdgeThreshold, float, 0.166f)
	DEFINE_ARTIST_SHADER_CONSTANT(fFxaaEdgeThresholdMin, float, 0.0833f)
	// DoF
	DEFINE_ARTIST_SHADER_CONSTANT(fFocalDepth, float, 100.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fFocalLength, float, 75.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fFStop, float, 3.5f)
	DEFINE_ARTIST_SHADER_CONSTANT(fCoC, float, 0.02f)
	DEFINE_ARTIST_SHADER_CONSTANT(fNearDofStart, float, 1.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fNearDofFalloff, float, 2.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fFarDofStart, float, 1.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fFarDofFalloff, float, 3.f)
	DEFINE_ARTIST_SHADER_CONSTANT(bManualDof, bool, false)
	DEFINE_ARTIST_SHADER_CONSTANT(bDebugFocus, bool, false)
	DEFINE_ARTIST_SHADER_CONSTANT(bAutofocus, bool, true)
	DEFINE_ARTIST_SHADER_CONSTANT(f2FocusPoint, Vec2f, Vec2f(0.5f, 0.5f))
	DEFINE_ARTIST_SHADER_CONSTANT(fMaxBlur, float, 1.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fHighlightThreshold, float, 1.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fHighlightGain, float, 10.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fBokehBias, float, 0.75f)
	DEFINE_ARTIST_SHADER_CONSTANT(fBokehFringe, float, 2.f)
	DEFINE_ARTIST_SHADER_CONSTANT(bPentagonBokeh, bool, false)
	DEFINE_ARTIST_SHADER_CONSTANT(fPentagonFeather, float, 0.4f)
	DEFINE_ARTIST_SHADER_CONSTANT(bUseNoise, bool, false)
	DEFINE_ARTIST_SHADER_CONSTANT(fNoiseAmount, float, 0.0001f)
	DEFINE_ARTIST_SHADER_CONSTANT(bBlurDepth, bool, false)
	DEFINE_ARTIST_SHADER_CONSTANT(fDepthBlurSize, float, 0.001f)
	DEFINE_ARTIST_SHADER_CONSTANT(bVignetting, bool, true)
	DEFINE_ARTIST_SHADER_CONSTANT(fVignOut, float, 1.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fVignIn, float, 0.f)
	DEFINE_ARTIST_SHADER_CONSTANT(fVignFade, float, 22.f)

	//////////////////////////////////////
	// Shader constants - code driven	//
	//////////////////////////////////////
	DEFINE_UTILITY_SHADER_CONSTANT(f44WorldViewProjMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(f44WorldViewMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(texDiffuse, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(texNormal, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(bHasNormalMap, bool)
	DEFINE_UTILITY_SHADER_CONSTANT(texSpec, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(bHasSpecMap, bool)
	DEFINE_UTILITY_SHADER_CONSTANT(fSpecIntensity, float)
	DEFINE_UTILITY_SHADER_CONSTANT(f2HalfTexelOffset, Vec2f)
	DEFINE_UTILITY_SHADER_CONSTANT(texDepthBuffer, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(f44SkyViewProjMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(texSkyTex, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(texDiffuseBuffer, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(f44InvProjMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(texNormalBuffer, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(texShadowMap, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(f2OneOverShadowMapSize, Vec2f)
	DEFINE_UTILITY_SHADER_CONSTANT(f44ViewMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(f44InvViewProjMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(f44ScreenToLightViewMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(f3LightDir, Vec3f)
	DEFINE_UTILITY_SHADER_CONSTANT(f2CascadeBoundsMin, Vec2f*)
	DEFINE_UTILITY_SHADER_CONSTANT(f2CascadeBoundsMax, Vec2f*)
	DEFINE_UTILITY_SHADER_CONSTANT(f44CascadeProjMat, Matrix44f*)
	DEFINE_UTILITY_SHADER_CONSTANT(texLightAccumulationBuffer, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(poissonDisk, Vec2f*)
	DEFINE_UTILITY_SHADER_CONSTANT(f44LightWorldViewProjMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(f44LightWorldViewMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(texRSMFluxBuffer, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(texRSMNormalBuffer, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(texRSMDepthBuffer, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(f3RSMKernel, Vec3f*)
	DEFINE_UTILITY_SHADER_CONSTANT(f44RSMProjMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(f44RSMInvProjMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(f44ViewToRSMViewMat, Matrix44f)
	DEFINE_UTILITY_SHADER_CONSTANT(texIndirectLightAccumulationBuffer, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(bIsUpscalePass, bool)
	DEFINE_UTILITY_SHADER_CONSTANT(texLumaCalcInput, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(bInitialLumaPass, bool)
	DEFINE_UTILITY_SHADER_CONSTANT(bFinalLumaPass, bool)
	DEFINE_UTILITY_SHADER_CONSTANT(texAvgLuma, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(texSource, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(f2TexelSize, Vec2f)
	DEFINE_UTILITY_SHADER_CONSTANT(bLumaAdaptationPass, bool)
	DEFINE_UTILITY_SHADER_CONSTANT(fFrameTime, float)
	DEFINE_UTILITY_SHADER_CONSTANT(texLumaTarget, unsigned int)
	DEFINE_UTILITY_SHADER_CONSTANT(bApplyBrightnessFilter, bool)
	DEFINE_UTILITY_SHADER_CONSTANT(nKernel, int)
	DEFINE_UTILITY_SHADER_CONSTANT(nDownsampleFactor, int)
	DEFINE_UTILITY_SHADER_CONSTANT(bBlurPass, bool)
	DEFINE_UTILITY_SHADER_CONSTANT(f2TexSourceSize, Vec2f)
	//--------------------------------------------------------------------------
}
