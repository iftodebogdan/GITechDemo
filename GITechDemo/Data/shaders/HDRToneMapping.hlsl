/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   HDRToneMapping.hlsl
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
#include "Utils.hlsli"
#include "PostProcessingUtils.hlsli"

struct HDRToneMappingConstantTable
{
    CB_float2 HalfTexelOffset;
    CB_float ExposureBias;      // Exposure amount
    CB_float ShoulderStrength;  // = 0.15;
    CB_float LinearStrength;    // = 0.50;
    CB_float LinearAngle;       // = 0.10;
    CB_float ToeStrength;       // = 0.20;
    CB_float ToeNumerator;      // = 0.02;
    CB_float ToeDenominator;    // = 0.30;
    CB_float LinearWhite;       // = 11.2;
    CB_float FrameTime;
    CB_float FilmGrainAmount;
    CB_bool ApplyColorCorrection;
};

#ifdef HLSL
cbuffer HDRToneMappingResourceTable
{
    sampler2D HDRToneMappingSourceTexture;            // Source HDR texture
    sampler2D HDRToneMappingAvgLumaTexture;           // 1x1 average luma texture
    sampler3D HDRToneMappingColorCorrectionTexture;   // Color correction texture

    HDRToneMappingConstantTable HDRToneMappingParams;
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
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + HDRToneMappingParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
float3 ReinhardTonemap(const float3 color, const float avgLuma)
{
    return color / (1.f + avgLuma);
}

float3 DuikerOptimizedTonemap(const float3 color)
{
    float3 x = max(0, color - 0.004f);
    return (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
}

//////////////////////////////////////////////////////////////////////////////
// Filmic tone mapping operator used in Uncharted 2                         //
// http://filmicgames.com/Downloads/GDC_2010/Uncharted2-Hdr-Lighting.pptx   //
//////////////////////////////////////////////////////////////////////////////
float3 FilmicTonemap(const float3 color)
{
    return
        (
            (
                color * (HDRToneMappingParams.ShoulderStrength * color + HDRToneMappingParams.LinearStrength * HDRToneMappingParams.LinearAngle)
                + HDRToneMappingParams.ToeStrength * HDRToneMappingParams.ToeNumerator
            ) /
            (
                color * (HDRToneMappingParams.ShoulderStrength * color + HDRToneMappingParams.LinearStrength)
                + HDRToneMappingParams.ToeStrength * HDRToneMappingParams.ToeDenominator
            )
        ) - HDRToneMappingParams.ToeNumerator / HDRToneMappingParams.ToeDenominator;
}

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    //////////////////////////////////////////////////////////////////
    // Convert color from gamma space to linear space               //
    // http://http.developer.nvidia.com/GPUGems3/gpugems3_ch24.html //
    //////////////////////////////////////////////////////////////////

    // Linear gamma conversion done by SamplerState::SetSRGBEnabled()
    //float3 sourceColor = pow(abs(tex2D(HDRToneMappingSourceTexture, input.TexCoord)), 2.2f).rgb;
    float3 sourceColor = tex2D(HDRToneMappingSourceTexture, input.TexCoord).rgb;

    float avgLuma = tex2D(HDRToneMappingAvgLumaTexture, float2(0.5f, 0.5f)).r;
    sourceColor /= avgLuma;

    //////////////////////////////////////////
    // Apply tone mapping operator          //
    // http://filmicgames.com/archives/75   //
    //////////////////////////////////////////

    // Reinhard
    //float3 finalColor = ReinhardTonemap(sourceColor * HDRToneMappingParams.ExposureBias, avgLuma);

    // Duiker
    //float3 finalColor = DuikerOptimizedTonemap(sourceColor * HDRToneMappingParams.ExposureBias);

    // Uncharted 2
    float3 finalColor = FilmicTonemap(sourceColor * HDRToneMappingParams.ExposureBias);
    const float3 whiteScale = rcp(FilmicTonemap(HDRToneMappingParams.LinearWhite));
    finalColor *= whiteScale;

    // Color correction
    if(HDRToneMappingParams.ApplyColorCorrection)
        finalColor = tex3D(HDRToneMappingColorCorrectionTexture, saturate(finalColor)).rgb;

    // Convert back to gamma space (not required for Duiker tonemap)
    // NB: Gamma correction done by RenderState::SetSRGBWriteEnabled()
    //color = float4(pow(abs(finalColor), 1.f / 2.2f), 1);
    // Encode gamma-corrected luma in the alpha channel for FXAA
    color = float4(finalColor, pow(abs(dot(finalColor, LUMA_COEF)), 1.f / 2.2f));

    // Film grain needs to be applied after tonemapping, so as not to be affected by exposure variance
    if(HDRToneMappingParams.FilmGrainAmount > 0.f)
        color.rgb += (frac(sin(dot(input.TexCoord + float2(HDRToneMappingParams.FrameTime, HDRToneMappingParams.FrameTime), float2(12.9898f, 78.233f))) * 43758.5453f) * 2.f - 1.f) * HDRToneMappingParams.FilmGrainAmount;
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL