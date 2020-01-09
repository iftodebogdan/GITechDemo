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

struct RSMCaptureConstantTable
{
    GPU_float4x4 RSMWorldViewProjMat;
    GPU_float4x4 LightWorldViewMat;
};

#ifdef HLSL
cbuffer RSMCaptureResourceTable
{
    sampler2D RSMCapture_Diffuse; // Diffuse color

    RSMCaptureConstantTable RSMCaptureParams;
};

struct VSOut
{
    float4 Position :   SV_POSITION;
    float2 TexCoord :   TEXCOORD0;
    float3 Normal   :   NORMAL;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
struct VSIn
{
    float4 Position :   POSITION;
    float2 TexCoord :   TEXCOORD;
    float3 Normal   :   NORMAL;
};

void vsmain(VSIn input, out VSOut output)
{
    output.Position = mul(RSMCaptureParams.RSMWorldViewProjMat, input.Position);
    output.TexCoord = input.TexCoord;
    output.Normal = normalize(mul((float3x3)RSMCaptureParams.LightWorldViewMat, input.Normal));
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
struct PSOut
{
    float4 Flux     :   SV_TARGET0;
    float4 Normal   :   SV_TARGET1;
};

void psmain(VSOut input, out PSOut output)
{
    // Sample the diffuse texture
    const float4 diffuseColor = tex2D(RSMCapture_Diffuse, input.TexCoord);
    output.Flux = diffuseColor;

    // Vertex normal will suffice
    output.Normal = EncodeNormal(input.Normal);
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
