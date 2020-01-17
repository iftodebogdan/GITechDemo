/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   BilateralBlur.hlsl
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

TEXTURE_2D_RESOURCE(BilateralBlur_Source);      // The texture to be blurred
TEXTURE_2D_RESOURCE(BilateralBlur_DepthBuffer); // Scene depth

struct BilateralBlurUtils
{
    static const unsigned int SampleCount = 15;
    static const unsigned int HalfSampleCount = 7;// Number of samples on each side of the kernel
};

CBUFFER_RESOURCE(BilateralBlur,
    GPU_float2 HalfTexelOffset;
    GPU_float2 BlurDir;         // Horizontal or vertical blur
    GPU_float4 TexSize;         // zw: normalized size of a texel
    GPU_float BlurDepthFalloff; // Depth threshold for edge detection
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
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + BilateralBlurParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 color : SV_TARGET)
{
    const float gaussianFilterWeight[] =
    {
        0.14446445f, 0.13543542f,
        0.11153505f, 0.08055309f,
        0.05087564f, 0.02798160f,
        0.01332457f, 0.00545096f
    };

    color = float4(0.f, 0.f, 0.f, 1.f);
    float totalWeight = 0.f;

    // Get reference downsampled depth (center of kernel)
    const float refDepth = tex2D(BilateralBlur_DepthBuffer, input.TexCoord).r;

    UNROLL for (int i = -int(BilateralBlurUtils::HalfSampleCount); i <= int(BilateralBlurUtils::HalfSampleCount); i++)
    {
        // Calculate coordinates for sampling source texture
        const float2 offset = i * BilateralBlurParams.BlurDir * BilateralBlurParams.TexSize.zw;
        const float2 sampleTexCoord = input.TexCoord + offset;

        // Sample source color
        const float3 sampleColor = tex2D(BilateralBlur_Source, sampleTexCoord).rgb;
        // Calculate downsampled depth for the above sample
        const float sampleDepth = tex2D(BilateralBlur_DepthBuffer, sampleTexCoord).r;

        // Simple depth-aware filtering
        const float depthDiff = BilateralBlurParams.BlurDepthFalloff * abs(sampleDepth - refDepth);
        const float weight = exp(-depthDiff * depthDiff) * gaussianFilterWeight[abs(i)];

        color.rgb += weight * sampleColor;
        totalWeight += weight;
    }

    color.rgb /= totalWeight;

    // Attempt to save some bandwidth
    clip(-!any(color.rgb));
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
