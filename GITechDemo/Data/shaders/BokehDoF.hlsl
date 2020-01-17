/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   BokehDoF.hlsl
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

#include "PostProcessingUtils.hlsli"
#include "Utils.hlsli"

TEXTURE_2D_RESOURCE(BokehDoF_Source);       // Source texture to be blurred
TEXTURE_2D_RESOURCE(BokehDoF_DepthBuffer);  // Source depth buffer
TEXTURE_2D_RESOURCE(BokehDoF_TargetFocus);  // Used for autofocus

struct BokehDofUtils
{
    static const unsigned int ApertureBladeCount = 6;
};

CBUFFER_RESOURCE(BokehDoF,
    GPU_float2 HalfTexelOffset;
    GPU_float4 TexSize; // xy: size, in texels, of source image; zw: normalized size of a texel

    // Camera properties
    GPU_float FocalDepth;       // Focal distance value in meters (overridden by 'bAutofocus')
    GPU_float FocalLength;      // Focal length in mm
    GPU_float FStop;            // F-stop value
    GPU_float CoC;              // Circle of confusion size in mm (35mm film = 0.03mm)
    GPU_float ApertureSize;     // Determines size of bokeh
    GPU_bool  Autofocus;        // Overrides fFocalDepth value with value taken from depth buffer
    GPU_bool  AnamorphicBokeh;  // Stretch bokeh effect like on an anamorphic lens

    // Brightness filter (directly affects bokeh abundance and prominence)
    GPU_float HighlightThreshold;   // Brightness-pass filter threshold (higher = sparser bokeh)
    GPU_float HighlightGain;        // Brightness gain (higher = more prominent bokeh)

    // Vignetting effect
    GPU_bool  Vignetting;   //  Use optical lens vignetting
    GPU_float VignOut;      //  Vignetting outer border
    GPU_float VignIn;       //  Vignetting inner border
    GPU_float VignFade;     //  F-stops until vignette fades

    // Chromatic aberration
    GPU_float ChromaShiftAmount; // Color separation factor

    // Lens distortion
    GPU_float QuarticDistortionCoef;
    GPU_float CubicDistortionModifier;
    GPU_float DistortionScale;
);

#ifdef HLSL
struct VSOut
{
    float4  Position : SV_POSITION;
    float2  TexCoord : TEXCOORD0;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, float2 texCoord : TEXCOORD, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + BokehDoFParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
//------------------------------------------
/*
    Depth of Field with Bokeh shader

    Based on ideas gathered from several sources including GTA V, Prototype 2, Skylanders, etc.
    http://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study-part-3/
    https://mynameismjp.wordpress.com/2011/02/28/bokeh/
    http://casual-effects.blogspot.ro/2013/09/the-skylanders-swap-force-depth-of.html
*/
//------------------------------------------

void ApplyLensDistortion(in out float2 texCoord)
{
    if (BokehDoFParams.QuarticDistortionCoef != 0 || BokehDoFParams.CubicDistortionModifier != 0 || BokehDoFParams.DistortionScale != 0)
    {
        const float aspectRatioScale = BokehDoFParams.TexSize.y * BokehDoFParams.TexSize.z; // y / (1 / x)
        texCoord -= 0.5f;
        const float r2 = aspectRatioScale * aspectRatioScale * texCoord.x * texCoord.x + texCoord.y * texCoord.y;
        const float f = 1.f + r2 * (BokehDoFParams.QuarticDistortionCoef + BokehDoFParams.CubicDistortionModifier * sqrt(r2));
        texCoord *= f * BokehDoFParams.DistortionScale;
        texCoord += 0.5f;
    }
}

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    // Offsets for aperture blade shaped blur kernel
    const float2 kernelOffset[BokehDofUtils::ApertureBladeCount] =
    {
        sin(0.f / BokehDofUtils::ApertureBladeCount * 6.283f), cos(0.f / BokehDofUtils::ApertureBladeCount * 6.283f),
        sin(1.f / BokehDofUtils::ApertureBladeCount * 6.283f), cos(1.f / BokehDofUtils::ApertureBladeCount * 6.283f),
        sin(2.f / BokehDofUtils::ApertureBladeCount * 6.283f), cos(2.f / BokehDofUtils::ApertureBladeCount * 6.283f),
        sin(3.f / BokehDofUtils::ApertureBladeCount * 6.283f), cos(3.f / BokehDofUtils::ApertureBladeCount * 6.283f),
        sin(4.f / BokehDofUtils::ApertureBladeCount * 6.283f), cos(4.f / BokehDofUtils::ApertureBladeCount * 6.283f),
        sin(5.f / BokehDofUtils::ApertureBladeCount * 6.283f), cos(5.f / BokehDofUtils::ApertureBladeCount * 6.283f),
        //sin(6.f / BokehDofUtils::ApertureBladeCount * 6.283f), cos(6.f / BokehDofUtils::ApertureBladeCount * 6.283f),
        //sin(7.f / BokehDofUtils::ApertureBladeCount * 6.283f), cos(7.f / BokehDofUtils::ApertureBladeCount * 6.283f)
    };

    // Apply lens distortion equation to texture coordinates
    ApplyLensDistortion(input.TexCoord);

    // Initial sample
    const float4 centerSample = tex2D(BokehDoF_Source, input.TexCoord);
    float dofBlurFactor = centerSample.a * 2.f - 1.f;
    float3 accumSamples = centerSample.rgb;
    float weightTotal = 1.f;

    // Accumulate the rest of the samples, if required. Otherwise, just calculate blur factor and output it in alpha.
    if (BokehDoFParams.ApertureSize > 0.f)
    {
        // Fix aspect ratio of blur kernel
        const float aspectRatioScale = BokehDoFParams.TexSize.y * BokehDoFParams.TexSize.z;

        for (unsigned int i = 0; i < BokehDofUtils::ApertureBladeCount; i++)
        {
            // Retrieve sample color and CoC value
            const float4 sampleSrc = tex2D(BokehDoF_Source, input.TexCoord + kernelOffset[i] * float2(BokehDoFParams.AnamorphicBokeh ? 1.f / 2.4f : 1.f, 1.f) * BokehDoFParams.ApertureSize * dofBlurFactor * float2(aspectRatioScale, 1.f));
            const float sampleDofBlurFactor = sampleSrc.a * 2.f - 1.f;

            // Calculate sample weight
            const float luma   = dot(sampleSrc.rgb, PostProcessingUtils::LumaCoef);
            const float weight =
                max((luma > BokehDoFParams.HighlightThreshold) *            // Luma threshold for applying highlight gain (more prominent bokeh)
                BokehDoFParams.HighlightGain * abs(dofBlurFactor), 1.f) *   // Weight based on CoC values of center and sampled points,
                saturate(sampleDofBlurFactor * dofBlurFactor);              // saturated in order to avoid blending front and back out-of-focus areas

            // Accumulate current weighted sample
            accumSamples += sampleSrc.rgb * weight;
            weightTotal += weight;
        }
    }
    else
    {
        // Calculate the scene depth in world space coordinates
        const float linearDepth = tex2D(BokehDoF_DepthBuffer, input.TexCoord).r;

        // Calculate focal plane
        const float focalPoint = BokehDoFParams.Autofocus ? tex2D(BokehDoF_TargetFocus, float2(0.5f, 0.5f)).r : BokehDoFParams.FocalDepth;

        // Calculate DoF blur factor
        const float focalPointMm  = focalPoint  * 1000.f; // Focal point in mm
        const float linearDepthMm = linearDepth * 1000.f; // Linear depth in mm
        const float a = (linearDepthMm * BokehDoFParams.FocalLength) / (linearDepthMm - BokehDoFParams.FocalLength);
        const float b = (focalPointMm  * BokehDoFParams.FocalLength) / (focalPointMm  - BokehDoFParams.FocalLength);
        const float c = (focalPointMm  - BokehDoFParams.FocalLength) / (focalPointMm  * BokehDoFParams.FStop * BokehDoFParams.CoC);
        dofBlurFactor = saturate(abs(a - b) * c) * sign(linearDepth - focalPoint);

        // Apply chromatic aberration effect
        if (BokehDoFParams.ChromaShiftAmount > 0.f)
        {
            // The effect is more intense towards the exterior of the screen
            // NB: The distance of the screen coordinates from the screen center is normalized
            // by multiplying to the inverse of the length of the screen diagonal in normalized
            // screen space units: 
            // distance * (1.f / (0.5f * sqrt(2.f))) = distance * (2.f / sqrt(2.f)) = distance * sqrt(2.f) = distance * 1.414213562f
            const float chromaShiftFactor = smoothstep(0.f, 1.f, distance(input.TexCoord, float2(0.5f, 0.5f)) * 1.414213562f);

            // Apply chromatic aberration effect
            accumSamples.r = tex2D(BokehDoF_Source, input.TexCoord + chromaShiftFactor * float2(0.f, 1.f)       * BokehDoFParams.TexSize.zw * BokehDoFParams.ChromaShiftAmount * dofBlurFactor).r;
            accumSamples.g = tex2D(BokehDoF_Source, input.TexCoord + chromaShiftFactor * float2(-0.866f, -0.5f) * BokehDoFParams.TexSize.zw * BokehDoFParams.ChromaShiftAmount * dofBlurFactor).g;
            accumSamples.b = tex2D(BokehDoF_Source, input.TexCoord + chromaShiftFactor * float2(0.866f, -0.5f)  * BokehDoFParams.TexSize.zw * BokehDoFParams.ChromaShiftAmount * dofBlurFactor).b;
        }

        // Apply vignetting
        if (BokehDoFParams.Vignetting)
        {
            const float fadeFactor = BokehDoFParams.FStop / BokehDoFParams.VignFade;
            accumSamples *= smoothstep(BokehDoFParams.VignOut + fadeFactor, BokehDoFParams.VignIn + fadeFactor, distance(input.TexCoord, float2(0.5f, 0.5f)));
        }
    }

    // Output color
    color = float4(accumSamples / weightTotal, dofBlurFactor * 0.5f + 0.5f);
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
