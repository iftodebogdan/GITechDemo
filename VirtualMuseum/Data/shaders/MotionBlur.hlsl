/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   MotionBlur.hlsl
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

TEXTURE_2D_RESOURCE(MotionBlur_Source);      // Color texture
TEXTURE_2D_RESOURCE(MotionBlur_DepthBuffer); // Depth buffer

CBUFFER_RESOURCE(MotionBlur,
    GPU_float2 HalfTexelOffset;
    GPU_float NumSamples;           // The number of samples along the velocity vector
    GPU_float FrameTime;            // Frame duration
    GPU_float Intensity;            // Intensity of the motion blur effect
);

#ifdef HLSL
struct VSOut
{
    float4  Position  : SV_POSITION;
    float2  TexCoord  : TEXCOORD0;
    float2  ScreenPos : TEXCOORD1;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, float2 texCoord : TEXCOORD, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + MotionBlurParams.HalfTexelOffset;
    output.ScreenPos = position.xy;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 color : SV_TARGET)
{
    //////////////////////////////////////////////////////////////////
    // Motion blur effect                                           //
    // http://http.developer.nvidia.com/GPUGems3/gpugems3_ch27.html //
    //////////////////////////////////////////////////////////////////

    color = float4(0.f, 0.f, 0.f, 1.f);

    // Compute NDC space position
    const float depth = tex2D(MotionBlur_DepthBuffer, input.TexCoord).r;
    const float4 NDCPos = float4(input.ScreenPos, depth, 1.f);

    // Compute world space position
    const float4 worldPosPreW = mul(FrameParams.InvViewProjMat, NDCPos);
    const float4 worldPos = worldPosPreW / worldPosPreW.w;

    // Compute last frame NDC space position
    const float4 prevNDCPosPreW = mul(FrameParams.PrevViewProjMat, worldPos);
    const float4 prevNDCPos = prevNDCPosPreW / prevNDCPosPreW.w;

    // Compute pixel velocity
    const float2 NDCSpeed = (NDCPos.xy - prevNDCPos.xy) / MotionBlurParams.FrameTime; // Speed (NDC units / s)

    float2 velocity = NDCSpeed
        * MotionBlurParams.Intensity    // Scale velocity by intensity value
        / MotionBlurParams.NumSamples;  // Divide by the number of samples (so that there would be no
                                        // change in perceived intensity if sample count varies)

    // Clamp it so as not to ruin the effect at high speed
    velocity = normalize(velocity) * clamp(length(velocity), 0.f, 0.01f);

    // Sample along the velocity vector and average the result
    for (int i = 0; i < MotionBlurParams.NumSamples; i++)
    {
        color.rgb += tex2D(MotionBlur_Source, input.TexCoord + velocity * i).rgb;
    }

    color.rgb /= MotionBlurParams.NumSamples;
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
