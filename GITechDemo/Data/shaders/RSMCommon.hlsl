/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	RSMCommon.hlsl
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "PostProcessingUtils.hlsl"
#include "Utils.hlsl"

const sampler2D	texRSMFluxBuffer;	// RSM flux data
const sampler2D texRSMNormalBuffer;	// RSM normal data
const sampler2D texRSMDepthBuffer;	// RSM depth data

const sampler2D	texNormalBuffer;	// G-Buffer view-space normals

// Composite matrix for transforming coordinates from render
// camera projective space to light projective space
// NB: perspective divide (i.e. w-divide) required
const float4x4 f44ScreenToLightViewMat;

// Matrix for transforming coordinates from light
// view space to light projective space
const float4x4 f44RSMProjMat;

// Matrix for transforming coordinates from
// light projective space to light view space
const float4x4 f44RSMInvProjMat;

// Composite matrix for transforming from render
// camera view space to light view space
const float4x4 f44ViewToRSMViewMat;

// This kernel is based on a Poisson Disk kernel.
// The samples' coordinates are "pushed" towards the exterior of
// the disk by calculating the square root of the sample coordinates.
// The sample weights are in the z component. Their value are
// actually the distance of each sample before being pushed outward
// from the center of the kernel. In other words, the sample density
// is higher towards the edges of the kernel, but the weights do not
// vary linearly with the samples' offsets.
#define RSM_NUM_PASSES (1)			// The number of passes
#define RSM_SAMPLES_PER_PASS (64)	// The number of samples from the RSM in each pass

// The kernel for sampling the RSM
#if RSM_APPLY_PASS
const float3 f3RSMKernel[RSM_SAMPLES_PER_PASS];
#elif RSM_UPSCALE_PASS
const float3 f3RSMKernel[RSM_NUM_PASSES * RSM_SAMPLES_PER_PASS];
#endif

// Intensity of the indirect light
const float fRSMIntensity;

// Scale the kernel for tweaking between more
// "concentrated" bounces and larger bounce distances
const float fRSMKernelScale;

// For upsampling
const sampler2D	texSource;		// The texture to be upsampled
const bool bDebugUpscalePass;	// Shows pixels that could not be interpolated and need reshading

// Set a threshold which controls the level of sensitivity of the edge detection.
const float fWeightThreshold;

void ApplyRSM(const float2 f2TexCoord, const float fDepth, out float4 colorOut)
{
	colorOut = float4(0.f, 0.f, 0.f, 0.f);

	//////////////////////////////////////////////////////////////////
	// Reflective Shadow Map										//
	// http://www.vis.uni-stuttgart.de/~dachsbcn/download/rsm.pdf	//
	//////////////////////////////////////////////////////////////////

	// Calculate normalized device coordinates (NDC) space position of currently shaded pixel
	const float4 f4ScreenProjSpacePos = float4(f2TexCoord * float2(2.f, -2.f) - float2(1.f, -1.f), fDepth, 1.f);
	// Transform pixel coordinates from NDC space to RSM view space
	const float4 f4RSMViewSpacePos = mul(f44ScreenToLightViewMat, f4ScreenProjSpacePos);
	// Perspective w-divide
	const float3 f3RSMViewSpacePos = f4RSMViewSpacePos.xyz * rcp(f4RSMViewSpacePos.w);
	// Sample normal for currently shaded pixel and transform to RSM view space
	const float3 f3RSMViewSpaceNormal = mul((float3x3)f44ViewToRSMViewMat, DecodeNormal(tex2D(texNormalBuffer, f2TexCoord)));
	// Transform point to RSM NDC space
	const float3 f3RSMProjSpacePos = mul(f44RSMProjMat, float4(f3RSMViewSpacePos, 1.f)).xyz;
	// Calculate texture coordinates for the currently shaded pixel in the RSM texture
	const float2 f2RSMTexCoord = f3RSMProjSpacePos.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

	// Sample around the corresponding location in the RSM
	// and accumulate light contribution from each VPL
#if RSM_APPLY_PASS
	const unsigned int nEndIdx = RSM_SAMPLES_PER_PASS;
#elif RSM_UPSCALE_PASS
	const unsigned int nEndIdx = RSM_NUM_PASSES * RSM_SAMPLES_PER_PASS;
#else
	#error Either RSM_APPLY_PASS or RSM_UPSCALE_PASS must be defined!
#endif

	UNROLL for (unsigned int i = 0; i < nEndIdx; i++)
	{
		// Add a bias to the texture coordinate calculated above
		const float2 f2RSMSampleTexCoord = f2RSMTexCoord + f3RSMKernel[i].xy * fRSMKernelScale;
		// The radiant flux for the current sample
		const float4 f4RSMSampleFlux = tex2D(texRSMFluxBuffer, f2RSMSampleTexCoord) * f3RSMKernel[i].z;
		// The normal for the current sample
		const float3 f3RSMSampleNormal = DecodeNormal(tex2D(texRSMNormalBuffer, f2RSMSampleTexCoord));
		// Sample coordinates in RSM NDC space
		const float4 f4RSMSampleProjSpacePos =
			float4(
				f2RSMSampleTexCoord * float2(2.f, -2.f) - float2(1.f, -1.f),
				tex2D(texRSMDepthBuffer, f2RSMSampleTexCoord).r, 1.f
				);
		// Sample coordinates in RSM view space
		const float3 f3RSMSamplePos = mul(f44RSMInvProjMat, f4RSMSampleProjSpacePos).xyz;
		// Distance vector (RSM view space coordinates between shaded pixel and RSM sample
		const float3 f3DeltaPos = f3RSMViewSpacePos - f3RSMSamplePos;
		// Calculate the light contribution from this VPL
		colorOut +=
			f4RSMSampleFlux * fRSMIntensity *
			max(0.f, dot(f3RSMSampleNormal, f3DeltaPos)) *
			max(0.f, dot(f3RSMViewSpaceNormal, -f3DeltaPos)) /
			pow(length(f3DeltaPos), 4.f);
	}
}
