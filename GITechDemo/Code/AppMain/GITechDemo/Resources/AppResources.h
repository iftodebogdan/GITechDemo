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

#define TEXTURE_1D_RESOURCE(textureName) CREATE_SHADER_CONSTANT_HANDLE(textureName, s3dSampler1D)
#define TEXTURE_2D_RESOURCE(textureName) CREATE_SHADER_CONSTANT_HANDLE(textureName, s3dSampler2D)
#define TEXTURE_3D_RESOURCE(textureName) CREATE_SHADER_CONSTANT_HANDLE(textureName, s3dSampler3D)
#define TEXTURE_CUBE_RESOURCE(textureName) CREATE_SHADER_CONSTANT_HANDLE(textureName, s3dSamplerCUBE)
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
    /*
    // Shader constants
    //  - General purpose
    CREATE_SHADER_CONSTANT_HANDLE(f44WorldMat,              Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44ViewMat,               Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44InvViewMat,            Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44ProjMat,               Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44InvProjMat,            Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44WorldViewMat,          Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44ViewProjMat,           Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44InvViewProjMat,        Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44WorldViewProjMat,      Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44LightViewMat,          Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44LightViewProjMat,      Matrix44f*      );
    CREATE_SHADER_CONSTANT_HANDLE(f44LightWorldViewProjMat, Matrix44f*      );
    CREATE_SHADER_CONSTANT_HANDLE(f44ScreenToLightViewMat,  Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(fFrameTime,               float           );
    CREATE_SHADER_CONSTANT_HANDLE(fZNear,                   float           );
    CREATE_SHADER_CONSTANT_HANDLE(fZFar,                    float           );

    //  - Multiple appearances
    CREATE_SHADER_CONSTANT_HANDLE(f2HalfTexelOffset,        Vec2f           );
    CREATE_SHADER_CONSTANT_HANDLE(f4TexSize,                Vec4f           );
    CREATE_SHADER_CONSTANT_HANDLE(texSource,                s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texDiffuse,               s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texDepthBuffer,           s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texHypDepthBuffer,        s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texLinDepthBuffer,        s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texQuarterDepthBuffer,    s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texNormalBuffer,          s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texShadowMap,             s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texLumaInput,             s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(f3LightDir,               Vec3f           );
    CREATE_SHADER_CONSTANT_HANDLE(nKernel,                  int             );
    CREATE_SHADER_CONSTANT_HANDLE(bSingleChannelCopy,       bool            );
    CREATE_SHADER_CONSTANT_HANDLE(f4CustomColorModulator,   Vec4f           );
    CREATE_SHADER_CONSTANT_HANDLE(nTexMipCount,             int             );
    CREATE_SHADER_CONSTANT_HANDLE(bApplyTonemap,            bool            );

    //  - GBufferGeneration.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(texNormal,                s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(bHasNormalMap,            bool            );
    CREATE_SHADER_CONSTANT_HANDLE(texSpec,                  s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(bHasSpecMap,              bool            );
    CREATE_SHADER_CONSTANT_HANDLE(texMatType,               s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texRoughness,             s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(fSpecIntensity,           float           );

    //  - DirectionalLight.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(texMaterialBuffer,        s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texIrradianceMap,         s3dSamplerCUBE  );
    CREATE_SHADER_CONSTANT_HANDLE(texEnvMap,                s3dSamplerCUBE  );
    CREATE_SHADER_CONSTANT_HANDLE(nBRDFModel,               int             );
    CREATE_SHADER_CONSTANT_HANDLE(texDiffuseBuffer,         s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(fAmbientFactor,           float           );
    CREATE_SHADER_CONSTANT_HANDLE(fIrradianceFactor,        float           );
    CREATE_SHADER_CONSTANT_HANDLE(fReflectionFactor,        float           );
    CREATE_SHADER_CONSTANT_HANDLE(f2OneOverShadowMapSize,   Vec2f           );
    CREATE_SHADER_CONSTANT_HANDLE(fDiffuseFactor,           float           );
    CREATE_SHADER_CONSTANT_HANDLE(fSpecFactor,              float           );
    CREATE_SHADER_CONSTANT_HANDLE(bDebugCascades,           bool            );

    //  - Skybox.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(f44SkyViewProjMat,        Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(texSkyCube,               s3dSamplerCUBE  );
    CREATE_SHADER_CONSTANT_HANDLE(fSunRadius,               float           );
    CREATE_SHADER_CONSTANT_HANDLE(fSunBrightness,           float           );

    //  - CSMUtils.hlslii
    CREATE_SHADER_CONSTANT_HANDLE(f2CascadeBoundsMin,       Vec2f*          );
    CREATE_SHADER_CONSTANT_HANDLE(f2CascadeBoundsMax,       Vec2f*          );
    CREATE_SHADER_CONSTANT_HANDLE(f44CascadeProjMat,        Matrix44f*      );
    CREATE_SHADER_CONSTANT_HANDLE(fCascadeBlendSize,        float           );

    //  - Utils.hlsli
    CREATE_SHADER_CONSTANT_HANDLE(f2PoissonDisk,            Vec2f*          );

    //  - DirectionalLightVolume.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(f44InvLightViewMat,       Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f3CameraPositionLightVS,  Vec3f           );
    CREATE_SHADER_CONSTANT_HANDLE(fRaymarchDistanceLimit,   float           );
    CREATE_SHADER_CONSTANT_HANDLE(fLightIntensity,          float           );
    CREATE_SHADER_CONSTANT_HANDLE(fMultScatterIntensity,    float           );
    CREATE_SHADER_CONSTANT_HANDLE(texDitherMap,             s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texNoise,                 s3dSampler3D    );
    CREATE_SHADER_CONSTANT_HANDLE(fElapsedTime,             float           );
    CREATE_SHADER_CONSTANT_HANDLE(f3FogBox,                 Vec3f           );
    CREATE_SHADER_CONSTANT_HANDLE(f3FogSpeed,               Vec3f           );
    CREATE_SHADER_CONSTANT_HANDLE(fFogVerticalFalloff,      float           );
    CREATE_SHADER_CONSTANT_HANDLE(nSampleCount,             int             );

    //  - RSMCapture.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(f44LightWorldViewMat,     Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44RSMWorldViewProjMat,   Matrix44f       );

    //  - RSMCommon.hlsli
    CREATE_SHADER_CONSTANT_HANDLE(texRSMFluxBuffer,         s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texRSMNormalBuffer,       s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(texRSMDepthBuffer,        s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(f3RSMKernel,              Vec3f*          );
    CREATE_SHADER_CONSTANT_HANDLE(f44RSMProjMat,            Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44RSMInvProjMat,         Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(f44ViewToRSMViewMat,      Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(fRSMIntensity,            float           );
    CREATE_SHADER_CONSTANT_HANDLE(fRSMKernelScale,          float           );

    //  - RSMUpscale.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(fWeightThreshold,         float           );
    CREATE_SHADER_CONSTANT_HANDLE(bDebugUpscalePass,        bool            );

    //  - LumaCapture.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(bInitialLumaPass,         bool            );
    CREATE_SHADER_CONSTANT_HANDLE(bFinalLumaPass,           bool            );
    CREATE_SHADER_CONSTANT_HANDLE(f2AvgLumaClamp,           Vec2f           );

    //  - LumaAdapt.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(fLumaAdaptSpeed,          float           );
    CREATE_SHADER_CONSTANT_HANDLE(texLumaTarget,            s3dSampler2D    );

    //  - Downsample.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(fBrightnessThreshold,     float           );
    CREATE_SHADER_CONSTANT_HANDLE(bApplyBrightnessFilter,   bool            );
    CREATE_SHADER_CONSTANT_HANDLE(nDownsampleFactor,        int             );
    CREATE_SHADER_CONSTANT_HANDLE(bDepthDownsample,         bool            );
    CREATE_SHADER_CONSTANT_HANDLE(bReconstructDepth,        bool            );

    //  - Bloom.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(fBloomPower,              float           );
    CREATE_SHADER_CONSTANT_HANDLE(fBloomStrength,           float           );
    CREATE_SHADER_CONSTANT_HANDLE(bAdjustIntensity,         bool            );

    //  - SSAO.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(fSSAOSampleRadius,        float           );
    CREATE_SHADER_CONSTANT_HANDLE(fSSAOIntensity,           float           );
    CREATE_SHADER_CONSTANT_HANDLE(fSSAOScale,               float           );
    CREATE_SHADER_CONSTANT_HANDLE(fSSAOBias,                float           );

    //  - BokehDoF.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(fFocalDepth,              float           );
    CREATE_SHADER_CONSTANT_HANDLE(fFocalLength,             float           );
    CREATE_SHADER_CONSTANT_HANDLE(fFStop,                   float           );
    CREATE_SHADER_CONSTANT_HANDLE(fCoC,                     float           );
    CREATE_SHADER_CONSTANT_HANDLE(bAutofocus,               bool            );
    CREATE_SHADER_CONSTANT_HANDLE(texTargetFocus,           s3dSampler2D    );
    CREATE_SHADER_CONSTANT_HANDLE(fApertureSize,            float           );
    CREATE_SHADER_CONSTANT_HANDLE(fHighlightThreshold,      float           );
    CREATE_SHADER_CONSTANT_HANDLE(fHighlightGain,           float           );
    CREATE_SHADER_CONSTANT_HANDLE(bAnamorphicBokeh,         bool            );
    CREATE_SHADER_CONSTANT_HANDLE(bVignetting,              bool            );
    CREATE_SHADER_CONSTANT_HANDLE(fVignOut,                 float           );
    CREATE_SHADER_CONSTANT_HANDLE(fVignIn,                  float           );
    CREATE_SHADER_CONSTANT_HANDLE(fVignFade,                float           );
    CREATE_SHADER_CONSTANT_HANDLE(fChromaShiftAmount,       float           );
    CREATE_SHADER_CONSTANT_HANDLE(fQuarticDistortionCoef,   float           );
    CREATE_SHADER_CONSTANT_HANDLE(fCubicDistortionModifier, float           );
    CREATE_SHADER_CONSTANT_HANDLE(fDistortionScale,         float           );

    //  - PostProcessingUtils.hlsli
    CREATE_SHADER_CONSTANT_HANDLE(f2LinearDepthEquation,    Vec2f           );
    CREATE_SHADER_CONSTANT_HANDLE(f2DepthHalfTexelOffset,   Vec2f           );

    //  - MotionBlur.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(f44PrevViewProjMat,       Matrix44f       );
    CREATE_SHADER_CONSTANT_HANDLE(fMotionBlurIntensity,     float           );
    CREATE_SHADER_CONSTANT_HANDLE(nMotionBlurNumSamples,    int             );

    //  - SphericalLensFlareFeatures.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(texGhostColorLUT,         s3dSampler1D    );
    CREATE_SHADER_CONSTANT_HANDLE(nGhostSamples,            int             );
    CREATE_SHADER_CONSTANT_HANDLE(fGhostDispersal,          float           );
    CREATE_SHADER_CONSTANT_HANDLE(fGhostRadialWeightExp,    float           );
    CREATE_SHADER_CONSTANT_HANDLE(fHaloSize,                float           );
    CREATE_SHADER_CONSTANT_HANDLE(fHaloRadialWeightExp,     float           );
    CREATE_SHADER_CONSTANT_HANDLE(bChromaShift,             bool            );
    CREATE_SHADER_CONSTANT_HANDLE(fShiftFactor,             float           );

    //  - LensFlareApply.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(LensFlareFeatures, s3dSampler2D);
    CREATE_SHADER_CONSTANT_HANDLE(LensFlareDirt, s3dSampler2D);
    CREATE_SHADER_CONSTANT_HANDLE(LensFlareStarBurst, s3dSampler2D);
    //CREATE_SHADER_CONSTANT_HANDLE(LensFlareApplyParams, LensFlareApplyConstantTable);

    //  - BilateralBlur.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(f2BlurDir,                Vec2f           );
    CREATE_SHADER_CONSTANT_HANDLE(fBlurDepthFalloff,        float           );

    //  - NearestDepthUpscale.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(fUpsampleDepthThreshold, float);

    //  - ScreenSpaceReflection.hlsl
    CREATE_SHADER_CONSTANT_HANDLE(texHDRSceneTexture, s3dSampler2D);
    CREATE_SHADER_CONSTANT_HANDLE(fThickness, float);
    CREATE_SHADER_CONSTANT_HANDLE(fSampleStride, float);
    CREATE_SHADER_CONSTANT_HANDLE(fMaxSteps, float);
    CREATE_SHADER_CONSTANT_HANDLE(fMaxRayDist, float);
    CREATE_SHADER_CONSTANT_HANDLE(f44ViewToRasterMat, Matrix44f);
    CREATE_SHADER_CONSTANT_HANDLE(bUseDither, bool);
    CREATE_SHADER_CONSTANT_HANDLE(fReflectionIntensity, float);
    */
    // Used for fullscreen effects, post-processing, etc.
    extern VertexBuffer*    FullScreenTri;
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
