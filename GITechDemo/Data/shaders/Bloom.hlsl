/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Bloom.hlsl
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
#include "Utils.hlsli"

TEXTURE_2D_RESOURCE(Bloom_Source); // The texture to be blurred

CBUFFER_RESOURCE(Bloom,
    GPU_float2 HalfTexelOffset;
    GPU_float Strength;         // Intensity of bloom
    GPU_float Power;            // Exponent of bloom
    GPU_float4 TexSize;         // zw: normalized size of a texel
    GPU_int Kernel;             // Kernel size for current pass
    GPU_bool AdjustIntensity;   // Apply the effects of fBloomStrength and fBloomPower
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
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + BloomParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 color : SV_TARGET)
{
    color = KawaseBlur(Bloom_Source, BloomParams.TexSize.zw, input.TexCoord, BloomParams.Kernel);

    if(BloomParams.AdjustIntensity)
        color = pow(abs(color), BloomParams.Power) * BloomParams.Strength;
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
