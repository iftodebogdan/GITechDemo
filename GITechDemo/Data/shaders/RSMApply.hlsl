/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   RSMApply.hlsl
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

// This define will modify the behaviour of the RSMApply() function so that 
// it is aware that we are in the apply pass (instead of the upscale pass).
#define RSM_APPLY_PASS (1)
#include "RSMCommon.hlsli"
#undef RSM_APPLY_PASS

TEXTURE_2D_RESOURCE(RSMApply_DepthBuffer);

CBUFFER_RESOURCE(RSMApply,
    GPU_float2 HalfTexelOffset;
);

#ifdef HLSL
struct VSOut
{
    float4 Position :   SV_POSITION;
    float2 TexCoord :   TEXCOORD0;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + RSMApplyParams.HalfTexelOffset;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
void psmain(VSOut input, out float4 color : SV_TARGET)
{
    color = float4(0.f, 0.f, 0.f, 1.f);

    // Early depth test so that we don't shade
    // at the far plane (where the sky is drawn)
    const float depth = tex2D(RSMApply_DepthBuffer, input.TexCoord).r;
    DEPTH_KILL(depth, 1.f);

    ApplyRSM(input.TexCoord, depth, color);
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
