#ifndef SCENE_RESOURCE_DECLARATION_H_
#define SCENE_RESOURCE_DECLARATION_H_

#include "RenderResource.h"

#define DECLARE_MODEL(NAME) \
	extern GITechDemoApp::Model NAME;

#define DECLARE_TEXTURE(NAME) \
	extern GITechDemoApp::Texture NAME;

#define DECLARE_SHADER(NAME) \
	extern GITechDemoApp::Shader NAME;

#define DECLARE_SHADER_CONSTANT(NAME, TYPE) \
	extern GITechDemoApp::ShaderConstantTemplate<TYPE> NAME;

#define DECLARE_RENDER_TARGET(NAME) \
	extern GITechDemoApp::RenderTarget NAME;

namespace GITechDemoApp
{
	// Models (manages its own textures, no need to declare them)
	DECLARE_MODEL(SponzaScene)
	//------------------------------------------------------

	// Textures (not referenced by model, custom/utility textures, etc.)
	DECLARE_TEXTURE(SkyTexture)
	//------------------------------------------------------

	// Render targets
	DECLARE_RENDER_TARGET(GBuffer)
	DECLARE_RENDER_TARGET(ShadowMapDir)
	DECLARE_RENDER_TARGET(LightAccumulationBuffer)
	DECLARE_RENDER_TARGET(RSMBuffer)
	DECLARE_RENDER_TARGET(IndirectLightAccumulationBuffer)
	DECLARE_RENDER_TARGET(HDRDownsampleBuffer)
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer0)
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer1)
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer2)
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer3)
	DECLARE_RENDER_TARGET(AdaptedLuminance0)
	DECLARE_RENDER_TARGET(AdaptedLuminance1)
	DECLARE_RENDER_TARGET(HDRBloomBuffer0)
	DECLARE_RENDER_TARGET(HDRBloomBuffer1)
	DECLARE_RENDER_TARGET(LDRToneMappedImageBuffer)
	DECLARE_RENDER_TARGET(LDRFxaaImageBuffer)
	DECLARE_RENDER_TARGET(AmbientOcclusionBuffer0)
	DECLARE_RENDER_TARGET(AmbientOcclusionBuffer1)
	DECLARE_RENDER_TARGET(DepthOfFieldFullBuffer)
	DECLARE_RENDER_TARGET(DepthOfFieldQuarterBuffer)

	extern RenderTarget* AverageLuminanceBuffer[4];
	extern RenderTarget* HDRBloomBuffer[2];
	extern RenderTarget* AmbientOcclusionBuffer[2];
	extern RenderTarget* AdaptedLuminance[2];
	//------------------------------------------------------

	// Shaders
	DECLARE_SHADER(SkyBoxShader)
	DECLARE_SHADER(GBufferGenerationShader)
	DECLARE_SHADER(DeferredLightAmbShader)
	DECLARE_SHADER(DeferredLightDirShader)
	DECLARE_SHADER(DepthPassShader)
	DECLARE_SHADER(DepthCopyShader)
	DECLARE_SHADER(ColorCopyShader)
	DECLARE_SHADER(RSMCaptureShader)
	DECLARE_SHADER(RSMApplyShader)
	DECLARE_SHADER(DownsampleShader)
	DECLARE_SHADER(LumaCalcShader)
	DECLARE_SHADER(HDRToneMappingShader)
	DECLARE_SHADER(BloomShader)
	DECLARE_SHADER(FxaaShader)
	DECLARE_SHADER(SsaoShader)
	DECLARE_SHADER(BokehDofShader)
	//------------------------------------------------------

	// Shader constants
	DECLARE_SHADER_CONSTANT(f44WorldViewProjMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(f44WorldViewMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(texDiffuse, unsigned int)
	DECLARE_SHADER_CONSTANT(texNormal, unsigned int)
	DECLARE_SHADER_CONSTANT(bHasNormalMap, bool)
	DECLARE_SHADER_CONSTANT(texSpec, unsigned int)
	DECLARE_SHADER_CONSTANT(bHasSpecMap, bool)
	DECLARE_SHADER_CONSTANT(fSpecIntensity, float)
	DECLARE_SHADER_CONSTANT(f2HalfTexelOffset, Vec2f)
	DECLARE_SHADER_CONSTANT(texDepthBuffer, unsigned int)
	DECLARE_SHADER_CONSTANT(f44SkyViewProjMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(texSkyTex, unsigned int)
	DECLARE_SHADER_CONSTANT(texDiffuseBuffer, unsigned int)
	DECLARE_SHADER_CONSTANT(fAmbientFactor, float)
	DECLARE_SHADER_CONSTANT(f44InvProjMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(texNormalBuffer, unsigned int)
	DECLARE_SHADER_CONSTANT(texShadowMap, unsigned int)
	DECLARE_SHADER_CONSTANT(fShadowDepthBias, float)
	DECLARE_SHADER_CONSTANT(f2OneOverShadowMapSize, Vec2f)
	DECLARE_SHADER_CONSTANT(f44ViewMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(f44InvViewProjMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(f44ScreenToLightViewMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(f3LightDir, Vec3f)
	DECLARE_SHADER_CONSTANT(fDiffuseFactor, float)
	DECLARE_SHADER_CONSTANT(fSpecFactor, float)
	DECLARE_SHADER_CONSTANT(bDebugCascades, bool)
	DECLARE_SHADER_CONSTANT(f2CascadeBoundsMin, Vec2f*)
	DECLARE_SHADER_CONSTANT(f2CascadeBoundsMax, Vec2f*)
	DECLARE_SHADER_CONSTANT(f44CascadeProjMat, Matrix44f*)
	DECLARE_SHADER_CONSTANT(fCascadeBlendSize, float)
	DECLARE_SHADER_CONSTANT(texLightAccumulationBuffer, unsigned int)
	DECLARE_SHADER_CONSTANT(poissonDisk, Vec2f*)
	DECLARE_SHADER_CONSTANT(f44LightWorldViewProjMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(f44LightWorldViewMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(texRSMFluxBuffer, unsigned int)
	DECLARE_SHADER_CONSTANT(texRSMNormalBuffer, unsigned int)
	DECLARE_SHADER_CONSTANT(texRSMDepthBuffer, unsigned int)
	DECLARE_SHADER_CONSTANT(f3RSMKernel, Vec3f*)
	DECLARE_SHADER_CONSTANT(f44RSMProjMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(f44RSMInvProjMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(f44ViewToRSMViewMat, Matrix44f)
	DECLARE_SHADER_CONSTANT(fRSMIntensity, float)
	DECLARE_SHADER_CONSTANT(fRSMKernelScale, float)
	DECLARE_SHADER_CONSTANT(fSunRadius, float)
	DECLARE_SHADER_CONSTANT(fSunBrightness, float)
	DECLARE_SHADER_CONSTANT(texIndirectLightAccumulationBuffer, unsigned int)
	DECLARE_SHADER_CONSTANT(bIsUpscalePass, bool)
	DECLARE_SHADER_CONSTANT(texLumaCalcInput, unsigned int)
	DECLARE_SHADER_CONSTANT(bInitialLumaPass, bool)
	DECLARE_SHADER_CONSTANT(bFinalLumaPass, bool)
	DECLARE_SHADER_CONSTANT(texAvgLuma, unsigned int)
	DECLARE_SHADER_CONSTANT(fExposureBias, float)
	DECLARE_SHADER_CONSTANT(f2AvgLumaClamp, Vec2f)
	DECLARE_SHADER_CONSTANT(texSource, unsigned int)
	DECLARE_SHADER_CONSTANT(f2TexelSize, Vec2f)
	DECLARE_SHADER_CONSTANT(fShoulderStrength, float)
	DECLARE_SHADER_CONSTANT(fLinearStrength, float)
	DECLARE_SHADER_CONSTANT(fLinearAngle, float)
	DECLARE_SHADER_CONSTANT(fToeStrength, float)
	DECLARE_SHADER_CONSTANT(fToeNumerator, float)
	DECLARE_SHADER_CONSTANT(fToeDenominator, float)
	DECLARE_SHADER_CONSTANT(fLinearWhite, float)
	DECLARE_SHADER_CONSTANT(bLumaAdaptationPass, bool)
	DECLARE_SHADER_CONSTANT(fLumaAdaptSpeed, float)
	DECLARE_SHADER_CONSTANT(fFrameTime, float)
	DECLARE_SHADER_CONSTANT(texLumaTarget, unsigned int)
	DECLARE_SHADER_CONSTANT(fBrightnessThreshold, float)
	DECLARE_SHADER_CONSTANT(bApplyBrightnessFilter, bool)
	DECLARE_SHADER_CONSTANT(fBloomPower, float)
	DECLARE_SHADER_CONSTANT(nKernel, int)
	DECLARE_SHADER_CONSTANT(nDownsampleFactor, int)
	DECLARE_SHADER_CONSTANT(fBloomStrength, float)
	DECLARE_SHADER_CONSTANT(fFxaaSubpix, float)
	DECLARE_SHADER_CONSTANT(fFxaaEdgeThreshold, float)
	DECLARE_SHADER_CONSTANT(fFxaaEdgeThresholdMin, float)
	DECLARE_SHADER_CONSTANT(fSSAOSampleRadius, float)
	DECLARE_SHADER_CONSTANT(fSSAOIntensity, float)
	DECLARE_SHADER_CONSTANT(fSSAOScale, float)
	DECLARE_SHADER_CONSTANT(fSSAOBias, float)
	DECLARE_SHADER_CONSTANT(bBlurPass, bool)
	DECLARE_SHADER_CONSTANT(f2TexSourceSize, Vec2f)
	DECLARE_SHADER_CONSTANT(fFocalDepth, float)
	DECLARE_SHADER_CONSTANT(fFocalLength, float)
	DECLARE_SHADER_CONSTANT(fFStop, float)
	DECLARE_SHADER_CONSTANT(fCoC, float)
	DECLARE_SHADER_CONSTANT(fNearDofStart, float)
	DECLARE_SHADER_CONSTANT(fNearDofFalloff, float)
	DECLARE_SHADER_CONSTANT(fFarDofStart, float)
	DECLARE_SHADER_CONSTANT(fFarDofFalloff, float)
	DECLARE_SHADER_CONSTANT(bManualDof, bool)
	DECLARE_SHADER_CONSTANT(bDebugFocus, bool)
	DECLARE_SHADER_CONSTANT(bAutofocus, bool)
	DECLARE_SHADER_CONSTANT(f2FocusPoint, Vec2f)
	DECLARE_SHADER_CONSTANT(fMaxBlur, float)
	DECLARE_SHADER_CONSTANT(fHighlightThreshold, float)
	DECLARE_SHADER_CONSTANT(fHighlightGain, float)
	DECLARE_SHADER_CONSTANT(fBokehBias, float)
	DECLARE_SHADER_CONSTANT(fBokehFringe, float)
	DECLARE_SHADER_CONSTANT(bPentagonBokeh, bool)
	DECLARE_SHADER_CONSTANT(fPentagonFeather, float)
	DECLARE_SHADER_CONSTANT(bUseNoise, bool)
	DECLARE_SHADER_CONSTANT(fNoiseAmount, float)
	DECLARE_SHADER_CONSTANT(bBlurDepth, bool)
	DECLARE_SHADER_CONSTANT(fDepthBlurSize, float)
	DECLARE_SHADER_CONSTANT(bVignetting, bool)
	DECLARE_SHADER_CONSTANT(fVignOut, float)
	DECLARE_SHADER_CONSTANT(fVignIn, float)
	DECLARE_SHADER_CONSTANT(fVignFade, float)
	//-------------------------------------------------------
}

#endif