/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   RSMCommon.hlsli
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

#ifndef RSMCOMMON_HLSLI
#define RSMCOMMON_HLSLI

#include "PostProcessingUtils.hlsli"
#include "Utils.hlsli"
#include "CSMUtils.hlsli"

TEXTURE_2D_RESOURCE(RSMCommon_RSMFluxBuffer);   // RSM flux data
TEXTURE_2D_RESOURCE(RSMCommon_RSMNormalBuffer); // RSM normal data
TEXTURE_2D_RESOURCE(RSMCommon_RSMDepthBuffer);  // RSM depth data
TEXTURE_2D_RESOURCE(RSMCommon_NormalBuffer);    // G-Buffer view-space normals

struct RSM
{
    // This kernel is based on a Poisson Disk kernel.
    // The samples' coordinates are "pushed" towards the exterior of
    // the disk by calculating the square root of the sample coordinates.
    // The sample weights are in the z component. Their value are
    // actually the distance of each sample before being pushed outward
    // from the center of the kernel. In other words, the sample density
    // is higher towards the edges of the kernel, but the weights do not
    // vary linearly with the samples' offsets.
    static const unsigned int PassCount = 4; // The number of passes
    static const unsigned int SamplesPerPass = 16; // The number of samples from the RSM in each pass
    static const unsigned int SampleCount = PassCount * SamplesPerPass; // Total number of samples
};

CBUFFER_RESOURCE(RSMCommon,
    // The kernel for sampling the RSM
    GPU_float3 KernelApplyPass[RSM::SamplesPerPass];
    GPU_float3 KernelUpscalePass[RSM::SampleCount];

    // Intensity of the indirect light
    GPU_float Intensity;

    // Scale the kernel for tweaking between more
    // "concentrated" bounces and larger bounce distances
    GPU_float KernelScale;

    // Matrix for transforming coordinates from light
    // view space to light projective space
    GPU_float4x4 RSMProjMat;

    // Matrix for transforming coordinates from
    // light projective space to light view space
    GPU_float4x4 RSMInvProjMat;

    // Composite matrix for transforming from render
    // camera view space to light view space
    GPU_float4x4 ViewToRSMViewMat;
);

#ifdef HLSL
void ApplyRSM(const float2 texCoord, const float depth, out float4 colorOut)
{
    colorOut = float4(0.f, 0.f, 0.f, 0.f);

    //////////////////////////////////////////////////////////////////
    // Reflective Shadow Map                                        //
    // http://www.vis.uni-stuttgart.de/~dachsbcn/download/rsm.pdf   //
    //////////////////////////////////////////////////////////////////

    // Calculate normalized device coordinates (NDC) space position of currently shaded pixel
    const float4 screenProjSpacePos = float4(texCoord * float2(2.f, -2.f) - float2(1.f, -1.f), depth, 1.f);
    // Transform pixel coordinates from NDC space to RSM view space
    const float4 RSMViewSpacePosW = mul(FrameParams.ScreenToLightViewMat, screenProjSpacePos);
    // Perspective w-divide
    const float3 RSMViewSpacePos = RSMViewSpacePosW.xyz / RSMViewSpacePosW.w;
    // Sample normal for currently shaded pixel and transform to RSM view space
    const float3 RSMViewSpaceNormal = mul((float3x3)RSMCommonParams.ViewToRSMViewMat, DecodeNormal(tex2D(RSMCommon_NormalBuffer, texCoord)));
    // Transform point to RSM NDC space
    const float3 RSMProjSpacePos = mul(RSMCommonParams.RSMProjMat, float4(RSMViewSpacePos, 1.f)).xyz;
    // Calculate texture coordinates for the currently shaded pixel in the RSM texture
    const float2 RSMTexCoord = RSMProjSpacePos.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

    // Sample around the corresponding location in the RSM
    // and accumulate light contribution from each VPL
#if RSM_APPLY_PASS
    const unsigned int endIdx = RSM::SamplesPerPass;
    const float3 kernel[] = RSMCommonParams.KernelApplyPass;
#elif RSM_UPSCALE_PASS
    const unsigned int endIdx = RSM::SampleCount;
    const float3 kernel[] = RSMCommonParams.KernelUpscalePass;
#else
    #error Either RSM_APPLY_PASS or RSM_UPSCALE_PASS must be defined!
#endif

#if RSM_APPLY_PASS
    UNROLL
#elif RSM_UPSCALE_PASS
    LOOP
#endif
    for (unsigned int i = 0; i < endIdx; i++)
    {
        // Add a bias to the texture coordinate calculated above
        const float2 RSMSampleTexCoord = RSMTexCoord + kernel[i].xy * RSMCommonParams.KernelScale;
        // The radiant flux for the current sample
        const float4 RSMSampleFlux = tex2D(RSMCommon_RSMFluxBuffer, RSMSampleTexCoord) * kernel[i].z;
        // The normal for the current sample
        const float3 RSMSampleNormal = DecodeNormal(tex2D(RSMCommon_RSMNormalBuffer, RSMSampleTexCoord));
        // Sample coordinates in RSM NDC space
        const float4 RSMSampleProjSpacePos =
            float4(
                RSMSampleTexCoord * float2(2.f, -2.f) - float2(1.f, -1.f),
                tex2D(RSMCommon_RSMDepthBuffer, RSMSampleTexCoord).r, 1.f
                );
        // Sample coordinates in RSM view space
        const float3 RSMSamplePos = mul(RSMCommonParams.RSMInvProjMat, RSMSampleProjSpacePos).xyz;
        // Distance vector (RSM view space coordinates between shaded pixel and RSM sample
        const float3 deltaPos = RSMViewSpacePos - RSMSamplePos;
        // Calculate the light contribution from this VPL
        colorOut +=
            RSMSampleFlux * RSMCommonParams.Intensity *
            max(0.f, dot(RSMSampleNormal, deltaPos)) *
            max(0.f, dot(RSMViewSpaceNormal, -deltaPos)) /
            pow(length(deltaPos), 4.f);
    }
}

#endif // HLSL
#endif // RSMCOMMON_HLSLI
