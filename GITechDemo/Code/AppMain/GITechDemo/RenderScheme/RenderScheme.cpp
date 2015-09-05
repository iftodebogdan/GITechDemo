#include "stdafx.h"

#include "RenderScheme.h"

//////////////////////////
// Some utility defines	//
//////////////////////////
GITechDemoApp::RenderPass GITechDemoApp::RenderScheme::RootPass("Root Pass", nullptr);

#define ROOT_PASS RenderScheme::GetRootPass()

#define IMPLEMENT_CHILD_PASS(Object, Class, Name, Parent) namespace GITechDemoApp { Class Object ( Name, & Parent ); }
///////////////////////////////////////////////////////////

//////////////////////////////
// Start adding passes here	//
//////////////////////////////
#include "ShadowMapDirectionalLightPass.h"
IMPLEMENT_CHILD_PASS(SHADOW_MAP_DIRECTIONAL_LIGHT_PASS, ShadowMapDirectionalLightPass, "Shadow Map Directional Light Pass", ROOT_PASS)

#include "RSMDirectionalLightPass.h"
IMPLEMENT_CHILD_PASS(RSM_DIRECTIONAL_LIGHT_PASS, RSMDirectionalLightPass, "RSM Directional Light Pass", ROOT_PASS)

#include "GBufferPass.h"
IMPLEMENT_CHILD_PASS(GBUFFER_PASS, GBufferPass, "G-Buffer Pass", ROOT_PASS)

#include "LightingPass.h"
IMPLEMENT_CHILD_PASS(LIGHTING_PASS, LightingPass, "Lighting Pass", ROOT_PASS)

#include "SkyPass.h"
IMPLEMENT_CHILD_PASS(SKY_PASS, SkyPass, "Sky Pass", LIGHTING_PASS)

#include "DirectionalLightPass.h"
IMPLEMENT_CHILD_PASS(DIRECTIONAL_LIGHT_PASS, DirectionalLightPass, "Directional Light Pass", LIGHTING_PASS)

#include "DirectionalIndirectLightPass.h"
IMPLEMENT_CHILD_PASS(DIRECTIONAL_INDIRECT_LIGHT_PASS, DirectionalIndirectLightPass, "Directional Indirect Light Pass", LIGHTING_PASS)

#include "PostProcessingPass.h"
IMPLEMENT_CHILD_PASS(POST_PROCESSING_PASS, PostProcessingPass, "Post-Processing Pass", ROOT_PASS)

#include "AmbientOcclusionPass.h"
IMPLEMENT_CHILD_PASS(AMBIENT_OCCLUSION_PASS, AmbientOcclusionPass, "Ambient Occlusion Pass", POST_PROCESSING_PASS)

#include "HDRDownsamplePass.h"
IMPLEMENT_CHILD_PASS(HDR_DOWNSAMPLE_PASS, HDRDownsamplePass, "HDR Downsample Pass", POST_PROCESSING_PASS)

#include "DepthOfFieldPass.h"
IMPLEMENT_CHILD_PASS(DEPTH_OF_FIELD_PASS, DepthOfFieldPass, "Depth of Field Pass", POST_PROCESSING_PASS)

#include "BloomPass.h"
IMPLEMENT_CHILD_PASS(BLOOM_PASS, BloomPass, "Bloom Pass", POST_PROCESSING_PASS)

#include "HDRToneMappingPass.h"
IMPLEMENT_CHILD_PASS(HDR_TONE_MAPPING_PASS, HDRToneMappingPass, "HDR Tone Mapping Pass", POST_PROCESSING_PASS)

#include "FXAAPass.h"
IMPLEMENT_CHILD_PASS(FXAA_PASS, FXAAPass, "FXAA Pass", POST_PROCESSING_PASS)

#include "HUDPass.h"
IMPLEMENT_CHILD_PASS(HUD_PASS, HUDPass, "HUD", ROOT_PASS)
