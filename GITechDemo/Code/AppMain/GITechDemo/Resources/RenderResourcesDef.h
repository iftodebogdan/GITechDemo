#ifndef RENDER_RESOURCES_DEF_H_
#define RENDER_RESOURCES_DEF_H_

#include <Renderer.h>

#include "RenderResource.h"

#define DECLARE_MODEL(NAME) extern GITechDemoApp::Model NAME

#define DECLARE_TEXTURE(NAME) extern GITechDemoApp::Texture NAME

#define DECLARE_SHADER(NAME) extern GITechDemoApp::Shader NAME

#define DECLARE_SHADER_CONSTANT(NAME, TYPE) extern GITechDemoApp::ShaderConstantTemplate<TYPE> NAME

#define DECLARE_RENDER_TARGET(NAME) extern GITechDemoApp::RenderTarget NAME

namespace GITechDemoApp
{
	// Used for fullscreen effects, post-processing, etc.
	extern LibRendererDll::VertexBuffer*	FullScreenTri;

	// Used to set the nBRDFModel shader constant
	enum BRDFModel
	{
		BLINN_PHONG = 0,
		COOK_TORRANCE_GGX,
		COOK_TORRANCE_BECKMANN
	};

	// Shaders
	DECLARE_SHADER(BokehDofShader);
	DECLARE_SHADER(RSMUpscaleShader);
	DECLARE_SHADER(RSMApplyShader);
	DECLARE_SHADER(DeferredLightDirShader);
	DECLARE_SHADER(SkyBoxShader);
	DECLARE_SHADER(GBufferGenerationShader);
	DECLARE_SHADER(DepthPassShader);
	DECLARE_SHADER(DepthCopyShader);
	DECLARE_SHADER(ColorCopyShader);
	DECLARE_SHADER(RSMCaptureShader);
	DECLARE_SHADER(DownsampleShader);
	DECLARE_SHADER(LumaCalcShader);
	DECLARE_SHADER(HDRToneMappingShader);
	DECLARE_SHADER(BloomShader);
	DECLARE_SHADER(FxaaShader);
	DECLARE_SHADER(SsaoShader);
	DECLARE_SHADER(HUDTextShader);
	DECLARE_SHADER(MotionBlurShader);
	//------------------------------------------------------

	// Models (manage their own textures, no need to declare them)
	DECLARE_MODEL(SponzaScene);
	//------------------------------------------------------

	// Textures (not referenced by models, i.e. custom/utility textures, etc.)
	DECLARE_TEXTURE(SkyTexture);
	DECLARE_TEXTURE(IrradianceTexture);
	DECLARE_TEXTURE(EnvironmentTexture);
	//------------------------------------------------------

	// Render targets
	DECLARE_RENDER_TARGET(GBuffer);
	DECLARE_RENDER_TARGET(ShadowMapDir);
	DECLARE_RENDER_TARGET(LightAccumulationBuffer);
	DECLARE_RENDER_TARGET(RSMBuffer);
	DECLARE_RENDER_TARGET(IndirectLightAccumulationBuffer);
	DECLARE_RENDER_TARGET(HDRDownsampleQuarterBuffer);
	DECLARE_RENDER_TARGET(HDRDownsampleSixteenthBuffer);
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer0);
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer1);
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer2);
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer3);
	DECLARE_RENDER_TARGET(AdaptedLuminance0);
	DECLARE_RENDER_TARGET(AdaptedLuminance1);
	DECLARE_RENDER_TARGET(HDRBloomBuffer0);
	DECLARE_RENDER_TARGET(HDRBloomBuffer1);
	DECLARE_RENDER_TARGET(LDRToneMappedImageBuffer);
	DECLARE_RENDER_TARGET(LDRFxaaImageBuffer);
	DECLARE_RENDER_TARGET(AmbientOcclusionFullBuffer0);
	DECLARE_RENDER_TARGET(AmbientOcclusionFullBuffer1);
	DECLARE_RENDER_TARGET(AmbientOcclusionQuarterBuffer0);
	DECLARE_RENDER_TARGET(AmbientOcclusionQuarterBuffer1);
	DECLARE_RENDER_TARGET(DepthOfFieldFullBuffer);
	DECLARE_RENDER_TARGET(DepthOfFieldQuarterBuffer);
	DECLARE_RENDER_TARGET(AutofocusBuffer0);
	DECLARE_RENDER_TARGET(AutofocusBuffer1);
	DECLARE_RENDER_TARGET(MotionBlurBuffer);

	enum HDRDownsampleBufferIndex {
		QUARTER = 0,
		SIXTEENTH = 1
	};

	extern RenderTarget* HDRDownsampleBuffer[2];
	extern RenderTarget* AverageLuminanceBuffer[4];
	extern RenderTarget* HDRBloomBuffer[2];
	extern RenderTarget* AmbientOcclusionFullBuffer[2];
	extern RenderTarget* AmbientOcclusionQuarterBuffer[2];
	extern RenderTarget* AdaptedLuminance[2];
	extern RenderTarget* AutofocusBuffer[2];
	//------------------------------------------------------

	// Shader constants
	DECLARE_SHADER_CONSTANT(f44WorldMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44ProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44ViewProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44WorldViewProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44WorldViewMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(texDiffuse, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(texNormal, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(bHasNormalMap, bool);
	DECLARE_SHADER_CONSTANT(texSpec, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(bHasSpecMap, bool);
	DECLARE_SHADER_CONSTANT(texMatType, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(texRoughness, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(fSpecIntensity, float);
	DECLARE_SHADER_CONSTANT(f2HalfTexelOffset, Vec2f);
	DECLARE_SHADER_CONSTANT(texDepthBuffer, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(texMaterialBuffer, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(texIrradianceMap, LibRendererDll::SamplerCUBE);
	DECLARE_SHADER_CONSTANT(texEnvMap, LibRendererDll::SamplerCUBE);
	DECLARE_SHADER_CONSTANT(nBRDFModel, unsigned int);
	DECLARE_SHADER_CONSTANT(f44SkyViewProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(texSkyCube, LibRendererDll::SamplerCUBE);
	DECLARE_SHADER_CONSTANT(texDiffuseBuffer, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(fAmbientFactor, float);
	DECLARE_SHADER_CONSTANT(fIrradianceFactor, float);
	DECLARE_SHADER_CONSTANT(fReflectionFactor, float);
	DECLARE_SHADER_CONSTANT(f44InvProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(texNormalBuffer, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(texShadowMap, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(f2OneOverShadowMapSize, Vec2f);
	DECLARE_SHADER_CONSTANT(f44ViewMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44InvViewMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44InvViewProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44ScreenToLightViewMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f3LightDir, Vec3f);
	DECLARE_SHADER_CONSTANT(fDiffuseFactor, float);
	DECLARE_SHADER_CONSTANT(fSpecFactor, float);
	DECLARE_SHADER_CONSTANT(bDebugCascades, bool);
	DECLARE_SHADER_CONSTANT(f2CascadeBoundsMin, Vec2f*);
	DECLARE_SHADER_CONSTANT(f2CascadeBoundsMax, Vec2f*);
	DECLARE_SHADER_CONSTANT(f44CascadeProjMat, Matrix44f*);
	DECLARE_SHADER_CONSTANT(fCascadeBlendSize, float);
	DECLARE_SHADER_CONSTANT(f2PoissonDisk, Vec2f*);
	DECLARE_SHADER_CONSTANT(f44LightViewMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44InvLightViewMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44LightWorldViewProjMat, Matrix44f*);
	DECLARE_SHADER_CONSTANT(f44LightWorldViewMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44RSMWorldViewProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44LightRotMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44LightViewProjMat, Matrix44f*);
	DECLARE_SHADER_CONSTANT(texRSMFluxBuffer, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(texRSMNormalBuffer, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(texRSMDepthBuffer, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(f3RSMKernel, Vec3f*);
	DECLARE_SHADER_CONSTANT(f44RSMProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44RSMInvProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44ViewToRSMViewMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(fRSMIntensity, float);
	DECLARE_SHADER_CONSTANT(fRSMKernelScale, float);
	DECLARE_SHADER_CONSTANT(fWeightThreshold, float);
	DECLARE_SHADER_CONSTANT(bDebugUpscalePass, bool);
	DECLARE_SHADER_CONSTANT(fSunRadius, float);
	DECLARE_SHADER_CONSTANT(fSunBrightness, float);
	DECLARE_SHADER_CONSTANT(texLumaCalcInput, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(bInitialLumaPass, bool);
	DECLARE_SHADER_CONSTANT(bFinalLumaPass, bool);
	DECLARE_SHADER_CONSTANT(texAvgLuma, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(fExposureBias, float);
	DECLARE_SHADER_CONSTANT(f2AvgLumaClamp, Vec2f);
	DECLARE_SHADER_CONSTANT(texSource, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(f2TexelSize, Vec2f);
	DECLARE_SHADER_CONSTANT(fShoulderStrength, float);
	DECLARE_SHADER_CONSTANT(fLinearStrength, float);
	DECLARE_SHADER_CONSTANT(fLinearAngle, float);
	DECLARE_SHADER_CONSTANT(fToeStrength, float);
	DECLARE_SHADER_CONSTANT(fToeNumerator, float);
	DECLARE_SHADER_CONSTANT(fToeDenominator, float);
	DECLARE_SHADER_CONSTANT(fLinearWhite, float);
	DECLARE_SHADER_CONSTANT(bLumaAdaptationPass, bool);
	DECLARE_SHADER_CONSTANT(fLumaAdaptSpeed, float);
	DECLARE_SHADER_CONSTANT(fFrameTime, float);
	DECLARE_SHADER_CONSTANT(texLumaTarget, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(fBrightnessThreshold, float);
	DECLARE_SHADER_CONSTANT(bApplyBrightnessFilter, bool);
	DECLARE_SHADER_CONSTANT(fBloomPower, float);
	DECLARE_SHADER_CONSTANT(nKernel, int);
	DECLARE_SHADER_CONSTANT(nDownsampleFactor, int);
	DECLARE_SHADER_CONSTANT(fBloomStrength, float);
	DECLARE_SHADER_CONSTANT(bAdjustIntensity, bool);
	DECLARE_SHADER_CONSTANT(fFxaaSubpix, float);
	DECLARE_SHADER_CONSTANT(fFxaaEdgeThreshold, float);
	DECLARE_SHADER_CONSTANT(fFxaaEdgeThresholdMin, float);
	DECLARE_SHADER_CONSTANT(fSSAOSampleRadius, float);
	DECLARE_SHADER_CONSTANT(fSSAOIntensity, float);
	DECLARE_SHADER_CONSTANT(fSSAOScale, float);
	DECLARE_SHADER_CONSTANT(fSSAOBias, float);
	DECLARE_SHADER_CONSTANT(bBlurPass, bool);
	DECLARE_SHADER_CONSTANT(f2TexSourceSize, Vec2f);
	DECLARE_SHADER_CONSTANT(fFocalDepth, float);
	DECLARE_SHADER_CONSTANT(fFocalLength, float);
	DECLARE_SHADER_CONSTANT(fFStop, float);
	DECLARE_SHADER_CONSTANT(fCoC, float);
	DECLARE_SHADER_CONSTANT(fNearDofStart, float);
	DECLARE_SHADER_CONSTANT(fNearDofFalloff, float);
	DECLARE_SHADER_CONSTANT(fFarDofStart, float);
	DECLARE_SHADER_CONSTANT(fFarDofFalloff, float);
	DECLARE_SHADER_CONSTANT(bManualDof, bool);
	DECLARE_SHADER_CONSTANT(bDebugFocus, bool);
	DECLARE_SHADER_CONSTANT(bAutofocus, bool);
	DECLARE_SHADER_CONSTANT(fMaxBlur, float);
	DECLARE_SHADER_CONSTANT(fHighlightThreshold, float);
	DECLARE_SHADER_CONSTANT(fHighlightGain, float);
	DECLARE_SHADER_CONSTANT(fBokehBias, float);
	DECLARE_SHADER_CONSTANT(fBokehFringe, float);
	DECLARE_SHADER_CONSTANT(bPentagonBokeh, bool);
	DECLARE_SHADER_CONSTANT(fPentagonFeather, float);
	DECLARE_SHADER_CONSTANT(bUseNoise, bool);
	DECLARE_SHADER_CONSTANT(fNoiseAmount, float);
	DECLARE_SHADER_CONSTANT(bBlurDepth, bool);
	DECLARE_SHADER_CONSTANT(fDepthBlurSize, float);
	DECLARE_SHADER_CONSTANT(bVignetting, bool);
	DECLARE_SHADER_CONSTANT(fVignOut, float);
	DECLARE_SHADER_CONSTANT(fVignIn, float);
	DECLARE_SHADER_CONSTANT(fVignFade, float);
	DECLARE_SHADER_CONSTANT(fZNear, float);
	DECLARE_SHADER_CONSTANT(fZFar, float);
	DECLARE_SHADER_CONSTANT(f2LinearDepthEquation, Vec2f);
	DECLARE_SHADER_CONSTANT(texTargetFocus, LibRendererDll::Sampler2D);
	DECLARE_SHADER_CONSTANT(f3TextColor, Vec3f);
	DECLARE_SHADER_CONSTANT(f44PrevViewProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(fMotionBlurIntensity, float);
	//-------------------------------------------------------
}

#endif //RENDER_RESOURCES_H_