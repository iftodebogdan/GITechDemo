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

TEXTURE_1D_RESOURCE(UI_Texture1D);
TEXTURE_2D_RESOURCE(UI_Texture2D); // Usually font texture
TEXTURE_3D_RESOURCE(UI_Texture3D);
TEXTURE_CUBE_RESOURCE(UI_TextureCube);

CBUFFER_RESOURCE(UI,
    GPU_float4x4 UIProjMat;
    GPU_uint TextureSwitch;
    GPU_uint MipLevel;
    GPU_uint FaceIdx;
    GPU_float DepthSlice;
);

#ifdef HLSL
struct VSOut
{
    float4  Position  :   SV_POSITION;
    float2  TexCoord  :   TEXCOORD0;
    float4  Color     :   COLOR;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float2 position : POSITION, float4 color : COLOR, float2 texCoord : TEXCOORD, out VSOut output)
{
    output.Position = mul(UIParams.UIProjMat, float4(position, 0.f, 1.f));
    output.TexCoord = texCoord;
    output.Color = color;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 color : SV_TARGET)
{
    switch (UIParams.TextureSwitch)
    {
    case 1:
        color = tex1Dlod(UI_Texture1D, float4(input.TexCoord.x, 0, 0, UIParams.MipLevel)) * input.Color; break;
    case 2:
        color = tex2Dlod(UI_Texture2D, float4(input.TexCoord, 0, UIParams.MipLevel)) * input.Color; break;
    case 3:
        color = tex3Dlod(UI_Texture3D, float4(input.TexCoord, UIParams.DepthSlice, UIParams.MipLevel)) * input.Color; break;
    case 4:
        const float2 cubeUV = input.TexCoord * 2.f - 1.f;
        switch (UIParams.FaceIdx)
        {
        default:
        case 0:
            color = texCUBElod(UI_TextureCube, float4(-1.f, cubeUV.yx * float2(-1.f, 1.f), UIParams.MipLevel)) * input.Color; break;
        case 1:
            color = texCUBElod(UI_TextureCube, float4(cubeUV.x, 1.f, cubeUV.y, UIParams.MipLevel)) * input.Color; break;
        case 2:
            color = texCUBElod(UI_TextureCube, float4(cubeUV.x, -1.f, -cubeUV.y, UIParams.MipLevel)) * input.Color; break;
        case 3:
            color = texCUBElod(UI_TextureCube, float4(cubeUV * float2(1.f, -1.f), 1.f, UIParams.MipLevel)) * input.Color; break;
        case 4:
            color = texCUBElod(UI_TextureCube, float4(1.f, -cubeUV.yx, UIParams.MipLevel)) * input.Color; break;
        case 5:
            color = texCUBElod(UI_TextureCube, float4(-cubeUV, -1.f, UIParams.MipLevel)) * input.Color; break;
        };
        break;
    default:
        color = input.Color; break;
    };
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
