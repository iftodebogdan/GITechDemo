#include "stdafx.h"

#include "RenderResources.h"
using namespace GITechDemoApp;
using namespace LibRendererDll;

//////////////////////////
// Some utility defines	//
//////////////////////////
#define IMPLEMENT_MODEL(NAME, FILEPATH) GITechDemoApp::Model NAME(FILEPATH)

#define IMPLEMENT_TEXTURE(NAME, FILEPATH) GITechDemoApp::Texture NAME(FILEPATH)

#define IMPLEMENT_SHADER(NAME, FILEPATH) GITechDemoApp::Shader NAME(FILEPATH)

#define IMPLEMENT_ARTIST_SHADER_CONSTANT(NAME, TYPE, DEFAULT_VALUE) GITechDemoApp::ShaderConstantTemplate<TYPE> NAME(#NAME, DEFAULT_VALUE)

#define IMPLEMENT_UTILITY_SHADER_CONSTANT(NAME, TYPE) GITechDemoApp::ShaderConstantTemplate<TYPE> NAME(#NAME)

#define IMPLEMENT_RENDER_TARGET(NAME, TARGETCOUNT, RT0, RT1, RT2, RT3, W, H, DEPTH) GITechDemoApp::RenderTarget NAME(#NAME, TARGETCOUNT, RT0, RT1, RT2, RT3, (unsigned int)W, (unsigned int)H, DEPTH)

#define IMPLEMENT_DYNAMIC_RENDER_TARGET(NAME, TARGETCOUNT, RT0, RT1, RT2, RT3, W, H, DEPTH) GITechDemoApp::RenderTarget NAME(#NAME, TARGETCOUNT, RT0, RT1, RT2, RT3, (float)W, (float)H, DEPTH)
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
	// Models	//
	//////////////
	IMPLEMENT_MODEL(SponzaScene,	"models/sponza/sponza.lrm");
	//------------------------------------------------------

	//////////////
	// Textures	//
	//////////////
	IMPLEMENT_TEXTURE(SkyTexture,			"textures/sky.lrt");
	IMPLEMENT_TEXTURE(IrradianceTexture,	"textures/irradiance.lrt");
	IMPLEMENT_TEXTURE(EnvironmentTexture,	"textures/envmap.lrt");
	//------------------------------------------------------

	//////////////////////
	// Render targets	//
	//////////////////////
	// Geometry buffer (GBuffer)
	//	RT0:	A8R8G8B8 with diffuse albedo in RGB and specular power in A
	//	RT1:	G16R16F with compressed normals (stereographic projection)
	//	RT2:	G16R16 with material type (dielectric/metallic) in R and roughness in G
	//	DS:		INTZ (for sampling as a regular texture later)
	IMPLEMENT_DYNAMIC_RENDER_TARGET(GBuffer, 3, PF_A8R8G8B8, PF_G16R16F, PF_G16R16, PF_NONE, 1.f, 1.f, PF_INTZ);

	// Shadow map for the directional light (the dummy color buffer is required because of DX9 limitations
	IMPLEMENT_RENDER_TARGET(ShadowMapDir, 1, PF_A8, PF_NONE, PF_NONE, PF_NONE, SHADOW_MAP_SIZE[0], SHADOW_MAP_SIZE[1], PF_INTZ);

	// Render target in which we accumulate the light contribution from all light sources (also known as the scene color buffer)
	// It contains a regular depth-stencil surface in which we will copy-resolve our INTZ depth texture from the GBuffer
	// (the depth is required for correctly rendering the sky, among other things)
	IMPLEMENT_DYNAMIC_RENDER_TARGET(LightAccumulationBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_D24S8);

	// Reflective Shadow Map render target
	IMPLEMENT_RENDER_TARGET(RSMBuffer, 2, PF_A8R8G8B8, PF_G16R16F, PF_NONE, PF_NONE, RSM_SIZE, RSM_SIZE, PF_INTZ);

	// Indirect lighting accumulation buffer (quarter resolution)
	IMPLEMENT_DYNAMIC_RENDER_TARGET(IndirectLightAccumulationBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);

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
	IMPLEMENT_DYNAMIC_RENDER_TARGET(HDRBloomBuffer0, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.25f, 0.25f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(HDRBloomBuffer1, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.25f, 0.25f, PF_NONE);

	// LDR tone mapped render target
	IMPLEMENT_DYNAMIC_RENDER_TARGET(LDRToneMappedImageBuffer, 1, PF_A8R8G8B8, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);

	// LDR FXAA render target
	IMPLEMENT_DYNAMIC_RENDER_TARGET(LDRFxaaImageBuffer, 1, PF_A8R8G8B8, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);

	// Ambient occlusion render targets
	IMPLEMENT_DYNAMIC_RENDER_TARGET(AmbientOcclusionFullBuffer0, 1, PF_L8, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(AmbientOcclusionFullBuffer1, 1, PF_L8, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(AmbientOcclusionQuarterBuffer0, 1, PF_L8, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(AmbientOcclusionQuarterBuffer1, 1, PF_L8, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);

	// Depth of field render targets
	IMPLEMENT_DYNAMIC_RENDER_TARGET(DepthOfFieldFullBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(DepthOfFieldQuarterBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);
	IMPLEMENT_RENDER_TARGET(AutofocusBuffer0, 1, PF_R32F, PF_NONE, PF_NONE, PF_NONE, 1u, 1u, PF_NONE);
	IMPLEMENT_RENDER_TARGET(AutofocusBuffer1, 1, PF_R32F, PF_NONE, PF_NONE, PF_NONE, 1u, 1u, PF_NONE);

	// Arrays of render targets for easier handling
	RenderTarget* HDRDownsampleBuffer[2] = {
		&GITechDemoApp::HDRDownsampleQuarterBuffer,
		&GITechDemoApp::HDRDownsampleSixteenthBuffer
	};
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
	RenderTarget* AmbientOcclusionFullBuffer[2] = {
		&GITechDemoApp::AmbientOcclusionFullBuffer0,
		&GITechDemoApp::AmbientOcclusionFullBuffer1
	};
	RenderTarget* AmbientOcclusionQuarterBuffer[2] = {
		&GITechDemoApp::AmbientOcclusionQuarterBuffer0,
		&GITechDemoApp::AmbientOcclusionQuarterBuffer1
	};
	RenderTarget* AdaptedLuminance[2] = {
		&GITechDemoApp::AdaptedLuminance0,
		&GITechDemoApp::AdaptedLuminance1
	};
	RenderTarget* AutofocusBuffer[2] = {
		&GITechDemoApp::AutofocusBuffer0,
		&GITechDemoApp::AutofocusBuffer1
	};
	//------------------------------------------------------

	//////////////
	// Shaders	//
	//////////////
	IMPLEMENT_SHADER(SkyBoxShader,				"shaders/SkyBox.hlsl");
	IMPLEMENT_SHADER(GBufferGenerationShader,	"shaders/GBufferGeneration.hlsl");
	IMPLEMENT_SHADER(DeferredLightDirShader,	"shaders/DeferredLightDir.hlsl");
	IMPLEMENT_SHADER(DepthPassShader,			"shaders/DepthPass.hlsl");
	IMPLEMENT_SHADER(DepthCopyShader,			"shaders/DepthCopy.hlsl");
	IMPLEMENT_SHADER(ColorCopyShader,			"shaders/ColorCopy.hlsl");
	IMPLEMENT_SHADER(RSMCaptureShader,			"shaders/RSMCapture.hlsl");
	IMPLEMENT_SHADER(RSMApplyShader,			"shaders/RSMApply.hlsl");
	IMPLEMENT_SHADER(RSMUpscaleShader,			"shaders/RSMUpscale.hlsl");
	IMPLEMENT_SHADER(DownsampleShader,			"shaders/Downsample.hlsl");
	IMPLEMENT_SHADER(LumaCalcShader,			"shaders/LumaCalc.hlsl");
	IMPLEMENT_SHADER(HDRToneMappingShader,		"shaders/HDRToneMapping.hlsl");
	IMPLEMENT_SHADER(BloomShader,				"shaders/Bloom.hlsl");
	IMPLEMENT_SHADER(FxaaShader,				"shaders/FXAA.hlsl");
	IMPLEMENT_SHADER(SsaoShader,				"shaders/SSAO.hlsl");
	IMPLEMENT_SHADER(BokehDofShader,			"shaders/BokehDoF.hlsl");
	//------------------------------------------------------

	//////////////////////////////////////
	// Shader constants - artist driven	//
	//////////////////////////////////////
	/* General parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fZNear,				float,			2.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fZFar,					float,			5000.f					);

	/* Directional light parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fDiffuseFactor,		float,			20.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSpecFactor,			float,			25.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fAmbientFactor,		float,			0.25f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fIrradianceFactor,		float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fReflectionFactor,		float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(nBRDFModel,			unsigned int,	COOK_TORRANCE_BECKMANN	);

	/* Sun parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSunRadius,			float,			1000.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSunBrightness,		float,			500.f					);

	/* Directional light shadow map parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fShadowDepthBias,		float,			0.0025f					);

	/* Cascaded Shadow Map parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bDebugCascades,		bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fCascadeBlendSize,		float,			25.f					);

	/* Reflective Shadow Map parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fRSMIntensity,			float,			300.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fRSMKernelScale,		float,			0.015f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fWeightThreshold,		float,			0.02f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bDebugUpscalePass,		bool,			false					);

	/* Screen-Space Ambient Occlusion */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSSAOSampleRadius,		float,			10.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSSAOIntensity,		float,			5.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSSAOScale,			float,			0.05f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fSSAOBias,				float,			0.25f					);

	/* Postprocessing parameters */
	// Tonemapping
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fExposureBias,			float,			0.15f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(f2AvgLumaClamp,		Vec2f,			Vec2f(0.00001f, 0.25f)	);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fShoulderStrength,		float,			0.15f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLinearStrength,		float,			0.5f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLinearAngle,			float,			0.07f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fToeStrength,			float,			3.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fToeNumerator,			float,			0.02f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fToeDenominator,		float,			0.25f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLinearWhite,			float,			11.2f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fLumaAdaptSpeed,		float,			1.f						);
	// Bloom
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBrightnessThreshold,	float,			0.5f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBloomPower,			float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBloomStrength,		float,			0.25f					);
	// FXAA
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFxaaSubpix,			float,			0.75f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFxaaEdgeThreshold,	float,			0.166f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFxaaEdgeThresholdMin,	float,			0.0833f					);
	// DoF
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFocalDepth,			float,			100.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFocalLength,			float,			75.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFStop,				float,			3.5f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fCoC,					float,			0.02f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fNearDofStart,			float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fNearDofFalloff,		float,			2.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFarDofStart,			float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFarDofFalloff,		float,			3.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bManualDof,			bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bDebugFocus,			bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bAutofocus,			bool,			true					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(f2FocusPoint,			Vec2f,			Vec2f(0.5f, 0.5f)		);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fMaxBlur,				float,			1.5f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fHighlightThreshold,	float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fHighlightGain,		float,			10.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBokehBias,			float,			0.75f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBokehFringe,			float,			2.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bPentagonBokeh,		bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fPentagonFeather,		float,			0.4f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bUseNoise,				bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fNoiseAmount,			float,			0.0001f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bBlurDepth,			bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fDepthBlurSize,		float,			0.001f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bVignetting,			bool,			true					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fVignOut,				float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fVignIn,				float,			0.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fVignFade,				float,			22.f					);

	//////////////////////////////////////
	// Shader constants - code driven	//
	//////////////////////////////////////
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44WorldMat,							Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44ProjMat,							Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44ViewProjMat,						Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44WorldViewProjMat,					Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44WorldViewMat,						Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texDiffuse,							Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texNormal,							Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bHasNormalMap,						bool			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texSpec,								Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bHasSpecMap,							bool			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texMatType,							Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texRoughness,							Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(fSpecIntensity,						float			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2HalfTexelOffset,					Vec2f			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texDepthBuffer,						Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texMaterialBuffer,					Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texIrradianceMap,						SamplerCUBE		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texEnvMap,							SamplerCUBE		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44SkyViewProjMat,					Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texSkyCube,							SamplerCUBE		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texDiffuseBuffer,						Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44InvProjMat,						Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texNormalBuffer,						Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texShadowMap,							Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2OneOverShadowMapSize,				Vec2f			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44ViewMat,							Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44InvViewMat,						Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44InvViewProjMat,					Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44ScreenToLightViewMat,				Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f3LightDir,							Vec3f			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2CascadeBoundsMin,					Vec2f*			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2CascadeBoundsMax,					Vec2f*			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44CascadeProjMat,					Matrix44f*		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLightAccumulationBuffer,			Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2PoissonDisk,						Vec2f*			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44LightViewMat,						Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44InvLightViewMat,					Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44LightWorldViewProjMat,				Matrix44f*		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44RSMWorldViewProjMat,				Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44LightWorldViewMat,					Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44LightRotMat,						Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44LightViewProjMat,					Matrix44f*		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texRSMFluxBuffer,						Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texRSMNormalBuffer,					Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texRSMDepthBuffer,					Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f3RSMKernel,							Vec3f*			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44RSMProjMat,						Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44RSMInvProjMat,						Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44ViewToRSMViewMat,					Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texIndirectLightAccumulationBuffer,	Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLumaCalcInput,						Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bInitialLumaPass,						bool			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bFinalLumaPass,						bool			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texAvgLuma,							Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texSource,							Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2TexelSize,							Vec2f			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bLumaAdaptationPass,					bool			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(fFrameTime,							float			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLumaTarget,						Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bApplyBrightnessFilter,				bool			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(nKernel,								int				);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(nDownsampleFactor,					int				);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bBlurPass,							bool			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2TexSourceSize,						Vec2f			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(bAdjustIntensity,						bool			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f2LinearDepthEquation,				Vec2f			);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texTargetFocus,						Sampler2D		);
	//--------------------------------------------------------------------------
}
