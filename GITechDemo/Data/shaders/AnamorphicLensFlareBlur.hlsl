/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   AnamorphicLensFlareBlur.hlsl
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

struct AnamorphicLensFlareBlurConstantTable
{
    GPU_float2 HalfTexelOffset;
    GPU_float4 TexSize;     // zw: normalized size of texel
    GPU_int Kernel;          // Kernel size for current pass
};

#ifdef HLSL
cbuffer AnamorphicLensFlareBlurResourceTable
{
    sampler2D AnamorphicLensFlareBlur_Source;  // The texture to be blurred

    AnamorphicLensFlareBlurConstantTable AnamorphicLensFlareBlurParams;
};

struct VSOut
{
    float4  Position    :   SV_POSITION;
    float2  TexCoord    :   TEXCOORD0;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, float2 texCoord : TEXCOORD, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + AnamorphicLensFlareBlurParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
float4 KawaseBlurAnamorphic(const sampler2D texSource, const float2 texelSize, const float2 texCoord, const int kernel)
{
    float4 color = float4(0.f, 0.f, 0.f, 0.f);

    UNROLL for (int i = -1; i <= 1; i += 2)
    {
        UNROLL for (int j = -1; j <= 1; j += 2)
        {
            const float2 texelOffset = texelSize * float2(i, j);
            const float2 halfTexelOffset = 0.5f * texelOffset;
            const float2 halfTexelSize = 0.5f * texelSize;
            const float2 sampleCenter = texCoord + halfTexelOffset + texelOffset * (kernel + 0.5f) * float2(1.f, 0.f);

            UNROLL for (int x = -1; x <= 1; x += 2)
                UNROLL for (int y = -1; y <= 1; y += 2)
                    color += tex2D(texSource, sampleCenter + halfTexelSize * float2(x, y));
        }
    }

    return color * 0.0625f;
}

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    color = KawaseBlurAnamorphic(AnamorphicLensFlareBlur_Source, AnamorphicLensFlareBlurParams.TexSize.zw, input.TexCoord, AnamorphicLensFlareBlurParams.Kernel);
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL