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

#include "Utils.hlsli"
#include "PostProcessingUtils.hlsli"

TEXTURE_2D_RESOURCE(ColorCopy_SourceTexture); // The texture to be copied

CBUFFER_RESOURCE(ColorCopy,
    GPU_bool SingleChannelCopy;
    GPU_bool ApplyTonemap;
    GPU_float4 CustomColorModulator;
);

#ifdef HLSL
struct VSOut
{
    float4  Position  :   SV_POSITION;
    float2  TexCoord  :   TEXCOORD0;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, float2 texCoord : TEXCOORD, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

    PatchVSOutputPositionForHalfPixelOffset(output.Position);
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 color : SV_TARGET)
{
    // Simple color texture copy onto a color render target
    if (ColorCopyParams.SingleChannelCopy)
    {
        // For use with single channel textures (e.g. SSAO buffer)
        color = tex2D(ColorCopy_SourceTexture, input.TexCoord).rrrr;
    }
    else
    {
        color = tex2D(ColorCopy_SourceTexture, input.TexCoord);
    }

    color *= ColorCopyParams.CustomColorModulator;

    // Tonemapping, mainly used by SSR to prevent specular highlights
    // from ruining mip chain of light accumulation buffer.
    if (ColorCopyParams.ApplyTonemap)
    {
        color.rgb /= (1.f + dot(PostProcessingUtils::LumaCoef, color.rgb));
        color.rgb = saturate(color.rgb);
    }
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
