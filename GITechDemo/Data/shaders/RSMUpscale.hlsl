/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   RSMUpscale.hlsl
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

// This define will modify the behaviour of the RSMApply() function so that it will
// perform all RSM_NUM_PASSES * RSM_SAMPLES_PER_PASS texture fetches in a single pass
// (As opposed to the apply pass which is a multi-pass approach).
#define RSM_UPSCALE_PASS (1)
#include "RSMCommon.hlsli"

struct RSMUpscaleConstantTable
{
    GPU_float2 HalfTexelOffset;
    GPU_float WeightThreshold; // Set a threshold which controls the level of sensitivity of the edge detection.
    GPU_bool DebugUpscalePass; // Shows pixels that could not be interpolated and need reshading
};

#ifdef HLSL
cbuffer RSMUpscaleResourceTable
{
    sampler2D RSMUpscale_Source;      // The texture to be upsampled
    sampler2D RSMUpscale_DepthBuffer; // G-Buffer depth values

    RSMUpscaleConstantTable RSMUpscaleParams;
};

struct VSOut
{
    float4 Position :   SV_POSITION;
    float2 TexCoord :   TEXCOORD0;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + RSMUpscaleParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
bool PerformUpscale(const float2 texCoord, const float depth, out float4 colorOut);

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    color = float4(0.f, 0.f, 0.f, 1.f);

    // Early depth test so that we don't shade
    // at the far plane (where the sky is drawn)
    const float depth = tex2D(RSMUpscale_DepthBuffer, input.TexCoord).r;
    DEPTH_KILL(depth, 1.f);

    // If we have successfully interpolated this pixel there is no need to reshade it.
    if (!PerformUpscale(input.TexCoord, depth, color))
    {
        ApplyRSM(input.TexCoord, depth, color);

        if (RSMUpscaleParams.DebugUpscalePass)
            color += float4(1.f, 0.f, 0.f, 0.f);
    }
}

bool PerformUpscale(const float2 texCoord, const float depth, out float4 colorOut)
{
    // Sample the color from the quarter-resolution render target
    const float4 color = tex2D(RSMUpscale_Source, texCoord);

    // If the sample is black we can safely clip this
    // pixel in an attempt to save some bandwidth
    clip(-!any(color.rgb));

    // Easier to write f3TexelOffset.xz or f3TexelOffset.zy than
    // float2(0.5f * f2HalfTexelOffset.x, 0.f) or float2(0.f, 0.5f * f2HalfTexelOffset.y);
    // NB: f2HalfTexelOffset is half the width and half the height of a texel from the
    // IndirectLightAccumulationBuffer render target (quarter resolution). As such, we require
    // half of that width and half of that height in order to offset our coordinates from the
    // center of a quarter resolution texel to the center of a full resolution texel.
    const float3 texelOffset = float3(0.5f * RSMUpscaleParams.HalfTexelOffset, 0.f);

    // Sample the normal for our reference pixel (i.e. the one we're shading)
    const float3 refNormal = DecodeNormal(tex2D(RSMCommon_NormalBuffer, texCoord));
    // Sample the neighbours' normals
    const float3 neighbourNormalN = DecodeNormal(tex2D(RSMCommon_NormalBuffer, texCoord - texelOffset.zy));
    const float3 neighbourNormalS = DecodeNormal(tex2D(RSMCommon_NormalBuffer, texCoord + texelOffset.zy));
    const float3 neighbourNormalW = DecodeNormal(tex2D(RSMCommon_NormalBuffer, texCoord - texelOffset.xz));
    const float3 neighbourNormalE = DecodeNormal(tex2D(RSMCommon_NormalBuffer, texCoord + texelOffset.xz));

    // Sample the depth for our reference pixel
    const float refDepth = depth;
    // Sample the neighbours' depths
    const float neighbourDepthN = tex2D(RSMUpscale_DepthBuffer, texCoord - texelOffset.zy).r;
    const float neighbourDepthS = tex2D(RSMUpscale_DepthBuffer, texCoord + texelOffset.zy).r;
    const float neighbourDepthW = tex2D(RSMUpscale_DepthBuffer, texCoord - texelOffset.xz).r;
    const float neighbourDepthE = tex2D(RSMUpscale_DepthBuffer, texCoord + texelOffset.xz).r;

    // Calculate a weight based on normal differences between the reference pixel and its neighbours
    const float weightNormalN = pow(saturate(dot(refNormal, neighbourNormalN)), 32);
    const float weightNormalS = pow(saturate(dot(refNormal, neighbourNormalS)), 32);
    const float weightNormalW = pow(saturate(dot(refNormal, neighbourNormalW)), 32);
    const float weightNormalE = pow(saturate(dot(refNormal, neighbourNormalE)), 32);

    // Calculate a weight based on depth differences between the reference pixel and its neighbours
    const float weightDepthN = rcp(abs(refDepth - neighbourDepthN) + 0.00001f);
    const float weightDepthS = rcp(abs(refDepth - neighbourDepthS) + 0.00001f);
    const float weightDepthW = rcp(abs(refDepth - neighbourDepthW) + 0.00001f);
    const float weightDepthE = rcp(abs(refDepth - neighbourDepthE) + 0.00001f);

    // Put all weights into a float4
    const float4 weight =
        normalize(float4(
            weightNormalN * weightDepthN,
            weightNormalS * weightDepthS,
            weightNormalW * weightDepthW,
            weightNormalE * weightDepthE));

    // If none of the weights fall below the threshold, or if the pixel is black,
    // then it is valid and it doesn't require reshading using the RSM algorithm.
    if (all(saturate(weight - RSMUpscaleParams.WeightThreshold.xxxx)) || !any(color.rgb))
    {
        colorOut = color;
        return true;
    }

    return false;
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
