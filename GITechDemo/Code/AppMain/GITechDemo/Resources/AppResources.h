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

#ifndef APP_RESOURCES_H_
#define APP_RESOURCES_H_

#include <Renderer.h>
#include "RenderResource.h"

//////////////////////////
// Some handy defines	//
//////////////////////////
#define CREATE_MODEL_HANDLE(NAME) extern GITechDemoApp::Model NAME
#define CREATE_TEXTURE_HANDLE(NAME) extern GITechDemoApp::Texture NAME
#define CREATE_SHADER_HANDLE(NAME) extern GITechDemoApp::Shader NAME
#define CREATE_SHADER_CONSTANT_HANDLE(NAME, TYPE) extern GITechDemoApp::ShaderConstantTemplate<TYPE> NAME
#define CREATE_RENDER_TARGET_HANDLE(NAME) extern GITechDemoApp::RenderTarget NAME
///////////////////////////////////////////////////////////

namespace GITechDemoApp
{
	using namespace Synesthesia3D;

	// Shaders
	CREATE_SHADER_HANDLE(BokehDofShader);
	CREATE_SHADER_HANDLE(RSMUpscaleShader);
	CREATE_SHADER_HANDLE(RSMApplyShader);
	CREATE_SHADER_HANDLE(DirectionalLightShader);
	CREATE_SHADER_HANDLE(SkyBoxShader);
	CREATE_SHADER_HANDLE(GBufferGenerationShader);
	CREATE_SHADER_HANDLE(DepthPassShader);
	CREATE_SHADER_HANDLE(DepthCopyShader);
	CREATE_SHADER_HANDLE(ColorCopyShader);
	CREATE_SHADER_HANDLE(RSMCaptureShader);
	CREATE_SHADER_HANDLE(DownsampleShader);
	CREATE_SHADER_HANDLE(LumaCaptureShader);
	CREATE_SHADER_HANDLE(LumaAdaptShader);
	CREATE_SHADER_HANDLE(HDRToneMappingShader);
	CREATE_SHADER_HANDLE(BloomShader);
	CREATE_SHADER_HANDLE(FxaaShader);
	CREATE_SHADER_HANDLE(SsaoShader);
	CREATE_SHADER_HANDLE(HUDTextShader);
	CREATE_SHADER_HANDLE(MotionBlurShader);
	CREATE_SHADER_HANDLE(SphericalLensFlareFeaturesShader);
	CREATE_SHADER_HANDLE(LensFlareApplyShader);
	CREATE_SHADER_HANDLE(DirectionalLightVolumeShader);
	CREATE_SHADER_HANDLE(BilateralBlurShader);
	CREATE_SHADER_HANDLE(NearestDepthUpscaleShader);
	CREATE_SHADER_HANDLE(AnamorphicLensFlareFeaturesShader);
	CREATE_SHADER_HANDLE(AnamorphicLensFlareBlurShader);
	//------------------------------------------------------

	// Models (manage their own textures, no need to declare them)
	CREATE_MODEL_HANDLE(SponzaScene);
	//------------------------------------------------------

	// Textures (not referenced by models, i.e. custom/utility textures, etc.)
	CREATE_TEXTURE_HANDLE(SkyTexture);
	CREATE_TEXTURE_HANDLE(IrradianceTexture);
	CREATE_TEXTURE_HANDLE(EnvironmentTexture);
	CREATE_TEXTURE_HANDLE(LensFlareGhostColorLUT);
	CREATE_TEXTURE_HANDLE(LensFlareDirt);
	CREATE_TEXTURE_HANDLE(LensFlareStarBurst);
	CREATE_TEXTURE_HANDLE(BayerMatrix);
	CREATE_TEXTURE_HANDLE(NoiseTexture);
	//------------------------------------------------------

	// Render targets
	CREATE_RENDER_TARGET_HANDLE(GBuffer);
	CREATE_RENDER_TARGET_HANDLE(ShadowMapDir);
	CREATE_RENDER_TARGET_HANDLE(LightAccumulationBuffer);
	CREATE_RENDER_TARGET_HANDLE(RSMBuffer);
	CREATE_RENDER_TARGET_HANDLE(IndirectLightAccumulationBuffer);
	CREATE_RENDER_TARGET_HANDLE(VolumetricLightFullBuffer0);
	CREATE_RENDER_TARGET_HANDLE(VolumetricLightFullBuffer1);
	CREATE_RENDER_TARGET_HANDLE(VolumetricLightQuarterBuffer0);
	CREATE_RENDER_TARGET_HANDLE(VolumetricLightQuarterBuffer1);
	CREATE_RENDER_TARGET_HANDLE(HDRDownsampleQuarterBuffer);
	CREATE_RENDER_TARGET_HANDLE(HDRDownsampleSixteenthBuffer);
	CREATE_RENDER_TARGET_HANDLE(AverageLuminanceBuffer0);
	CREATE_RENDER_TARGET_HANDLE(AverageLuminanceBuffer1);
	CREATE_RENDER_TARGET_HANDLE(AverageLuminanceBuffer2);
	CREATE_RENDER_TARGET_HANDLE(AverageLuminanceBuffer3);
	CREATE_RENDER_TARGET_HANDLE(AdaptedLuminance0);
	CREATE_RENDER_TARGET_HANDLE(AdaptedLuminance1);
	CREATE_RENDER_TARGET_HANDLE(BloomBuffer0);
	CREATE_RENDER_TARGET_HANDLE(BloomBuffer1);
	CREATE_RENDER_TARGET_HANDLE(LDRToneMappedImageBuffer);
	CREATE_RENDER_TARGET_HANDLE(LDRFxaaImageBuffer);
	CREATE_RENDER_TARGET_HANDLE(SSAOFullBuffer0);
	CREATE_RENDER_TARGET_HANDLE(SSAOFullBuffer1);
	CREATE_RENDER_TARGET_HANDLE(SSAOQuarterBuffer0);
	CREATE_RENDER_TARGET_HANDLE(SSAOQuarterBuffer1);
	CREATE_RENDER_TARGET_HANDLE(DepthOfFieldBuffer0);
	CREATE_RENDER_TARGET_HANDLE(DepthOfFieldBuffer1);
	CREATE_RENDER_TARGET_HANDLE(AutofocusBuffer0);
	CREATE_RENDER_TARGET_HANDLE(AutofocusBuffer1);
	CREATE_RENDER_TARGET_HANDLE(MotionBlurBuffer);
	CREATE_RENDER_TARGET_HANDLE(SphericalLensFlareBuffer0);
	CREATE_RENDER_TARGET_HANDLE(SphericalLensFlareBuffer1);
	CREATE_RENDER_TARGET_HANDLE(AnamorphicLensFlareBuffer0);
	CREATE_RENDER_TARGET_HANDLE(AnamorphicLensFlareBuffer1);
	CREATE_RENDER_TARGET_HANDLE(AnamorphicLensFlareBuffer2);

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
	extern RenderTarget* DepthOfFieldBuffer[2];
	extern RenderTarget* AdaptedLuminance[2];
	extern RenderTarget* AutofocusBuffer[2];
	extern RenderTarget* SphericalLensFlareBuffer[2];
	extern RenderTarget* AnamorphicLensFlareBuffer[3];
	//------------------------------------------------------

	// Shader constants
	//	- General purpose
	CREATE_SHADER_CONSTANT_HANDLE(f44WorldMat,				Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44ViewMat,				Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44InvViewMat,			Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44ProjMat,				Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44InvProjMat,			Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44WorldViewMat,			Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44ViewProjMat,			Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44InvViewProjMat,		Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44WorldViewProjMat,		Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44LightViewMat,			Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44LightViewProjMat,		Matrix44f*	);
	CREATE_SHADER_CONSTANT_HANDLE(f44LightWorldViewProjMat,	Matrix44f*	);
	CREATE_SHADER_CONSTANT_HANDLE(f44ScreenToLightViewMat,	Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(fFrameTime,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fZNear,					float		);
	CREATE_SHADER_CONSTANT_HANDLE(fZFar,					float		);

	//	- Multiple appearances
	CREATE_SHADER_CONSTANT_HANDLE(f2HalfTexelOffset,		Vec2f		);
	CREATE_SHADER_CONSTANT_HANDLE(f2TexelSize,				Vec2f		);
	CREATE_SHADER_CONSTANT_HANDLE(f2TexSize,				Vec2f		);
	CREATE_SHADER_CONSTANT_HANDLE(texSource,				sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texDiffuse,				sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texDepthBuffer,			sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texNormalBuffer,			sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texShadowMap,				sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texLumaInput,				sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(f3LightDir,				Vec3f		);
	CREATE_SHADER_CONSTANT_HANDLE(nKernel,					int			);
	CREATE_SHADER_CONSTANT_HANDLE(bSingleChannelCopy,		bool		);
	CREATE_SHADER_CONSTANT_HANDLE(nSampleCount,				int			);

	//	- GBufferGeneration.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(texNormal,				sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(bHasNormalMap,			bool		);
	CREATE_SHADER_CONSTANT_HANDLE(texSpec,					sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(bHasSpecMap,				bool		);
	CREATE_SHADER_CONSTANT_HANDLE(texMatType,				sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texRoughness,				sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(fSpecIntensity,			float		);

	//	- DirectionalLight.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(texMaterialBuffer,		sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texIrradianceMap,			samplerCUBE	);
	CREATE_SHADER_CONSTANT_HANDLE(texEnvMap,				samplerCUBE	);
	CREATE_SHADER_CONSTANT_HANDLE(nBRDFModel,				int			);
	CREATE_SHADER_CONSTANT_HANDLE(texDiffuseBuffer,			sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(fAmbientFactor,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fIrradianceFactor,		float		);
	CREATE_SHADER_CONSTANT_HANDLE(fReflectionFactor,		float		);
	CREATE_SHADER_CONSTANT_HANDLE(f2OneOverShadowMapSize,	Vec2f		);
	CREATE_SHADER_CONSTANT_HANDLE(fDiffuseFactor,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fSpecFactor,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(bDebugCascades,			bool		);

	//	- Skybox.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(f44SkyViewProjMat,		Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(texSkyCube,				samplerCUBE	);
	CREATE_SHADER_CONSTANT_HANDLE(fSunRadius,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fSunBrightness,			float		);

	//	- CSMUtils.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(f2CascadeBoundsMin,		Vec2f*		);
	CREATE_SHADER_CONSTANT_HANDLE(f2CascadeBoundsMax,		Vec2f*		);
	CREATE_SHADER_CONSTANT_HANDLE(f44CascadeProjMat,		Matrix44f*	);
	CREATE_SHADER_CONSTANT_HANDLE(fCascadeBlendSize,		float		);

	//	- Utils.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(f2PoissonDisk,			Vec2f*		);

	//	- DirectionalLightVolume.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(f44InvLightViewMat,		Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f3CameraPositionLightVS,	Vec3f		);
	CREATE_SHADER_CONSTANT_HANDLE(fRaymarchDistanceLimit,	float		);
	CREATE_SHADER_CONSTANT_HANDLE(fLightIntensity,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fMultScatterIntensity,	float		);
	CREATE_SHADER_CONSTANT_HANDLE(texDitherMap,				sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texNoise,					sampler3D	);
	CREATE_SHADER_CONSTANT_HANDLE(fElapsedTime,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(f3FogBox,					Vec3f		);
	CREATE_SHADER_CONSTANT_HANDLE(f3FogSpeed,				Vec3f		);
	CREATE_SHADER_CONSTANT_HANDLE(fFogVerticalFalloff,		float		);

	//	- RSMCapture.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(f44LightWorldViewMat,		Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44RSMWorldViewProjMat,	Matrix44f	);

	//	- RSMCommon.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(texRSMFluxBuffer,			sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texRSMNormalBuffer,		sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texRSMDepthBuffer,		sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(f3RSMKernel,				Vec3f*		);
	CREATE_SHADER_CONSTANT_HANDLE(f44RSMProjMat,			Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44RSMInvProjMat,			Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(f44ViewToRSMViewMat,		Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(fRSMIntensity,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fRSMKernelScale,			float		);

	//	- RSMUpscale.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(fWeightThreshold,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(bDebugUpscalePass,		bool		);

	//	- LumaCapture.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(bInitialLumaPass,			bool		);
	CREATE_SHADER_CONSTANT_HANDLE(bFinalLumaPass,			bool		);
	CREATE_SHADER_CONSTANT_HANDLE(f2AvgLumaClamp,			Vec2f		);
	
	//	- HDRToneMapping.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(texAvgLuma,				sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(fExposureBias,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fShoulderStrength,		float		);
	CREATE_SHADER_CONSTANT_HANDLE(fLinearStrength,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fLinearAngle,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fToeStrength,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fToeNumerator,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fToeDenominator,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fLinearWhite,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fFilmGrainAmount,			float		);

	//	- LumaAdapt.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(fLumaAdaptSpeed,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(texLumaTarget,			sampler2D	);

	//	- Downsample.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(fBrightnessThreshold,		float		);
	CREATE_SHADER_CONSTANT_HANDLE(bApplyBrightnessFilter,	bool		);
	CREATE_SHADER_CONSTANT_HANDLE(nDownsampleFactor,		int			);

	//	- Bloom.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(fBloomPower,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fBloomStrength,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(bAdjustIntensity,			bool		);

	//	- FXAA.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(fFxaaSubpix,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fFxaaEdgeThreshold,		float		);
	CREATE_SHADER_CONSTANT_HANDLE(fFxaaEdgeThresholdMin,	float		);

	//	- SSAO.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(fSSAOSampleRadius,		float		);
	CREATE_SHADER_CONSTANT_HANDLE(fSSAOIntensity,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fSSAOScale,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fSSAOBias,				float		);

	//	- BokehDoF.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(fFocalDepth,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fFocalLength,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fFStop,					float		);
	CREATE_SHADER_CONSTANT_HANDLE(fCoC,						float		);
	CREATE_SHADER_CONSTANT_HANDLE(bAutofocus,				bool		);
	CREATE_SHADER_CONSTANT_HANDLE(texTargetFocus,			sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(fApertureSize,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fHighlightThreshold,		float		);
	CREATE_SHADER_CONSTANT_HANDLE(fHighlightGain,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(bVignetting,				bool		);
	CREATE_SHADER_CONSTANT_HANDLE(fVignOut,					float		);
	CREATE_SHADER_CONSTANT_HANDLE(fVignIn,					float		);
	CREATE_SHADER_CONSTANT_HANDLE(fVignFade,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fChromaShiftAmount,		float		);

	//	- PostProcessingUtils.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(f2LinearDepthEquation,	Vec2f		);
	CREATE_SHADER_CONSTANT_HANDLE(f2DepthHalfTexelOffset,	Vec2f		);

	//	- HUDText.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(f3TextColor,				Vec3f		);

	//	- MotionBlur.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(f44PrevViewProjMat,		Matrix44f	);
	CREATE_SHADER_CONSTANT_HANDLE(fMotionBlurIntensity,		float		);
	CREATE_SHADER_CONSTANT_HANDLE(nMotionBlurNumSamples,	int			);

	//	- SphericalLensFlareFeatures.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(texGhostColorLUT,			sampler1D	);
	CREATE_SHADER_CONSTANT_HANDLE(nGhostSamples,			int			);
	CREATE_SHADER_CONSTANT_HANDLE(fGhostDispersal,			float		);
	CREATE_SHADER_CONSTANT_HANDLE(fGhostRadialWeightExp,	float		);
	CREATE_SHADER_CONSTANT_HANDLE(fHaloSize,				float		);
	CREATE_SHADER_CONSTANT_HANDLE(fHaloRadialWeightExp,		float		);
	CREATE_SHADER_CONSTANT_HANDLE(bChromaShift,				bool		);
	CREATE_SHADER_CONSTANT_HANDLE(fShiftFactor,				float		);

	//	- LensFlareApply.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(texLensFlareFeatures,		sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texLensFlareDirt,			sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(texLensFlareStarBurst,	sampler2D	);
	CREATE_SHADER_CONSTANT_HANDLE(fLensDirtIntensity,		float		);
	CREATE_SHADER_CONSTANT_HANDLE(fLensStarBurstIntensity,	float		);
	CREATE_SHADER_CONSTANT_HANDLE(f33LensFlareStarBurstMat,	Matrix33f	);
	
	//	- BilateralBlur.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(f2BlurDir,				Vec2f		);
	CREATE_SHADER_CONSTANT_HANDLE(fBlurDepthFalloff,		float		);

	//	- NearestDepthUpscale.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(fUpsampleDepthThreshold,	float		);

	//	- AnamorphicLensFlareFeatures.hlsl
	CREATE_SHADER_CONSTANT_HANDLE(fAnamorphicIntensity,		float		);
	//-------------------------------------------------------

	// Used for fullscreen effects, post-processing, etc.
	extern VertexBuffer*	FullScreenTri;

	// Used to set the nBRDFModel shader constant
	enum BRDFModel
	{
		BLINN_PHONG = 0,
		COOK_TORRANCE_GGX,
		COOK_TORRANCE_BECKMANN
	};
}

///////////////////////////////////////////////////////////
#undef CREATE_MODEL_HANDLE
#undef CREATE_TEXTURE_HANDLE
#undef CREATE_SHADER_HANDLE
#undef CREATE_SHADER_CONSTANT_HANDLE
#undef CREATE_RENDER_TARGET_HANDLE
///////////////////////////////////////////////////////////

#endif // APP_RESOURCES_H_
