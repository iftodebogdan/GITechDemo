/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   AppResources.cpp
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

#include "AppResources.h"
#include "ArtistParameter.h"
#include "PBRMaterialTestPass.h"

#include "LensFlarePass.h"
#include "HDRToneMappingPass.h"
#include "FXAAPass.h"

using namespace GITechDemoApp;
using namespace Synesthesia3D;

vector<RenderResource*> RenderResource::arrResources; // Moved from RenderResource.cpp
vector<ArtistParameter*> ArtistParameter::ms_arrParams; // Moved from ArtistParameter.cpp

//////////////////
// DO NOT USE!  //
//////////////////

#define CONCAT2(x, y) x##y
#define CONCAT(x, y) CONCAT2(x, y)
#define CREATE_UNIQUE_NAME CONCAT(ArtistParam, __COUNTER__)
#define EXPAND( x ) x
#define SHADER_CONSTANT_WITH_INITIALIZE(Name, Type, Val) ShaderConstantTemplate<Type> Name(#Name, Val)
#define SHADER_CONSTANT_NO_INITIALIZE(Name, Type) ShaderConstantTemplate<Type> Name(#Name)
#define INFER_SHADER_CONSTANT_FUNC(_1, _2, _3, Func, ...) Func
#define STATIC_RENDER_TARGET unsigned int
#define DYNAMIC_RENDER_TARGET float
#define RENDER_TARGET_FUNC_ONE(Name, RT0, Width, Height, DepthFormat, RTType) RenderTarget Name(#Name, 1, RT0, PF_NONE, PF_NONE, PF_NONE, (RTType)Width, (RTType)Height, DepthFormat)
#define RENDER_TARGET_FUNC_TWO(Name, RT0, RT1, Width, Height, DepthFormat, RTType) RenderTarget Name(#Name, 2, RT0, RT1, PF_NONE, PF_NONE, (RTType)Width, (RTType)Height, DepthFormat)
#define RENDER_TARGET_FUNC_THREE(Name, RT0, RT1, RT2, Width, Height, DepthFormat, RTType) RenderTarget Name(#Name, 3, RT0, RT1, RT2, PF_NONE, (RTType)Width, (RTType)Height, DepthFormat)
#define RENDER_TARGET_FUNC_FOUR(Name, RT0, RT1, RT2, RT3, Width, Height, DepthFormat, RTType) RenderTarget Name(#Name, 4, RT0, RT1, RT2, RT3, (RTType)Width, (RTType)Height, DepthFormat)
#define INFER_RENDER_TARGET_FUNC(_1, _2, _3, _4, _5, _6, _7, _8, Func, ...) Func

///////////////////////////////////////////////////////////

//////////////////////////
// Some handy defines   //
//////////////////////////

#define CREATE_MODEL_OBJECT(Name, Filepath) Model Name(Filepath)
#define CREATE_TEXTURE_OBJECT(Name, Filepath) Texture Name(Filepath)
#define CREATE_SHADER_OBJECT(Name, Filepath) Shader Name(Filepath)
#define CREATE_PBR_MATERIAL(Name, FolderPath) PBRMaterial Name(FolderPath)
#define CREATE_SHADER_CONSTANT_OBJECT(... /* Name, Type, Val[opt] */) namespace HLSL { EXPAND(EXPAND(INFER_SHADER_CONSTANT_FUNC(__VA_ARGS__, SHADER_CONSTANT_WITH_INITIALIZE, SHADER_CONSTANT_NO_INITIALIZE))(__VA_ARGS__)); }
#define CREATE_STATIC_RENDER_TARGET_OBJECT(... /* Name, RT0, RT1[opt], RT2[opt], RT3[opt], Width, Height, DepthFormat */) EXPAND(EXPAND(INFER_RENDER_TARGET_FUNC(__VA_ARGS__, RENDER_TARGET_FUNC_FOUR, RENDER_TARGET_FUNC_THREE, RENDER_TARGET_FUNC_TWO, RENDER_TARGET_FUNC_ONE))(__VA_ARGS__, STATIC_RENDER_TARGET))
#define CREATE_DYNAMIC_RENDER_TARGET_OBJECT(... /* Name, RT0, RT1[opt], RT2[opt], RT3[opt], WidthRatio, HeightRatio, DepthFormat */) EXPAND(EXPAND(INFER_RENDER_TARGET_FUNC(__VA_ARGS__, RENDER_TARGET_FUNC_FOUR, RENDER_TARGET_FUNC_THREE, RENDER_TARGET_FUNC_TWO, RENDER_TARGET_FUNC_ONE))(__VA_ARGS__, DYNAMIC_RENDER_TARGET))
#define CREATE_ARTIST_PARAMETER_OBJECT(Name, Desc, Category, Param, StepVal) ArtistParameter CREATE_UNIQUE_NAME (Name, Desc, Category, & Param, StepVal, typeid(Param).hash_code())
#define CREATE_ARTIST_BOOLPARAM_OBJECT(Name, Desc, Category, Param) CREATE_ARTIST_PARAMETER_OBJECT(Name, Desc, Category, Param, 1.f)

///////////////////////////////////////////////////////////

namespace GITechDemoApp
{
    //////////////////////////////////////////////////////
    // Setup access to externally declared variables    //
    //////////////////////////////////////////////////////

    // Camera
    extern bool CAMERA_INFINITE_PROJ;
    extern float CAMERA_FOV;
    extern float CAMERA_MOVE_SPEED;
    extern float CAMERA_SPEED_UP_FACTOR;
    extern float CAMERA_SLOW_DOWN_FACTOR;
    extern float CAMERA_ROTATE_SPEED;
    extern bool CAMERA_ANIMATION_ENABLED;
    extern int CAMERA_ANIMATION_TIMEOUT_SECONDS;

    // G-Buffer
    extern bool GBUFFER_Z_PREPASS;
    extern int DIFFUSE_ANISOTROPY;
    extern bool GBUFFER_USE_NORMAL_MAPS;
    extern int GBUFFER_DEBUG_VIEW;
    extern bool GBUFFER_DEBUG_VIEW_DEPTH;
    extern bool DRAW_ALPHA_TEST_GEOMETRY;

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
    extern bool RSM_USE_BILATERAL_BLUR;
    extern bool DEBUG_RSM_CAMERA;

    // Directional light volumetric effect
    extern bool DIR_LIGHT_VOLUME_ENABLE;
    extern bool DIR_LIGHT_VOLUME_QUARTER_RES;
    extern bool DIR_LIGHT_VOLUME_BLUR_SAMPLES;
    extern bool DIR_LIGHT_VOLUME_BLUR_DEPTH_AWARE;
    extern bool DIR_LIGHT_VOLUME_UPSCALE_DEPTH_AWARE;
    extern Vec4f DIR_LIGHT_VOLUME_COLOR;

    // Post-processing
    extern bool POST_PROCESSING_ENABLED;

    // SSR
    extern bool SSR_ENABLED;
    extern bool SSR_MANUAL_MAX_STEPS;

    // SSAO
    extern bool SSAO_ENABLED;
    extern bool SSAO_USE_QUARTER_RESOLUTION_BUFFER;

    // Bokeh DoF
    extern bool DOF_ENABLED;
    extern float DOF_AUTOFOCUS_TIME;
    extern int DOF_NUM_PASSES;

    // Motion blur
    extern bool MOTION_BLUR_ENABLED;

    // Bloom
    extern bool BLOOM_ENABLED;

    // Lens flare
    extern LensFlarePass LENS_FLARE_PASS;
    extern bool LENS_FLARE_ENABLED;
    extern float LENS_FLARE_BRIGHTNESS_THRESHOLD;
    extern bool LENS_FLARE_ANAMORPHIC;

    // Tone mapping
    extern HDRToneMappingPass HDR_TONE_MAPPING_PASS;
    extern bool HDR_TONE_MAPPING_ENABLED;
    extern bool SRGB_COLOR_CORRECTION;

    // FXAA
    extern FXAAPass FXAA_PASS;
    extern bool FXAA_ENABLED;

    // Window properties
    extern bool FULLSCREEN_ENABLED;
    extern bool BORDERLESS_ENABLED;
    extern int FULLSCREEN_RESOLUTION_X;
    extern int FULLSCREEN_RESOLUTION_Y;
    extern int FULLSCREEN_REFRESH_RATE;
    extern bool VSYNC_ENABLED;

    // Some misc. resources
    extern const Vec<unsigned int, 2> SHADOW_MAP_SIZE;
    extern const unsigned int RSM_SIZE;
    VertexBuffer*   FullScreenTri = nullptr;

    //------------------------------------------------------



    //////////////////////////////////////////
    // Start adding render resources here   //
    //////////////////////////////////////////

    //////////////
    // Models   //
    //////////////

    CREATE_MODEL_OBJECT(SponzaScene,    "models/sponza/sponza.s3dmdl");
    CREATE_MODEL_OBJECT(SphereModel,    "models/pbr-test/sphere.s3dmdl");

    //------------------------------------------------------



    //////////////
    // Shaders  //
    //////////////

    CREATE_SHADER_OBJECT(DirectionalLightVolumeShader,      "shaders/DirectionalLightVolume.hlsl");
    CREATE_SHADER_OBJECT(RSMUpscaleShader,                  "shaders/RSMUpscale.hlsl");
    CREATE_SHADER_OBJECT(RSMApplyShader,                    "shaders/RSMApply.hlsl");
    CREATE_SHADER_OBJECT(ScreenSpaceReflectionShader,       "shaders/ScreenSpaceReflection.hlsl");
    CREATE_SHADER_OBJECT(DirectionalLightShader,            "shaders/DirectionalLight.hlsl");
    CREATE_SHADER_OBJECT(SphericalLensFlareFeaturesShader,  "shaders/SphericalLensFlareFeatures.hlsl");
    CREATE_SHADER_OBJECT(SsaoShader,                        "shaders/SSAO.hlsl");
    CREATE_SHADER_OBJECT(BilateralBlurShader,               "shaders/BilateralBlur.hlsl");
    CREATE_SHADER_OBJECT(BokehDofShader,                    "shaders/BokehDoF.hlsl");
    CREATE_SHADER_OBJECT(FxaaShader,                        "shaders/FXAA.hlsl");
    CREATE_SHADER_OBJECT(AnamorphicLensFlareBlurShader,     "shaders/AnamorphicLensFlareBlur.hlsl");
    CREATE_SHADER_OBJECT(AnamorphicLensFlareFeaturesShader, "shaders/AnamorphicLensFlareFeatures.hlsl");
    CREATE_SHADER_OBJECT(BloomShader,                       "shaders/Bloom.hlsl");
    CREATE_SHADER_OBJECT(DownsampleShader,                  "shaders/Downsample.hlsl");
    CREATE_SHADER_OBJECT(LumaCaptureShader,                 "shaders/LumaCapture.hlsl");
    CREATE_SHADER_OBJECT(NearestDepthUpscaleShader,         "shaders/NearestDepthUpscale.hlsl");
    CREATE_SHADER_OBJECT(MotionBlurShader,                  "shaders/MotionBlur.hlsl");
    CREATE_SHADER_OBJECT(GBufferGenerationShader,           "shaders/GBufferGeneration.hlsl");
    CREATE_SHADER_OBJECT(HDRToneMappingShader,              "shaders/HDRToneMapping.hlsl");
    CREATE_SHADER_OBJECT(LumaAdaptShader,                   "shaders/LumaAdapt.hlsl");
    CREATE_SHADER_OBJECT(ColorCopyShader,                   "shaders/ColorCopy.hlsl");
    CREATE_SHADER_OBJECT(RSMCaptureShader,                  "shaders/RSMCapture.hlsl");
    CREATE_SHADER_OBJECT(DepthPassShader,                   "shaders/DepthPass.hlsl");
    CREATE_SHADER_OBJECT(DepthPassAlphaTestShader,          "shaders/DepthPassAlphaTest.hlsl");
    CREATE_SHADER_OBJECT(SkyBoxShader,                      "shaders/SkyBox.hlsl");
    CREATE_SHADER_OBJECT(LensFlareApplyShader,              "shaders/LensFlareApply.hlsl");
    CREATE_SHADER_OBJECT(UIShader,                          "shaders/UI.hlsl");
    CREATE_SHADER_OBJECT(DepthCopyShader,                   "shaders/DepthCopy.hlsl");

    //------------------------------------------------------



    ///////////////////
    // PBR Materials //
    ///////////////////

    CREATE_PBR_MATERIAL(BambooWood,                     "bamboo_wood");
    CREATE_PBR_MATERIAL(CrateredRock,                   "cratered_rock");
    CREATE_PBR_MATERIAL(DirtySlightlyPittedConcrete,    "dirty_slightly_pitted_concrete");
    CREATE_PBR_MATERIAL(DryBrownDirt,                   "dry_brown_dirt");
    CREATE_PBR_MATERIAL(GreasyWornMetal,                "greasy_worn_metal");
    CREATE_PBR_MATERIAL(OctogonStoneCobble,             "octogon_stone_cobble");
    CREATE_PBR_MATERIAL(PaintPeelingConcrete,           "paint_peeling_concrete");
    CREATE_PBR_MATERIAL(PolishedGranite,                "polished_granite");
    CREATE_PBR_MATERIAL(PolishedSpeckledMarble,         "polished_speckled_marble");
    CREATE_PBR_MATERIAL(RockInfusedWithCopper,          "rock_infused_with_copper");
    CREATE_PBR_MATERIAL(RustedStreakedIron,             "rusted_streaked_iron");
    CREATE_PBR_MATERIAL(RustedIron,                     "rusted_iron");
    CREATE_PBR_MATERIAL(ScuffedAluminum,                "scuffed_aluminum");
    CREATE_PBR_MATERIAL(ScuffedCopper,                  "scuffed_copper");
    CREATE_PBR_MATERIAL(ScuffedGold,                    "scuffed_gold");
    CREATE_PBR_MATERIAL(ScuffedIron,                    "scuffed_iron");
    CREATE_PBR_MATERIAL(ScuffedTitanium,                "scuffed_titanium");
    CREATE_PBR_MATERIAL(WornOutOldBrickWall,            "worn_out_old_brick_wall");
    CREATE_PBR_MATERIAL(WornPaintedCement,              "worn_painted_cement");
    CREATE_PBR_MATERIAL(WornScuffedPlastic,             "worn_scuffed_plastic");

    //------------------------------------------------------



    //////////////
    // Textures //
    //////////////

    CREATE_TEXTURE_OBJECT(SkyTexture,               "textures/sky.s3dtex");
    CREATE_TEXTURE_OBJECT(IrradianceTexture,        "textures/irradiance.s3dtex");
    CREATE_TEXTURE_OBJECT(EnvironmentTexture,       "textures/envmap.s3dtex");
    CREATE_TEXTURE_OBJECT(LensFlareGhostColorLUT,   "textures/LensFlareGhostColorLUT.s3dtex");
    CREATE_TEXTURE_OBJECT(LensFlareDirtTexture,     "textures/LensFlareDirt.s3dtex");
    CREATE_TEXTURE_OBJECT(LensFlareStarBurstTexture,"textures/LensFlareStarBurst.s3dtex");
    CREATE_TEXTURE_OBJECT(BayerMatrix,              "textures/bayer_matrix.s3dtex");
    CREATE_TEXTURE_OBJECT(NoiseTexture,             "textures/noise.s3dtex");
    CREATE_TEXTURE_OBJECT(ColorCorrectionTexture,   "textures/ContrastEnhance.s3dtex");

    //------------------------------------------------------



    //////////////////////
    // Render targets   //
    //////////////////////

    // Geometry buffer (G-Buffer)
    //  RT0:    A8R8G8B8 with diffuse albedo in RGB and specular power in A
    //  RT1:    G16R16F with compressed normals (stereographic projection)
    //  RT2:    G16R16 with material type (dielectric/metallic) in R and roughness in G (Cook-Torrance only)
    //  RT3:    G16R16F with vertex normals, which are innately low frequency, required to reduce noise from indirect lighting
    //  DS:     INTZ (for sampling as a regular texture later)
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(GBuffer, PF_A8R8G8B8, PF_G16R16F, PF_G16R16, PF_G16R16F, 1.f, 1.f, PF_INTZ);

    // Utility depth buffers: hyperbolic and camera space, full and quarter resolution depth buffers
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(HyperbolicQuarterDepthBuffer, PF_R32F, 0.5f, 0.5f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(LinearFullDepthBuffer, PF_R32F, 1.f, 1.f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(LinearQuarterDepthBuffer, PF_R32F, 0.5f, 0.5f, PF_NONE);

    // Shadow map for the directional light (the dummy color buffer is required because of DX9 limitations
    CREATE_STATIC_RENDER_TARGET_OBJECT(ShadowMapDir, PF_NONE, SHADOW_MAP_SIZE[0], SHADOW_MAP_SIZE[1], PF_INTZ);

    // Render target in which we accumulate the light contribution from all light sources (also known as the scene color buffer)
    // It contains a regular depth-stencil surface in which we will copy-resolve our INTZ depth texture from the G-Buffer
    // (the depth is required for correctly rendering the sky, among other things)
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(LightAccumulationBuffer, PF_A16B16G16R16F, 1.f, 1.f, PF_D24S8);

    // Reflective Shadow Map render target
    CREATE_STATIC_RENDER_TARGET_OBJECT(RSMBuffer, PF_A8R8G8B8, PF_G16R16F, RSM_SIZE, RSM_SIZE, PF_INTZ);

    // Indirect lighting accumulation buffer (quarter resolution)
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(IndirectLightAccumulationBuffer0, PF_A16B16G16R16F, 0.5f, 0.5f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(IndirectLightAccumulationBuffer1, PF_A16B16G16R16F, 0.5f, 0.5f, PF_NONE);

    // Volumetric light accumulation buffer
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(VolumetricLightFullBuffer0, PF_R16F, 1.f, 1.f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(VolumetricLightFullBuffer1, PF_R16F, 1.f, 1.f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(VolumetricLightQuarterBuffer0, PF_R16F, 0.5f, 0.5f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(VolumetricLightQuarterBuffer1, PF_R16F, 0.5f, 0.5f, PF_NONE);

    // HDR downsampled buffer (1/4 and 1/16 resolution)
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(HDRDownsampleQuarterBuffer, PF_A16B16G16R16F, 0.5f, 0.5f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(HDRDownsampleSixteenthBuffer, PF_A16B16G16R16F, 0.25f, 0.25f, PF_NONE);

    // HDR downsampled buffer for bloom pass (1/16 resolution, no volumetric lighting)
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(HDRDownsampleForBloomBuffer, PF_A16B16G16R16F, 0.25f, 0.25f, PF_NONE);

    // Average luminance buffers (64x64, 16x16, 4x4, two 1x1, one target luma, one current luma)
    CREATE_STATIC_RENDER_TARGET_OBJECT(AverageLuminanceBuffer0, PF_R16F, 64u, 64u, PF_NONE);
    CREATE_STATIC_RENDER_TARGET_OBJECT(AverageLuminanceBuffer1, PF_R16F, 16u, 16u, PF_NONE);
    CREATE_STATIC_RENDER_TARGET_OBJECT(AverageLuminanceBuffer2, PF_R16F, 4u, 4u, PF_NONE);
    CREATE_STATIC_RENDER_TARGET_OBJECT(AverageLuminanceBuffer3, PF_R16F, 1u, 1u, PF_NONE);

    // Adapted luminance for simulating light adaptation effect
    CREATE_STATIC_RENDER_TARGET_OBJECT(AdaptedLuminance0, PF_R16F, 1u, 1u, PF_NONE);
    CREATE_STATIC_RENDER_TARGET_OBJECT(AdaptedLuminance1, PF_R16F, 1u, 1u, PF_NONE);

    // HDR bloom render targets
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(BloomBuffer0, PF_A16B16G16R16F, 0.25f, 0.25f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(BloomBuffer1, PF_A16B16G16R16F, 0.25f, 0.25f, PF_NONE);

    // LDR tone mapped render target
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(LDRToneMappedImageBuffer, PF_A8R8G8B8, 1.f, 1.f, PF_NONE);

    // LDR FXAA render target
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(LDRFxaaImageBuffer, PF_A8R8G8B8, 1.f, 1.f, PF_NONE);

    // Ambient occlusion render targets
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(SSAOFullBuffer0, PF_L8, 1.f, 1.f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(SSAOFullBuffer1, PF_L8, 1.f, 1.f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(SSAOQuarterBuffer0, PF_L8, 0.5f, 0.5f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(SSAOQuarterBuffer1, PF_L8, 0.5f, 0.5f, PF_NONE);

    // Depth of field render targets
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(DepthOfFieldBuffer0, PF_A16B16G16R16F, 1.f, 1.f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(DepthOfFieldBuffer1, PF_A16B16G16R16F, 1.f, 1.f, PF_NONE);
    CREATE_STATIC_RENDER_TARGET_OBJECT(AutofocusBuffer0, PF_R16F, 1u, 1u, PF_NONE);
    CREATE_STATIC_RENDER_TARGET_OBJECT(AutofocusBuffer1, PF_R16F, 1u, 1u, PF_NONE);

    // Motion blur render target
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(MotionBlurBuffer, PF_A16B16G16R16F, 1.f, 1.f, PF_NONE);

    // Lens flare render targets
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(SphericalLensFlareBuffer0, PF_A16B16G16R16F, 0.5f, 0.5f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(SphericalLensFlareBuffer1, PF_A16B16G16R16F, 0.5f, 0.5f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(AnamorphicLensFlareBuffer0, PF_A16B16G16R16F, 0.03125f, 0.5f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(AnamorphicLensFlareBuffer1, PF_A16B16G16R16F, 0.03125f, 0.5f, PF_NONE);
    CREATE_DYNAMIC_RENDER_TARGET_OBJECT(AnamorphicLensFlareBuffer2, PF_A16B16G16R16F, 0.25f, 0.25f, PF_NONE);
    
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

    RenderTarget* DepthOfFieldBuffer[2] = {
        &DepthOfFieldBuffer0,
        &DepthOfFieldBuffer1
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

    RenderTarget* IndirectLightAccumulationBuffer[2] = {
        &IndirectLightAccumulationBuffer0,
        &IndirectLightAccumulationBuffer1
    };

    //------------------------------------------------------



    //////////////////////////////////////
    // Shader constants - artist driven //
    //////////////////////////////////////

    /* General parameters */
    CREATE_SHADER_CONSTANT_OBJECT(fZNear,                   float,          10.f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fZFar,                    float,          5000.f                  );

    /* Directional light parameters */
    CREATE_SHADER_CONSTANT_OBJECT(fDiffuseFactor,           float,          7.5f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fSpecFactor,              float,          15.f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fAmbientFactor,           float,          0.15f                   );
    CREATE_SHADER_CONSTANT_OBJECT(fIrradianceFactor,        float,          1.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(fReflectionFactor,        float,          1.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(nBRDFModel,               int,            COOK_TORRANCE_GGX       );

    /* Volumetric directional light parameters */
    CREATE_SHADER_CONSTANT_OBJECT(nSampleCount,             int,            32                      );
    CREATE_SHADER_CONSTANT_OBJECT(fSampleDistrib,           float,          0.5f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fLightIntensity,          float,          0.5f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fMultScatterIntensity,    float,          0.05f                   );
    CREATE_SHADER_CONSTANT_OBJECT(f3FogSpeed,               Vec3f,          Vec3f(25.f, -15.f, 25.f));
    CREATE_SHADER_CONSTANT_OBJECT(fFogVerticalFalloff,      float,          15.f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fBlurDepthFalloff,        float,          0.0025f                 );
    CREATE_SHADER_CONSTANT_OBJECT(fUpsampleDepthThreshold,  float,          0.0015f                 );

    /* Sun parameters */
    CREATE_SHADER_CONSTANT_OBJECT(fSunRadius,               float,          1000.f                  );
    CREATE_SHADER_CONSTANT_OBJECT(fSunBrightness,           float,          500.f                   );

    /* Cascaded Shadow Map parameters */
    CREATE_SHADER_CONSTANT_OBJECT(bDebugCascades,           bool,           false                   );
    CREATE_SHADER_CONSTANT_OBJECT(fCascadeBlendSize,        float,          50.f                    );

    /* Reflective Shadow Map parameters */
    CREATE_SHADER_CONSTANT_OBJECT(fRSMIntensity,            float,          200.f                   );
    CREATE_SHADER_CONSTANT_OBJECT(fRSMKernelScale,          float,          0.025f                  );
    CREATE_SHADER_CONSTANT_OBJECT(fWeightThreshold,         float,          0.002f                  );
    CREATE_SHADER_CONSTANT_OBJECT(bDebugUpscalePass,        bool,           false                   );

    /* Screen-Space Ambient Occlusion */
    CREATE_SHADER_CONSTANT_OBJECT(fSSAOSampleRadius,        float,          10.f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fSSAOIntensity,           float,          5.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(fSSAOScale,               float,          0.05f                   );
    CREATE_SHADER_CONSTANT_OBJECT(fSSAOBias,                float,          0.25f                   );

    /* Screen Space Reflection */
    CREATE_SHADER_CONSTANT_OBJECT(fReflectionIntensity,     float,          1.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(fThickness,               float,          100.f                   );
    CREATE_SHADER_CONSTANT_OBJECT(fSampleStride,            float,          25.f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fMaxSteps,                float,          200.f                   );
    CREATE_SHADER_CONSTANT_OBJECT(fMaxRayDist,              float,          3000.f                  );
    CREATE_SHADER_CONSTANT_OBJECT(bUseDither,               bool,           true                    );

    /* Post-processing parameters */
    // Bloom
    CREATE_SHADER_CONSTANT_OBJECT(fBrightnessThreshold,     float,          0.2f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fBloomPower,              float,          1.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(fBloomStrength,           float,          0.75f                   );
    // DoF
    CREATE_SHADER_CONSTANT_OBJECT(fFocalDepth,              float,          100.f                   );
    CREATE_SHADER_CONSTANT_OBJECT(fFocalLength,             float,          100.f                   );
    CREATE_SHADER_CONSTANT_OBJECT(fFStop,                   float,          2.8f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fCoC,                     float,          0.03f                   );
    CREATE_SHADER_CONSTANT_OBJECT(bAutofocus,               bool,           true                    );
    CREATE_SHADER_CONSTANT_OBJECT(fHighlightThreshold,      float,          3.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(fHighlightGain,           float,          1.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(fApertureSize,            float,          0.0075f                 );
    CREATE_SHADER_CONSTANT_OBJECT(bAnamorphicBokeh,         bool,           false                   );
    CREATE_SHADER_CONSTANT_OBJECT(bVignetting,              bool,           true                    );
    CREATE_SHADER_CONSTANT_OBJECT(fVignOut,                 float,          0.75f                   );
    CREATE_SHADER_CONSTANT_OBJECT(fVignIn,                  float,          0.25f                   );
    CREATE_SHADER_CONSTANT_OBJECT(fVignFade,                float,          15.f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fChromaShiftAmount,       float,          1.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(fQuarticDistortionCoef,   float,          0.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(fCubicDistortionModifier, float,          0.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(fDistortionScale,         float,          1.f                     );
    // Motion blur
    CREATE_SHADER_CONSTANT_OBJECT(fMotionBlurIntensity,     float,          0.01f                   );
    CREATE_SHADER_CONSTANT_OBJECT(nMotionBlurNumSamples,    int,            5                       );
    // Lens flare
    CREATE_SHADER_CONSTANT_OBJECT(nGhostSamples,            int,            5                       );
    CREATE_SHADER_CONSTANT_OBJECT(fGhostDispersal,          float,          0.37f                   );
    CREATE_SHADER_CONSTANT_OBJECT(fGhostRadialWeightExp,    float,          1.5f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fHaloSize,                float,          0.6f                    );
    CREATE_SHADER_CONSTANT_OBJECT(fHaloRadialWeightExp,     float,          5.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(bChromaShift,             bool,           true                    );
    CREATE_SHADER_CONSTANT_OBJECT(fShiftFactor,             float,          1.f                     );
    CREATE_SHADER_CONSTANT_OBJECT(fAnamorphicIntensity,     float,          5.f                     );

    CREATE_SHADER_CONSTANT_OBJECT(LensFlareFeatures, s3dSampler2D);
    CREATE_SHADER_CONSTANT_OBJECT(LensFlareDirt, s3dSampler2D);
    CREATE_SHADER_CONSTANT_OBJECT(LensFlareStarBurst, s3dSampler2D);
    CREATE_SHADER_CONSTANT_OBJECT(LensFlareApplyParams, LensFlareApplyConstantTable);

    // Tone mapping
    CREATE_SHADER_CONSTANT_OBJECT(f2AvgLumaClamp, Vec2f, Vec2f(0.0001f, 0.75f));
    CREATE_SHADER_CONSTANT_OBJECT(fLumaAdaptSpeed, float, 1.f);
    CREATE_SHADER_CONSTANT_OBJECT(HDRToneMappingSourceTexture, s3dSampler2D);
    CREATE_SHADER_CONSTANT_OBJECT(HDRToneMappingAvgLumaTexture, s3dSampler2D);
    CREATE_SHADER_CONSTANT_OBJECT(HDRToneMappingColorCorrectionTexture, s3dSampler2D);
    CREATE_SHADER_CONSTANT_OBJECT(HDRToneMappingParams, HDRToneMappingConstantTable);

    // FXAA
    CREATE_SHADER_CONSTANT_OBJECT(FXAASourceTexture, s3dSampler2D);
    CREATE_SHADER_CONSTANT_OBJECT(FXAADepthBuffer, s3dSampler2D);
    CREATE_SHADER_CONSTANT_OBJECT(FXAAParams, FXAAConstantTable);

    // ColorCopy.hlsl
    CREATE_SHADER_CONSTANT_OBJECT(ColorCopySourceTexture, s3dSampler2D);
    CREATE_SHADER_CONSTANT_OBJECT(ColorCopyParams, ColorCopyConstantTable);

    // UI.hlsl
    CREATE_SHADER_CONSTANT_OBJECT(UITexture1D, s3dSampler1D);
    CREATE_SHADER_CONSTANT_OBJECT(UITexture2D, s3dSampler2D);
    CREATE_SHADER_CONSTANT_OBJECT(UITexture3D, s3dSampler3D);
    CREATE_SHADER_CONSTANT_OBJECT(UITextureCube, s3dSamplerCUBE);
    CREATE_SHADER_CONSTANT_OBJECT(UIParams, UIConstantTable);
    //------------------------------------------------------



    //////////////////////////////////////
    // Shader constants - code driven   //
    //////////////////////////////////////

    CREATE_SHADER_CONSTANT_OBJECT(f44WorldMat,                  Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44ProjMat,                   Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44ViewProjMat,               Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44WorldViewProjMat,          Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44WorldViewMat,              Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(texDiffuse,                   s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texNormal,                    s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(bHasNormalMap,                bool            );
    CREATE_SHADER_CONSTANT_OBJECT(texSpec,                      s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(bHasSpecMap,                  bool            );
    CREATE_SHADER_CONSTANT_OBJECT(texMatType,                   s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texRoughness,                 s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(fSpecIntensity,               float           );
    CREATE_SHADER_CONSTANT_OBJECT(f2HalfTexelOffset,            Vec2f           );
    CREATE_SHADER_CONSTANT_OBJECT(texDepthBuffer,               s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texHypDepthBuffer,            s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texLinDepthBuffer,            s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texQuarterDepthBuffer,        s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texMaterialBuffer,            s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texIrradianceMap,             s3dSamplerCUBE  );
    CREATE_SHADER_CONSTANT_OBJECT(texEnvMap,                    s3dSamplerCUBE  );
    CREATE_SHADER_CONSTANT_OBJECT(f44SkyViewProjMat,            Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(texSkyCube,                   s3dSamplerCUBE  );
    CREATE_SHADER_CONSTANT_OBJECT(texDiffuseBuffer,             s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(f44InvProjMat,                Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(texNormalBuffer,              s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texShadowMap,                 s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(f2OneOverShadowMapSize,       Vec2f           );
    CREATE_SHADER_CONSTANT_OBJECT(f44ViewMat,                   Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44InvViewMat,                Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44InvViewProjMat,            Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44ScreenToLightViewMat,      Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f3LightDir,                   Vec3f           );
    CREATE_SHADER_CONSTANT_OBJECT(f2CascadeBoundsMin,           Vec2f*          );
    CREATE_SHADER_CONSTANT_OBJECT(f2CascadeBoundsMax,           Vec2f*          );
    CREATE_SHADER_CONSTANT_OBJECT(f44CascadeProjMat,            Matrix44f*      );
    CREATE_SHADER_CONSTANT_OBJECT(f2PoissonDisk,                Vec2f*          );
    CREATE_SHADER_CONSTANT_OBJECT(f44LightViewMat,              Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44InvLightViewMat,           Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44LightWorldViewProjMat,     Matrix44f*      );
    CREATE_SHADER_CONSTANT_OBJECT(f44RSMWorldViewProjMat,       Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44LightWorldViewMat,         Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44LightViewProjMat,          Matrix44f*      );
    CREATE_SHADER_CONSTANT_OBJECT(texRSMFluxBuffer,             s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texRSMNormalBuffer,           s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texRSMDepthBuffer,            s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(f3RSMKernel,                  Vec3f*          );
    CREATE_SHADER_CONSTANT_OBJECT(f44RSMProjMat,                Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44RSMInvProjMat,             Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(f44ViewToRSMViewMat,          Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(texLumaInput,                 s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(bInitialLumaPass,             bool            );
    CREATE_SHADER_CONSTANT_OBJECT(bFinalLumaPass,               bool            );
    CREATE_SHADER_CONSTANT_OBJECT(texAvgLuma,                   s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(texColorCorrection,           s3dSampler3D    );
    CREATE_SHADER_CONSTANT_OBJECT(texSource,                    s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(fFrameTime,                   float           );
    CREATE_SHADER_CONSTANT_OBJECT(texLumaTarget,                s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(bApplyBrightnessFilter,       bool            );
    CREATE_SHADER_CONSTANT_OBJECT(nKernel,                      int             );
    CREATE_SHADER_CONSTANT_OBJECT(nDownsampleFactor,            int             );
    CREATE_SHADER_CONSTANT_OBJECT(bDepthDownsample,             bool            );
    CREATE_SHADER_CONSTANT_OBJECT(bReconstructDepth,            bool            );
    CREATE_SHADER_CONSTANT_OBJECT(f4TexSize,                    Vec4f           );
    CREATE_SHADER_CONSTANT_OBJECT(bAdjustIntensity,             bool            );
    CREATE_SHADER_CONSTANT_OBJECT(f2LinearDepthEquation,        Vec2f           );
    CREATE_SHADER_CONSTANT_OBJECT(texTargetFocus,               s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(f44PrevViewProjMat,           Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(texGhostColorLUT,             s3dSampler1D    );
    CREATE_SHADER_CONSTANT_OBJECT(bSingleChannelCopy,           bool            );
    CREATE_SHADER_CONSTANT_OBJECT(f4CustomColorModulator,       Vec4f           );
    CREATE_SHADER_CONSTANT_OBJECT(bApplyTonemap,                bool            );
    CREATE_SHADER_CONSTANT_OBJECT(f3CameraPositionLightVS,      Vec3f           );
    CREATE_SHADER_CONSTANT_OBJECT(fRaymarchDistanceLimit,       float           );
    CREATE_SHADER_CONSTANT_OBJECT(texDitherMap,                 s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(f2BlurDir,                    Vec2f           );
    CREATE_SHADER_CONSTANT_OBJECT(f2DepthHalfTexelOffset,       Vec2f           );
    CREATE_SHADER_CONSTANT_OBJECT(texNoise,                     s3dSampler3D    );
    CREATE_SHADER_CONSTANT_OBJECT(fElapsedTime,                 float           );
    CREATE_SHADER_CONSTANT_OBJECT(f3FogBox,                     Vec3f           );
    CREATE_SHADER_CONSTANT_OBJECT(texHDRSceneTexture,           s3dSampler2D    );
    CREATE_SHADER_CONSTANT_OBJECT(f44ViewToRasterMat,           Matrix44f       );
    CREATE_SHADER_CONSTANT_OBJECT(nTexMipCount,                 int             );

    //--------------------------------------------------------------------------



    //////////////////////////////////////////
    // Start adding artist parameters here  //
    //////////////////////////////////////////

    // Window properties -----------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Fullscreen enabled",
        "Toggle between window mode and fulscreen mode",
        "Window",
        FULLSCREEN_ENABLED);
    
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Borderless windowed mode",
        "Toggle between regular windowed and borderless windowed mode",
        "Window",
        BORDERLESS_ENABLED);
    
    CREATE_ARTIST_PARAMETER_OBJECT(
        "Resolution X (width)",
        "Set the resolution on the X axis (only affects fullscreen mode)",
        "Window",
        FULLSCREEN_RESOLUTION_X,
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Resolution Y (height)",
        "Set the resolution on the Y axis (only affects fullscreen mode)",
        "Window",
        FULLSCREEN_RESOLUTION_Y,
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Refresh rate",
        "Set the refresh rate of the display (only affects fullscreen mode)",
        "Window",
        FULLSCREEN_REFRESH_RATE,
        1.f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "VSync enabled",
        "Synchronizes backbuffer swapping with screen refresh rate",
        "Window",
        VSYNC_ENABLED);
    //------------------------------------------------------

    // Camera ----------------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Infinite projection",
        "Use a projection matrix with an infinite far plane",
        "Camera",
        CAMERA_INFINITE_PROJ);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Z-Near",
        "Distance to the nearest Z clip plane",
        "Camera",
        fZNear.GetCurrentValue(),
        0.1f);
    
    CREATE_ARTIST_PARAMETER_OBJECT(
        "Z-Far",
        "Distance to the furthest Z clip plane (if not infinite projection)",
        "Camera",
        fZFar.GetCurrentValue(),
        10.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "FOV",
        "Vertical field of view",
        "Camera",
        CAMERA_FOV,
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Movement speed",
        "Camera movement speed",
        "Camera",
        CAMERA_MOVE_SPEED,
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Speed up factor",
        "Camera speed multiplier when pressing the 'speed up' button",
        "Camera",
        CAMERA_SPEED_UP_FACTOR,
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Slow down factor",
        "Camera speed multiplier when pressing the 'slow down' button",
        "Camera",
        CAMERA_SLOW_DOWN_FACTOR,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Rotation speed",
        "Camera rotation speed",
        "Camera",
        CAMERA_ROTATE_SPEED,
        1.f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Camera animation",
        "Camera animation toggle (no collisions with world geometry)",
        "Camera",
        CAMERA_ANIMATION_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Animation timeout",
        "Seconds it takes until the camera animation kicks in",
        "Camera",
        CAMERA_ANIMATION_TIMEOUT_SECONDS,
        1.f);
    //------------------------------------------------------

    // G-Buffer --------------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Z-prepass",
        "Populate the scene's depth buffer before generating the G-Buffer",
        "G-Buffer",
        GBUFFER_Z_PREPASS);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Draw alpha test geometry",
        "Draw geometry that uses alpha testing when Z-prepass is active.",
        "G-Buffer",
        DRAW_ALPHA_TEST_GEOMETRY);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Diffuse anisotropic level",
        "Anisotropic filtering level for diffuse textures",
        "G-Buffer",
        DIFFUSE_ANISOTROPY,
        1.f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Use normal maps",
        "Toggles the use of normal maps or vertex normals",
        "G-Buffer",
        GBUFFER_USE_NORMAL_MAPS);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Debug view G-Buffer",
        "0 - albedo; 1 - normals; 2 - material/roughness; 3 - vertex normals",
        "G-Buffer",
        GBUFFER_DEBUG_VIEW,
        1.f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Debug view depth buffer",
        "View the depth buffer (linear, scaled by far plane distance)",
        "G-Buffer",
        GBUFFER_DEBUG_VIEW_DEPTH);
    //------------------------------------------------------

    // Directional light -----------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Directional lights enable",
        "Toggle the rendering of directional lights",
        "Directional light",
        DIRECTIONAL_LIGHT_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "BRDF model",
        "0 - Blinn-Phong; 1 - Cook-Torrance GGX; 2 - Cook-Torrance Beckmann; 3 - Ashikhmin-Shirley; 4 - Ward",
        "Directional light",
        nBRDFModel.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Diffuse factor",
        "Scale value for diffuse light equation",
        "Directional light",
        fDiffuseFactor.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Specular factor",
        "Scale value for specular light equation (Blinn-Phong only)",
        "Directional light",
        fSpecFactor.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Ambient factor",
        "Scale value for ambient light equation",
        "Directional light",
        fAmbientFactor.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Irradiance factor",
        "Scale value for irradiance map (Cook-Torrance only)",
        "Directional light",
        fIrradianceFactor.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Reflection factor",
        "Scale value for reflection map (Cook-Torrance only)",
        "Directional light",
        fReflectionFactor.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Light animation",
        "Directional light animation toggle",
        "Directional light",
        DIRECTIONAL_LIGHT_ANIMATION_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Light direction - X axis",
        "Directional light direction on the X axis of the world",
        "Directional light",
        f3LightDir.GetCurrentValue()[0],
        0.01f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Light direction - Z axis",
        "Directional light direction on the Z axis of the world",
        "Directional light",
        f3LightDir.GetCurrentValue()[2],
        0.01f);
    //------------------------------------------------------

    // CSM -------------------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Debug cascades",
        "Draw cascades with different colors",
        "Cascaded shadow map",
        bDebugCascades.GetCurrentValue());

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Cascade blend size",
        "The size of the blend band between overlapping cascades",
        "Cascaded shadow map",
        fCascadeBlendSize.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "CSM distribution",
        "Factor affecting the distribution of shadow map cascades",
        "Cascaded shadow map",
        CASCADE_SPLIT_FACTOR,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "CSM range",
        "Shadow draw distance",
        "Cascaded shadow map",
        CASCADE_MAX_VIEW_DEPTH,
        10.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Depth bias 1",
        "Depth bias for cascade 1",
        "Cascaded shadow map",
        DEPTH_BIAS[0],
        0.0001f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Slope scaled depth bias 1",
        "Slope scaled depth bias for cascade 1",
        "Cascaded shadow map",
        SLOPE_SCALED_DEPTH_BIAS[0],
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Depth bias 2",
        "Depth bias for cascade 2",
        "Cascaded shadow map",
        DEPTH_BIAS[1],
        0.0001f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Slope scaled depth bias 2",
        "Slope scaled depth bias for cascade 2",
        "Cascaded shadow map",
        SLOPE_SCALED_DEPTH_BIAS[1],
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Depth bias 3",
        "Depth bias for cascade 3",
        "Cascaded shadow map",
        DEPTH_BIAS[2],
        0.0001f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Slope scaled depth bias 3",
        "Slope scaled depth bias for cascade 3",
        "Cascaded shadow map",
        SLOPE_SCALED_DEPTH_BIAS[2],
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Depth bias 4",
        "Depth bias for cascade 4",
        "Cascaded shadow map",
        DEPTH_BIAS[3],
        0.0001f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Slope scaled depth bias 4",
        "Slope scaled depth bias for cascade 4",
        "Cascaded shadow map",
        SLOPE_SCALED_DEPTH_BIAS[3],
        0.1f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Debug CSM camera",
        "Draw the cascaded shadow map on-screen",
        "Cascaded shadow map",
        DEBUG_CSM_CAMERA);
    //------------------------------------------------------
    
    // Screen space reflections ----------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "SSR enable",
        "Toggle the rendering of screen space reflections",
        "Screen space reflections",
        SSR_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Reflection intensity",
        "Scale value for the intensity of reflections",
        "Screen space reflections",
        fReflectionIntensity.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Object thickness",
        "Used to determine if ray hits are valid",
        "Screen space reflections",
        fThickness.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Sample stride",
        "Number of pixels to jump between each ray march iteration",
        "Screen space reflections",
        fSampleStride.GetCurrentValue(),
        10.f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Manual maximum steps",
        "Manually adjust maximum number of steps, or calculate it dynamically",
        "Screen space reflections",
        SSR_MANUAL_MAX_STEPS);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Maximum step count",
        "Maximum number of ray march iterations before returning a miss",
        "Screen space reflections",
        fMaxSteps.GetCurrentValue(),
        10.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Maximum ray distance",
        "Maximum distance to ray march before returning a miss",
        "Screen space reflections",
        fMaxRayDist.GetCurrentValue(),
        100.f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Use dithering",
        "Use a bayer matrix to offset the starting positions of rays",
        "Screen space reflections",
        bUseDither.GetCurrentValue());
    //------------------------------------------------------

    // SSAO ------------------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "SSAO enable",
        "Toggle the rendering of screen space ambient occlusion",
        "SSAO",
        SSAO_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "SSAO sample radius",
        "Radius in which occluders are searched for",
        "SSAO",
        fSSAOSampleRadius.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "SSAO intensity",
        "Intensity of SSAO effect",
        "SSAO",
        fSSAOIntensity.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "SSAO scale",
        "Scale for the occlusion attenuation with distance",
        "SSAO",
        fSSAOScale.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "SSAO bias",
        "Bias for the occlusion attenuation with normal differences",
        "SSAO",
        fSSAOBias.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Quarter resolution SSAO",
        "Toggle rendering into a quarter resolution buffer",
        "SSAO",
        SSAO_USE_QUARTER_RESOLUTION_BUFFER);
    //------------------------------------------------------

    // RSM -------------------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Indirect lights enable",
        "Toggle the rendering of indirect lights",
        "Reflective shadow map",
        INDIRECT_LIGHT_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Intensity",
        "The intensity of the indirect light",
        "Reflective shadow map",
        fRSMIntensity.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Kernel scale",
        "Scale value for the kernel size for sampling the RSM",
        "Reflective shadow map",
        fRSMKernelScale.GetCurrentValue(),
        0.001f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Upscale threshold",
        "Affects the number of rejected pixels during upscaling",
        "Reflective shadow map",
        fWeightThreshold.GetCurrentValue(),
        0.001f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Debug upscale pass",
        "Draws rejected pixels with a red color",
        "Reflective shadow map",
        bDebugUpscalePass.GetCurrentValue());

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Quarter resolution RSM",
        "Toggle rendering into a quarter resolution buffer",
        "Reflective shadow map",
        RSM_USE_QUARTER_RESOLUTION_BUFFER);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Bilateral blur RSM",
        "Toggle blurring of the RSM accumulation buffer",
        "Reflective shadow map",
        RSM_USE_BILATERAL_BLUR);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Debug RSM camera",
        "Draw the reflective shadow map on-screen",
        "Reflective shadow map",
        DEBUG_RSM_CAMERA);
    //------------------------------------------------------

    // Sky -------------------------------------------------
    CREATE_ARTIST_PARAMETER_OBJECT(
        "Sun radius",
        "Affects the radius of the sun",
        "Sky",
        fSunRadius.GetCurrentValue(),
        10.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Sun brightness",
        "Affects the brightness of the sun",
        "Sky",
        fSunBrightness.GetCurrentValue(),
        10.f);
    //------------------------------------------------------

    // Volumetric lights -----------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Volumetric lights enable",
        "Toggle the rendering of the volumetric lighting effect (directional)",
        "Volumetric lights",
        DIR_LIGHT_VOLUME_ENABLE);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Accumulation sample count",
        "The number of samples taken across the ray's length",
        "Volumetric lights",
        nSampleCount.GetCurrentValue(),
        10.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Accumulation sample distribution",
        "< 1 means higher density of samples near the camera",
        "Volumetric lights",
        fSampleDistrib.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Accumulation intensity",
        "Intensity of the volumetric effect",
        "Volumetric lights",
        fLightIntensity.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Mult. scatter intensity",
        "Intensity of the faked multiple scattering effect",
        "Volumetric lights",
        fMultScatterIntensity.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Fog color - red",
        "Red color channel value for fog",
        "Volumetric lights",
        DIR_LIGHT_VOLUME_COLOR[0],
        0.01f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Fog color - green",
        "Green color channel value for fog",
        "Volumetric lights",
        DIR_LIGHT_VOLUME_COLOR[1],
        0.01f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Fog color - blue",
        "Blue color channel value for fog",
        "Volumetric lights",
        DIR_LIGHT_VOLUME_COLOR[2],
        0.01f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Fog vertical falloff",
        "Factor for the exponential vertical falloff of the fog effect",
        "Volumetric lights",
        fFogVerticalFalloff.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Fog speed X axis",
        "Speed of fog effect on the X axis (world space units / sec)",
        "Volumetric lights",
        f3FogSpeed.GetCurrentValue()[0],
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT("Fog speed Y axis",
        "Speed of fog effect on the Y axis (world space units / sec)",
        "Volumetric lights",
        f3FogSpeed.GetCurrentValue()[1],
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Fog speed Z axis",
        "Speed of fog effect on the Z axis (world space units / sec)",
        "Volumetric lights",
        f3FogSpeed.GetCurrentValue()[2],
        1.f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Blur samples",
        "Toggle the use of an additional blur pass",
        "Volumetric lights",
        DIR_LIGHT_VOLUME_BLUR_SAMPLES);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Depth-aware blur",
        "Make the blur pass depth-aware so as not to cause light bleeding",
        "Volumetric lights",
        DIR_LIGHT_VOLUME_BLUR_DEPTH_AWARE);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Blur depth falloff",
        "A scaling factor for the blur weights around edges",
        "Volumetric lights",
        fBlurDepthFalloff.GetCurrentValue(),
        0.0001f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Depth-aware upscaling",
        "Make the upscale pass depth-aware so as not to cause artifacts",
        "Volumetric lights",
        DIR_LIGHT_VOLUME_UPSCALE_DEPTH_AWARE);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Upsample depth threshold",
        "A threshold for edge detection used to reduce upscaling artifacts",
        "Volumetric lights",
        fUpsampleDepthThreshold.GetCurrentValue(),
        0.0001f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Quarter resolution accumulation",
        "Toggle the use of an intermediary quarter resolution target",
        "Volumetric lights",
        DIR_LIGHT_VOLUME_QUARTER_RES);
    //------------------------------------------------------

    // Post-processing -------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Post-processing enable",
        "Toggle post-processing effects",
        "Post-processing effects",
        POST_PROCESSING_ENABLED);
    //------------------------------------------------------

    // Bokeh DoF -------------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "DoF enable",
        "Toggle the rendering of the depth of field effect",
        "Depth of field",
        DOF_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Focal depth",
        "Focal distance value in meters (overridden by autofocus)",
        "Depth of field",
        fFocalDepth.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Focal length",
        "Focal length in mm",
        "Depth of field",
        fFocalLength.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "F-stop",
        "F-stop value",
        "Depth of field",
        fFStop.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Circle of confusion",
        "Circle of confusion size in mm (35mm film = 0.03mm)",
        "Depth of field",
        fCoC.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Aperture size",
        "Affects size of bokeh",
        "Depth of field",
        fApertureSize.GetCurrentValue(),
        0.001f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "DoF pass count",
        "The number of times to apply the DoF shader",
        "Depth of field",
        DOF_NUM_PASSES,
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Highlight threshold",
        "Brightness-pass filter threshold (higher = sparser bokeh)",
        "Depth of field",
        fHighlightThreshold.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Highlight gain",
        "Brightness gain (higher = more prominent bokeh)",
        "Depth of field",
        fHighlightGain.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Anamorphic bokeh",
        "Stretch bokeh effect like on an anamorphic lens",
        "Depth of field",
        bAnamorphicBokeh.GetCurrentValue());

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Autofocus",
        "Use autofocus",
        "Depth of field",
        bAutofocus.GetCurrentValue());

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Autofocus time",
        "Autofocus animation duration in seconds",
        "Depth of field",
        DOF_AUTOFOCUS_TIME,
        1.f);
    //------------------------------------------------------

    // Vignetting (part of DoF shader) ---------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Vignetting",
        "Optical lens vignetting effect",
        "Vignetting",
        bVignetting.GetCurrentValue());

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Vignetting out",
        "Vignetting outer border",
        "Vignetting",
        fVignOut.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Vignetting in",
        "Vignetting inner border",
        "Vignetting",
        fVignIn.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Vignetting fade",
        "F-stops until vignette fades",
        "Vignetting",
        fVignFade.GetCurrentValue(),
        1.f);
    //------------------------------------------------------

    // Chromatic aberration (part of DoF shader) -----------
    CREATE_ARTIST_PARAMETER_OBJECT(
        "Chroma shift amount",
        "The amount of chromatic separation",
        "Chromatic aberration",
        fChromaShiftAmount.GetCurrentValue(),
        0.1f);
    //------------------------------------------------------

    // Lens distortion (part of DoF shader) ----------------
    CREATE_ARTIST_PARAMETER_OBJECT(
        "K coefficient",
        "Quartic distortion coefficient",
        "Lens distortion",
        fQuarticDistortionCoef.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "K cube modifier",
        "Cubic distortion modifier",
        "Lens distortion",
        fCubicDistortionModifier.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Distortion scale",
        "Scales the screen to compensate for overscan/underscan",
        "Lens distortion",
        fDistortionScale.GetCurrentValue(),
        0.01f);
    //------------------------------------------------------

    // Motion blur -----------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Motion blur enable",
        "Toggle the rendering of the motion blur effect",
        "Motion blur",
        MOTION_BLUR_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Motion blur intensity",
        "The intensity of the motion blur effect",
        "Motion blur",
        fMotionBlurIntensity.GetCurrentValue(),
        0.01f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Motion blur sample count",
        "The number of samples along the velocity vector",
        "Motion blur",
        nMotionBlurNumSamples.GetCurrentValue(),
        1.f);
    //------------------------------------------------------

    // Bloom -----------------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Bloom enable",
        "Toggle the rendering of the bloom effect",
        "Bloom",
        BLOOM_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Bloom brightness threshold",
        "Threshold for the low-pass brightness filter",
        "Bloom",
        fBrightnessThreshold.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Bloom power",
        "Exponent of bloom intensity value",
        "Bloom",
        fBloomPower.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Bloom strength",
        "Strength of bloom intensity value",
        "Bloom",
        fBloomStrength.GetCurrentValue(),
        0.1f);
    //------------------------------------------------------

    // Lens flare ------------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Lens flare enable",
        "Toggle the rendering of the lens flare effect",
        "Lens flare",
        LENS_FLARE_ENABLED);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Anamorphic lens flare",
        "Choose between anamorphic and spherical lens flare",
        "Lens flare",
        LENS_FLARE_ANAMORPHIC);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Anamorphic intensity",
        "Adjust the intensity of the anamophic lens flares",
        "Lens flare",
        fAnamorphicIntensity.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Brigthness threshold",
        "Brightness threshold for lens flare feature generation",
        "Lens flare",
        LENS_FLARE_BRIGHTNESS_THRESHOLD,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Ghost sample count",
        "Number of samples for \"ghost\" features",
        "Lens flare",
        nGhostSamples.GetCurrentValue(),
        1.f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Ghost dispersion factor",
        "Dispersion factor (distance) for \"ghost\" features",
        "Lens flare",
        fGhostDispersal.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Ghost radial weight",
        "Falloff factor for bright spots that are near screen edges",
        "Lens flare",
        fGhostRadialWeightExp.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Halo size",
        "Halo size scale factor",
        "Lens flare",
        fHaloSize.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Halo radial weight",
        "Determines halo thickness",
        "Lens flare",
        fHaloRadialWeightExp.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Chroma shift enable",
        "Toggle the application of a chromatic aberration effect",
        "Lens flare",
        bChromaShift.GetCurrentValue());

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Chroma shift factor",
        "Intensity of chromatic aberration effect",
        "Lens flare",
        fShiftFactor.GetCurrentValue(),
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Lens dirt intensity",
        "Scale factor for lens dirt texture samples",
        "Lens flare",
        LENS_FLARE_PASS.DirtIntensity,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Lens star burst intensity",
        "Scale factor for lens star burst texture samples",
        "Lens flare",
        LENS_FLARE_PASS.StarBurstIntensity,
        0.1f);
    //------------------------------------------------------

    // Tone mapping ----------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Tone mapping enable",
        "Toggle tone mapping",
        "HDR tone mapping",
        HDR_TONE_MAPPING_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Exposure bias",
        "Scales color intensities before tone mapping",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.ExposureBias,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Lower luma bound",
        "Minimum average luma clamp used for exposure adjustment",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.AvgLumaClamp[0],
        0.01f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Upper luma bound",
        "Maximum average luma clamp used for exposure adjustment",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.AvgLumaClamp[1],
        0.01f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Shoulder strength",
        "Strength of the shoulder part of the filmic tone mapping curve",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.ShoulderStrength,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Linear strength",
        "Strength of the linear part of the filmic tone mapping curve",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.LinearStrength,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Linear angle",
        "Angle of the linear part of the filmic tone mapping curve",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.LinearAngle,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Toe strength",
        "Strength of the toe part of the filmic tone mapping curve",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.ToeStrength,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Toe numerator",
        "Numerator of the toe part of the filmic tone mapping curve",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.ToeNumerator,
        0.01f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Toe denominator",
        "Denominator of the toe part of the filmic tone mapping curve",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.ToeDenominator,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Linear white",
        "Reference linear white value of the filmic tone mapping curve",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.LinearWhite,
        0.1f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Exposure adapt speed",
        "Seconds in which the exposure adapts to scene brightness",
        "HDR tone mapping",
        HDR_TONE_MAPPING_PASS.LumaAdaptSpeed,
        0.1f);
    //------------------------------------------------------

    // Color correction (part of HDR tonemapping shader) ---
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Apply color correction",
        "Use the provided 3D color lookup table to do color correction",
        "Color correction",
        HDR_TONE_MAPPING_PASS.ApplyColorCorrection);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "sRGB color lookup table",
        "Apply gamma correction when sampling the 3D color lookup table",
        "Color correction",
        SRGB_COLOR_CORRECTION);
    //------------------------------------------------------

    // Film grain (part of HDR tonemapping shader) ---------
    CREATE_ARTIST_PARAMETER_OBJECT(
        "Film grain amount",
        "Amount of film grain applied to the image",
        "Film grain",
        HDR_TONE_MAPPING_PASS.FilmGrainAmount,
        0.001f);
    //------------------------------------------------------

    // FXAA ------------------------------------------------
    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "FXAA enable",
        "Toggle the FXAA filter",
        "FXAA",
        FXAA_ENABLED);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Antialiasing factor",
        "Amount of sub-pixel aliasing removal",
        "FXAA",
        FXAA_PASS.Subpix,
        0.01f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Edge threshold",
        "Minimum amount of local contrast to apply algorithm",
        "FXAA",
        FXAA_PASS.EdgeThreshold,
        0.01f);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Darkness threshold",
        "Keeps the algorithm from processing darks",
        "FXAA",
        FXAA_PASS.EdgeThresholdMin,
        0.01f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Apply FXAA only on edges",
        "Enables depth based edge detection for conditional FXAA application",
        "FXAA",
        FXAA_PASS.UseEdgeDetection);

    CREATE_ARTIST_PARAMETER_OBJECT(
        "Edge detection threshold",
        "Adjusts threshold of depth based edge detection algorithm",
        "FXAA",
        FXAA_PASS.EdgeDepthThreshold,
        0.0001f);

    CREATE_ARTIST_BOOLPARAM_OBJECT(
        "Debug FXAA edge detection",
        "Highlight pixels that have FXAA applied",
        "FXAA",
        FXAA_PASS.DebugEdgeDetection);
    //------------------------------------------------------
}
