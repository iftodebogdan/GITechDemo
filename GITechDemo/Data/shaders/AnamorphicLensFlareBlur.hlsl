/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	AnamorphicLensFlareBlur.hlsl
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
#include "Utils.hlsli"

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
const sampler2D	texSource;	// The texture to be blurred
const float4 f4TexSize;		// zw: normalized size of texel
const int nKernel;			// Kernel size for current pass

float4 KawaseBlurAnamorphic(const sampler2D texSource, const float2 f2TexelSize, const float2 f2TexCoord, const int nKernel)
{
	float4 f4Color = float4(0.f, 0.f, 0.f, 0.f);

	UNROLL for (int i = -1; i <= 1; i += 2)
	{
		UNROLL for (int j = -1; j <= 1; j += 2)
		{
			const float2 f2TexelOffset = f2TexelSize * float2(i, j);
			const float2 f2HalfTexelOffset = 0.5f * f2TexelOffset;
			const float2 f2HalfTexelSize = 0.5f * f2TexelSize;
			const float2 f2SampleCenter = f2TexCoord + f2HalfTexelOffset + f2TexelOffset * nKernel * float2(1.f, 0.f);

			UNROLL for (int x = -1; x <= 1; x += 2)
				UNROLL for (int y = -1; y <= 1; y += 2)
				f4Color += tex2D(texSource, f2SampleCenter + f2HalfTexelSize * float2(x, y));
		}
	}

	return f4Color * 0.0625f;
}

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = KawaseBlurAnamorphic(texSource, f4TexSize.zw, input.f2TexCoord, nKernel);
}
////////////////////////////////////////////////////////////////////