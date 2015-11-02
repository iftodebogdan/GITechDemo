#include "stdafx.h"

#include "RenderScheme.h"

//////////////////////////
// Some useful defines	//
//////////////////////////
#define CREATE_ROOT_PASS() GITechDemoApp::RenderPass GITechDemoApp::RenderScheme::RootPass("Root Pass", nullptr);
#define ROOT_PASS RenderScheme::GetRootPass()
#define ADD_RENDER_PASS(Object, Class, Name, Parent) namespace GITechDemoApp { Class Object ( Name, & Parent ); }
///////////////////////////////////////////////////////////



//////////////////////////////
// Start adding passes here	//
//////////////////////////////

// The root pass (mandatory)
CREATE_ROOT_PASS()

	// Render the shadow map cascades for the directional light
	#include "ShadowMapDirectionalLightPass.h"
	ADD_RENDER_PASS(SHADOW_MAP_DIRECTIONAL_LIGHT_PASS, ShadowMapDirectionalLightPass, "Shadow Map Directional Light Pass", ROOT_PASS)

	// Render the reflective shadow map for the directional light
	#include "RSMDirectionalLightPass.h"
	ADD_RENDER_PASS(RSM_DIRECTIONAL_LIGHT_PASS, RSMDirectionalLightPass, "RSM Directional Light Pass", ROOT_PASS)

	// Generate the geometry buffer
	#include "GBufferPass.h"
	ADD_RENDER_PASS(GBUFFER_PASS, GBufferPass, "G-Buffer Pass", ROOT_PASS)

	// Compute scene lighting
	#include "LightingPass.h"
	ADD_RENDER_PASS(LIGHTING_PASS, LightingPass, "Lighting Pass", ROOT_PASS)

		// Render the sky box
		#include "SkyPass.h"
		ADD_RENDER_PASS(SKY_PASS, SkyPass, "Sky Pass", LIGHTING_PASS)

		// Compute direct light contribution from the directional light
		#include "DirectionalLightPass.h"
		ADD_RENDER_PASS(DIRECTIONAL_LIGHT_PASS, DirectionalLightPass, "Directional Light Pass", LIGHTING_PASS)

		// Compute indirect (1 unoccluded bounce) light contribution from the directional light
		#include "DirectionalIndirectLightPass.h"
		ADD_RENDER_PASS(DIRECTIONAL_INDIRECT_LIGHT_PASS, DirectionalIndirectLightPass, "Directional Indirect Light Pass", LIGHTING_PASS)

	// Apply post-processing effect chain
	#include "PostProcessingPass.h"
	ADD_RENDER_PASS(POST_PROCESSING_PASS, PostProcessingPass, "Post-Processing Pass", ROOT_PASS)

		// Screen space ambient occlusion
		#include "AmbientOcclusionPass.h"
		ADD_RENDER_PASS(AMBIENT_OCCLUSION_PASS, AmbientOcclusionPass, "Ambient Occlusion Pass", POST_PROCESSING_PASS)

		// HDR framebuffer downsampling (1/4 and 1/16) for bloom, tone mapping, etc.
		#include "HDRDownsamplePass.h"
		ADD_RENDER_PASS(HDR_DOWNSAMPLE_PASS, HDRDownsamplePass, "HDR Downsample Pass", POST_PROCESSING_PASS)

		// Depth of field effect with bokeh (dynamic branching, pixel shader only)
		#include "DepthOfFieldPass.h"
		ADD_RENDER_PASS(DEPTH_OF_FIELD_PASS, DepthOfFieldPass, "Depth of Field Pass", POST_PROCESSING_PASS)

		// Motion blur effect
		#include "MotionBlurPass.h"
		ADD_RENDER_PASS(MOTION_BLUR_PASS, MotionBlurPass, "Motion Blur Pass", POST_PROCESSING_PASS)

		// Bloom (light bleeding) effect
		#include "BloomPass.h"
		ADD_RENDER_PASS(BLOOM_PASS, BloomPass, "Bloom Pass", POST_PROCESSING_PASS)

		// Lens flare effect
		#include "LensFlarePass.h"
		ADD_RENDER_PASS(LENS_FLARE_PASS, LensFlarePass, "Lens Flare Pass", POST_PROCESSING_PASS)

		// Tone mapping (HDR linear space to LDR gamma space conversion)
		#include "HDRToneMappingPass.h"
		ADD_RENDER_PASS(HDR_TONE_MAPPING_PASS, HDRToneMappingPass, "HDR Tone Mapping Pass", POST_PROCESSING_PASS)

		// Fast approximate anti-aliasing
		#include "FXAAPass.h"
		ADD_RENDER_PASS(FXAA_PASS, FXAAPass, "FXAA Pass", POST_PROCESSING_PASS)

	// Head-up display
	#include "HUDPass.h"
	ADD_RENDER_PASS(HUD_PASS, HUDPass, "HUD", ROOT_PASS)
