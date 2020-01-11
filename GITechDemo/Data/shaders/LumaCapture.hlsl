/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   LumaCapture.hlsl
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

// One of the two 1x1 textures with the current average luma value
TEXTURE_2D_RESOURCE(LumaCapture_LumaInput);

CBUFFER_RESOURCE(LumaCapture,
    GPU_float2 HalfTexelOffset;

    // Flags for deciding code path
    GPU_bool InitialLumaPass;
    GPU_bool FinalLumaPass;

    // Minimum and maximum values for average luma
    GPU_float2 AvgLumaClamp;

    // zw: size of a texel
    GPU_float4 TexSize;
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
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + LumaCaptureParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 color : SV_TARGET)
{
    color = float4(0.f, 0.f, 0.f, 0.f);

    // The initial pass samples the luminance of the source average luma
    // texture using a 3x3 kernel of sample points, downscaling to an image
    // containing the log() of averages
    if (LumaCaptureParams.InitialLumaPass)
    {
        const float2 kernel = LumaCaptureParams.TexSize.zw * 0.33333333f;
        float logLumSum = 0.f;
        UNROLL for (float i = -1.f; i <= 1.f; i++)
            UNROLL for (float j = -1.f; j <= 1.f; j++)
            {
                // The average luma can be calculated from part of the source image,
                // hence the scale of the texture coordinates
                const float2 scaledTexCoord = input.TexCoord;// *float2(0.4f, 0.6f) + float2(0.3f, 0.2f);
                const float3 sampleSource = tex2D(LumaCapture_LumaInput, scaledTexCoord + kernel * float2(i, j)).rgb;
                logLumSum += log(dot(sampleSource, LUMA_COEF) + 0.0001f);
            }

        logLumSum *= 0.11111111f;
        color = float4(logLumSum, logLumSum, logLumSum, 1.f);
    }
    // The rest of the passes further downscale the average luma texture
    else
    {
        float avgLuma = 0.f;
        UNROLL for (float i = -1.5f; i <= 1.5f; i += 1.f)
            UNROLL for (float j = -1.5f; j <= 1.5f; j += 1.f)
                avgLuma += tex2D(LumaCapture_LumaInput, input.TexCoord + LumaCaptureParams.TexSize.zw * float2(i, j)).r;

        // On the final pass, we do an exp() and store the value into
        // the final 1x1 average luma texture
        if (LumaCaptureParams.FinalLumaPass)
        {
            avgLuma = exp(avgLuma * 0.0625f);
            avgLuma = clamp(avgLuma, LumaCaptureParams.AvgLumaClamp.x, LumaCaptureParams.AvgLumaClamp.y);
        }
        else
            avgLuma *= 0.0625f;

        color = float4(avgLuma, avgLuma, avgLuma, 1.f);
    }
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
