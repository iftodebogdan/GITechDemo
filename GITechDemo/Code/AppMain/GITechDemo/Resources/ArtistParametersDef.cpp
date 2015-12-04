#include "stdafx.h"

#include "ArtistParameter.h"
#include "RenderResourcesDef.h"
using namespace GITechDemoApp;

//////////////////////////
// Some utility defines	//
//////////////////////////
#define TOKEN_CONCAT2(x, y) x##y
#define TOKEN_CONCAT(x, y) TOKEN_CONCAT2(x, y)
#define CREATE_UNIQUE_TOKEN TOKEN_CONCAT(TOKEN, __COUNTER__)
#define IMPLEMENT_ARTIST_PARAMETER(NAME, DESC, CATEGORY, PARAM, FLT_STEP) ArtistParameter CREATE_UNIQUE_TOKEN (NAME, DESC, CATEGORY, & PARAM, FLT_STEP, typeid(PARAM).hash_code())
///////////////////////////////////////////////////////////

//////////////////////////////////////////
// Start adding artist parameters here	//
//////////////////////////////////////////
namespace GITechDemoApp
{
	//////////////////////////////////////////////////////
	// Setup access to externally declared variables	//
	//////////////////////////////////////////////////////
	// Camera
	extern float CAMERA_MOVE_SPEED;
	extern float CAMERA_SPEED_UP_FACTOR;
	extern float CAMERA_SLOW_DOWN_FACTOR;
	extern float CAMERA_ROTATE_SPEED;
	extern bool CAMERA_ANIMATION_ENABLED;
	extern int CAMERA_ANIMATION_TIMEOUT_SECONDS;

	// G-Buffer
	extern bool GBUFFER_Z_PREPASS;

	// Directional light
	extern bool DIRECTIONAL_LIGHT_ENABLED;
	extern bool DIRECTIONAL_LIGHT_ANIMATION_ENABLED;

	// CSM
	extern bool DEBUG_CSM_CAMERA;
	extern float CASCADE_SPLIT_FACTOR;
	extern float CASCADE_MAX_VIEW_DEPTH;
	extern float DEPTH_BIAS[];
	extern float SLOPE_SCALED_DEPTH_BIAS[];

	// RSM
	extern bool INDIRECT_LIGHT_ENABLED;
	extern bool RSM_USE_QUARTER_RESOLUTION_BUFFER;
	extern bool DEBUG_RSM_CAMERA;

	// Directional light volumetric effect
	extern bool DIR_LIGHT_VOLUME_ENABLE;
	extern bool DIR_LIGHT_VOLUME_QUARTER_RES;
	extern bool DIR_LIGHT_VOLUME_BLUR_SAMPLES;
	extern bool DIR_LIGHT_VOLUME_BLUR_DEPTH_AWARE;
	extern bool DIR_LIGHT_VOLUME_UPSCALE_DEPTH_AWARE;

	// Post-processing
	extern bool POST_PROCESSING_ENABLED;

	// SSAO
	extern bool SSAO_ENABLED;
	extern bool SSAO_USE_QUARTER_RESOLUTION_BUFFER;

	// Bokeh DoF
	extern bool DOF_ENABLED;
	extern bool DOF_USE_QUARTER_RESOLUTION_BUFFER;
	extern float DOF_AUTOFOCUS_TIME;

	// Motion blur
	extern bool MOTION_BLUR_ENABLED;

	// Bloom
	extern bool BLOOM_ENABLED;

	// Lens flare
	extern bool LENS_FLARE_ENABLED;
	extern float LENS_FLARE_BRIGHTNESS_THRESHOLD;

	// Tone mapping
	extern bool HDR_TONE_MAPPING_ENABLED;

	// FXAA
	extern bool FXAA_ENABLED;

	// Fullscreen resolution
	extern bool FULLSCREEN_ENABLED;
	extern int FULLSCREEN_RESOLUTION_X;
	extern int FULLSCREEN_RESOLUTION_Y;
	//------------------------------------------------------

	//////////////////////////////////////////////
	// Parameters that are tunable from the HUD	//
	//////////////////////////////////////////////
	// Camera
	IMPLEMENT_ARTIST_PARAMETER("Z-Near",					"Distance to the nearest Z clip plane",									"Camera",					fZNear.GetCurrentValue(),					0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Z-Far",						"Distance to the furthest Z clip plane",								"Camera",					fZFar.GetCurrentValue(),					10.f);
	IMPLEMENT_ARTIST_PARAMETER("Movement speed",			"Camera movement speed",												"Camera",					CAMERA_MOVE_SPEED,							1.f);
	IMPLEMENT_ARTIST_PARAMETER("Speed up factor",			"Camera speed multiplier when pressing the 'speed up' button",			"Camera",					CAMERA_SPEED_UP_FACTOR,						1.f);
	IMPLEMENT_ARTIST_PARAMETER("Slow down factor",			"Camera speed multiplier when pressing the 'slow down' button",			"Camera",					CAMERA_SLOW_DOWN_FACTOR,					0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Rotation speed",			"Camera rotation speed",												"Camera",					CAMERA_ROTATE_SPEED,						1.f);
	IMPLEMENT_ARTIST_PARAMETER("Camera animation",			"Camera animation toggle (no collisions with world geometry)",			"Camera",					CAMERA_ANIMATION_ENABLED,					1.f);
	IMPLEMENT_ARTIST_PARAMETER("Animation timeout",			"Seconds it takes until the camera animation kicks in",					"Camera",					CAMERA_ANIMATION_TIMEOUT_SECONDS,			1.f);

	// G-Buffer
	IMPLEMENT_ARTIST_PARAMETER("Z-prepass",					"Populate the scene's depth buffer before generating the G-Buffer",		"G-Buffer",					GBUFFER_Z_PREPASS,							1.f);

	// Directional light
	IMPLEMENT_ARTIST_PARAMETER("Directional lights enable",	"Toggle the rendering of directional lights",							"Directional light",		DIRECTIONAL_LIGHT_ENABLED,					1.f);
	IMPLEMENT_ARTIST_PARAMETER("BRDF model",				"0 - Blinn-Phong; 1 - Cook-Torrance GGX; 2 - Cook-Torrance Beckmann",	"Directional light",		nBRDFModel.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Diffuse factor",			"Scale value for diffuse light equation",								"Directional light",		fDiffuseFactor.GetCurrentValue(),			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Specular factor",			"Scale value for specular light equation (Blinn-Phong only)",			"Directional light",		fSpecFactor.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Ambient factor",			"Scale value for ambient light equation",								"Directional light",		fAmbientFactor.GetCurrentValue(),			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Irradiance factor",			"Scale value for irradiance map (Cook-Torrance only)",					"Directional light",		fIrradianceFactor.GetCurrentValue(),		0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Reflection factor",			"Scale value for reflection map (Cook-Torrance only)",					"Directional light",		fReflectionFactor.GetCurrentValue(),		0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Light animation",			"Directional light animation toggle",									"Directional light",		DIRECTIONAL_LIGHT_ANIMATION_ENABLED,		1.f);
	IMPLEMENT_ARTIST_PARAMETER("Light direction - X axis",	"Directional light direction on the X axis of the world",				"Directional light",		f3LightDir.GetCurrentValue()[0],			0.01f);
	IMPLEMENT_ARTIST_PARAMETER("Light direction - Z axis",	"Directional light direction on the Z axis of the world",				"Directional light",		f3LightDir.GetCurrentValue()[2],			0.01f);


	// CSM
	IMPLEMENT_ARTIST_PARAMETER("Debug cascades",			"Draw cascades with different colors",									"Cascaded shadow map",		bDebugCascades.GetCurrentValue(),			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Cascade blend size",		"The size of the blend band between overlapping cascades",				"Cascaded shadow map",		fCascadeBlendSize.GetCurrentValue(),		1.f);
	IMPLEMENT_ARTIST_PARAMETER("CSM distribution",			"Factor affecting the distribution of shadow map cascades",				"Cascaded shadow map",		CASCADE_SPLIT_FACTOR,						0.1f);
	IMPLEMENT_ARTIST_PARAMETER("CSM range",					"Shadow draw distance",													"Cascaded shadow map",		CASCADE_MAX_VIEW_DEPTH,						10.f);
	IMPLEMENT_ARTIST_PARAMETER("Depth bias 1",				"Depth bias for cascade 1",												"Cascaded shadow map",		DEPTH_BIAS[0],								0.0001f);
	IMPLEMENT_ARTIST_PARAMETER("Slope scaled depth bias 1",	"Slope scaled depth bias for cascade 1",								"Cascaded shadow map",		SLOPE_SCALED_DEPTH_BIAS[0],					0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Depth bias 2",				"Depth bias for cascade 2",												"Cascaded shadow map",		DEPTH_BIAS[1],								0.0001f);
	IMPLEMENT_ARTIST_PARAMETER("Slope scaled depth bias 2",	"Slope scaled depth bias for cascade 2",								"Cascaded shadow map",		SLOPE_SCALED_DEPTH_BIAS[1],					0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Depth bias 3",				"Depth bias for cascade 3",												"Cascaded shadow map",		DEPTH_BIAS[2],								0.0001f);
	IMPLEMENT_ARTIST_PARAMETER("Slope scaled depth bias 3",	"Slope scaled depth bias for cascade 3",								"Cascaded shadow map",		SLOPE_SCALED_DEPTH_BIAS[2],					0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Depth bias 4",				"Depth bias for cascade 4",												"Cascaded shadow map",		DEPTH_BIAS[3],								0.0001f);
	IMPLEMENT_ARTIST_PARAMETER("Slope scaled depth bias 4",	"Slope scaled depth bias for cascade 4",								"Cascaded shadow map",		SLOPE_SCALED_DEPTH_BIAS[3],					0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Debug CSM camera",			"Draw the cascaded shadow map on-screen",								"Cascaded shadow map",		DEBUG_CSM_CAMERA,							1.f);
	
	// RSM
	IMPLEMENT_ARTIST_PARAMETER("Indirect lights enable",	"Toggle the rendering of indirect lights",								"Reflective shadow map",	INDIRECT_LIGHT_ENABLED,						1.f);
	IMPLEMENT_ARTIST_PARAMETER("Intensity",					"The intensity of the indirect light",									"Reflective shadow map",	fRSMIntensity.GetCurrentValue(),			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Kernel scale",				"Scale value for the kernel size for sampling the RSM",					"Reflective shadow map",	fRSMKernelScale.GetCurrentValue(),			0.001f);
	IMPLEMENT_ARTIST_PARAMETER("Upscale threshold",			"Affects the number of rejected pixels during upscaling",				"Reflective shadow map",	fWeightThreshold.GetCurrentValue(),			0.01f);
	IMPLEMENT_ARTIST_PARAMETER("Debug upscale pass",		"Draws rejected pixels with a red color",								"Reflective shadow map",	bDebugUpscalePass.GetCurrentValue(),		1.f);
	IMPLEMENT_ARTIST_PARAMETER("Quarter resolution",		"Toggle rendering into a quarter resolution buffer",					"Reflective shadow map",	RSM_USE_QUARTER_RESOLUTION_BUFFER,			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Debug RSM camera",			"Draw the reflective shadow map on-screen",								"Reflective shadow map",	DEBUG_RSM_CAMERA,							1.f);

	// Volumetric lights
	IMPLEMENT_ARTIST_PARAMETER("Volumetric lights enable",	"Toggle the rendering of the volumetric lighting effect (directional)",	"Volumetric lights",		DIR_LIGHT_VOLUME_ENABLE,					1.f);
	IMPLEMENT_ARTIST_PARAMETER("Sample count",				"The number of samples taken across the ray's length",					"Volumetric lights",		nSampleCount.GetCurrentValue(),				10.f);
	IMPLEMENT_ARTIST_PARAMETER("Intensity",					"Intensity of the volumetric effect",									"Volumetric lights",		fLightIntensity.GetCurrentValue(),			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Mult. scatter intensity",	"Intensity of the faked multiple scattering effect",					"Volumetric lights",		fMultScatterIntensity.GetCurrentValue(),	0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Fog vertical falloff",		"Factor for the exponential vertical falloff of the fog effect",		"Volumetric lights",		fFogVerticalFalloff.GetCurrentValue(),		1.f);
	IMPLEMENT_ARTIST_PARAMETER("Fog speed X axis",			"Speed of fog effect on the X axis (world space units / sec)",			"Volumetric lights",		f3FogSpeed.GetCurrentValue()[0],			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Fog speed Y axis",			"Speed of fog effect on the Y axis (world space units / sec)",			"Volumetric lights",		f3FogSpeed.GetCurrentValue()[1],			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Fog speed Z axis",			"Speed of fog effect on the Z axis (world space units / sec)",			"Volumetric lights",		f3FogSpeed.GetCurrentValue()[2],			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Blur samples",				"Toggle the use of an additional blur pass",							"Volumetric lights",		DIR_LIGHT_VOLUME_BLUR_SAMPLES,				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Depth-aware blur",			"Make the blur pass depth-aware so as not to cause light bleeding",		"Volumetric lights",		DIR_LIGHT_VOLUME_BLUR_DEPTH_AWARE,			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Blur depth falloff",		"A scaling factor for the blur weights around edges",					"Volumetric lights",		fBlurDepthFalloff.GetCurrentValue(),		0.0001f);
	IMPLEMENT_ARTIST_PARAMETER("Depth-aware upscaling",		"Make the upscale pass depth-aware so as not to cause artifacts",		"Volumetric lights",		DIR_LIGHT_VOLUME_UPSCALE_DEPTH_AWARE,		1.f);
	IMPLEMENT_ARTIST_PARAMETER("Upsample depth threshold",	"A threshold for edge detection used to reduce upscaling artifacts",	"Volumetric lights",		fUpsampleDepthThreshold.GetCurrentValue(),	0.0001f);
	IMPLEMENT_ARTIST_PARAMETER("Quarter resolution",		"Toggle the use of an intermediary quarter resolution target",			"Volumetric lights",		DIR_LIGHT_VOLUME_QUARTER_RES,				1.f);
	
	// Sky
	IMPLEMENT_ARTIST_PARAMETER("Sun radius",				"Affects the radius of the sun",										"Sky",						fSunRadius.GetCurrentValue(),				10.f);
	IMPLEMENT_ARTIST_PARAMETER("Sun brightness",			"Affects the brightness of the sun",									"Sky",						fSunBrightness.GetCurrentValue(),			10.f);
	
	// Post-processing
	IMPLEMENT_ARTIST_PARAMETER("Post-processing enable",	"Toggle post-processing effects",										"Post-processing effects",	POST_PROCESSING_ENABLED,					1.f);

	// SSAO
	IMPLEMENT_ARTIST_PARAMETER("SSAO enable",				"Toggle the rendering of screen space ambient occlusion",				"SSAO",						SSAO_ENABLED,								1.f);
	IMPLEMENT_ARTIST_PARAMETER("Sample radius",				"Radius in which occluders are searched for",							"SSAO",						fSSAOSampleRadius.GetCurrentValue(),		1.f);
	IMPLEMENT_ARTIST_PARAMETER("Intensity",					"Intensity of SSAO effect",												"SSAO",						fSSAOIntensity.GetCurrentValue(),			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Scale",						"Scale for the occlusion attenuation with distance",					"SSAO",						fSSAOScale.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Bias",						"Bias for the occlusion attenuation with normal differences",			"SSAO",						fSSAOBias.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Quarter resolution",		"Toggle rendering into a quarter resolution buffer",					"SSAO",						SSAO_USE_QUARTER_RESOLUTION_BUFFER,			1.f);
	
	// Bokeh DoF
	IMPLEMENT_ARTIST_PARAMETER("DoF enable",				"Toggle the rendering of the depth of field effect",					"Bokeh DoF",				DOF_ENABLED,								1.f);
	IMPLEMENT_ARTIST_PARAMETER("Focal depth",				"Focal distance value in meters (overridden by autofocus)",				"Bokeh DoF",				fFocalDepth.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Focal length",				"Focal length in mm",													"Bokeh DoF",				fFocalLength.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("F-stop",					"F-stop value",															"Bokeh DoF",				fFStop.GetCurrentValue(),					1.f);
	IMPLEMENT_ARTIST_PARAMETER("Circle of confusion",		"Circle of confusion size in mm (35mm film = 0.03mm)",					"Bokeh DoF",				fCoC.GetCurrentValue(),						0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Near start",				"Near plane DoF blur start (artist friendly parameter)",				"Bokeh DoF",				fNearDofStart.GetCurrentValue(),			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Near falloff",				"Near plane DoF blur falloff distance (artist friendly parameter)",		"Bokeh DoF",				fNearDofFalloff.GetCurrentValue(),			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Far start",					"Far plane DoF blur start (artist friendly parameter)",					"Bokeh DoF",				fFarDofStart.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Far falloff",				"Far plane DoF blur falloff distance (artist friendly parameter)",		"Bokeh DoF",				fFarDofFalloff.GetCurrentValue(),			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Manual DoF",				"Switch between artist friendly and physically based parameters",		"Bokeh DoF",				bManualDof.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Debug focus",				"Show debug auto focus point and focal range",							"Bokeh DoF",				bDebugFocus.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Autofocus",					"Use autofocus",														"Bokeh DoF",				bAutofocus.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Autofocus time",			"Autofocus animation duration in seconds",								"Bokeh DoF",				DOF_AUTOFOCUS_TIME,							1.f);
	IMPLEMENT_ARTIST_PARAMETER("Max blur",					"Blur factor (0.f = no blur, 1.f default)",								"Bokeh DoF",				fMaxBlur.GetCurrentValue(),					0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Highlight threshold",		"Brightness-pass filter threshold (higher = sparser bokeh)",			"Bokeh DoF",				fHighlightThreshold.GetCurrentValue(),		0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Highlight gain",			"Brightness gain (higher = more prominent bokeh)",						"Bokeh DoF",				fHighlightGain.GetCurrentValue(),			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Bokeh bias",				"Bokeh edge bias (shift weights towards edges)",						"Bokeh DoF",				fBokehBias.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Bokeh fringe",				"Bokeh chromatic aberration",											"Bokeh DoF",				fBokehFringe.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Pentagon bokeh",			"Use pentagon as bokeh shape",											"Bokeh DoF",				bPentagonBokeh.GetCurrentValue(),			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Pentagon feather",			"Pentagon edge feathering",												"Bokeh DoF",				fPentagonFeather.GetCurrentValue(),			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Noise",						"Use noise instead of pattern for sample dithering",					"Bokeh DoF",				bUseNoise.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Noise amount",				"Dither amount",														"Bokeh DoF",				fNoiseAmount.GetCurrentValue(),				0.001f);
	IMPLEMENT_ARTIST_PARAMETER("Blur depth",				"Blur the depth buffer for softer edges",								"Bokeh DoF",				bBlurDepth.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Depth blur size",			"Depth blur kernel size",												"Bokeh DoF",				fDepthBlurSize.GetCurrentValue(),			0.001f);
	IMPLEMENT_ARTIST_PARAMETER("Vignetting",				"Optical lens vignetting effect",										"Bokeh DoF",				bVignetting.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Vignetting out",			"Vignetting outer border",												"Bokeh DoF",				fVignOut.GetCurrentValue(),					0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Vignetting in",				"Vignetting inner border",												"Bokeh DoF",				fVignIn.GetCurrentValue(),					0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Vignetting fade",			"F-stops until vignette fades",											"Bokeh DoF",				fVignFade.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Quarter resolution",		"Toggle rendering into a quarter resolution buffer",					"Bokeh DoF",				DOF_USE_QUARTER_RESOLUTION_BUFFER,			1.f);

	// Motion blur
	IMPLEMENT_ARTIST_PARAMETER("Motion blur enable",		"Toggle the rendering of the motion blur effect",						"Motion blur",				MOTION_BLUR_ENABLED,						1.f);
	IMPLEMENT_ARTIST_PARAMETER("Motion blur intensity",		"The intensity of the motion blur effect",								"Motion blur",				fMotionBlurIntensity.GetCurrentValue(),		0.01f);
	IMPLEMENT_ARTIST_PARAMETER("Motion blur sample count",	"The number of samples along the velocity vector",						"Motion blur",				nMotionBlurNumSamples.GetCurrentValue(),	1.f);

	// Bloom
	IMPLEMENT_ARTIST_PARAMETER("Bloom enable",				"Toggle the rendering of the bloom effect",								"Bloom",					BLOOM_ENABLED,								1.f);
	IMPLEMENT_ARTIST_PARAMETER("Brightness threshold",		"Threshold for the low-pass brightness filter",							"Bloom",					fBrightnessThreshold.GetCurrentValue(),		0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Power",						"Exponent of bloom intensity value",									"Bloom",					fBloomPower.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Strength",					"Strength of bloom intensity value",									"Bloom",					fBloomStrength.GetCurrentValue(),			0.1f);

	// Lens flare
	IMPLEMENT_ARTIST_PARAMETER("Lens flare enable",			"Toggle the rendering of the lens flare effect",						"Lens flare",				LENS_FLARE_ENABLED,							1.f);
	IMPLEMENT_ARTIST_PARAMETER("Anamorphic lens flare",		"Choose between anamorphic and spherical lens flare",					"Lens flare",				bAnamorphic.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Anamorphic intensity",		"Adjust the intensity of the anamophic lens flares",					"Lens flare",				fAnamorphicIntensity.GetCurrentValue(),		1.f);
	IMPLEMENT_ARTIST_PARAMETER("Brigthness threshold",		"Brightness threshold for lens flare feature generation",				"Lens flare",				LENS_FLARE_BRIGHTNESS_THRESHOLD,			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Ghost sample count",		"Number of samples for \"ghost\" features",								"Lens flare",				nGhostSamples.GetCurrentValue(),			1.f);
	IMPLEMENT_ARTIST_PARAMETER("Ghost dispersion factor",	"Dispersion factor (distance) for \"ghost\" features",					"Lens flare",				fGhostDispersal.GetCurrentValue(),			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Ghost radial weight",		"Falloff factor for bright spots that are near screen edges",			"Lens flare",				fGhostRadialWeightExp.GetCurrentValue(),	0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Halo size",					"Halo size scale factor",												"Lens flare",				fHaloSize.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Halo radial weight",		"Determines halo thickness",											"Lens flare",				fHaloRadialWeightExp.GetCurrentValue(),		0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Chroma shift enable",		"Toggle the application of a chromatic abberation effect",				"Lens flare",				bChromaShift.GetCurrentValue(),				1.f);
	IMPLEMENT_ARTIST_PARAMETER("Chroma shift factor",		"Intensity of chromatic abberation effect",								"Lens flare",				fShiftFactor.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Lens dirt intensity",		"Scale factor for lens dirt texture samples",							"Lens flare",				fLensDirtIntensity.GetCurrentValue(),		0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Lens star burst intensity",	"Scale factor for lens star burst texture samples",						"Lens flare",				fLensStarBurstIntensity.GetCurrentValue(),	0.1f);

	// Tone mapping
	IMPLEMENT_ARTIST_PARAMETER("Tone mapping enable",		"Toggle tone mapping",													"HDR tone mapping",			HDR_TONE_MAPPING_ENABLED,					1.f);
	IMPLEMENT_ARTIST_PARAMETER("Exposure bias",				"Scales color intensities before tone mapping",							"HDR tone mapping",			fExposureBias.GetCurrentValue(),			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Lower luma bound",			"Minimum average luma clamp used for exposure adjustment",				"HDR tone mapping",			f2AvgLumaClamp.GetCurrentValue()[0],		0.01f);
	IMPLEMENT_ARTIST_PARAMETER("Upper luma bound",			"Maximum average luma clamp used for exposure adjustment",				"HDR tone mapping",			f2AvgLumaClamp.GetCurrentValue()[1],		0.01f);
	IMPLEMENT_ARTIST_PARAMETER("Shoulder strength",			"Strength of the shoulder part of the filmic tone mapping curve",		"HDR tone mapping",			fShoulderStrength.GetCurrentValue(),		0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Linear strength",			"Strength of the linear part of the filmic tone mapping curve",			"HDR tone mapping",			fLinearStrength.GetCurrentValue(),			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Linear angle",				"Angle of the linear part of the filmic tone mapping curve",			"HDR tone mapping",			fLinearAngle.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Toe strength",				"Strength of the toe part of the filmic tone mapping curve",			"HDR tone mapping",			fToeStrength.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Toe numerator",				"Numerator of the toe part of the filmic tone mapping curve",			"HDR tone mapping",			fToeNumerator.GetCurrentValue(),			0.01f);
	IMPLEMENT_ARTIST_PARAMETER("Toe denominator",			"Denominator of the toe part of the filmic tone mapping curve",			"HDR tone mapping",			fToeDenominator.GetCurrentValue(),			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Linear white",				"Reference linear white value of the filmic tone mapping curve",		"HDR tone mapping",			fLinearWhite.GetCurrentValue(),				0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Exposure adapt speed",		"Seconds in which the exposure adapts to scene brightness",				"HDR tone mapping",			fLumaAdaptSpeed.GetCurrentValue(),			0.1f);

	// FXAA
	IMPLEMENT_ARTIST_PARAMETER("FXAA enable",				"Toggle the FXAA filter",												"FXAA",						FXAA_ENABLED,								1.f);
	IMPLEMENT_ARTIST_PARAMETER("Antialiasing factor",		"Amount of sub-pixel aliasing removal",									"FXAA",						fFxaaSubpix.GetCurrentValue(),				0.01f);
	IMPLEMENT_ARTIST_PARAMETER("Edge threshold",			"Minimum amount of local contrast to apply algorithm",					"FXAA",						fFxaaEdgeThreshold.GetCurrentValue(),		0.01f);
	IMPLEMENT_ARTIST_PARAMETER("Darkness threshold",		"Keeps the algorithm from processing darks",							"FXAA",						fFxaaEdgeThresholdMin.GetCurrentValue(),	0.01f);

	// HUD
	IMPLEMENT_ARTIST_PARAMETER("Text color R",				"HUD text color - red component",										"HUD",						f3TextColor.GetCurrentValue()[0],			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Text color G",				"HUD text color - green component",										"HUD",						f3TextColor.GetCurrentValue()[1],			0.1f);
	IMPLEMENT_ARTIST_PARAMETER("Text color B",				"HUD text color - blue component",										"HUD",						f3TextColor.GetCurrentValue()[2],			0.1f);

	// Fullscreen resolution
	IMPLEMENT_ARTIST_PARAMETER("Fullscreen enabled",		"Toggle between window mode and fulscreen mode",						"Window",					FULLSCREEN_ENABLED,							1.f);
	IMPLEMENT_ARTIST_PARAMETER("Resolution X (width)",		"Set the resolution on the X axis (only affects fullscreen mode)",		"Window",					FULLSCREEN_RESOLUTION_X,					1.f);
	IMPLEMENT_ARTIST_PARAMETER("Resolution Y (height)",		"Set the resolution on the Y axis (only affects fullscreen mode)",		"Window",					FULLSCREEN_RESOLUTION_Y,					1.f);
	//------------------------------------------------------
}
