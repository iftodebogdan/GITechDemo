#ifndef SHADERS_H_
#define SHADERS_H_
namespace GITechDemoApp
{
    namespace HLSL
    {
        #include "BokehDoF.hlsl"
        #include "DirectionalLightVolume.hlsl"
        #include "NearestDepthUpscale.hlsl"
        #include "LumaCapture.hlsl"
        #include "Downsample.hlsl"
        #include "MotionBlur.hlsl"
        #include "LumaAdapt.hlsl"
        #include "Bloom.hlsl"
        #include "GBufferGeneration.hlsl"
        #include "DepthPassAlphaTest.hlsl"
        #include "DepthPass.hlsl"
        #include "BilateralBlur.hlsl"
        #include "DepthCopy.hlsl"
        #include "RSMApply.hlsl"
        #include "RSMCapture.hlsl"
        #include "RSMUpscale.hlsl"
        #include "ScreenSpaceReflection.hlsl"
        #include "DirectionalLight.hlsl"
        #include "Skybox.hlsl"
        #include "SphericalLensFlareFeatures.hlsl"
        #include "SSAO.hlsl"
        #include "AnamorphicLensFlareBlur.hlsl"
        #include "AnamorphicLensFlareFeatures.hlsl"
        #include "LensFlareApply.hlsl"
        #include "HDRToneMapping.hlsl"
        #include "FXAA.hlsl"
        #include "ColorCopy.hlsl"
        #include "UI.hlsl"
    }
}
#endif // SHADERS_H_
