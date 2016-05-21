/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	RSMUpscale.hlsl
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

// This define will modify the behaviour of the RSMApply() function so that it will
// perform all RSM_NUM_PASSES * RSM_SAMPLES_PER_PASS texture fetches in a single pass
// (As opposed to the apply pass which is a multi-pass approach).
#define RSM_UPSCALE_PASS (1)
#include "RSMCommon.hlsli"

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;

struct VSOut
{
	float4 f4Position	:	SV_POSITION;
	float2 f2TexCoord	:	TEXCOORD0;
};

void vsmain(float4 f4Position : POSITION, out VSOut output)
{
	output.f4Position = f4Position;
	output.f2TexCoord = f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D	texDepthBuffer;	// G-Buffer depth values

// Set a threshold which controls the level of sensitivity of the edge detection.
const float fWeightThreshold;

const bool bDebugUpscalePass;	// Shows pixels that could not be interpolated and need reshading

bool PerformUpscale(const float2 f2TexCoord, const float fDepth, out float4 colorOut);

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 1.f);

	// Early depth test so that we don't shade
	// at the far plane (where the sky is drawn)
	const float fDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;
	DEPTH_KILL(fDepth, 1.f);

	// If we have successfully interpolated this pixel there is no need to reshade it.
	if (!PerformUpscale(input.f2TexCoord, fDepth, f4Color))
	{
		ApplyRSM(input.f2TexCoord, fDepth, f4Color);

		if (bDebugUpscalePass)
			f4Color = float4(2.f * length(f4Color), 0.f, 0.f, 0.f);
	}
}

bool PerformUpscale(const float2 f2TexCoord, const float fDepth, out float4 colorOut)
{
	// Sample the color from the quarter-resolution render target
	const float4 f4Color = tex2D(texSource, f2TexCoord);

	// If the sample is black we can safely clip this
	// pixel in an attempt to save some bandwidth
	clip(-!any(f4Color.rgb));

	// Easier to write f3TexelOffset.xz or f3TexelOffset.zy than
	// float2(0.5f * f2HalfTexelOffset.x, 0.f) or float2(0.f, 0.5f * f2HalfTexelOffset.y);
	// NB: f2HalfTexelOffset is half the width and half the height of a texel from the
	// IndirectLightAccumulationBuffer render target (quarter resolution). As such, we require
	// half of that width and half of that height in order to offset our coordinates from the
	// center of a quarter resolution texel to the center of a full resolution texel.
	const float3 f3TexelOffset = float3(0.5f * f2HalfTexelOffset, 0.f);

	// Sample the normal for our reference pixel (i.e. the one we're shading)
	const float3 f3RefNormal = DecodeNormal(tex2D(texNormalBuffer, f2TexCoord));
	// Sample the neighbours' normals
	const float3 f3NeighbourNormalN = DecodeNormal(tex2D(texNormalBuffer, f2TexCoord - f3TexelOffset.zy));
	const float3 f3NeighbourNormalS = DecodeNormal(tex2D(texNormalBuffer, f2TexCoord + f3TexelOffset.zy));
	const float3 f3NeighbourNormalW = DecodeNormal(tex2D(texNormalBuffer, f2TexCoord - f3TexelOffset.xz));
	const float3 f3NeighbourNormalE = DecodeNormal(tex2D(texNormalBuffer, f2TexCoord + f3TexelOffset.xz));

	// Sample the depth for our reference pixel
	const float fRefDepth = fDepth;
	// Sample the neighbours' depths
	const float fNeighbourDepthN = tex2D(texDepthBuffer, f2TexCoord - f3TexelOffset.zy).r;
	const float fNeighbourDepthS = tex2D(texDepthBuffer, f2TexCoord + f3TexelOffset.zy).r;
	const float fNeighbourDepthW = tex2D(texDepthBuffer, f2TexCoord - f3TexelOffset.xz).r;
	const float fNeighbourDepthE = tex2D(texDepthBuffer, f2TexCoord + f3TexelOffset.xz).r;

	// Calculate a weight based on normal differences between the reference pixel and its neighbours
	const float fWeightNormalN = pow(saturate(dot(f3RefNormal, f3NeighbourNormalN)), 32);
	const float fWeightNormalS = pow(saturate(dot(f3RefNormal, f3NeighbourNormalS)), 32);
	const float fWeightNormalW = pow(saturate(dot(f3RefNormal, f3NeighbourNormalW)), 32);
	const float fWeightNormalE = pow(saturate(dot(f3RefNormal, f3NeighbourNormalE)), 32);

	// Calculate a weight based on depth differences between the reference pixel and its neighbours
	const float fWeightDepthN = rcp(abs(fRefDepth - fNeighbourDepthN) + 0.00001f);
	const float fWeightDepthS = rcp(abs(fRefDepth - fNeighbourDepthS) + 0.00001f);
	const float fWeightDepthW = rcp(abs(fRefDepth - fNeighbourDepthW) + 0.00001f);
	const float fWeightDepthE = rcp(abs(fRefDepth - fNeighbourDepthE) + 0.00001f);

	// Put all weights into a float4
	const float4 f4Weight =
		normalize(float4(
			fWeightNormalN * fWeightDepthN,
			fWeightNormalS * fWeightDepthS,
			fWeightNormalW * fWeightDepthW,
			fWeightNormalE * fWeightDepthE));

	// If none of the weights fall below the threshold, or if the pixel is black,
	// then it is valid and it doesn't require reshading using the RSM algorithm.
	if (all(saturate(f4Weight - fWeightThreshold.xxxx)) || !any(f4Color.rgb))
	{
		colorOut = f4Color;
		return true;
	}

	return false;
}
////////////////////////////////////////////////////////////////////