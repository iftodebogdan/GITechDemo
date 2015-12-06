/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	PostProcessingUtils.hlsl
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

#include "Common.hlsl"

//////////////////////////////////////////////////////////
// Hardcoded mip level when sampling from textures to	//
// prevent the GPU from calculating ddx(), ddy() for	//
// every sample (post-processing effects work with		//
// textures with 1 mip level)							//
//////////////////////////////////////////////////////////
#define tex2D(tex, tc) tex2Dlod(tex, float4((tc).xy, 0.f, 0.f))



//////////////////////////////////////////////////////////////////////////////////////////////
// Reconstruct the depth in view space coordinates from the hyperbolic depth				//
//------------------------------------------------------------------------------------------//
// LinearDepth = (fZNear * fZFar / (fZNear - fZFar)) / (Depth - (fZFar / (fZFar - fZNear)))	//
// f2LinearDepthEquation.x = fZNear * fZFar / (fZNear - fZFar)								//
// f2LinearDepthEquation.y = fZFar / (fZFar - fZNear)										//
//////////////////////////////////////////////////////////////////////////////////////////////
const float fZNear;
const float fZFar;
const float2 f2LinearDepthEquation;
float ReconstructDepth(float fHyperbolicDepth)
{
	return f2LinearDepthEquation.x * rcp(fHyperbolicDepth - f2LinearDepthEquation.y);
}

//////////////////////////////////////////////////////////////////////////
// Downsample depth to a quarter of the original resolution				//
// NB: f2TexCoord must be at the center of the quarter resolution texel	//
// (i.e. right at the middle of the 4 full resolution samples)			//
//////////////////////////////////////////////////////////////////////////
const float f2DepthHalfTexelOffset;	// Half texel offset of depth buffer
const float GetDownsampledDepth(const sampler2D texDepthBuffer, const float2 f2TexCoord)
{
	const float2 f2SampleOffset[] =
	{
		float2(-1.f, -1.f),
		float2(-1.f,  1.f),
		float2( 1.f, -1.f),
		float2( 1.f,  1.f)
	};

	// TODO: Shouldn't it be min()? Why do I get better results visually with max()?
	return
		max(
			max(
				tex2D(texDepthBuffer, f2TexCoord + f2DepthHalfTexelOffset * f2SampleOffset[0]).r,
				tex2D(texDepthBuffer, f2TexCoord + f2DepthHalfTexelOffset * f2SampleOffset[1]).r
				),
			max(
				tex2D(texDepthBuffer, f2TexCoord + f2DepthHalfTexelOffset * f2SampleOffset[2]).r,
				tex2D(texDepthBuffer, f2TexCoord + f2DepthHalfTexelOffset * f2SampleOffset[3]).r
				)
			);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Kawase blur, approximating a 35x35 Gaussian kernel																	//
// in the nKernel = [0-1-2-2-3] format over 5 passes																	//
// https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 KawaseBlur(const sampler2D texSource, const float2 f2TexelSize, const float2 f2TexCoord, const int nKernel)
{
	float4 f4Color = float4(0.f, 0.f, 0.f, 0.f);

	UNROLL for (int i = -1; i <= 1; i += 2)
	{
		UNROLL for (int j = -1; j <= 1; j += 2)
		{
			const float2 f2TexelOffset = f2TexelSize * float2(i, j);
			const float2 f2HalfTexelOffset = 0.5f * f2TexelOffset;
			const float2 f2HalfTexelSize = 0.5f * f2TexelSize;
			const float2 f2SampleCenter = f2TexCoord + f2HalfTexelOffset + f2TexelOffset * nKernel;

			UNROLL for (int x = -1; x <= 1; x += 2)
				UNROLL for (int y = -1; y <= 1; y += 2)
					f4Color += tex2D(texSource, f2SampleCenter + f2HalfTexelSize * float2(x, y));
		}
	}

	return f4Color * 0.0625f;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////
// Downsampling helpers	//
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



//////////////////////////////////////////
// Luminance vectors					//
// http://stackoverflow.com/a/24213274	//
//////////////////////////////////////////
#define ITUR_LUMA_VEC (float3(0.2125f, 0.7154f, 0.0721f))
#define CCIR601_LUMA_VEC (float3(0.299f, 0.587f, 0.114f))
#define LUMINANCE_VECTOR CCIR601_LUMA_VEC
////////////////////////////////////////////////////////////////
