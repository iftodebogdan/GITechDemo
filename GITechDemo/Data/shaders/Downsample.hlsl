/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	Downsample.hlsl
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

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;

struct VSOut
{
	float4	f4Position	:	SV_POSITION;
	float2	f2TexCoord	:	TEXCOORD0;
};

void vsmain(float4 f4Position : POSITION, float2 f2TexCoord : TEXCOORD, out VSOut output)
{
	output.f4Position = f4Position;
	output.f2TexCoord = f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D	texSource;	// Source texture for downsampling
const float2 f2TexelSize;	// Size of source texture texel
const int nDownsampleFactor;		// Switch for 2x2 or 4x4 downsampling
const bool bApplyBrightnessFilter;	// Switch for whether to apply a brightness pass filter
const float fBrightnessThreshold;	// Threshold for the brightness pass filter

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 0.f);

	if (nDownsampleFactor == 16)
		f4Color = Downsample4x4(texSource, input.f2TexCoord, f2TexelSize);

	if (nDownsampleFactor == 4)
		f4Color = Downsample2x2(texSource, input.f2TexCoord, f2TexelSize);

	if (nDownsampleFactor == 1)
		f4Color = tex2D(texSource, input.f2TexCoord);

	if (bApplyBrightnessFilter)
	{
		const float fBrightness = dot(f4Color.rgb, LUMINANCE_VECTOR);
		f4Color.rgb *= step(fBrightnessThreshold, fBrightness);
		//f4Color.rgb *= rcp(fBrightness);
		f4Color.rgb *= rcp(1.f + fBrightness);
	}
}
////////////////////////////////////////////////////////////////////