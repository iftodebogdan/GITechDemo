/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Utils.hlsli
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

#include "Common.hlsli"

//#define NORMAL_RECONSTRUCT_Z
//#define NORMAL_SPHERICAL_COORDINATES
//#define NORMAL_SPHEREMAP_TRANSFORM
#define NORMAL_STEREOGRAPHIC_PROJECTION

//////////////////////////////////////////////////////////////////
// Normal encoding / decoding using Z coordinate reconstruction //
//////////////////////////////////////////////////////////////////

#ifdef NORMAL_RECONSTRUCT_Z

float4 EncodeNormal(float3 n)
{
    return float4(n.xy * 0.5f + 0.5f, 0.f, 0.f);
}

float3 DecodeNormal(float4 enc)
{
    float3 n;
    n.xy = enc.xy * 2.f - 1.f;
    n.z = -sqrt(1.f - dot(n.xy, n.xy));
    return normalize(n);
}

#endif

////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// Normal encoding / decoding using spherical coordinates   //
// http://www.garagegames.com/community/blogs/view/15340    //
//////////////////////////////////////////////////////////////

#ifdef NORMAL_SPHERICAL_COORDINATES

float4 EncodeNormal(float3 n)
{
    return float4((float2(atan2(n.y, n.x) / PI, n.z) + 1.f) * 0.5f, 0.f, 0.f);
}

float3 DecodeNormal(float4 enc)
{
    float2 ang = enc.xy * 2.f - 1.f;
    float2 scth;
    sincos(ang.x * PI, scth.x, scth.y);
    float2 scphi = half2(sqrt(1.0 - ang.y*ang.y), ang.y);
    return normalize(float3(scth.y*scphi.x, scth.x*scphi.x, scphi.y));
}

#endif

////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Normal encoding / decoding using spherical environment mapping           //
// https://en.wikipedia.org/wiki/Lambert_azimuthal_equal-area_projection    //
//////////////////////////////////////////////////////////////////////////////

#ifdef NORMAL_SPHEREMAP_TRANSFORM

float4 EncodeNormal(float3 n)
{
    float f = sqrt(8.f * n.z + 8.f);
    return float4(n.xy / f + 0.5f, 0.f, 0.f);
}

float3 DecodeNormal(float4 enc)
{
    float2 fenc = enc.xy * 4.f - 2.f;
    float f = dot(fenc, fenc);
    float g = sqrt(1.f - f / 4.f);
    float3 n;
    n.xy = fenc * g;
    n.z = 1.f - f / 2.f;
    return normalize(n);
}

#endif

////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// Normal encoding / decoding using Stereographic Projection    //
// http://en.wikipedia.org/wiki/Stereographic_projection        //
//////////////////////////////////////////////////////////////////

#ifdef NORMAL_STEREOGRAPHIC_PROJECTION

float4 EncodeNormal(float3 n)
{
    n = normalize(n);
    const float scale = rcp(1.7777f);
    float2 enc = n.xy / (n.z + 1.f);
    enc *= scale;
    enc = enc * 0.5f + 0.5f;
    return float4(enc, 0.f, 0.f);
}

float3 DecodeNormal(float4 enc)
{
    const float scale = 1.7777f;
    const float3 nn = enc.xyz * float3(2.f * scale, 2.f * scale, 0.f) + float3(-scale, -scale, 1.f);
    const float g = 2.f / dot(nn.xyz, nn.xyz);
    const float3 n = float3(g * nn.xy, g - 1.f);
    return normalize(n);
}

#endif

////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////
// Random Number Generator                          //
// Source: http://stackoverflow.com/a/10625698      //
// Input: texture coords as the random number seed. //
// Output: Random number: [0,1)                     //
//////////////////////////////////////////////////////
float GenerateRandomNumber(float2 p)
{
    const float2 r = float2(
        23.1406926327792690,
        2.6651441426902251);
    return frac(cos(fmod(123456789., 1e-7 + 256. * dot(p, r))));
}
//////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////
// Percentage Closer Filtering variations                       //
// http://http.developer.nvidia.com/GPUGems/gpugems_ch11.html   //
//////////////////////////////////////////////////////////////////

float NoPCF(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    return tex2D(shadowMap, texCoord.xy).r > depthCompare;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Poisson Disk kernel                                                                                                      //
// https://electronicmeteor.wordpress.com/2013/02/05/poisson-disc-shadow-sampling-ridiculously-easy-and-good-looking-too/   //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float2 f2PoissonDisk[16];

float PCF2x2Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    float percentLit = 0.f;

    UNROLL for (int i = 0; i < 4; i++)
    {
        const bool isLit =
            tex2D(
            shadowMap,
            texCoord +
            f2PoissonDisk[i] * oneOverShadowMapSize
            ).r > depthCompare;
        percentLit += isLit;
    }

    return percentLit * 0.25f;
}

float PCF3x3Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    float percentLit = 0.f;

    UNROLL for (int i = 0; i < 9; i++)
    {
        const bool isLit =
            tex2D(
            shadowMap,
            texCoord +
            f2PoissonDisk[i] * oneOverShadowMapSize
            ).r > depthCompare;
        percentLit += isLit;
    }

    return percentLit * 0.1111111f;
}

float PCF12TapPoisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    float percentLit = 0.f;

    UNROLL for (int i = 0; i < 12; i++)
    {
        const bool isLit =
            tex2D(
            shadowMap,
            texCoord +
            f2PoissonDisk[i] * oneOverShadowMapSize
            ).r > depthCompare;
        percentLit += isLit;
    }

    return percentLit * 0.0833333f;
}

float PCF4x4Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    float percentLit = 0.f;

    UNROLL for (int i = 0; i < 16; i++)
    {
        const bool isLit =
            tex2D(
            shadowMap,
            texCoord +
            f2PoissonDisk[i] * oneOverShadowMapSize
            ).r > depthCompare;
        percentLit += isLit;
    }

    return percentLit * 0.0625f;
}

// Wow, this looks pretty good
float PCF4x4PoissonRotatedx4(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    float percentLit = 0.f;

    for (int i = 0; i < 16; i++)
    {
        int isLit =
            tex2D(
                shadowMap,
                texCoord +
                f2PoissonDisk[i].xy * oneOverShadowMapSize
                ).r > depthCompare;
        isLit +=
            tex2D(
                shadowMap,
                texCoord +
                float2(1.f - f2PoissonDisk[i].y, f2PoissonDisk[i].x) * oneOverShadowMapSize
                ).r > depthCompare;
        isLit +=
            tex2D(
                shadowMap,
                texCoord +
                (float2(1.f, 1.f) - f2PoissonDisk[i].xy) * oneOverShadowMapSize
                ).r > depthCompare;
        isLit +=
            tex2D(
                shadowMap,
                texCoord +
                float2(f2PoissonDisk[i].y, 1.f - f2PoissonDisk[i].x) * oneOverShadowMapSize
                ).r > depthCompare;
        percentLit += isLit;
    }
    
    return percentLit * 0.015625f;
}

//////////////////////////
// Dithered patterns    //
//////////////////////////
float PCF3x3Dithered(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    // use modulo to vary the sample pattern
    float2 offset = floor(texCoord.xy) % 3.0;
    
    float percentLit = 0.f;
    UNROLL for(int x = -1; x <= 1; x++)
        UNROLL for(int y = -1; y <= 1; y++)
        {
            const bool isLit =
                tex2D(
                    shadowMap,
                    texCoord +
                    float2(x, y) * oneOverShadowMapSize +
                    offset
                ).r > depthCompare;
            percentLit += isLit;
        }

    return percentLit * 0.111111f;
}

float PCF5x5Dithered(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    // use modulo to vary the sample pattern
    float2 offset = floor(texCoord.xy) % 5.0;
    
    float percentLit = 0.f;
    UNROLL for(int x = -2; x <= 2; x++)
        UNROLL for(int y = -2; y <= 2; y++)
        {
            const bool isLit =
                tex2D(
                    shadowMap,
                    texCoord +
                    float2(x, y) * oneOverShadowMapSize +
                    offset
                ).r > depthCompare;
            percentLit += isLit;
        }

    return percentLit * 0.04f;
}

float PCF8x8Dithered(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    // use modulo to vary the sample pattern
    float2 offset = floor(texCoord.xy) % 8.0;

    float x, y, percentLit = 0.f;
    UNROLL for (y = -3.5f; y <= 3.5f; y += 1.0f)
        UNROLL for (x = -3.5f; x <= 3.5f; x += 1.0f)
            percentLit += tex2D(shadowMap, texCoord + float2(x, y) * oneOverShadowMapSize + offset).r > depthCompare;

    return percentLit * 0.015625f;
}

//////////////////////
// Gaussian kernel  //
//////////////////////
float PCF5x5Gaussian(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    const float GaussianKernel[5][5] =
    {
        0.00296901674395065f, 0.013306209891014005f, 0.02193823127971504f, 0.013306209891014005f, 0.00296901674395065f, 
        0.013306209891014005f, 0.05963429543618023f, 0.09832033134884507f, 0.05963429543618023f, 0.013306209891014005f,
        0.02193823127971504f, 0.09832033134884507f, 0.16210282163712417f, 0.09832033134884507f, 0.02193823127971504f,
        0.013306209891014005f, 0.05963429543618023f, 0.09832033134884507f, 0.05963429543618023f, 0.013306209891014005f,
        0.00296901674395065f, 0.013306209891014005f, 0.02193823127971504f, 0.013306209891014005f, 0.00296901674395065f
    };
    float percentLit = 0.f;
    
    UNROLL for(int x = -2; x <= 2; x++)
        UNROLL for(int y = -2; y <= 2; y++)
        {
            const bool isLit =
                tex2D(
                    shadowMap,
                    texCoord +
                    float2(x, y) * oneOverShadowMapSize
                ).r > depthCompare;
            percentLit += isLit * GaussianKernel[x+2][y+2];
        }

    return percentLit;
}

//////////////////
// Regular PCF  //
//////////////////
float PCF3x3(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    float percentLit = 0.f;

    UNROLL for (int x = -1; x <= 1; x++)
        UNROLL for (int y = -1; y <= 1; y++)
        {
            const bool isLit =
                tex2D(
                shadowMap,
                texCoord +
                float2(x, y) * oneOverShadowMapSize
                ).r > depthCompare;
            percentLit += isLit;
        }

    return percentLit * 0.111111f;
}

float PCF8x8(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
    float percentLit = 0.f;
    UNROLL for (float y = -3.5f ; y <=3.5f ; y += 1.0f)
        UNROLL for (float x = -3.5f ; x <=3.5f ; x += 1.0f)
            percentLit += tex2D(shadowMap, texCoord + float2(x, y) * oneOverShadowMapSize).r > depthCompare;
    
    return percentLit * 0.015625f;
}


// This is safer than the previously used saturate(dot(x, y)) because it avoids divisions by zero
#define safe_saturate_dot(x, y) (clamp(dot(x, y), 0.0001f, 0.9999f))

////////////////////////////////////////////////////////////////
