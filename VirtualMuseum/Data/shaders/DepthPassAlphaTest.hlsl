/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   DepthPassAlphaTest.hlsl
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

#include "Utils.hlsli"

TEXTURE_2D_RESOURCE(DepthPassAlphaTest_Diffuse); // Diffuse color

CBUFFER_RESOURCE(DepthPassAlphaTest,
    GPU_float4x4 WorldViewProjMat;
);

#ifdef HLSL
struct VSOut
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
struct VSIn
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

void vsmain(VSIn input, out VSOut output)
{
    output.Position = mul(DepthPassAlphaTestParams.WorldViewProjMat, input.Position);
    output.TexCoord = input.TexCoord;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
float4 psmain(VSOut input) : SV_TARGET
{
    // Sample the diffuse texture
    float4 diffuseColor = tex2D(DepthPassAlphaTest_Diffuse, input.TexCoord);

    // Do an alpha-test
    ALPHA_TEST(diffuseColor.a, 0.5f);

    return float4(1.f, 0.f, 1.f, 1.f);
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
