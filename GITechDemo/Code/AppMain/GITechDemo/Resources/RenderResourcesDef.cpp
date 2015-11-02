#include "stdafx.h"

#include "RenderResourcesDef.h"
using namespace GITechDemoApp;
using namespace LibRendererDll;

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
	IMPLEMENT_SHADER(BokehDofShader, "shaders/BokehDoF.hlsl");
	IMPLEMENT_SHADER(RSMUpscaleShader, "shaders/RSMUpscale.hlsl");
	IMPLEMENT_SHADER(RSMApplyShader, "shaders/RSMApply.hlsl");
	IMPLEMENT_SHADER(DeferredLightDirShader, "shaders/DeferredLightDir.hlsl");
	IMPLEMENT_SHADER(SkyBoxShader, "shaders/SkyBox.hlsl");
	IMPLEMENT_SHADER(GBufferGenerationShader, "shaders/GBufferGeneration.hlsl");
	IMPLEMENT_SHADER(DepthPassShader, "shaders/DepthPass.hlsl");
	IMPLEMENT_SHADER(DepthCopyShader, "shaders/DepthCopy.hlsl");
	IMPLEMENT_SHADER(ColorCopyShader, "shaders/ColorCopy.hlsl");
	IMPLEMENT_SHADER(RSMCaptureShader, "shaders/RSMCapture.hlsl");
	IMPLEMENT_SHADER(DownsampleShader, "shaders/Downsample.hlsl");
	IMPLEMENT_SHADER(LumaCalcShader, "shaders/LumaCalc.hlsl");
	IMPLEMENT_SHADER(HDRToneMappingShader, "shaders/HDRToneMapping.hlsl");
	IMPLEMENT_SHADER(BloomShader, "shaders/Bloom.hlsl");
	IMPLEMENT_SHADER(FxaaShader, "shaders/FXAA.hlsl");
	IMPLEMENT_SHADER(SsaoShader, "shaders/SSAO.hlsl");
	IMPLEMENT_SHADER(HUDTextShader, "shaders/HUDText.hlsl");
	IMPLEMENT_SHADER(MotionBlurShader, "shaders/MotionBlur.hlsl");
	IMPLEMENT_SHADER(LensFlareFeaturesShader, "shaders/LensFlareFeatures.hlsl");
	IMPLEMENT_SHADER(LensFlareApplyShader, "shaders/LensFlareApply.hlsl");
	//------------------------------------------------------

	//////////////
	// Models	//
	//////////////
	IMPLEMENT_MODEL(SponzaScene,	"models/sponza/sponza.lrm");
	//------------------------------------------------------

	//////////////
	// Textures	//
	//////////////
	IMPLEMENT_TEXTURE(SkyTexture,				"textures/sky.lrt");
	IMPLEMENT_TEXTURE(IrradianceTexture,		"textures/irradiance.lrt");
	IMPLEMENT_TEXTURE(EnvironmentTexture,		"textures/envmap.lrt");
	IMPLEMENT_TEXTURE(LensFlareGhostColorLUT,	"textures/LensFlareGhostColorLUT.lrt");
	IMPLEMENT_TEXTURE(LensFlareDirt,			"textures/LensFlareDirt.lrt");
	IMPLEMENT_TEXTURE(LensFlareStarBurst,		"textures/LensFlareStarBurst.lrt");
	//------------------------------------------------------

	//////////////////////
	// Render targets	//
	//////////////////////
	// Geometry buffer (G-Buffer)
	//	RT0:	A8R8G8B8 with diffuse albedo in RGB and specular power in A
	//	RT1:	G16R16F with compressed normals (stereographic projection)
	//	RT2:	G16R16 with material type (dielectric/metallic) in R and roughness in G
	//	DS:		INTZ (for sampling as a regular texture later)
	IMPLEMENT_DYNAMIC_RENDER_TARGET(GBuffer, 3, PF_A8R8G8B8, PF_G16R16F, PF_G16R16, PF_NONE, 1.f, 1.f, PF_INTZ);

	// Shadow map for the directional light (the dummy color buffer is required because of DX9 limitations
	IMPLEMENT_RENDER_TARGET(ShadowMapDir, 1, PF_A8, PF_NONE, PF_NONE, PF_NONE, SHADOW_MAP_SIZE[0], SHADOW_MAP_SIZE[1], PF_INTZ);

	// Render target in which we accumulate the light contribution from all light sources (also known as the scene color buffer)
	// It contains a regular depth-stencil surface in which we will copy-resolve our INTZ depth texture from the G-Buffer
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
	IMPLEMENT_DYNAMIC_RENDER_TARGET(BloomBuffer0, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.25f, 0.25f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(BloomBuffer1, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.25f, 0.25f, PF_NONE);

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

	// Motion blur render target
	IMPLEMENT_DYNAMIC_RENDER_TARGET(MotionBlurBuffer, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 1.f, 1.f, PF_NONE);

	// Lens flare render targets
	IMPLEMENT_DYNAMIC_RENDER_TARGET(LensFlareBuffer0, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);
	IMPLEMENT_DYNAMIC_RENDER_TARGET(LensFlareBuffer1, 1, PF_A16B16G16R16F, PF_NONE, PF_NONE, PF_NONE, 0.5f, 0.5f, PF_NONE);
	
	// Arrays of render targets for easier handling
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
	RenderTarget* AmbientOcclusionFullBuffer[2] = {
		&AmbientOcclusionFullBuffer0,
		&AmbientOcclusionFullBuffer1
	};
	RenderTarget* AmbientOcclusionQuarterBuffer[2] = {
		&AmbientOcclusionQuarterBuffer0,
		&AmbientOcclusionQuarterBuffer1
	};
	RenderTarget* AdaptedLuminance[2] = {
		&AdaptedLuminance0,
		&AdaptedLuminance1
	};
	RenderTarget* AutofocusBuffer[2] = {
		&AutofocusBuffer0,
		&AutofocusBuffer1
	};
	RenderTarget* LensFlareBuffer[2] = {
		&LensFlareBuffer0,
		&LensFlareBuffer1
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
	IMPLEMENT_ARTIST_SHADER_CONSTANT(nBRDFModel,				unsigned int,	COOK_TORRANCE_BECKMANN	);

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
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fExposureBias,				float,			0.15f					);
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
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBrightnessThreshold,		float,			0.5f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBloomPower,				float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fBloomStrength,			float,			0.75f					);
	// FXAA
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFxaaSubpix,				float,			0.75f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFxaaEdgeThreshold,		float,			0.166f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFxaaEdgeThresholdMin,		float,			0.0833f					);
	// DoF
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFocalDepth,				float,			100.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFocalLength,				float,			75.f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFStop,					float,			3.5f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fCoC,						float,			0.02f					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fNearDofStart,				float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fNearDofFalloff,			float,			2.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFarDofStart,				float,			1.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fFarDofFalloff,			float,			3.f						);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bManualDof,				bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bDebugFocus,				bool,			false					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(bAutofocus,				bool,			true					);
	IMPLEMENT_ARTIST_SHADER_CONSTANT(fMaxBlur,					float,			1.5f					);
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

	/* HUD parameters */
	IMPLEMENT_ARTIST_SHADER_CONSTANT(f3TextColor,			Vec3f,			Vec3f(1.f, 1.f, 1.f)	);
	//------------------------------------------------------

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
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f44PrevViewProjMat,					Matrix44f		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texGhostColorLUT,						Sampler1D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLensFlareFeatures,					Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLensFlareDirt,						Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(texLensFlareStarBurst,				Sampler2D		);
	IMPLEMENT_UTILITY_SHADER_CONSTANT(f33LensFlareStarBurstMat,				Matrix33f		);
	//--------------------------------------------------------------------------
}
