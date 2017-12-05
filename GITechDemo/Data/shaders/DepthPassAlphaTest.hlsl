/*=============================================================================
 * This file is part of the "GITechDemo" application
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

// Vertex shader /////////////////////////////////////////////////
const float4x4 f44WorldViewProjMat;

struct VSIn
{
    float4 f4Position   :   POSITION;
    float2 f2TexCoord   :   TEXCOORD;
};

struct VSOut
{
    float4 f4Position   :   SV_POSITION;
    float2 f2TexCoord   :   TEXCOORD0;
};

void vsmain(VSIn input, out VSOut output)
{
    output.f4Position = mul(f44WorldViewProjMat, input.f4Position);
    output.f2TexCoord = input.f2TexCoord;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D texDiffuse; // Diffuse color

float4 psmain(VSOut input) : SV_TARGET
{
    // Sample the diffuse texture
    float4 f4DiffuseColor = tex2D(texDiffuse, input.f2TexCoord);

    // Do an alpha-test
    ALPHA_TEST(f4DiffuseColor.a, 0.5f);

    return float4(1.f, 0.f, 1.f, 1.f);
}
////////////////////////////////////////////////////////////////////