/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	AnamorphicLensFlareFeatures.hlsl
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

#include "PostProcessingUtils.hlsli"

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;

struct VSOut
{
	float4	f4Position			:	SV_POSITION;
	float2	f2TexCoord			:	TEXCOORD0;
	float2	f2FlippedTexCoord	:	TEXCOORD1;
};

void vsmain(float4 f4Position : POSITION, float2 f2TexCoord : TEXCOORD, out VSOut output)
{
	output.f4Position = f4Position;
	output.f2TexCoord = f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
	// Flip texture coordinates horizontally/vertically
	output.f2FlippedTexCoord = float2(1.f, 1.f) - (f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset);
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D	texSource;	// Source texture
const float4 f4TexSize;		// zw: size of source texture texel

const float fAnamorphicIntensity;

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 1.f);

	const float fWeight[] =
	{
		0.044256f,
		0.044036f, 0.043381f, 0.042311f, 0.040857f,
		0.03906f, 0.036972f, 0.034647f, 0.032145f,
		0.029528f, 0.026854f, 0.02418f, 0.021555f,
		0.019024f, 0.016624f, 0.014381f, 0.012318f
	};

	UNROLL for (int i = -16; i <= 16; i++)
	{
		// Calculate coordinates for sampling source texture
		const float2 f2Offset = float2(i * f4TexSize.z, 0.f);
		const float2 f2SampleTexCoord = input.f2TexCoord + f2Offset;

		// Sample the texture and give it a bluish tint
		float3 f3SampleColor = tex2D(texSource, f2SampleTexCoord).rgb;
		f3SampleColor.b += f3SampleColor.r + f3SampleColor.g;

		f4Color.rgb += fWeight[abs(i)] * f3SampleColor * fAnamorphicIntensity;
	}
}