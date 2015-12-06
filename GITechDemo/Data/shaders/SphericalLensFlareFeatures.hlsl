/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	SphericalLensFlareFeatures.hlsl
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
const float2 f2TexelSize;	// Size of source texture texel

// Ghost features
const sampler1D texGhostColorLUT;
const int nGhostSamples;
const float fGhostDispersal;
const float fGhostRadialWeightExp;

// Halo feature
const float fHaloSize;
const float fHaloRadialWeightExp;

// Chromatic abberation feature
const bool bChromaShift;
const float fShiftFactor;

// Replacement for tex2D() which adds a
// chromatic abberation effect to texture samples
float4 FetchChromaShiftedTextureSample(sampler2D tex, float2 texCoord)
{
	if (bChromaShift)
	{
		const float3 f2ShiftAmount = float3(
			-f2TexelSize.x * fShiftFactor,
			0.f,
			f2TexelSize.x * fShiftFactor);
		const float2 f2Dir = normalize(float2(0.5f, 0.5f) - texCoord);
		return float4(
			tex2D(tex, texCoord + f2Dir * f2ShiftAmount.r).r,
			tex2D(tex, texCoord + f2Dir * f2ShiftAmount.g).g,
			tex2D(tex, texCoord + f2Dir * f2ShiftAmount.b).b,
			1.f);
	}
	else
		return tex2D(tex, texCoord);
}

// Convert tex2D() calls to FetchChromaShiftedTextureSample() calls
#ifdef tex2D
#undef tex2D
#endif
#define tex2D(tex, texCoord) FetchChromaShiftedTextureSample(tex, texCoord)

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	//////////////////////////////////////////////////////////////////////////////
	// Lens flare effect														//
	// http://john-chapman-graphics.blogspot.ro/2013/02/pseudo-lens-flare.html	//
	//////////////////////////////////////////////////////////////////////////////

	f4Color = float4(0.f, 0.f, 0.f, 1.f);

	// Ghost vector to image center
	const float2 f2GhostVec = (float2(0.5f, 0.5f) - input.f2FlippedTexCoord) * fGhostDispersal;

	// Generate ghost features
	for (int i = 0; i < nGhostSamples; i++)
	{
		const float2 f2Offset = input.f2FlippedTexCoord + f2GhostVec * float(i);
		const float fGhostWeight =
			pow(abs(1.f -
				length(float2(0.5f, 0.5f) - f2Offset) *
				rcp(length(float2(0.5f, 0.5f)))),
				fGhostRadialWeightExp);
		f4Color.rgb += tex2D(texSource, f2Offset).rgb * fGhostWeight;
	}

	// Adjust ghosts' color using a LUT
	f4Color.rgb *= tex1D(texGhostColorLUT, length(float2(0.5f, 0.5f) - input.f2FlippedTexCoord) * rcp(length(float2(0.5f, 0.5f)))).rgb;

	// Generate halo feature
	const float2 f2HaloVec = normalize(f2GhostVec) * fHaloSize;
	const float fHaloWeight =
		pow(abs(1.f -
			length(
				float2(0.5f, 0.5f) -
				(input.f2FlippedTexCoord + f2HaloVec)
				) *
			rcp(length(float2(0.5f, 0.5f)))),
			fHaloRadialWeightExp);
	f4Color.rgb += tex2D(texSource, input.f2FlippedTexCoord + f2HaloVec).rgb * fHaloWeight;
}
////////////////////////////////////////////////////////////////////