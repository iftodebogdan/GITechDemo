/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   AnamorphicLensFlareFeatures.hlsl
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

TEXTURE_2D_RESOURCE(AnamorphicLensFlareFeatures_Source);  // Source texture

CBUFFER_RESOURCE(AnamorphicLensFlareFeatures,
    GPU_float2 HalfTexelOffset;
    GPU_float4 TexSize;     // zw: size of source texture texel

    GPU_float AnamorphicIntensity;
);

#ifdef HLSL
struct VSOut
{
    float4  Position        :   SV_POSITION;
    float2  TexCoord        :   TEXCOORD0;
    float2  FlippedTexCoord :   TEXCOORD1;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, float2 texCoord : TEXCOORD, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + AnamorphicLensFlareFeaturesParams.HalfTexelOffset;
    // Flip texture coordinates horizontally/vertically
    output.FlippedTexCoord = float2(1.f, 1.f) - (position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + AnamorphicLensFlareFeaturesParams.HalfTexelOffset);
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 color : SV_TARGET)
{
    color = float4(0.f, 0.f, 0.f, 1.f);

    const float weight[] =
    {
        0.044256f,
        0.044036f, 0.043381f, 0.042311f, 0.040857f,
        0.03906f, 0.036972f, 0.034647f, 0.032145f,
        0.029528f, 0.026854f, 0.02418f, 0.021555f,
        0.019024f, 0.016624f, 0.014381f, 0.012318f
    };

    UNROLL for (int i = -16; i <= 16; i++)
    {
        // Calculate coordinates for sampling source texture
        const float2 offset = float2(i * AnamorphicLensFlareFeaturesParams.TexSize.z, 0.f);
        const float2 sampleTexCoord = input.TexCoord + offset;

        // Sample the texture and give it a bluish tint
        float3 sampleColor = tex2D(AnamorphicLensFlareFeatures_Source, sampleTexCoord).rgb;
        sampleColor.b += sampleColor.r + sampleColor.g;

        color.rgb += weight[abs(i)] * sampleColor * AnamorphicLensFlareFeaturesParams.AnamorphicIntensity;
    }
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
