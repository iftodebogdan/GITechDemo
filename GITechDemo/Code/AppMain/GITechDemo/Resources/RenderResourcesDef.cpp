/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	RenderResourcesDef.cpp
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

#include "stdafx.h"

#include "RenderResourcesDef.h"
using namespace GITechDemoApp;
using namespace Synesthesia3D;

//////////////////////////
// Some utility defines	//
//////////////////////////
#define IMPLEMENT_MODEL(NAME, FILEPATH) Model NAME(FILEPATH)
#define IMPLEMENT_TEXTURE(NAME, FILEPATH) Texture NAME(FILEPATH)
#define IMPLEMENT_SHADER(NAME, FILEPATH) Shader NAME(FILEPATH)
#define IMPLEMENT_ARTIST_SHADER_CONSTANT(NAME, TYPE, DEFAULT_VALUE) ShaderConstantTemplate<TYPE> NAME(#NAME, DEFAULT_VALUE)
#define IMPLEMENT_UTILITY_SHADER_CONSTANT(NAME, TYPE) ShaderConstantTemplate<TYPE> NAME(#NAME)
#define IMPLEMENT_RENDER_TARGET(NAME, TARGETCOUNT, RT0, RT1, RT2, RT3, W, H, DEPTH) RenderTarget NAME(#NAME, TARGETCOUNT, RT0, RT1, RT2, RT3, (unsigned int)W, (unsigned int)H, DEPTH)
#define IMPLEMENT_DYNAMIC_RENDER_TARGET(NAME, TARGETCOUNT, RT0, RT1, RT2, RT3, W, H, DEPTH) RenderTarget NAME(#NAME, TARGETCOUNT, RT0, RT1, RT2, RT3, (float)W, (float)H, DEPTH)
///////////////////////////////////////////////////////////

//////////////////////////////////
// Start adding resources here	//
//////////////////////////////////
namespace GITechDemoApp
{
	// Some misc. resources
	extern const Vec<unsigned int, 2> SHADOW_MAP_SIZE;
	extern const unsigned int RSM_SIZE;
	VertexBuffer*	FullScreenTri = nullptr;

	//////////////
	// Shaders	//
	//////////////
	IMPLEMENT_SHADER(BokehDofShader,					"shaders/BokehDoF.hlsl");
	IMPLEMENT_SHADER(RSMUpscaleShader,					"shaders/RSMUpscale.hlsl");
	IMPLEMENT_SHADER(DirectionalLightVolumeShader,		"shaders/DirectionalLightVolume.hlsl");
	IMPLEMENT_SHADER(RSMApplyShader,					"shaders/RSMApply.hlsl");
	IMPLEMENT_SHADER(DirectionalLightShader,			"shaders/DirectionalLight.hlsl");
	IMPLEMENT_SHADER(SphericalLensFlareFeaturesShader,	"shaders/SphericalLensFlareFeatures.hlsl");
	IMPLEMENT_SHADER(SsaoShader,						"shaders/SSAO.hlsl");
	IMPLEMENT_SHADER(BilateralBlurShader,				"shaders/BilateralBlur.hlsl");
	IMPLEMENT_SHADER(FxaaShader,						"shaders/FXAA.hlsl");
	IMPLEMENT_SHADER(AnamorphicLensFlareBlurShader,		"shaders/AnamorphicLensFlareBlur.hlsl");
	IMPLEMENT_SHADER(AnamorphicLensFlareFeaturesShader,	"shaders/AnamorphicLensFlareFeatures.hlsl");
	IMPLEMENT_SHADER(BloomShader,						"shaders/Bloom.hlsl");
	IMPLEMENT_SHADER(DownsampleShader,					"shaders/Downsample.hlsl");
	IMPLEMENT_SHADER(LumaCaptureShader,					"shaders/LumaCapture.hlsl");
	IMPLEMENT_SHADER(NearestDepthUpscaleShader,			"shaders/NearestDepthUpscale.hlsl");
	IMPLEMENT_SHADER(MotionBlurShader,					"shaders/MotionBlur.hlsl");
	IMPLEMENT_SHADER(GBufferGenerationShader,			"shaders/GBufferGeneration.hlsl");
	IMPLEMENT_SHADER(HDRToneMappingShader,				"shaders/HDRToneMapping.hlsl");
	IMPLEMENT_SHADER(LumaAdaptShader,					"shaders/LumaAdapt.hlsl");
	IMPLEMENT_SHADER(ColorCopyShader,					"shaders/ColorCopy.hlsl");
	IMPLEMENT_SHADER(RSMCaptureShader,					"shaders/RSMCapture.hlsl");
	IMPLEMENT_SHADER(DepthPassShader,					"shaders/DepthPass.hlsl");
	IMPLEMENT_SHADER(SkyBoxShader,						"shaders/SkyBox.hlsl");
	IMPLEMENT_SHADER(LensFlareApplyShader,				"shaders/LensFlareApply.hlsl");
	IMPLEMENT_SHADER(HUDTextShader,						"shaders/HUDText.hlsl");
	IMPLEMENT_SHADER(DepthCopyShader,					"shaders/DepthCopy.hlsl");
	//------------------------------------------------------

	//////////////
	// Models	//
	//////////////
	IMPLEMENT_MODEL(SponzaScene,	"models/sponza/sponza.s3dmdl");
	//------------------------------------------------------

	//////////////
	// Textures	//
	//////////////
	IMPLEMENT_TEXTURE(SkyTexture,				"textures/sky.s3dtex");
	IMPLEMENT_TEXTURE(IrradianceTexture,		"textures/irradiance.s3dtex");
	IMPLEMENT_TEXTURE(EnvironmentTexture,		"textures/envmap.s3dtex");
	IMPLEMENT_TEXTURE(LensFlareGhostColorLUT,	"textures/LensFlareGhostColorLUT.s3dtex");
	IMPLEMENT_TEXTURE(LensFlareDirt,			"textures/LensFlareDirt.s3dtex");
	IMPLEMENT_TEXTURE(LensFlareStarBurst,		"textures/LensFlareStarBurst.s3dtex");
	IMPLEMENT_TEXTURE(BayerMatrix,				"textures/bayer_matrix.s3dtex");
	IMPLEMENT_TEXTURE(NoiseTexture,				"textures/noise.s3dtex");
	//------------------------------------------------------

	//////////////////////
	// Render targets	//
	//////////////////////
	// Geometry buffer (G-Buffer)
	//	RT0:	A8R8G8B8 with diffuse albedo in RGB and specular power in A
	//	RT1:	G16R16F with compressed normals (stereographic projection)
	//	RT2:	G16R16 with material type (dielectric/metallic) in R and roughness in G (Cook-Torrance only)
	//	DS:		INTZ (for sampling as a regular texture later)
	IMPLEMENT_DYNAMIC_RENDER_TARGET(GBuffer, 3, PF_A8R8G8B8, PF_G16R16F, PF_G16R16, PF_NONE, 1.f, 1.f, PF_INTZ);

	// Shadow map for the directional light (the dummy color buffer is required because of DX9 limitations
	IMPLEMENT_RENDER_TARGET(ShadowMapDir, 1, PF_NONE, PF_NONE, PF_NONE, PF_NONE, SHADOW_MAP_SIZE[0], SHADOW_MAP_SIZE[1], PF_INTZ);

	// Render target in which we accumulate the light contribution from all light sources (also known as the scene color buffer)
	// It contains a regular depth-stencil surface in which we will copy-resolve our INTZ depth texture from the G-Buffer
	// (the depth is required for correctly rendering the sky, among other things)
	IMPLEMENT_DYNAMIC_RENDER_TARGET(LightAccumulationBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_D24S8);

	// Reflective Shadow Map render target
	IMPLEMENT_RENDER_TARGET(RSMBuffer, 2, PF_A8R8G8B8, PF_G16R16F, PF_NONE, PF_NONE, RSM_SIZE, RSM_SIZE, PF_INTZ);

	// Indirect lighting accumulation buffer (quarter resolution)
	IMPLEMENT_DYNAMIC_RENDER_TARGET(IndirectLightAccumulationBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);

	// Volumetric light accumulation buffer
	IMPLEMENT_DYNAMIC_RENDER_TARGET(VolumetricLightFullBuffer0, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(VolumetricLightFullBuffer1, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(VolumetricLightQuarterBuffer0, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(VolumetricLightQuarterBuffer1, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);

	// HDR downsampled buffer (1/4 and 1/16 resolution)
	IMPLEMENT_DYNAMIC_RENDER_TARGET(HDRDownsampleQuarterBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(HDRDownsampleSixteenthBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.25f, 0.25f, PF_NONE);

	// Average luminance buffers (64x64, 16x16, 4x4, two 1x1, one target luma, one current luma)
	IMPLEMENT_RENDER_TARGET(AverageLuminanceBuffer0, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 64u, 64u, PF_NONE);
	IMPLEMENT_RENDER_TARGET(AverageLuminanceBuffer1, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 16u, 16u, PF_NONE);
	IMPLEMENT_RENDER_TARGET(AverageLuminanceBuffer2, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 4u, 4u, PF_NONE);
	IMPLEMENT_RENDER_TARGET(AverageLuminanceBuffer3, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 1u, 1u, PF_NONE);

	// Adapted luminance for simulating light adaptation effect
	IMPLEMENT_RENDER_TARGET(AdaptedLuminance0, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 1u, 1u, PF_NONE);
	IMPLEMENT_RENDER_TARGET(AdaptedLuminance1, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 1u, 1u, PF_NONE);

	// HDR bloom render targets
	IMPLEMENT_DYNAMIC_RENDER_TARGET(BloomBuffer0, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.25f, 0.25f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(BloomBuffer1, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.25f, 0.25f, PF_NONE);

	// LDR tone mapped render target
	IMPLEMENT_DYNAMIC_RENDER_TARGET(LDRToneMappedImageBuffer, 1, PF_A8R8G8B8, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);

	// LDR FXAA render target
	IMPLEMENT_DYNAMIC_RENDER_TARGET(LDRFxaaImageBuffer, 1, PF_A8R8G8B8, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);

	// Ambient occlusion render targets
	IMPLEMENT_DYNAMIC_RENDER_TARGET(SSAOFullBuffer0, 1, PF_L8, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(SSAOFullBuffer1, 1, PF_L8, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(SSAOQuarterBuffer0, 1, PF_L8, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(SSAOQuarterBuffer1, 1, PF_L8, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);

	// Depth of field render targets
	IMPLEMENT_DYNAMIC_RENDER_TARGET(DepthOfFieldFullBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(DepthOfFieldQuarterBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);
	IMPLEMENT_RENDER_TARGET(AutofocusBuffer0, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 1u, 1u, PF_NONE);
	IMPLEMENT_RENDER_TARGET(AutofocusBuffer1, 1, PF_R16F, PF_NONE, PF_NONE, PF_NONE, 1u, 1u, PF_NONE);

	// Motion blur render target
	IMPLEMENT_DYNAMIC_RENDER_TARGET(MotionBlurBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);

	// Lens flare render targets
	IMPLEMENT_DYNAMIC_RENDER_TARGET(SphericalLensFlareBuffer0, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(SphericalLensFlareBuffer1, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(AnamorphicLensFlareBuffer0, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.03125f, 0.5f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(AnamorphicLensFlareBuffer1, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.03125f, 0.5f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(AnamorphicLensFlareBuffer2, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.25f, 0.25f, PF_NONE);
	
	// Arrays of render targets for easier handling
	RenderTarget* VolumetricLightFullBuffer[2] = {
		&VolumetricLightFullBuffer0,
		&VolumetricLightFullBuffer1
	};

	RenderTarget* VolumetricLightQuarterBuffer[2] = {
		&VolumetricLightQuarterBuffer0,
		&VolumetricLightQuarterBuffer1
	};

	RenderTarget* HDRDownsampleBuffer[2] = {
		&HDRDownsampleQuarterBuffer,
		&HDRDownsampleSixteenthBuffer
	};
	RenderTarget* AverageLuminanceBuffer[4] = {
		&AverageLuminanceBuffer0,
		&AverageLuminanceBuffer1,
		&AverageLuminanceBuffer2,
		&AverageLuminanceBuffer3
	};
	RenderTarget* BloomBuffer[2] = {
		&BloomBuffer0,
		&BloomBuffer1
	};
	RenderTarget* SSAOFullBuffer[2] = {
		&SSAOFullBuffer0,
		&SSAOFullBuffer1
	};
	RenderTarget* SSAOQuarterBuffer[2] = {
		&SSAOQuarterBuffer0,
		&SSAOQuarterBuffer1
	};
	RenderTarget* AdaptedLuminance[2] = {
		&AdaptedLuminance0,
		&AdaptedLuminance1
	};
	RenderTarget* AutofocusBuffer[2] = {
		&AutofocusBuffer0,
		&AutofocusBuffer1
	};
	RenderTarget* SphericalLensFlareBuffer[2] = {
		&SphericalLensFlareBuffer0,
		&SphericalLensFlareBuffer1
	};
	RenderTarget* AnamorphicLensFlareBuffer[3] = {
		&AnamorphicLensFlareBuffer0,
		&AnamorphicLensFlareBuffer1,
		&AnamorphicLensFlareBuffer2
	};
	//------------------------------------------------------

	//////////////////////////////////////
	// Shader constants - artist driven	//
	//////////////////////////////////////
	/* General parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fZNear,					float,			10.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fZFar,						float,			5000.f					);

	/* Directional light parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fDiffuseFactor,			float,			20.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSpecFactor,				float,			25.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fAmbientFactor,			float,			0.25f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fIrradianceFactor,			float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fReflectionFactor,			float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(nBRDFModel,				int,			COOK_TORRANCE_BECKMANN	);

	/* Volumetric directional light parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(nSampleCount,				int,			32						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLightIntensity,			float,			0.5f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fMultScatterIntensity,		float,			0.1f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(f3FogSpeed,				Vec3f,			Vec3f(20.f, -10.f, 20.f));
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFogVerticalFalloff,		float,			25.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBlurDepthFalloff,			float,			0.0025f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fUpsampleDepthThreshold,	float,			0.0015f					);

	/* Sun parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSunRadius,				float,			1000.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSunBrightness,			float,			500.f					);

	/* Cascaded Shadow Map parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bDebugCascades,			bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fCascadeBlendSize,			float,			50.f					);

	/* Reflective Shadow Map parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fRSMIntensity,				float,			500.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fRSMKernelScale,			float,			0.025f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fWeightThreshold,			float,			0.002f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bDebugUpscalePass,			bool,			false					);

	/* Screen-Space Ambient Occlusion */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSSAOSampleRadius,			float,			10.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSSAOIntensity,			float,			5.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSSAOScale,				float,			0.05f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSSAOBias,					float,			0.25f					);

	/* Post-processing parameters */
	// Tone mapping
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fExposureBias,				float,			0.2f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(f2AvgLumaClamp,			Vec2f,			Vec2f(0.00001f, 0.25f)	);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fShoulderStrength,			float,			0.15f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLinearStrength,			float,			0.5f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLinearAngle,				float,			0.07f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fToeStrength,				float,			3.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fToeNumerator,				float,			0.02f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fToeDenominator,			float,			0.25f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLinearWhite,				float,			11.2f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLumaAdaptSpeed,			float,			1.f						);
	// Bloom
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBrightnessThreshold,		float,			0.6f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBloomPower,				float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBloomStrength,			float,			0.75f					);
	// FXAA
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFxaaSubpix,				float,			0.75f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFxaaEdgeThreshold,		float,			0.166f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFxaaEdgeThresholdMin,		float,			0.0833f					);
	// DoF
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFocalDepth,				float,			100.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFocalLength,				float,			75.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFStop,					float,			2.8f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fCoC,						float,			0.03f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fNearDofStart,				float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fNearDofFalloff,			float,			2.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFarDofStart,				float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFarDofFalloff,			float,			3.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bManualDof,				bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bDebugFocus,				bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bAutofocus,				bool,			true					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fMaxBlur,					float,			2.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fHighlightThreshold,		float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fHighlightGain,			float,			10.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBokehBias,				float,			0.75f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBokehFringe,				float,			2.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bPentagonBokeh,			bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fPentagonFeather,			float,			0.4f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bUseNoise,					bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fNoiseAmount,				float,			0.0001f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bBlurDepth,				bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fDepthBlurSize,			float,			0.001f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bVignetting,				bool,			true					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fVignOut,					float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fVignIn,					float,			0.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fVignFade,					float,			22.f					);
	// Motion blur
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fMotionBlurIntensity,		float,			0.01f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(nMotionBlurNumSamples,		int,			5						);
	// Lens flare
	IMPLEMENT_ARTIST_SHADER_CONSTANT(nGhostSamples,				int,			5						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fGhostDispersal,			float,			0.37f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fGhostRadialWeightExp,		float,			1.5f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fHaloSize,					float,			0.6f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fHaloRadialWeightExp,		float,			5.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bChromaShift,				bool,			true					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fShiftFactor,				float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLensDirtIntensity,		float,			0.3f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLensStarBurstIntensity,	float,			0.5f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fAnamorphicIntensity,		float,			5.f						);

	/* HUD parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(f3TextColor,				Vec3f,			Vec3f(1.f, 1.f, 1.f)	);
	//------------------------------------------------------

	//////////////////////////////////////
	// Shader constants - code driven	//
	//////////////////////////////////////
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44WorldMat,					Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44ProjMat,					Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44ViewProjMat,				Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44WorldViewProjMat,			Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44WorldViewMat,				Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texDiffuse,					sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texNormal,					sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bHasNormalMap,				bool		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texSpec,						sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bHasSpecMap,					bool		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texMatType,					sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texRoughness,					sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(fSpecIntensity,				float		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2HalfTexelOffset,			Vec2f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texDepthBuffer,				sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texMaterialBuffer,			sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texIrradianceMap,				samplerCUBE	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texEnvMap,					samplerCUBE	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44SkyViewProjMat,			Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texSkyCube,					samplerCUBE	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texDiffuseBuffer,				sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44InvProjMat,				Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texNormalBuffer,				sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texShadowMap,					sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2OneOverShadowMapSize,		Vec2f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44ViewMat,					Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44InvViewMat,				Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44InvViewProjMat,			Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44ScreenToLightViewMat,		Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f3LightDir,					Vec3f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2CascadeBoundsMin,			Vec2f*		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2CascadeBoundsMax,			Vec2f*		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44CascadeProjMat,			Matrix44f*	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2PoissonDisk,				Vec2f*		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44LightViewMat,				Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44InvLightViewMat,			Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44LightWorldViewProjMat,		Matrix44f*	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44RSMWorldViewProjMat,		Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44LightWorldViewMat,			Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44LightViewProjMat,			Matrix44f*	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texRSMFluxBuffer,				sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texRSMNormalBuffer,			sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texRSMDepthBuffer,			sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f3RSMKernel,					Vec3f*		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44RSMProjMat,				Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44RSMInvProjMat,				Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44ViewToRSMViewMat,			Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLumaInput,					sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bInitialLumaPass,				bool		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bFinalLumaPass,				bool		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texAvgLuma,					sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texSource,					sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2TexelSize,					Vec2f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(fFrameTime,					float		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLumaTarget,				sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bApplyBrightnessFilter,		bool		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(nKernel,						int			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(nDownsampleFactor,			int			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2TexSize,					Vec2f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bAdjustIntensity,				bool		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2LinearDepthEquation,		Vec2f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texTargetFocus,				sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44PrevViewProjMat,			Matrix44f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texGhostColorLUT,				sampler1D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLensFlareFeatures,			sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLensFlareDirt,				sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLensFlareStarBurst,		sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f33LensFlareStarBurstMat,		Matrix33f	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bSingleChannelCopy,			bool		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f3CameraPositionLightVS,		Vec3f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(fRaymarchDistanceLimit,		float		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texDitherMap,					sampler2D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2BlurDir,					Vec2f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2DepthHalfTexelOffset,		Vec2f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texNoise,						sampler3D	);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(fElapsedTime,					float		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f3FogBox,						Vec3f		);
	//--------------------------------------------------------------------------
}
