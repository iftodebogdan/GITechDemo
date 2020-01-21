/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   AppResources.h
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

#ifndef APP_RESOURCES_H_
#define APP_RESOURCES_H_

#include <Renderer.h>
#include "RenderResource.h"

//////////////////////////
// Some handy defines   //
//////////////////////////
#define CREATE_MODEL_HANDLE(NAME) extern GITechDemoApp::Model NAME
#define CREATE_TEXTURE_HANDLE(NAME) extern GITechDemoApp::Texture NAME
#define CREATE_SHADER_HANDLE(NAME) extern GITechDemoApp::Shader NAME
#define CREATE_SHADER_CONSTANT_HANDLE(NAME, TYPE) extern GITechDemoApp::ShaderConstantTemplate<TYPE> NAME;
#define CREATE_RENDER_TARGET_HANDLE(NAME) extern GITechDemoApp::RenderTarget NAME
#define SWAP_RENDER_TARGET_HANDLES(RT1, RT2) { GITechDemoApp::RenderTarget* const TEMP = RT1; RT1 = RT2; RT2 = TEMP; }

#define TEXTURE_1D_RESOURCE(TEX_NAME) CREATE_SHADER_CONSTANT_HANDLE(TEX_NAME, s3dSampler1D)
#define TEXTURE_2D_RESOURCE(TEX_NAME) CREATE_SHADER_CONSTANT_HANDLE(TEX_NAME, s3dSampler2D)
#define TEXTURE_3D_RESOURCE(TEX_NAME) CREATE_SHADER_CONSTANT_HANDLE(TEX_NAME, s3dSampler3D)
#define TEXTURE_CUBE_RESOURCE(TEX_NAME) CREATE_SHADER_CONSTANT_HANDLE(TEX_NAME, s3dSamplerCUBE)
#define CBUFFER_RESOURCE(CBUFFER_NAME, CBUFFER_BODY) \
struct CBUFFER_NAME##ConstantTable \
{ \
    CBUFFER_BODY \
}; \
CREATE_SHADER_CONSTANT_HANDLE(CBUFFER_NAME##Params, CBUFFER_NAME##ConstantTable)

// Only include Shaders.h in translation units other than AppResources.cpp or we won't be able
// to repurpose the XXX_RESOURCE() macros above to actually declare shader constant and textures.
// This will provide shader constants definitions.
#ifndef INCLUDED_FROM_APP_RESOURCES_CPP
#include "Shaders.h"
#endif // INCLUDED_SHADERS_H

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
    CREATE_SHADER_HANDLE(DepthPassAlphaTestShader);
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
    CREATE_SHADER_HANDLE(UIShader);
    CREATE_SHADER_HANDLE(MotionBlurShader);
    CREATE_SHADER_HANDLE(SphericalLensFlareFeaturesShader);
    CREATE_SHADER_HANDLE(LensFlareApplyShader);
    CREATE_SHADER_HANDLE(DirectionalLightVolumeShader);
    CREATE_SHADER_HANDLE(BilateralBlurShader);
    CREATE_SHADER_HANDLE(NearestDepthUpscaleShader);
    CREATE_SHADER_HANDLE(AnamorphicLensFlareFeaturesShader);
    CREATE_SHADER_HANDLE(AnamorphicLensFlareBlurShader);
    CREATE_SHADER_HANDLE(ScreenSpaceReflectionShader);
    //------------------------------------------------------

    // Models (manage their own textures, no need to declare them)
    CREATE_MODEL_HANDLE(SponzaScene);
    CREATE_MODEL_HANDLE(SphereModel);
    //------------------------------------------------------

    // Textures (not referenced by models, i.e. custom/utility textures, etc.)
    CREATE_TEXTURE_HANDLE(SkyTexture);
    CREATE_TEXTURE_HANDLE(IrradianceTexture);
    CREATE_TEXTURE_HANDLE(EnvironmentTexture);
    CREATE_TEXTURE_HANDLE(LensFlareGhostColorLUT);
    CREATE_TEXTURE_HANDLE(LensFlareDirtTexture);
    CREATE_TEXTURE_HANDLE(LensFlareStarBurstTexture);
    CREATE_TEXTURE_HANDLE(BayerMatrix);
    CREATE_TEXTURE_HANDLE(NoiseTexture);
    CREATE_TEXTURE_HANDLE(ColorCorrectionTexture);
    //------------------------------------------------------

    // Render targets
    CREATE_RENDER_TARGET_HANDLE(GBuffer);
    CREATE_RENDER_TARGET_HANDLE(HyperbolicQuarterDepthBuffer);
    CREATE_RENDER_TARGET_HANDLE(LinearFullDepthBuffer);
    CREATE_RENDER_TARGET_HANDLE(LinearQuarterDepthBuffer);
    CREATE_RENDER_TARGET_HANDLE(ShadowMapDir);
    CREATE_RENDER_TARGET_HANDLE(LightAccumulationBuffer);
    CREATE_RENDER_TARGET_HANDLE(RSMBuffer);
    CREATE_RENDER_TARGET_HANDLE(IndirectLightAccumulationBuffer0);
    CREATE_RENDER_TARGET_HANDLE(IndirectLightAccumulationBuffer1);
    CREATE_RENDER_TARGET_HANDLE(VolumetricLightFullBuffer0);
    CREATE_RENDER_TARGET_HANDLE(VolumetricLightFullBuffer1);
    CREATE_RENDER_TARGET_HANDLE(VolumetricLightQuarterBuffer0);
    CREATE_RENDER_TARGET_HANDLE(VolumetricLightQuarterBuffer1);
    CREATE_RENDER_TARGET_HANDLE(HDRDownsampleQuarterBuffer);
    CREATE_RENDER_TARGET_HANDLE(HDRDownsampleSixteenthBuffer);
    CREATE_RENDER_TARGET_HANDLE(HDRDownsampleForBloomBuffer);
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
    extern RenderTarget* IndirectLightAccumulationBuffer[2];
    //------------------------------------------------------

    // Used for fullscreen effects, post-processing, etc.
    extern VertexBuffer*    FullScreenTri;
    //------------------------------------------------------

    // Various configurable parameters not used directly in shaders
    struct RenderConfig
    {
        struct Window
        {
            static bool Fullscreen;
            static bool Borderless;
            static Vec2i Resolution;
            static int RefreshRate;
            static bool VSync;
        };

        struct Camera
        {
            static bool InfiniteProjection;
            static float FoV;
            static float MoveSpeed;
            static float SpeedUpFactor;
            static float SlowDownFactor;
            static float RotationSpeed;
            static bool Animation;
            static int AnimationTimeout;
        };

        struct Scene
        {
            static AABoxf WorldSpaceAABB;
            static AABoxf LightSpaceAABB;
        };

        struct GBuffer
        {
            static bool ZPrepass;
            static int DiffuseAnisotropy;
            static bool UseNormalMaps;
            static bool DrawAlphaTestGeometry;
            static int DebugViewColor;
            static bool DebugViewDepth;
        };

        struct DirectionalLight
        {
            static bool Animation;
            static bool Enabled;
            static float ReflectionFactor;
        };

        struct CascadedShadowMaps
        {
            static bool DebugCameraView;
            static float SplitFactor;
            static float MaxViewDepth;
            static const unsigned int PCFMaxSampleCount = 16;
            static const Vec2i ShadowMapSize;
            static float DepthBias[];
            static float SlopeScaledDepthBias[];
        };

        struct ReflectiveShadowMap
        {
            static bool DebugCameraView;
            static bool Enabled;
            static bool QuarterResolution;
            static bool BilateralBlur;
            static const unsigned int RSMSize = 1024;
        };

        struct DirectionalLightVolume
        {
            static bool Enabled;
            static bool QuarterResolution;
            static bool BlurSamples;
            static bool DepthAwareBlur;
            static float BlurDepthFalloff;
            static bool DepthAwareUpscale;
            static Vec4f LightColor;
        };

        struct PostProcessing
        {
            static bool Enabled;

            struct ScreenSpaceReflections
            {
                static bool Enabled;
                static bool ManualMaxSteps;
            };

            struct ScreenSpaceAmbientOcclusion
            {
                static bool Enabled;
                static bool QuarterResolution;
                static const unsigned int BlurKernelCount = 3;
                static const unsigned int BlurKernel[BlurKernelCount];
            };

            struct DepthOfField
            {
                static bool Enabled;
                static float AutofocusTime;
                static int PassCount;
                static float ApertureSize;
            };

            struct MotionBlur
            {
                static bool Enabled;
            };

            struct Bloom
            {
                static bool Enabled;
                static const unsigned int BlurKernelCount = 9;
                static const unsigned int BlurKernel[BlurKernelCount];
                static float BrightnessThreshold;
            };

            struct LensFlare
            {
                static bool Enabled;
                static float BrightnessThreshold;
                static bool Anamorphic;
                static const unsigned int BlurKernelCount = 3;
                static const unsigned int BlurKernel[BlurKernelCount];
                static const unsigned int AnamorphicBlurPassCount = 6;
            };

            struct ToneMapping
            {
                static bool Enabled;
                static bool sRGBColorCorrectionTexture;
                static float AutoExposureSpeed;
            };

            struct FastApproximateAntiAliasing
            {
                static bool Enabled;
            };
        };
    };
    //------------------------------------------------------
}

///////////////////////////////////////////////////////////
#undef CREATE_MODEL_HANDLE
#undef CREATE_TEXTURE_HANDLE
#undef CREATE_SHADER_HANDLE
#undef CREATE_SHADER_CONSTANT_HANDLE
#undef CREATE_RENDER_TARGET_HANDLE

#undef TEXTURE_1D_RESOURCE
#undef TEXTURE_2D_RESOURCE
#undef TEXTURE_3D_RESOURCE
#undef TEXTURE_CUBE_RESOURCE
#undef CBUFFER_RESOURCE
///////////////////////////////////////////////////////////

#endif // APP_RESOURCES_H_
