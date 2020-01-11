/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   NearestDepthUpscale.hlsl
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

TEXTURE_2D_RESOURCE(NearestDepthUpscale_Source);                // The texture to be upsampled
TEXTURE_2D_RESOURCE(NearestDepthUpscale_DepthBuffer);           // Scene depth values
TEXTURE_2D_RESOURCE(NearestDepthUpscale_QuarterDepthBuffer);    // Quarter resolution scene depth values

CBUFFER_RESOURCE(NearestDepthUpscale,
    GPU_float2 HalfTexelOffset;

    GPU_float4 TexSize; // xy: size of source texture; zw: normalized texel size

    // Set a threshold which controls the level of sensitivity of the edge detection.
    GPU_float UpsampleDepthThreshold;

    // True if volumetric light accumulation buffer has a single channel
    GPU_bool SingleChannelCopy;

    // Used to color the volumetric fog
    GPU_float4 CustomColorModulator;
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
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + NearestDepthUpscaleParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
// Retrieves the coordinates for each the 4 bilinear samples corresponding to the current fragment
const float2 GetBilinearSampleCoords(const float2 texCoord, const int idx);

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Based on "Nearest-Depth Filter" from the paper                                                       //
    // "Fast rendering of opacity mapped particles using DirectX 11 tessellation and mixed resolutions":    //
    // http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/OpacityMappingSDKWhitePaper.pdf     //
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Avoid blocky artefacts by using bilinear upsampling,
    // if the current fragment is not located on an edge
    color = float4(tex2D(NearestDepthUpscale_Source, input.TexCoord).rgb, 1.f);

    // If the sample is black we can safely clip this
    // pixel in an attempt to save some bandwidth
    clip(-!any(color.rgb));

    // Fetch the reference depth for the current high-resolution fragment
    const float refDepth = tex2D(NearestDepthUpscale_DepthBuffer, input.TexCoord).r;

    float minDepthDiff = 1.f;
    int nearestDepthIndex = 0;
    bool edgeSample = false;

    UNROLL for (int i = 0; i < 4; i++)
    {
        // Fetch the downsampled depth of the i-th bilinear sample corresponding to the current fragment
        const float sampleDepth = tex2D(NearestDepthUpscale_QuarterDepthBuffer, GetBilinearSampleCoords(input.TexCoord, i)).r;

        // Calculate the difference to the reference depth value
        const float currentDepthDiff = abs(sampleDepth - refDepth);

        // Reject the sample, if required
        edgeSample = edgeSample || (currentDepthDiff > NearestDepthUpscaleParams.UpsampleDepthThreshold);

        // Mark the best match we've gotten so far
        if (currentDepthDiff < minDepthDiff)
        {
            minDepthDiff = currentDepthDiff;
            nearestDepthIndex = i;
        }
    }

    // For edges, simply use the color of the low resolution sample that
    // best matches the full resolution depth value for the current pixel
    if (edgeSample)
        color.rgb = tex2D(NearestDepthUpscale_Source, GetBilinearSampleCoords(input.TexCoord, nearestDepthIndex)).rgb;

    if (NearestDepthUpscaleParams.SingleChannelCopy)
        color.rgb = color.r;

    color *= NearestDepthUpscaleParams.CustomColorModulator;
}

const float2 GetBilinearSampleCoords(const float2 texCoord, const int idx)
{
    const float2 sampleOffset[] =
    {
        float2(-1.f, -1.f),
        float2( 1.f, -1.f),
        float2(-1.f,  1.f),
        float2( 1.f,  1.f)
    };

    // Calculate the offset for the center of the 4 bilinear samples
    // from the low-resolution texture corresponding to the high-resolution
    // fragment, relative to the current high-resolution fragment's coordinates
    const float2 centerOffset = (step(float2(0.5f, 0.5f), frac(texCoord * NearestDepthUpscaleParams.TexSize.xy)) * 2.f - 1.f) * NearestDepthUpscaleParams.HalfTexelOffset;

    // Offset the current coordinates to the center of the 4 bilinear samples, then offset
    // it again to the center of the bilinear sample corresponding to the provided index
    return texCoord + centerOffset + NearestDepthUpscaleParams.TexSize.zw * 0.5f * sampleOffset[idx];
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
