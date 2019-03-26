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

#include "Common.hlsli"

struct UIConstantTable
{
    CB_float4x4 ProjMat;
    CB_uint TextureSwitch;
    CB_uint MipLevel;
    CB_uint FaceIdx;
    CB_float DepthSlice;
};

#ifdef HLSL
struct VSOut
{
    float4  f4Position  :   SV_POSITION;
    float2  f2TexCoord  :   TEXCOORD0;
    float4  f4Color     :   COLOR;
};

cbuffer UIResourceTable
{
    sampler1D UITexture1D;
    sampler2D UITexture2D; // Usually font texture
    sampler3D UITexture3D;
    samplerCUBE UITextureCube;

    UIConstantTable UIParams;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float2 f4Position : POSITION, float4 f4Color : COLOR, float2 f2TexCoord : TEXCOORD, out VSOut output)
{
    output.f4Position = mul(UIParams.ProjMat, float4(f4Position, 0.f, 1.f));
    output.f2TexCoord = f2TexCoord;
    output.f4Color = f4Color;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
    switch (UIParams.TextureSwitch)
    {
    case 1:
        f4Color = tex1Dlod(UITexture1D, float4(input.f2TexCoord.x, 0, 0, UIParams.MipLevel)) * input.f4Color; break;
    case 2:
        f4Color = tex2Dlod(UITexture2D, float4(input.f2TexCoord, 0, UIParams.MipLevel)) * input.f4Color; break;
    case 3:
        f4Color = tex3Dlod(UITexture3D, float4(input.f2TexCoord, UIParams.DepthSlice, UIParams.MipLevel)) * input.f4Color; break;
    case 4:
        const float2 cubeUV = input.f2TexCoord * 2.f - 1.f;
        switch (UIParams.FaceIdx)
        {
        default:
        case 0:
            f4Color = texCUBElod(UITextureCube, float4(-1.f, cubeUV.yx * float2(-1.f, 1.f), UIParams.MipLevel)) * input.f4Color; break;
        case 1:
            f4Color = texCUBElod(UITextureCube, float4(cubeUV.x, 1.f, cubeUV.y, UIParams.MipLevel)) * input.f4Color; break;
        case 2:
            f4Color = texCUBElod(UITextureCube, float4(cubeUV.x, -1.f, -cubeUV.y, UIParams.MipLevel)) * input.f4Color; break;
        case 3:
            f4Color = texCUBElod(UITextureCube, float4(cubeUV * float2(1.f, -1.f), 1.f, UIParams.MipLevel)) * input.f4Color; break;
        case 4:
            f4Color = texCUBElod(UITextureCube, float4(1.f, -cubeUV.yx, UIParams.MipLevel)) * input.f4Color; break;
        case 5:
            f4Color = texCUBElod(UITextureCube, float4(-cubeUV, -1.f, UIParams.MipLevel)) * input.f4Color; break;
        };
        break;
    default:
        f4Color = input.f4Color; break;
    };
}
#endif // PIXEL
#endif // HLSL
////////////////////////////////////////////////////////////////////
