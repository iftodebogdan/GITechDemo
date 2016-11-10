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

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;

struct VSOut
{
    float4  f4Position  :   SV_POSITION;
    float2  f2TexCoord  :   TEXCOORD0;
};

void vsmain(float4 f4Position : POSITION, float2 f2TexCoord : TEXCOORD, out VSOut output)
{
    output.f4Position = f4Position;
    output.f2TexCoord = f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D texSource;      // The texture to be blurred
const sampler2D texDepthBuffer; // Scene depth

const float2 f2BlurDir;             // Horizontal or vertical blur
const float4 f4TexSize;             // zw: normalized size of a texel

// Depth threshold for edge detection
const float fBlurDepthFalloff;

// Number of samples on each side of the kernel
#define NUM_SAMPLES_HALF    (7)

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
    const float fGaussianFilterWeight[] =
    {
        0.14446445f, 0.13543542f,
        0.11153505f, 0.08055309f,
        0.05087564f, 0.02798160f,
        0.01332457f, 0.00545096f
    };

    f4Color = float4(0.f, 0.f, 0.f, 1.f);
    float fTotalWeight = 0.f;

    // Get reference downsampled depth (center of kernel)
    const float fRefDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;

    UNROLL for (int i = -NUM_SAMPLES_HALF; i <= NUM_SAMPLES_HALF; i++)
    {
        // Calculate coordinates for sampling source texture
        const float2 f2Offset = i * f2BlurDir * f4TexSize.zw;
        const float2 f2SampleTexCoord = input.f2TexCoord + f2Offset;

        // Sample source color
        const float3 f3SampleColor = tex2D(texSource, f2SampleTexCoord).rgb;
        // Calculate downsampled depth for the above sample
        const float fSampleDepth = tex2D(texDepthBuffer, f2SampleTexCoord).r;

        // Simple depth-aware filtering
        const float fDepthDiff = fBlurDepthFalloff * abs(fSampleDepth - fRefDepth);
        const float fWeight = exp(-fDepthDiff * fDepthDiff) * fGaussianFilterWeight[abs(i)];

        f4Color.rgb += fWeight * f3SampleColor;
        fTotalWeight += fWeight;
    }

    f4Color.rgb /= fTotalWeight;

    // Attempt to save some bandwidth
    clip(-!any(f4Color.rgb));
}
////////////////////////////////////////////////////////////////////
