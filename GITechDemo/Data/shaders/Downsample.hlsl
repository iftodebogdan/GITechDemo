/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Downsample.hlsl
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

TEXTURE_2D_RESOURCE(Downsample_Source);  // Source texture for downsampling

CBUFFER_RESOURCE(Downsample,
    GPU_float2 HalfTexelOffset;
    GPU_float4 TexSize;     // zw: size of source texture texel

    // Color downsampling options
    GPU_int DownsampleFactor;        // Switch for 2x2 or 4x4 downsampling
    GPU_bool ApplyBrightnessFilter;  // Switch for whether to apply a brightness pass filter
    GPU_float BrightnessThreshold;   // Threshold for the brightness pass filter

    // Depth downsampling options
    GPU_bool DepthDownsample;    // Downsample depth values instead of colors
    GPU_bool ReconstructDepth;   // Reconstruct camera space depth from hyperbolic depth
);

#ifdef HLSL
struct VSOut
{
    float4  Position : SV_POSITION;
    float2  TexCoord : TEXCOORD0;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, float2 texCoord : TEXCOORD, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + DownsampleParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 color : SV_TARGET)
{
    color = float4(0.f, 0.f, 0.f, 0.f);

    if (DownsampleParams.DepthDownsample)
    {
        float sampleDepth;
        
        if (DownsampleParams.DownsampleFactor == 4)
            sampleDepth = GetDownsampledDepth(Downsample_Source, input.TexCoord).r;
        else
            sampleDepth = tex2D(Downsample_Source, input.TexCoord).r;
        
        if (DownsampleParams.ReconstructDepth)
            color = ReconstructDepth(sampleDepth);
        else
            color = sampleDepth;
    }
    else
    {
        if (DownsampleParams.DownsampleFactor == 16)
            color = Downsample4x4(Downsample_Source, input.TexCoord, DownsampleParams.TexSize.zw);

        if (DownsampleParams.DownsampleFactor == 4)
            color = Downsample2x2(Downsample_Source, input.TexCoord, DownsampleParams.TexSize.zw);

        if (DownsampleParams.DownsampleFactor == 1)
            color = tex2D(Downsample_Source, input.TexCoord);

        if (DownsampleParams.ApplyBrightnessFilter)
        {
            const float brightness = dot(color.rgb, PostProcessingUtils::LumaCoef);
            color.rgb *= step(DownsampleParams.BrightnessThreshold, brightness);
            //color.rgb /= brightness;
            color.rgb /= 1.f + brightness;
        }
    }
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
