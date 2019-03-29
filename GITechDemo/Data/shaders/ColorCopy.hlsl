/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   ColorCopy.hlsl
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

#include "PostProcessingUtils.hlsli"

struct ColorCopyConstantTable
{
    CB_float2 HalfTexelOffset;
    CB_bool SingleChannelCopy;
    CB_bool ApplyTonemap;
    CB_float4 CustomColorModulator;
};

#ifdef HLSL
cbuffer ColorCopyResourceTable
{
    const sampler2D ColorCopySourceTexture; // The texture to be copied

    ColorCopyConstantTable ColorCopyParams;
};

struct VSOut
{
    float4  f4Position  :   SV_POSITION;
    float2  f2TexCoord  :   TEXCOORD0;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 f4Position : POSITION, float2 f2TexCoord : TEXCOORD, out VSOut output)
{
    output.f4Position = f4Position;
    output.f2TexCoord = f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + ColorCopyParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
    // Simple color texture copy onto a color render target
    if(ColorCopyParams.SingleChannelCopy)
        // For use with single channel textures (e.g. SSAO buffer)
        f4Color = tex2D(ColorCopySourceTexture, input.f2TexCoord).rrrr;
    else
        f4Color = tex2D(ColorCopySourceTexture, input.f2TexCoord);

    f4Color *= ColorCopyParams.CustomColorModulator;

    // Tonemapping, mainly used by SSR to prevent specular highlights
    // from ruining mip chain of light accumulation buffer.
    if (ColorCopyParams.ApplyTonemap)
    {
        f4Color.rgb /= (1.f + dot(LUMA_COEF, f4Color.rgb));
        f4Color.rgb = saturate(f4Color.rgb);
    }
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
