/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   RenderScheme.cpp
 *      Author: Bogdan Iftode
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "stdafx.h"

#include "RenderScheme.h"

//////////////////////////
// Some useful defines  //
//////////////////////////
#define CREATE_ROOT_PASS() GITechDemoApp::RenderPass GITechDemoApp::RenderScheme::RootPass("Frame", nullptr);
#define ROOT_PASS RenderScheme::GetRootPass()
#define ADD_RENDER_PASS(Object, Class, Name, Parent) namespace GITechDemoApp { Class Object ( Name, & Parent ); }
///////////////////////////////////////////////////////////



//////////////////////////////
// Start adding passes here //
//////////////////////////////

// The root pass (mandatory)
CREATE_ROOT_PASS()

    // Render the shadow map cascades for the directional light
    #include "ShadowMapDirectionalLightPass.h"
    ADD_RENDER_PASS(SHADOW_MAP_DIRECTIONAL_LIGHT_PASS, ShadowMapDirectionalLightPass, "Shadow Map for Directional Light", ROOT_PASS)

    // Render the reflective shadow map for the directional light
    #include "RSMDirectionalLightPass.h"
    ADD_RENDER_PASS(RSM_DIRECTIONAL_LIGHT_PASS, RSMDirectionalLightPass, "Reflective Shadow Map for Directional Light", ROOT_PASS)

    // Generate the geometry buffer
    #include "GBufferPass.h"
    ADD_RENDER_PASS(GBUFFER_PASS, GBufferPass, "G-Buffer", ROOT_PASS)

    // Compute scene lighting
    #include "LightingPass.h"
    ADD_RENDER_PASS(LIGHTING_PASS, LightingPass, "Lighting", ROOT_PASS)

        // Compute direct light contribution from the directional light
        #include "DirectionalLightPass.h"
        ADD_RENDER_PASS(DIRECTIONAL_LIGHT_PASS, DirectionalLightPass, "Directional Light", LIGHTING_PASS)
    
        // Screen space ambient occlusion (done here so as not to affect indirect and volumetric lights)
        #include "SSAOPass.h"
        ADD_RENDER_PASS(SSAO_PASS, SSAOPass, "SSAO", LIGHTING_PASS)
    
        // Render the sky box (moved after SSAO so as to avoid halo artifacts on the sky)
        #include "SkyPass.h"
        ADD_RENDER_PASS(SKY_PASS, SkyPass, "Sky", LIGHTING_PASS)

        // Compute indirect light contribution from the directional light (1 unoccluded bounce)
        #include "DirectionalIndirectLightPass.h"
        ADD_RENDER_PASS(DIRECTIONAL_INDIRECT_LIGHT_PASS, DirectionalIndirectLightPass, "Indirect Light", LIGHTING_PASS)
    
        // Screen space reflection
        #include "ScreenSpaceReflectionPass.h"
        ADD_RENDER_PASS(SCREEN_SPACE_REFLECTION_PASS, ScreenSpaceReflectionPass, "Screen Space Reflection", LIGHTING_PASS)

        // Downsample the light accumulation buffer here, because we don't want the volumetric light to be bloomed
        #include "HDRDownsampleForBloomPass.h"
        ADD_RENDER_PASS(HDR_DOWNSAMPLE_FOR_BLOOM_PASS, HDRDownsampleForBloomPass, "HDR Downsample for Bloom", LIGHTING_PASS)

        // Volumetric directional light
        #include "DirectionalLightVolumePass.h"
        ADD_RENDER_PASS(DIRECTIONAL_LIGHT_VOLUME_PASS, DirectionalLightVolumePass, "Directional Light Volume", LIGHTING_PASS)

    // Apply post-processing effect chain
    #include "PostProcessingPass.h"
    ADD_RENDER_PASS(POST_PROCESSING_PASS, PostProcessingPass, "Post-Processing", ROOT_PASS)

        // HDR framebuffer downsampling (1/4 and 1/16) for tone mapping, etc.
        #include "HDRDownsamplePass.h"
        ADD_RENDER_PASS(HDR_DOWNSAMPLE_PASS, HDRDownsamplePass, "HDR Downsample", POST_PROCESSING_PASS)

        // Depth of field with bokeh, vignetting, chromatic aberration and lens distortion effects
        #include "DepthOfFieldPass.h"
        ADD_RENDER_PASS(DEPTH_OF_FIELD_PASS, DepthOfFieldPass, "Depth of Field", POST_PROCESSING_PASS)

        // Motion blur effect
        #include "MotionBlurPass.h"
        ADD_RENDER_PASS(MOTION_BLUR_PASS, MotionBlurPass, "Motion Blur", POST_PROCESSING_PASS)

        // Bloom (light bleeding) effect
        #include "BloomPass.h"
        ADD_RENDER_PASS(BLOOM_PASS, BloomPass, "Bloom", POST_PROCESSING_PASS)

        // Lens flare effect
        #include "LensFlarePass.h"
        ADD_RENDER_PASS(LENS_FLARE_PASS, LensFlarePass, "Lens Flare", POST_PROCESSING_PASS)

        // Tone mapping (HDR linear space to LDR gamma space conversion) and film grain (done here since it has to be independent of exposure)
        #include "HDRToneMappingPass.h"
        ADD_RENDER_PASS(HDR_TONE_MAPPING_PASS, HDRToneMappingPass, "HDR Tone Mapping", POST_PROCESSING_PASS)

        // Fast approximate anti-aliasing
        #include "FXAAPass.h"
        ADD_RENDER_PASS(FXAA_PASS, FXAAPass, "FXAA", POST_PROCESSING_PASS)

    // Head-up display
    #include "HUDPass.h"
    ADD_RENDER_PASS(HUD_PASS, HUDPass, "HUD", ROOT_PASS)
