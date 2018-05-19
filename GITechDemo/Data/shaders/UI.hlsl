/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   UI.hlsl
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

// Vertex shader /////////////////////////////////////////////////
const float4x4 f44UIProjMat;

struct VSOut
{
    float4  f4Position  :   SV_POSITION;
    float2  f2TexCoord  :   TEXCOORD0;
    float4  f4Color     :   COLOR;
};

void vsmain(float2 f4Position : POSITION, float4 f4Color : COLOR, float2 f2TexCoord : TEXCOORD, out VSOut output)
{
    output.f4Position = mul(f44UIProjMat, float4(f4Position, 0.f, 1.f));
    output.f2TexCoord = f2TexCoord;
    output.f4Color = f4Color;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler1D texUI1D;
const sampler2D texUI2D; // Usually font texture
const sampler3D texUI3D;
const samplerCUBE texUICube;

const unsigned int nUseTexUI;
const unsigned int nDepthSlice;

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
    switch (nUseTexUI)
    {
    case 1:
        f4Color = tex1D(texUI1D, input.f2TexCoord.x) * input.f4Color; break;
    case 2:
        f4Color = tex2D(texUI2D, input.f2TexCoord) * input.f4Color; break;
    case 3:
        f4Color = tex3D(texUI3D, float3(input.f2TexCoord, nDepthSlice)) * input.f4Color; break;
    case 4:
        f4Color = texCUBE(texUICube, float3(input.f2TexCoord, 1.f)) * input.f4Color; break; // TODO
    default:
        f4Color = input.f4Color; break;
    };
}
////////////////////////////////////////////////////////////////////