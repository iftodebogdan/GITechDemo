/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   SSAO.hlsl
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

struct SSAOConstantTable
{
    GPU_float2 HalfTexelOffset;

    GPU_float4x4 InvProjMat;   // Matrix for inversing the projection transform

    GPU_float SampleRadius;  // Radius of the sampling pattern
    GPU_float Intensity;     // Overall intensity of the SSAO effect
    GPU_float Scale;         // Scale for the occlusion attenuation with distance
    GPU_float Bias;          // Bias for the occlusion attenuation with normal differences
};

#ifdef HLSL
cbuffer SSAOResourceTable
{
    sampler2D SSAO_NormalBuffer;    // View-space normals
    sampler2D SSAO_DepthBuffer;     // Depth values

    SSAOConstantTable SSAOParams;
};

struct VSOut
{
    float4  Position  :   SV_POSITION;
    float2  TexCoord  :   TEXCOORD0;
    float2  ScreenPos :   TEXCOORD1;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, float2 texCoord : TEXCOORD, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + SSAOParams.HalfTexelOffset;
    output.ScreenPos = position.xy;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
/*
    Simple Screen-Space Ambient Occlusion shader.
    Based on the work of José María Méndez:
    http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753

    More exotic / esoteric / physically-based methods of calculating
    ambient occlusion are subject for future research, if time permits.
    (e.g. http://sirkan.iit.bme.hu/~szirmay/ambient8.pdf)
*/
float AOCalc(const float2 texCoord, const float2 offset, const float3 position, const float3 normal)
{
    const float2 sampleTexCoord     = texCoord + offset;
    const float2 sampleScreenPos    = sampleTexCoord * float2(2.f, -2.f) - float2(1.f, -1.f);
    const float sampleDepth         = tex2D(SSAO_DepthBuffer, sampleTexCoord).r;
    const float4 sampleProjPosition = float4(sampleScreenPos, sampleDepth, 1.f);
    const float4 samplePositionPreW = mul(SSAOParams.InvProjMat, sampleProjPosition);
    const float3 samplePosition     = samplePositionPreW.xyz / samplePositionPreW.w;

    const float3 posDiff = samplePosition - position;
    const float3 dir = normalize(posDiff);
    const float dist = length(posDiff) * SSAOParams.Scale;

    return max(0.f, dot(normal, dir) - SSAOParams.Bias) / (1.f + dist) * SSAOParams.Intensity;
}

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    const float2 kernel[4] =
    {
        float2( 1.f,     0.f),
        float2(-1.f,     0.f),
        float2( 0.f,     1.f),
        float2( 0.f,    -1.f)
    };

    const float depth = tex2D(SSAO_DepthBuffer, input.TexCoord).r;
    DEPTH_KILL(depth, 1.f);

    const float4 projPosition = float4(input.ScreenPos, depth, 1.f);
    const float4 positionPreW = mul(SSAOParams.InvProjMat, projPosition);
    const float3 position = positionPreW.xyz / positionPreW.w;
    const float3 normal = DecodeNormal(tex2D(SSAO_NormalBuffer, input.TexCoord));
    //const float2 f2Rand = float2(GenerateRandomNumber(input.f2TexCoord.xy), GenerateRandomNumber(input.f2TexCoord.yx));

    float AO = 0.0f;
    const float rad = SSAOParams.SampleRadius / position.z;

    UNROLL for (int i = 0; i < 4; i++)
    {
        // Noise isn't really needed since we'll be blurring the AO mask
        // and it won't look biased towards any discernable direction.
        //const float2 f2Offset1 = reflect(f2Kernel[i], f2Rand) * fRad;
        const float2 offset1 = kernel[i] * rad;
        const float2 offset2 = float2(
            offset1.x * 0.707f - offset1.y * 0.707f,
            offset1.x * 0.707f + offset1.y * 0.707f
            );

        AO += AOCalc(input.TexCoord, offset1 * 0.25f, position, normal);
        AO += AOCalc(input.TexCoord, offset2 * 0.5f, position, normal);
        AO += AOCalc(input.TexCoord, offset1 * 0.75f, position, normal);
        AO += AOCalc(input.TexCoord, offset2, position, normal);
    }

    AO *= 0.0625f;

    color = float4(AO, AO, AO, 1.f);
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////

#endif // HLSL