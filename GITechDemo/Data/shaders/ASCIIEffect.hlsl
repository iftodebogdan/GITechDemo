/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   ASCIIEffect.hlsl
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

TEXTURE_2D_RESOURCE(ASCIIEffect_Source); // Source texture
TEXTURE_2D_RESOURCE(ASCIIEffect_ASCIIMap); // Character map
TEXTURE_2D_RESOURCE(ASCIIEffect_SourceMipChain); // Source texture with full mip chain

CBUFFER_RESOURCE(ASCIIEffect,
    GPU_float4 SourceTexSize;       // zw: size of source texture texel (1.f / xy)
    GPU_float4 OutputSize;          // zw: size of output texel (1.f / xy)
    GPU_float ResolutionDescaler;   // Determines output resolution (source resolution / 2^ResolutionDescaler)
    GPU_float Gamma;                // Adjust gamma for source image luminance
    GPU_bool UseColor;
);

#ifdef HLSL
struct VSOut
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
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
#define ASCII_MAP_LUT_KEY_COUNT (20) // Number of glyphs in the ASCII character map texture

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    const float3 sourceMipChain = tex2Dlod(ASCIIEffect_SourceMipChain, float4(input.TexCoord.xy, 0, ASCIIEffectParams.ResolutionDescaler)).rgb;
    const float sourceMipLuma = saturate(dot(sourceMipChain, PostProcessingUtils::LumaCoef));
    const float charMapLUTKey = floor(pow(sourceMipLuma, ASCIIEffectParams.Gamma) * (ASCII_MAP_LUT_KEY_COUNT - 1));
    const float2 charMapCoord = frac(input.TexCoord * ASCIIEffectParams.OutputSize.xy) / float2(ASCII_MAP_LUT_KEY_COUNT, 1) + float2(charMapLUTKey / ASCII_MAP_LUT_KEY_COUNT, 0);
    const float4 sourceColor = tex2D(ASCIIEffect_Source, input.TexCoord);
    const float sourceLuma = dot(sourceColor.rgb, PostProcessingUtils::LumaCoef);

    color = tex2D(ASCIIEffect_ASCIIMap, charMapCoord).r * (ASCIIEffectParams.UseColor ? sourceColor : sourceLuma);
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
