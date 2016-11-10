/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   RSMCapture.hlsl
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
const float4x4 f44RSMWorldViewProjMat;
const float4x4 f44LightWorldViewMat;

struct VSIn
{
    float4 f4Position   :   POSITION;
    float2 f2TexCoord   :   TEXCOORD;
    float3 f3Normal     :   NORMAL;
};

struct VSOut
{
    float4 f4Position   :   SV_POSITION;
    float2 f2TexCoord   :   TEXCOORD0;
    float3 f3Normal     :   NORMAL;
};

void vsmain(VSIn input, out VSOut output)
{
    output.f4Position = mul(f44RSMWorldViewProjMat, input.f4Position);
    output.f2TexCoord = input.f2TexCoord;
    output.f3Normal = normalize(mul((float3x3)f44LightWorldViewMat, input.f3Normal));
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D texDiffuse; // Diffuse color

struct PSOut
{
    float4 f4Flux   :   SV_TARGET0;
    float4 f4Normal :   SV_TARGET1;
};

void psmain(VSOut input, out PSOut output)
{
    // Sample the diffuse texture
    const float4 f4DiffuseColor = tex2D(texDiffuse, input.f2TexCoord);
    output.f4Flux = f4DiffuseColor;

    // Vertex normal will suffice
    output.f4Normal = EncodeNormal(input.f3Normal);
}
////////////////////////////////////////////////////////////////////