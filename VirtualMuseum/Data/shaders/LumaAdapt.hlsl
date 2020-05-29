/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   LumaAdapt.hlsl
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

TEXTURE_2D_RESOURCE(LumaAdapt_LumaInput); // One of the two 1x1 textures with the current average luma value
TEXTURE_2D_RESOURCE(LumaAdapt_LumaTarget); // The 1x1 texture with the target average luma value

CBUFFER_RESOURCE(LumaAdapt,
    GPU_float2 HalfTexelOffset;
    GPU_float LumaAdaptSpeed; // The speed of the animation
    GPU_float FrameTime; // Last frame's duration in seconds
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
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + LumaAdaptParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 color : SV_TARGET)
{
    color = float4(0.f, 0.f, 0.f, 0.f);

    // Two 1x1 textures containing last and current frames' average lumas are used
    // to slowly adjust the exposure of the HDR image when tone mapping.
    const float currLuma = tex2D(LumaAdapt_LumaInput, float2(0.5f, 0.5f)).r;
    const float targetLuma = tex2D(LumaAdapt_LumaTarget, float2(0.5f, 0.5f)).r;
    float newLuma = currLuma + (targetLuma - currLuma) * (LumaAdaptParams.FrameTime / max(LumaAdaptParams.LumaAdaptSpeed, 0.01f));

    if (currLuma < targetLuma)
        newLuma = clamp(newLuma, currLuma, targetLuma);
    else
        newLuma = clamp(newLuma, targetLuma, currLuma);

    color = float4(newLuma, newLuma, newLuma, 1.f);
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
