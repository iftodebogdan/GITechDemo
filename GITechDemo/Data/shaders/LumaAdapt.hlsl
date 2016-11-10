/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	LumaAdapt.hlsl
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

// One of the two 1x1 textures with the current average luma value
const sampler2D	texLumaInput;

// The 1x1 texture with the target average luma value
const sampler2D texLumaTarget;

// The speed of the animation
const float fLumaAdaptSpeed;

// Last frame's duration in seconds
const float fFrameTime;

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 0.f);

	// Two 1x1 textures containing last and current frames' average lumas are used
	// to slowly adjust the exposure of the HDR image when tone mapping.
	const float fCurrLuma = tex2D(texLumaInput, float2(0.5f, 0.5f)).r;
	const float fTargetLuma = tex2D(texLumaTarget, float2(0.5f, 0.5f)).r;
	float fNewLuma = fCurrLuma + (fTargetLuma - fCurrLuma) * (fFrameTime / clamp(fLumaAdaptSpeed, 0.01f, 3.402823466e+38f));

	if (fCurrLuma < fTargetLuma)
		fNewLuma = clamp(fNewLuma, fCurrLuma, fTargetLuma);
	else
		fNewLuma = clamp(fNewLuma, fTargetLuma, fCurrLuma);

	f4Color = float4(fNewLuma, fNewLuma, fNewLuma, 1.f);
}
////////////////////////////////////////////////////////////////////