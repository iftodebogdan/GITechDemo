/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   DirectionalLightVolume.hlsl
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
#include "CSMUtils.hlsli"
#include "BRDFUtils.hlsli"

TEXTURE_2D_RESOURCE(DirectionalLightVolume_ShadowMap); // Cascaded shadow maps
TEXTURE_2D_RESOURCE(DirectionalLightVolume_DepthBuffer); // Scene depth
TEXTURE_3D_RESOURCE(DirectionalLightVolume_Noise); // Noise texture (for fog)

CBUFFER_RESOURCE(DirectionalLightVolume,
    GPU_float4 TexSize; // xy: size, in texels, of destination texture; zw : normalized texel size

    GPU_float       SampleCount;            // Number of samples along ray
    GPU_float       SampleDistrib;          // Distribution of samples along ray
    GPU_float       LightIntensity;         // Intensity of the effect when in direct light
    GPU_float       MultScatterIntensity;   // Intensity of the effect when in indirect light
    GPU_float3      FogBox;                 // Bounding box of fog texture in world space coordinates
    GPU_float3      FogSpeed;               // Fog speed modifier for each axis (world space units / sec)
    GPU_float       FogVerticalFalloff;     // Vertical fallof factor for the volumetric fog effect

    GPU_float3      CameraPositionLightVS;  // Light view space position of camera
    GPU_float       RaymarchDistanceLimit;  // Fallback if we can't find a tighter limit
    GPU_float       ElapsedTime;            // Elapsed time
);

#ifdef HLSL
struct VSOut
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float2 ScreenPos: TEXCOORD2;
    float2 TexelIdx : TEXCOORD3;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, out VSOut output)
{
    output.Position   = position;
    output.TexCoord   = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
    output.ScreenPos  = position.xy;
    output.TexelIdx   = DirectionalLightVolumeParams.TexSize.xy * (position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f));

    PatchVSOutputPositionForHalfPixelOffset(output.Position);
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
#define OFFSET_RAY_SAMPLES  // If defined, scatter multiple ray samples across grid of PostProcessingUtils::InterleavedGridSize x PostProcessingUtils::InterleavedGridSize pixels
#define USE_BAYER_MATRIX    // If defined, sample grid offsets from texture, else use procedurally generated values
#define FOG_DENSITY_MAP     // If defined, use 3D noise texture as a fog density map

// Radiative transport equation terms (http://sirkan.iit.bme.hu/~szirmay/lightshaft.pdf)
#define TAU     (0.0001f)   // Probability of collision
//#define PHI       (1.f)       // Source power of the light
#define PHI BRDFParams.DiffuseFactor

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Based on the work of Benjamin Glatzel <bglatzel@deck13.com>:                                                                             //
    // http://bglatzel.movingblocks.net/wp-content/uploads/2014/05/Volumetric-Lighting-for-Many-Lights-in-Lords-of-the-Fallen-With-Notes.pdf    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    color = float4(0.f, 0.f, 0.f, 1.f);

    // Intersect ray with scene
    const float sceneDepth = tex2D(DirectionalLightVolume_DepthBuffer, input.TexCoord).r;
    const float4 rayPosLightVSW = mul(FrameParams.ScreenToLightViewMat, float4(input.ScreenPos, sceneDepth, 1.f));
    float3 rayPosLightVS = rayPosLightVSW.xyz / rayPosLightVSW.w;

    // Reduce noisiness by truncating the starting position
    const float3 rayLightVS = DirectionalLightVolumeParams.CameraPositionLightVS - rayPosLightVS;
    const float3 rayDirLightVS = normalize(rayLightVS);
    float raymarchDistance = /*trunc*/(clamp(length(rayLightVS), 0.f, DirectionalLightVolumeParams.RaymarchDistanceLimit));

    // Calculate the size of each step
    const float stepLen = raymarchDistance / (float)DirectionalLightVolumeParams.SampleCount;

    // Calculate the offsets on the ray according to the interleaved sampling pattern
#ifdef OFFSET_RAY_SAMPLES
#ifdef USE_BAYER_MATRIX
    const float rayStartOffset = GetDitherAmount(input.TexCoord, DirectionalLightVolumeParams.TexSize.xy) * stepLen;
#else // USE_BAYER_MATRIX
    const float2 interleavedPos = fmod(ceil(input.TexelIdx), PostProcessingUtils::InterleavedGridSize);
    const float rayStartOffset = (interleavedPos.y * PostProcessingUtils::InterleavedGridSize + interleavedPos.x) * (stepLen * PostProcessingUtils::InterleavedGridSizeSqrRcp);
#endif // USE_BAYER_MATRIX
#else // OFFSET_RAY_SAMPLES
    const float rayStartOffset = 0.f;
#endif // OFFSET_RAY_SAMPLES

    // The ray starting position has changed, if the raymarch distance was clamped, and must be updated.
    //rayPosLightVS += rayStartOffset * rayDirLightVS;
    rayPosLightVS = DirectionalLightVolumeParams.CameraPositionLightVS + rayDirLightVS * (rayStartOffset - raymarchDistance);

    // Calculate ray position in fog texture space for fog effect.
    const float3 fogBoxRcp = rcp(DirectionalLightVolumeParams.FogBox);
    const float3 rayPosWS = mul(FrameParams.InvLightViewMat, float4(rayPosLightVS, 1.f)).xyz * fogBoxRcp;
    const float3 rayDirWS = normalize(mul(FrameParams.InvLightViewMat, float4(rayDirLightVS, 0.f)).xyz) * fogBoxRcp;
    const float3 fogSampleOffset = DirectionalLightVolumeParams.ElapsedTime * DirectionalLightVolumeParams.FogSpeed * fogBoxRcp;

    // Ray marching (could be optimized with a hardcoded sample count value and unrolling this 'for')
    LOOP for (int i = 0; i < DirectionalLightVolumeParams.SampleCount; i++)
    {
        // warning X3571: pow(f, e) will not work for negative f, use abs(f) or conditionally handle negative values if you expect them
        const float sampleDist = pow(abs((stepLen * i) / raymarchDistance), DirectionalLightVolumeParams.SampleDistrib) * raymarchDistance;
        const float3 samplePosLightVS = rayPosLightVS + rayDirLightVS * sampleDist;
        const float3 samplePosWS = rayPosWS + rayDirWS * sampleDist;

        // Find the best valid cascade
        const unsigned int validCascade = GetCascadeIdx(samplePosLightVS.xy);

        // Calculate texture coordinates, then sample from the cascade we found above
        const float3 cascadeTexCoord = GetCascadeSpacePos(samplePosLightVS, validCascade);

        // Fetch whether the current position on the ray is visible from the light's perspective - or not
        const bool lit = (tex2D(DirectionalLightVolume_ShadowMap, cascadeTexCoord.xy).r > cascadeTexCoord.z);

        // Distance to the current position on the ray in light view-space
        // NB: We don't want to attenuate the directional light
        //const float lightDist = length(samplePosLightVS - float3(DirectionalLightVolumeParams.CameraPositionLightVS.xy, DirectionalLightVolumeParams.CameraPositionLightVS.z - DirectionalLightVolumeParams.RaymarchDistanceLimit));
        //const float lightDistRcp = rcp(lightDist);

#ifdef FOG_DENSITY_MAP
        // Sample the fog/noise texture and apply an exponential vertical falloff
        // NB: ddx()/ddy() (i.e. any texture fetch operation with auto mip selection) requires loop unrolling, so go for tex3Dlod() instead
        const float density = tex3Dlod(DirectionalLightVolume_Noise, float4(samplePosWS + fogSampleOffset, 0.f)).r * exp(-samplePosWS.y * samplePosWS.y * DirectionalLightVolumeParams.FogVerticalFalloff);
#else // FOG_DENSITY_MAP
        const float density = exp(-samplePosWS.y * samplePosWS.y * DirectionalLightVolumeParams.FogVerticalFalloff);
#endif

        // Calculate the final light contribution for the sample on the ray
        // and add it to the total contribution of the ray
        //f4Color.rgb += TAU * (bLit * (PHI * 0.25f * PI_RCP) * fLightDistRcp * fLightDistRcp) * exp(-fLightDist * TAU) * exp(-fSampleDist * TAU) * fStepLen;
        color.rgb += (DirectionalLightVolumeParams.MultScatterIntensity + DirectionalLightVolumeParams.LightIntensity * lit) * exp(-sampleDist * TAU) * density * TAU * (PHI * 0.25f * Utils::PiRcp) * sampleDist; // * (lightDistRcp * lightDistRcp) * exp(-lightDist * TAU)
    }

    // Moved some calculations outside of the above 'for' as an optimization
    //color.rgb *= TAU * (PHI * 0.25f * PI_RCP) * stepLen;
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
