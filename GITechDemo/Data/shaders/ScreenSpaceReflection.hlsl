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

TEXTURE_2D_RESOURCE(ScreenSpaceReflection_HDRSceneTexture); // Scene color buffer
TEXTURE_2D_RESOURCE(ScreenSpaceReflection_DiffuseBuffer);   // Diffuse color
TEXTURE_2D_RESOURCE(ScreenSpaceReflection_MaterialBuffer);  // Roughness and material type (metallic/dielectric)
TEXTURE_2D_RESOURCE(ScreenSpaceReflection_LinDepthBuffer);  // View space linear Z buffer
TEXTURE_2D_RESOURCE(ScreenSpaceReflection_NormalBuffer);    // View space normal buffer

CBUFFER_RESOURCE(ScreenSpaceReflection,
    GPU_float4 TexSize;   // xy: dimensions of scene/depth texture; zw: normalized texel size (1/xy)
    GPU_int TexMipCount;  // Number of mips the scene texture has

    GPU_float ReflectionIntensity;   // Scale value for reflected color from raymarching.
    GPU_float Thickness;             // Camera space thickness to ascribe to each pixel in the depth buffer.
    GPU_float SampleStride;          // Step in horizontal or vertical pixels between samples. This is a float because integer math is slow on GPUs, but should be set to an integer >= 1.
    GPU_float MaxSteps;              // Maximum number of iterations. Higher gives better images but may be slow.
    GPU_float MaxRayDist;            // Maximum camera-space distance to trace before returning a miss.
    GPU_bool UseDither;              // Use dithering on ray starting positions.

    GPU_float4x4 ViewToRasterMat; // Projection matrix that maps to pixel coordinates
);

#ifdef HLSL
struct VSOut
{
    float4  Position  :   SV_POSITION;
    float2  TexCoord  :   TEXCOORD0;
    float3  ViewVec   :   TEXCOORD1;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, float2 texCoord : TEXCOORD, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    output.ViewVec = mul(FrameParams.InvProjMat, float4(position.xy, 0.f, 1.f)).xyz;

    PatchVSOutputPositionForHalfPixelOffset(output.Position);
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
/*
    Screen Space Reflection shader.
    Based on the work of Morgan McGuire and Michael Mara:
    http://jcgt.org/published/0003/04/04/
*/

bool CastSSRRay(float3 rayOrigin, float3 rayDir, float jitterAmount, out float2 hitTexelCoord, out float3 hitPoint);

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    float2 hitTexelCoord; float3 hitPoint; color = 0;

    const float depth = tex2D(ScreenSpaceReflection_LinDepthBuffer, input.TexCoord).r;
    DEPTH_KILL(depth, PostProcessingParams.ZFar * 0.999f);

    const float3    materialColor   = tex2D(ScreenSpaceReflection_DiffuseBuffer, input.TexCoord).rgb;
    const float2    material        = tex2D(ScreenSpaceReflection_MaterialBuffer, input.TexCoord).rg;
    const float     materialType    = material.r;
    const float     roughness       = material.g;

    const float3 viewDir           = normalize(input.ViewVec);
    const float3 normal            = DecodeNormal(tex2D(ScreenSpaceReflection_NormalBuffer, input.TexCoord));
    const float3 rayStartPosVS     = input.ViewVec * depth + normal * max(0.01f * depth, 0.001f);
    const float3 reflectedRayDir   = reflect(viewDir, normal);
    const float ditherAmount       = ScreenSpaceReflectionParams.UseDither ? GetDitherAmount(input.TexCoord, ScreenSpaceReflectionParams.TexSize.xy) : 0.f;

    // Sample the environment map
    float3 envAlbedo = texCUBElod(BRDF_EnvMap, float4(mul((float3x3)BRDFParams.InvViewMat, reflectedRayDir), ComputeMipFromRoughness(roughness, BRDF::EnvMapMipCount))).rgb * BRDFParams.ReflectionFactor;

    if (CastSSRRay(rayStartPosVS, reflectedRayDir, ditherAmount, hitTexelCoord, hitPoint))
    {
        const float distFromRayOrigin = length(hitPoint - rayStartPosVS);
        const float normalizedDistance = saturate(distFromRayOrigin / ScreenSpaceReflectionParams.MaxRayDist);
        const float distanceFadeFactor = 1.f - normalizedDistance * normalizedDistance;
        const float2 hitTexCoord = float2(hitTexelCoord * ScreenSpaceReflectionParams.TexSize.zw);
        const float screenEdgeFactor = saturate((0.5f - max(abs(0.5f - hitTexCoord.x), abs(0.5f - hitTexCoord.y))) * 10.f);
        const float3 SSRColor = tex2Dlod(ScreenSpaceReflection_HDRSceneTexture, float4(hitTexCoord, 0.f, ComputeMipFromRoughness(roughness, ScreenSpaceReflectionParams.TexMipCount))).rgb;

        // Mix reflection with environment map sample
        envAlbedo = lerp(
            envAlbedo,
            SSRColor * ScreenSpaceReflectionParams.ReflectionIntensity,
            distanceFadeFactor * screenEdgeFactor);
    }

    const float3 specularAlbedo = lerp(0.03f, materialColor, materialType);
    const float3 envFresnel = FresnelRoughnessTerm(specularAlbedo, roughness, normal, viewDir);

    color = float4(envFresnel * envAlbedo, 1.f);
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

bool CastSSRRay(float3 rayOrigin, float3 rayDir, float jitterAmount, out float2 hitTexelCoord, out float3 hitPoint)
{
    // Clip ray to a near plane in 3D (doesn't have to be *the* near plane, although that would be a good idea)
    float rayLength =
        ((rayOrigin.z + rayDir.z * ScreenSpaceReflectionParams.MaxRayDist) < PostProcessingParams.ZNear) ?
        (rayOrigin.z - PostProcessingParams.ZNear) / rayDir.z :
        ScreenSpaceReflectionParams.MaxRayDist;
    float3 csEndPoint = rayDir * rayLength + rayOrigin;

    // Project into screen space
    float4 H0 = mul(ScreenSpaceReflectionParams.ViewToRasterMat, float4(rayOrigin.xy, rayOrigin.z, 1.0));
    float4 H1 = mul(ScreenSpaceReflectionParams.ViewToRasterMat, float4(csEndPoint.xy, csEndPoint.z, 1.0));

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
    float3 Q0 = rayOrigin * k0;
    float3 Q1 = csEndPoint * k1;

    // Screen-space endpoints
    float2 P0 = round(H0.xy * k0);
    float2 P1 = round(H1.xy * k1);

    /*
    // [Optional clipping to frustum sides here]
    float xMax=ScreenSpaceReflectionParams.TexSize.x-0.5, xMin=0.5, yMax= ScreenSpaceReflectionParams.TexSize.y-0.5, yMin=0.5;
    float alpha = 0.0;

    // Assume P0 is in the viewport (P1 - P0 is never zero when clipping)
    if ((P1.y > yMax) || (P1.y < yMin))
        alpha = (P1.y - ((P1.y > yMax) ? yMax : yMin)) / (P1.y - P0.y);

    if ((P1.x > xMax) || (P1.x < xMin))
        alpha = max(alpha, (P1.x - ((P1.x > xMax) ? xMax : xMin)) / (P1.x - P0.x));

    P1 = lerp(P1, P0, alpha); k1 = lerp(k1, k0, alpha); Q1 = lerp(Q1, Q0, alpha);
    */

    // Initialize to off screen
    hitTexelCoord = float2(-1.0, -1.0);

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
    dP *= ScreenSpaceReflectionParams.SampleStride; dQ *= ScreenSpaceReflectionParams.SampleStride; dk *= ScreenSpaceReflectionParams.SampleStride;

    // Offset the starting values by the jitter fraction
    P0 += dP * jitterAmount; Q0 += dQ * jitterAmount; k0 += dk * jitterAmount;

    // Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, and k from k0 to k1
    float3 Q = Q0;
    float  k = k0;

    // We track the ray depth at +/- 1/2 pixel to treat pixels as clip-space solid 
    // voxels. Because the depth at -1/2 for a given pixel will be the same as at 
    // +1/2 for the previous iteration, we actually only have to compute one value 
    // per iteration.
    float prevZMaxEstimate = rayOrigin.z;
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
        (stepCount < ScreenSpaceReflectionParams.MaxSteps) &&
        ((rayZMax > sceneZMax + ScreenSpaceReflectionParams.Thickness) ||
        (rayZMin < sceneZMax)) &&
            (sceneZMax != 0.0);
        P += dP, Q.z += dQ.z, k += dk, stepCount += 1.0)
    {
        hitTexelCoord = permute ? P.yx : P;

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
        sceneZMax = tex2D(ScreenSpaceReflection_LinDepthBuffer, hitTexelCoord * ScreenSpaceReflectionParams.TexSize.zw).r;
    }

    Q.xy += dQ.xy * stepCount;
    hitPoint = Q * (1.0 / k);

    // Matches the new loop condition:
    return (rayZMax <= sceneZMax + ScreenSpaceReflectionParams.Thickness) && (rayZMin >= sceneZMax) && length(hitPoint - rayOrigin) <= rayLength;
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
