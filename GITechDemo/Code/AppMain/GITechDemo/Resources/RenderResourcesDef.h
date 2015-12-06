/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	RenderResourcesDef.h
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

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
	using namespace Synesthesia3D;

	// Used for fullscreen effects, post-processing, etc.
	extern VertexBuffer*	FullScreenTri;

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
	DECLARE_SHADER(DirectionalLightShader);
	DECLARE_SHADER(SkyBoxShader);
	DECLARE_SHADER(GBufferGenerationShader);
	DECLARE_SHADER(DepthPassShader);
	DECLARE_SHADER(DepthCopyShader);
	DECLARE_SHADER(ColorCopyShader);
	DECLARE_SHADER(RSMCaptureShader);
	DECLARE_SHADER(DownsampleShader);
	DECLARE_SHADER(LumaCaptureShader);
	DECLARE_SHADER(LumaAdaptShader);
	DECLARE_SHADER(HDRToneMappingShader);
	DECLARE_SHADER(BloomShader);
	DECLARE_SHADER(FxaaShader);
	DECLARE_SHADER(SsaoShader);
	DECLARE_SHADER(HUDTextShader);
	DECLARE_SHADER(MotionBlurShader);
	DECLARE_SHADER(SphericalLensFlareFeaturesShader);
	DECLARE_SHADER(LensFlareApplyShader);
	DECLARE_SHADER(DirectionalLightVolumeShader);
	DECLARE_SHADER(BilateralBlurShader);
	DECLARE_SHADER(NearestDepthUpscaleShader);
	DECLARE_SHADER(AnamorphicLensFlareFeaturesShader);
	DECLARE_SHADER(AnamorphicLensFlareBlurShader);
	//------------------------------------------------------

	// Models (manage their own textures, no need to declare them)
	DECLARE_MODEL(SponzaScene);
	//------------------------------------------------------

	// Textures (not referenced by models, i.e. custom/utility textures, etc.)
	DECLARE_TEXTURE(SkyTexture);
	DECLARE_TEXTURE(IrradianceTexture);
	DECLARE_TEXTURE(EnvironmentTexture);
	DECLARE_TEXTURE(LensFlareGhostColorLUT);
	DECLARE_TEXTURE(LensFlareDirt);
	DECLARE_TEXTURE(LensFlareStarBurst);
	DECLARE_TEXTURE(BayerMatrix);
	DECLARE_TEXTURE(NoiseTexture);
	//------------------------------------------------------

	// Render targets
	DECLARE_RENDER_TARGET(GBuffer);
	DECLARE_RENDER_TARGET(ShadowMapDir);
	DECLARE_RENDER_TARGET(LightAccumulationBuffer);
	DECLARE_RENDER_TARGET(RSMBuffer);
	DECLARE_RENDER_TARGET(IndirectLightAccumulationBuffer);
	DECLARE_RENDER_TARGET(VolumetricLightFullBuffer0);
	DECLARE_RENDER_TARGET(VolumetricLightFullBuffer1);
	DECLARE_RENDER_TARGET(VolumetricLightQuarterBuffer0);
	DECLARE_RENDER_TARGET(VolumetricLightQuarterBuffer1);
	DECLARE_RENDER_TARGET(HDRDownsampleQuarterBuffer);
	DECLARE_RENDER_TARGET(HDRDownsampleSixteenthBuffer);
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer0);
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer1);
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer2);
	DECLARE_RENDER_TARGET(AverageLuminanceBuffer3);
	DECLARE_RENDER_TARGET(AdaptedLuminance0);
	DECLARE_RENDER_TARGET(AdaptedLuminance1);
	DECLARE_RENDER_TARGET(BloomBuffer0);
	DECLARE_RENDER_TARGET(BloomBuffer1);
	DECLARE_RENDER_TARGET(LDRToneMappedImageBuffer);
	DECLARE_RENDER_TARGET(LDRFxaaImageBuffer);
	DECLARE_RENDER_TARGET(SSAOFullBuffer0);
	DECLARE_RENDER_TARGET(SSAOFullBuffer1);
	DECLARE_RENDER_TARGET(SSAOQuarterBuffer0);
	DECLARE_RENDER_TARGET(SSAOQuarterBuffer1);
	DECLARE_RENDER_TARGET(DepthOfFieldFullBuffer);
	DECLARE_RENDER_TARGET(DepthOfFieldQuarterBuffer);
	DECLARE_RENDER_TARGET(AutofocusBuffer0);
	DECLARE_RENDER_TARGET(AutofocusBuffer1);
	DECLARE_RENDER_TARGET(MotionBlurBuffer);
	DECLARE_RENDER_TARGET(SphericalLensFlareBuffer0);
	DECLARE_RENDER_TARGET(SphericalLensFlareBuffer1);
	DECLARE_RENDER_TARGET(AnamorphicLensFlareBuffer0);
	DECLARE_RENDER_TARGET(AnamorphicLensFlareBuffer1);
	DECLARE_RENDER_TARGET(AnamorphicLensFlareBuffer2);

	enum HDRDownsampleBufferIndex {
		QUARTER = 0,
		SIXTEENTH = 1
	};

	extern RenderTarget* VolumetricLightFullBuffer[2];
	extern RenderTarget* VolumetricLightQuarterBuffer[2];
	extern RenderTarget* HDRDownsampleBuffer[2];
	extern RenderTarget* AverageLuminanceBuffer[4];
	extern RenderTarget* BloomBuffer[2];
	extern RenderTarget* SSAOFullBuffer[2];
	extern RenderTarget* SSAOQuarterBuffer[2];
	extern RenderTarget* AdaptedLuminance[2];
	extern RenderTarget* AutofocusBuffer[2];
	extern RenderTarget* SphericalLensFlareBuffer[2];
	extern RenderTarget* AnamorphicLensFlareBuffer[3];
	//------------------------------------------------------

	// Shader constants
	DECLARE_SHADER_CONSTANT(f44WorldMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44ProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44ViewProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44WorldViewProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(f44WorldViewMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(texDiffuse, Sampler2D);
	DECLARE_SHADER_CONSTANT(texNormal, Sampler2D);
	DECLARE_SHADER_CONSTANT(bHasNormalMap, bool);
	DECLARE_SHADER_CONSTANT(texSpec, Sampler2D);
	DECLARE_SHADER_CONSTANT(bHasSpecMap, bool);
	DECLARE_SHADER_CONSTANT(texMatType, Sampler2D);
	DECLARE_SHADER_CONSTANT(texRoughness, Sampler2D);
	DECLARE_SHADER_CONSTANT(fSpecIntensity, float);
	DECLARE_SHADER_CONSTANT(f2HalfTexelOffset, Vec2f);
	DECLARE_SHADER_CONSTANT(texDepthBuffer, Sampler2D);
	DECLARE_SHADER_CONSTANT(texMaterialBuffer, Sampler2D);
	DECLARE_SHADER_CONSTANT(texIrradianceMap, SamplerCUBE);
	DECLARE_SHADER_CONSTANT(texEnvMap, SamplerCUBE);
	DECLARE_SHADER_CONSTANT(nBRDFModel, unsigned int);
	DECLARE_SHADER_CONSTANT(f44SkyViewProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(texSkyCube, SamplerCUBE);
	DECLARE_SHADER_CONSTANT(texDiffuseBuffer, Sampler2D);
	DECLARE_SHADER_CONSTANT(fAmbientFactor, float);
	DECLARE_SHADER_CONSTANT(fIrradianceFactor, float);
	DECLARE_SHADER_CONSTANT(fReflectionFactor, float);
	DECLARE_SHADER_CONSTANT(f44InvProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(texNormalBuffer, Sampler2D);
	DECLARE_SHADER_CONSTANT(texShadowMap, Sampler2D);
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
	DECLARE_SHADER_CONSTANT(texRSMFluxBuffer, Sampler2D);
	DECLARE_SHADER_CONSTANT(texRSMNormalBuffer, Sampler2D);
	DECLARE_SHADER_CONSTANT(texRSMDepthBuffer, Sampler2D);
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
	DECLARE_SHADER_CONSTANT(texLumaInput, Sampler2D);
	DECLARE_SHADER_CONSTANT(bInitialLumaPass, bool);
	DECLARE_SHADER_CONSTANT(bFinalLumaPass, bool);
	DECLARE_SHADER_CONSTANT(texAvgLuma, Sampler2D);
	DECLARE_SHADER_CONSTANT(fExposureBias, float);
	DECLARE_SHADER_CONSTANT(f2AvgLumaClamp, Vec2f);
	DECLARE_SHADER_CONSTANT(texSource, Sampler2D);
	DECLARE_SHADER_CONSTANT(f2TexelSize, Vec2f);
	DECLARE_SHADER_CONSTANT(fShoulderStrength, float);
	DECLARE_SHADER_CONSTANT(fLinearStrength, float);
	DECLARE_SHADER_CONSTANT(fLinearAngle, float);
	DECLARE_SHADER_CONSTANT(fToeStrength, float);
	DECLARE_SHADER_CONSTANT(fToeNumerator, float);
	DECLARE_SHADER_CONSTANT(fToeDenominator, float);
	DECLARE_SHADER_CONSTANT(fLinearWhite, float);
	DECLARE_SHADER_CONSTANT(fLumaAdaptSpeed, float);
	DECLARE_SHADER_CONSTANT(fFrameTime, float);
	DECLARE_SHADER_CONSTANT(texLumaTarget, Sampler2D);
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
	DECLARE_SHADER_CONSTANT(f2TexSize, Vec2f);
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
	DECLARE_SHADER_CONSTANT(texTargetFocus, Sampler2D);
	DECLARE_SHADER_CONSTANT(f3TextColor, Vec3f);
	DECLARE_SHADER_CONSTANT(f44PrevViewProjMat, Matrix44f);
	DECLARE_SHADER_CONSTANT(fMotionBlurIntensity, float);
	DECLARE_SHADER_CONSTANT(nMotionBlurNumSamples, int);
	DECLARE_SHADER_CONSTANT(texGhostColorLUT, Sampler1D);
	DECLARE_SHADER_CONSTANT(nGhostSamples, int);
	DECLARE_SHADER_CONSTANT(fGhostDispersal, float);
	DECLARE_SHADER_CONSTANT(fGhostRadialWeightExp, float);
	DECLARE_SHADER_CONSTANT(fHaloSize, float);
	DECLARE_SHADER_CONSTANT(fHaloRadialWeightExp, float);
	DECLARE_SHADER_CONSTANT(bChromaShift, bool);
	DECLARE_SHADER_CONSTANT(fShiftFactor, float);
	DECLARE_SHADER_CONSTANT(texLensFlareFeatures, Sampler2D);
	DECLARE_SHADER_CONSTANT(texLensFlareDirt, Sampler2D);
	DECLARE_SHADER_CONSTANT(texLensFlareStarBurst, Sampler2D);
	DECLARE_SHADER_CONSTANT(fLensDirtIntensity, float);
	DECLARE_SHADER_CONSTANT(fLensStarBurstIntensity, float);
	DECLARE_SHADER_CONSTANT(f33LensFlareStarBurstMat, Matrix33f);
	DECLARE_SHADER_CONSTANT(bSingleChannelCopy, bool);
	DECLARE_SHADER_CONSTANT(f3CameraPositionLightVS, Vec3f);
	DECLARE_SHADER_CONSTANT(fRaymarchDistanceLimit, float);
	DECLARE_SHADER_CONSTANT(fLightIntensity, float);
	DECLARE_SHADER_CONSTANT(fMultScatterIntensity, float);
	DECLARE_SHADER_CONSTANT(texDitherMap, Sampler2D);
	DECLARE_SHADER_CONSTANT(f2BlurDir, Vec2f);
	DECLARE_SHADER_CONSTANT(fUpsampleDepthThreshold, float);
	DECLARE_SHADER_CONSTANT(f2DepthHalfTexelOffset, Vec2f);
	DECLARE_SHADER_CONSTANT(fBlurDepthFalloff, float);
	DECLARE_SHADER_CONSTANT(nSampleCount, int);
	DECLARE_SHADER_CONSTANT(texNoise, Sampler3D);
	DECLARE_SHADER_CONSTANT(fElapsedTime, float);
	DECLARE_SHADER_CONSTANT(f3FogBox, Vec3f);
	DECLARE_SHADER_CONSTANT(f3FogSpeed, Vec3f);
	DECLARE_SHADER_CONSTANT(fFogVerticalFalloff, float);
	DECLARE_SHADER_CONSTANT(fAnamorphicIntensity, float);
	//-------------------------------------------------------
}

#endif //RENDER_RESOURCES_H_
