/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	Skybox.hlsl
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

// Vertex shader /////////////////////////////////////////////////
const float4x4 f44SkyViewProjMat;

struct VSIn
{
	float4 f4Position	:	POSITION;
};

struct VSOut
{
	float4 f4Position	:	SV_POSITION;
	float3 f3TexCoord	:	TEXCOORD0;
};

void vsmain(VSIn input, out VSOut output)
{
	output.f4Position	=	mul(f44SkyViewProjMat, input.f4Position);
	output.f3TexCoord	=	normalize(input.f4Position.xyz);
	output.f4Position.z =	output.f4Position.w; // Position sky at far plane
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const samplerCUBE texSkyCube;	// Sky cubemap

const float3 f3LightDir;	// Direction of sunlight
const float fSunRadius;		// Determines sun radius
const float fSunBrightness;	// Determines sun brightness

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	// Do a dot product of the view direciton and the sunlight direction
	const float3 f3SunDir	=	normalize(-f3LightDir);
	const float	fSunDot		=	dot(normalize(input.f3TexCoord), f3SunDir);

	f4Color  =	texCUBE(texSkyCube, input.f3TexCoord);
	f4Color +=	pow(max(0.f, fSunDot), fSunRadius) * fSunBrightness;
}
////////////////////////////////////////////////////////////////////