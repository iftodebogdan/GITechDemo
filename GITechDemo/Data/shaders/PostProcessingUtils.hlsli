/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   PostProcessingUtils.hlsli
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

#ifndef POSTPROCESSINGUTILS_HLSLI
#define POSTPROCESSINGUTILS_HLSLI

#include "Common.hlsli"

TEXTURE_2D_RESOURCE(PostProcessing_DitherMap);   // PostProcessingUtils::InterleavedGridSize x PostProcessingUtils::InterleavedGridSize texture containing sample offsets

struct PostProcessingUtils
{
    // These must match dither map (Bayer matrix) texture dimensions
    static const unsigned int InterleavedGridSize = 8;
    static const unsigned int InterleavedGridSizeSqr = 64;

#ifdef HLSL
    //////////////////////////////////////////
    // Luma coefficients                    //
    // http://stackoverflow.com/a/24213274  //
    //////////////////////////////////////////
    static const float3 REC709LumaCoef = float3(0.2126f, 0.7152f, 0.0722f);
    static const float3 REC601LumaCoef = float3(0.299f, 0.587f, 0.114f);
    static const float3 LumaCoef = REC601LumaCoef;
    ////////////////////////////////////////////////////////////////

    static const float InterleavedGridSizeRcp = 0.125f;
    static const float InterleavedGridSizeSqrRcp = 0.015625f;
#endif
};

CBUFFER_RESOURCE(PostProcessing,
    GPU_float ZNear;
    GPU_float ZFar;
    GPU_float2 LinearDepthEquation;
    GPU_float DepthHalfTexelOffset; // Half texel offset of depth buffer
);

#ifdef HLSL
//////////////////////////////////////////////////////////
// Hardcoded mip level when sampling from textures to   //
// prevent the GPU from calculating ddx(), ddy() for    //
// every sample (post-processing effects work with      //
// textures with 1 mip level)                           //
//////////////////////////////////////////////////////////
#define tex2D(tex, tc) tex2Dlod(tex, float4((tc).xy, 0.f, 0.f))



//////////////////////////////////////////////////////////////////////////////////////////
// Reconstruct the depth in view space coordinates from the hyperbolic depth            //
//--------------------------------------------------------------------------------------//
// linearDepth = (zNear * zFar / (zNear - zFar)) / (depth - (zFar / (zFar - zNear)))    //
// linearDepthEquation.x = zNear * zFar / (zNear - zFar)                                //
// linearDepthEquation.y = zFar / (zFar - zNear)                                        //
//////////////////////////////////////////////////////////////////////////////////////////
float ReconstructDepth(float hyperbolicDepth)
{
    return PostProcessingParams.LinearDepthEquation.x / (hyperbolicDepth - PostProcessingParams.LinearDepthEquation.y);
}

//////////////////////////////////////////////////////////////////////////
// Downsample depth to a quarter of the original resolution             //
// NB: texCoord must be at the center of the quarter resolution texel   //
// (i.e. right at the middle of the 4 full resolution samples)          //
//////////////////////////////////////////////////////////////////////////
#define DEPTH_DOWNSAMPLE_FUNC(x, y) (max(x, y))
const float GetDownsampledDepth(const sampler2D texDepthBuffer, const float2 texCoord)
{
    const float2 sampleOffset[] =
    {
        float2(-1.f, -1.f),
        float2(-1.f,  1.f),
        float2( 1.f, -1.f),
        float2( 1.f,  1.f)
    };

    return
        DEPTH_DOWNSAMPLE_FUNC(
            DEPTH_DOWNSAMPLE_FUNC(
                tex2D(texDepthBuffer, texCoord + PostProcessingParams.DepthHalfTexelOffset * sampleOffset[0]).r,
                tex2D(texDepthBuffer, texCoord + PostProcessingParams.DepthHalfTexelOffset * sampleOffset[1]).r
                ),
            DEPTH_DOWNSAMPLE_FUNC(
                tex2D(texDepthBuffer, texCoord + PostProcessingParams.DepthHalfTexelOffset * sampleOffset[2]).r,
                tex2D(texDepthBuffer, texCoord + PostProcessingParams.DepthHalfTexelOffset * sampleOffset[3]).r
                )
            );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Kawase blur, approximating a 35x35 Gaussian kernel                                                                   //
// in the kernel = [0-1-2-2-3] format over 5 passes                                                                     //
// https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 KawaseBlur(const sampler2D texSource, const float2 texelSize, const float2 texCoord, const int kernel)
{
    float4 color = float4(0.f, 0.f, 0.f, 0.f);

    UNROLL for (int i = -1; i <= 1; i += 2)
    {
        UNROLL for (int j = -1; j <= 1; j += 2)
        {
            const float2 texelOffset = texelSize * float2(i, j);
            const float2 halfTexelOffset = 0.5f * texelOffset;
            const float2 halfTexelSize = 0.5f * texelSize;
            const float2 sampleCenter = texCoord + halfTexelOffset + texelOffset * (kernel + 0.5f);

            UNROLL for (int x = -1; x <= 1; x += 2)
                UNROLL for (int y = -1; y <= 1; y += 2)
                    color += tex2D(texSource, sampleCenter + halfTexelSize * float2(x, y));
        }
    }

    return color * 0.0625f;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////
// Downsampling helpers //
//////////////////////////
float4 Downsample4x4(sampler2D tex, float2 texCoord, float2 texelSize)
{
    float4 color = float4(0.f, 0.f, 0.f, 0.f);

    UNROLL for (float i = -1.5f; i <= 1.5f; i += 1.f)
        UNROLL for (float j = -1.5f; j <= 1.5f; j += 1.f)
        color += tex2D(tex, texCoord + texelSize * float2(i, j));

    return color * 0.0625f;
}

float4 Downsample2x2(sampler2D tex, float2 texCoord, float2 texelSize)
{
    float4 color = float4(0.f, 0.f, 0.f, 0.f);

    UNROLL for (float i = -0.5f; i <= 0.5f; i += 1.f)
        UNROLL for (float j = -0.5f; j <= 0.5f; j += 1.f)
            color += tex2D(tex, texCoord + texelSize * float2(i, j));

    return color * 0.25f;
}
////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////
// Bayer matrix stored in a texture, for dithering //
/////////////////////////////////////////////////////
float GetDitherAmount(float2 texCoord, float2 texSize)
{
    return tex2D(PostProcessing_DitherMap, texCoord * texSize * PostProcessingUtils::InterleavedGridSizeRcp).r * (255.f / (PostProcessingUtils::InterleavedGridSizeSqr - 1));
}

float GetDitherAmount(float2 texelIdx)
{
    return tex2D(PostProcessing_DitherMap, texelIdx * PostProcessingUtils::InterleavedGridSizeRcp).r * (255.f / (PostProcessingUtils::InterleavedGridSizeSqr - 1));
}

#endif // HLSL

#endif // POSTPROCESSINGUTILS_HLSLI
