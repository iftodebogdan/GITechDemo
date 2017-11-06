/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   ScreenSpaceReflection.hlsl
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
#include "BRDFUtils.hlsli"

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;
const float4x4 f44InvProjMat;

struct VSOut
{
    float4  f4Position  :   SV_POSITION;
    float2  f2TexCoord  :   TEXCOORD0;
    float3  f3ViewVec   :   TEXCOORD1;
};

void vsmain(float4 f4Position : POSITION, float2 f2TexCoord : TEXCOORD, out VSOut output)
{
    output.f4Position = f4Position;
    output.f2TexCoord = f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
    output.f3ViewVec = mul(f44InvProjMat, float4(f4Position.xy, 0.f, 1.f)).xyz;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D     texHDRSceneTexture; // Scene color buffer
const sampler2D     texDiffuseBuffer;   // Diffuse color
const sampler2D     texMaterialBuffer;  // Roughness and material type (metallic/dielectric)
const float4x4      f44ViewToRasterMat; // Projection matrix that maps to pixel coordinates
const sampler2D     texLinDepthBuffer;  // View space linear Z buffer
const sampler2D     texNormalBuffer;    // View space normal buffer
const float4        f4TexSize;          // xy: dimensions of scene/depth texture; zw: normalized texel size (1/xy)
const int           nTexMipCount;       // Number of mips the scene texture has

/*
    Screen Space Reflection shader.
    Based on the work of Morgan McGuire and Michael Mara:
    http://jcgt.org/published/0003/04/04/
*/

const float fReflectionIntensity;   // Scale value for reflected color from raymarching.
const float fThickness;             // Camera space thickness to ascribe to each pixel in the depth buffer.
const float fSampleStride;          // Step in horizontal or vertical pixels between samples. This is a float because integer math is slow on GPUs, but should be set to an integer >= 1.
const float fMaxSteps;              // Maximum number of iterations. Higher gives better images but may be slow.
const float fMaxRayDist;            // Maximum camera-space distance to trace before returning a miss.
const bool bUseDither;              // Use dithering on ray starting positions.

bool CastSSRRay(float3 f3RayOrigin, float3 f3RayDir, float fJitterAmount, out float2 f2HitTexelCoord, out float3 f3HitPoint);

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
    float2 f2HitTexelCoord; float3 f3HitPoint; f4Color = 0;

    const float     fDepth = tex2D(texLinDepthBuffer, input.f2TexCoord).r;
    DEPTH_KILL(fDepth, fZFar * 0.999f);

    const float3    f3MaterialColor     = tex2D(texDiffuseBuffer, input.f2TexCoord).rgb;
    const float2    f2Material          = tex2D(texMaterialBuffer, input.f2TexCoord).rg;
    const float     fMaterialType       = f2Material.r;
    const float     fRoughness          = f2Material.g;

    const float3    f3ViewDir           = normalize(input.f3ViewVec);
    const float3    f3Normal            = DecodeNormal(tex2D(texNormalBuffer, input.f2TexCoord));
    const float3    f3RayStartPosVS     = input.f3ViewVec * fDepth + f3Normal * max(0.01f * fDepth, 0.001f);
    const float3    f3ReflectedRayDir   = reflect(f3ViewDir, f3Normal);
    const float     fDitherAmount       = bUseDither ? GetDitherAmount(input.f2TexCoord, f4TexSize.xy) : 0.f;

    // Sample the environment map
    float3 f3EnvAlbedo = texCUBElod(texEnvMap, float4(mul((float3x3)f44InvViewMat, f3ReflectedRayDir), ComputeMipFromRoughness(fRoughness, ENVIRONMENT_MAP_MIP_COUNT))).rgb * fReflectionFactor;

    if (CastSSRRay(f3RayStartPosVS, f3ReflectedRayDir, fDitherAmount, f2HitTexelCoord, f3HitPoint))
    {
        const float fDistFromRayOrigin = length(f3HitPoint - f3RayStartPosVS);
        const float fNormalizedDistance = saturate(fDistFromRayOrigin / fMaxRayDist);
        const float fDistanceFadeFactor = 1.f - fNormalizedDistance * fNormalizedDistance;
        const float2 f2HitTexCoord = float2(f2HitTexelCoord * f4TexSize.zw);
        const float fScreenEdgeFactor = saturate((0.5f - max(abs(0.5f - f2HitTexCoord.x), abs(0.5f - f2HitTexCoord.y))) * 10.f);
        const float3 f3SSRColor = tex2Dlod(texHDRSceneTexture, float4(f2HitTexCoord, 0.f, ComputeMipFromRoughness(fRoughness, nTexMipCount))).rgb;

        // Mix reflection with environment map sample
        f3EnvAlbedo = lerp(
            f3EnvAlbedo,
            f3SSRColor * fReflectionIntensity,
            fDistanceFadeFactor * fScreenEdgeFactor);
    }

    const float3 f3SpecularAlbedo = lerp(0.03f, f3MaterialColor, fMaterialType);
    const float3 f3EnvFresnel = FresnelRoughnessTerm(f3SpecularAlbedo, fRoughness, f3Normal, f3ViewDir);

    f4Color = float4(f3EnvFresnel * f3EnvAlbedo, 1.f);
}
////////////////////////////////////////////////////////////////////

void swap(in out float a, in out float b) {
    float temp = a;
    a = b;
    b = temp;
}

float distanceSquared(float2 A, float2 B) {
    A -= B;
    return dot(A, A);
}

bool CastSSRRay(float3 f3RayOrigin, float3 f3RayDir, float fJitterAmount, out float2 f2HitTexelCoord, out float3 f3HitPoint)
{
    // Clip ray to a near plane in 3D (doesn't have to be *the* near plane, although that would be a good idea)
    float rayLength =
        ((f3RayOrigin.z + f3RayDir.z * fMaxRayDist) < fZNear) ?
        (f3RayOrigin.z - fZNear) / f3RayDir.z :
        fMaxRayDist;
    float3 csEndPoint = f3RayDir * rayLength + f3RayOrigin;

    // Project into screen space
    float4 H0 = mul(f44ViewToRasterMat, float4(f3RayOrigin.xy, f3RayOrigin.z, 1.0));
    float4 H1 = mul(f44ViewToRasterMat, float4(csEndPoint.xy, csEndPoint.z, 1.0));

    // There are a lot of divisions by w that can be turned into multiplications
    // at some minor precision loss...and we need to interpolate these 1/w values
    // anyway.
    //
    // Because the caller was required to clip to the near plane,
    // this homogeneous division (projecting from 4D to 2D) is guaranteed 
    // to succeed. 
    float k0 = 1.0 / H0.w;
    float k1 = 1.0 / H1.w;

    // Switch the original points to values that interpolate linearly in 2D
    float3 Q0 = f3RayOrigin * k0;
    float3 Q1 = csEndPoint * k1;

    // Screen-space endpoints
    float2 P0 = round(H0.xy * k0);
    float2 P1 = round(H1.xy * k1);

    /*
    // [Optional clipping to frustum sides here]
    float xMax=f2TexSize.x-0.5, xMin=0.5, yMax= f2TexSize.y-0.5, yMin=0.5;
    float alpha = 0.0;

    // Assume P0 is in the viewport (P1 - P0 is never zero when clipping)
    if ((P1.y > yMax) || (P1.y < yMin))
        alpha = (P1.y - ((P1.y > yMax) ? yMax : yMin)) / (P1.y - P0.y);

    if ((P1.x > xMax) || (P1.x < xMin))
        alpha = max(alpha, (P1.x - ((P1.x > xMax) ? xMax : xMin)) / (P1.x - P0.x));

    P1 = lerp(P1, P0, alpha); k1 = lerp(k1, k0, alpha); Q1 = lerp(Q1, Q0, alpha);
    */

    // Initialize to off screen
    f2HitTexelCoord = float2(-1.0, -1.0);

    // If the line is degenerate, make it cover at least one pixel
    // to avoid handling zero-pixel extent as a special case later
    P1 += ((distanceSquared(P0, P1) < 0.0001) ? 0.01 : 0.0).xx;

    float2 delta = P1 - P0;

    // Permute so that the primary iteration is in x to reduce
    // large branches later
    bool permute = false;
    if (abs(delta.x) < abs(delta.y))
    {
        // More-vertical line. Create a permutation that swaps x and y in the output
        permute = true;

        // Directly swizzle the inputs
        delta = delta.yx;
        P1 = P1.yx;
        P0 = P0.yx;
    }

    // From now on, "x" is the primary iteration direction and "y" is the secondary one

    float stepDirection = sign(delta.x);
    float invdx = stepDirection / delta.x;
    float2 dP = float2(stepDirection, invdx * delta.y);

    // Track the derivatives of Q and k
    float3 dQ = (Q1 - Q0) * invdx;
    float   dk = (k1 - k0) * invdx;

    // Scale derivatives by the desired pixel stride
    dP *= fSampleStride; dQ *= fSampleStride; dk *= fSampleStride;

    // Offset the starting values by the jitter fraction
    P0 += dP * fJitterAmount; Q0 += dQ * fJitterAmount; k0 += dk * fJitterAmount;

    // Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, and k from k0 to k1
    float3 Q = Q0;
    float  k = k0;

    // We track the ray depth at +/- 1/2 pixel to treat pixels as clip-space solid 
    // voxels. Because the depth at -1/2 for a given pixel will be the same as at 
    // +1/2 for the previous iteration, we actually only have to compute one value 
    // per iteration.
    float prevZMaxEstimate = f3RayOrigin.z;
    float stepCount = 0.0;
    float rayZMax = prevZMaxEstimate, rayZMin = prevZMaxEstimate;
    float sceneZMax = rayZMax + 1e4;

    // P1.x is never modified after this point, so pre-scale it by 
    // the step direction for a signed comparison
    float end = P1.x * stepDirection;

    // We only advance the z field of Q in the inner loop, since
    // Q.xy is never used until after the loop terminates.

    LOOP for (float2 P = P0;
        ((P.x * stepDirection) <= end) &&
        (stepCount < fMaxSteps) &&
        ((rayZMax > sceneZMax + fThickness) ||
        (rayZMin < sceneZMax)) &&
            (sceneZMax != 0.0);
        P += dP, Q.z += dQ.z, k += dk, stepCount += 1.0)
    {
        f2HitTexelCoord = permute ? P.yx : P;

        // The depth range that the ray covers within this loop
        // iteration.  Assume that the ray is moving in increasing z
        // and swap if backwards.  Because one end of the interval is
        // shared between adjacent iterations, we track the previous
        // value and then swap as needed to ensure correct ordering
        rayZMin = prevZMaxEstimate;

        // Compute the value at 1/2 pixel into the future
        rayZMax = (dQ.z * 0.5 + Q.z) / (dk * 0.5 + k);
        prevZMaxEstimate = rayZMax;

        if (rayZMin < rayZMax)
            swap(rayZMin, rayZMax);

        // Camera-space z of the background
        sceneZMax = tex2D(texLinDepthBuffer, f2HitTexelCoord * f4TexSize.zw).r;
    }

    Q.xy += dQ.xy * stepCount;
    f3HitPoint = Q * (1.0 / k);

    // Matches the new loop condition:
    return (rayZMax <= sceneZMax + fThickness) && (rayZMin >= sceneZMax) && length(f3HitPoint - f3RayOrigin) <= rayLength;
}
