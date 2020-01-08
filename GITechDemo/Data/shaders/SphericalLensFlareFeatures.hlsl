/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   SphericalLensFlareFeatures.hlsl
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

struct SphericalLensFlareFeaturesConstantTable
{
    CB_float2 HalfTexelOffset;
    CB_float4 TexSize; // zw: size of source texture texel

    // Ghost features
    CB_int GhostSamples;
    CB_float GhostDispersal;
    CB_float GhostRadialWeightExp;

    // Halo feature
    CB_float HaloSize;
    CB_float HaloRadialWeightExp;

    // Chromatic aberration feature
    CB_bool ChromaShift;
    CB_float ShiftFactor;
};

#ifdef HLSL
cbuffer SphericalLensFlareFeaturesResourceTable
{
    sampler1D SphericalLensFlareFeaturesGhostColorLUT;
    sampler2D SphericalLensFlareFeaturesSource;  // Source texture

    SphericalLensFlareFeaturesConstantTable SphericalLensFlareFeaturesParams;
};

struct VSOut
{
    float4  Position          :   SV_POSITION;
    float2  TexCoord          :   TEXCOORD0;
    float2  FlippedTexCoord   :   TEXCOORD1;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, float2 texCoord : TEXCOORD, out VSOut output)
{
    output.Position = position;
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + SphericalLensFlareFeaturesParams.HalfTexelOffset;
    // Flip texture coordinates horizontally/vertically
    output.FlippedTexCoord = float2(1.f, 1.f) - (position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + SphericalLensFlareFeaturesParams.HalfTexelOffset);
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
// Replacement for tex2D() which adds a
// chromatic aberration effect to texture samples
float4 FetchChromaShiftedTextureSample(sampler2D tex, float2 texCoord)
{
    if (SphericalLensFlareFeaturesParams.ChromaShift)
    {
        const float3 shiftAmount = float3(
            -SphericalLensFlareFeaturesParams.TexSize.z * SphericalLensFlareFeaturesParams.ShiftFactor,
            0.f,
            SphericalLensFlareFeaturesParams.TexSize.z * SphericalLensFlareFeaturesParams.ShiftFactor);
        const float2 dir = normalize(float2(0.5f, 0.5f) - texCoord);
        return float4(
            tex2D(tex, texCoord + dir * shiftAmount.r).r,
            tex2D(tex, texCoord + dir * shiftAmount.g).g,
            tex2D(tex, texCoord + dir * shiftAmount.b).b,
            1.f);
    }
    else
        return tex2D(tex, texCoord);
}

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    //////////////////////////////////////////////////////////////////////////////
    // Lens flare effect                                                        //
    // http://john-chapman-graphics.blogspot.ro/2013/02/pseudo-lens-flare.html  //
    //////////////////////////////////////////////////////////////////////////////

    color = float4(0.f, 0.f, 0.f, 1.f);

    // Ghost vector to image center
    const float2 ghostVec = (float2(0.5f, 0.5f) - input.FlippedTexCoord) * SphericalLensFlareFeaturesParams.GhostDispersal;

    // Generate ghost features
    for (int i = 0; i < SphericalLensFlareFeaturesParams.GhostSamples; i++)
    {
        const float2 offset = input.FlippedTexCoord + ghostVec * float(i);
        const float ghostWeight =
            pow(abs(1.f -
                length(float2(0.5f, 0.5f) - offset) /
                length(float2(0.5f, 0.5f))),
                SphericalLensFlareFeaturesParams.GhostRadialWeightExp);
        color.rgb += FetchChromaShiftedTextureSample(SphericalLensFlareFeaturesSource, offset).rgb * ghostWeight;
    }

    // Adjust ghosts' color using a LUT
    color.rgb *= tex1D(SphericalLensFlareFeaturesGhostColorLUT, length(float2(0.5f, 0.5f) - input.FlippedTexCoord) / length(float2(0.5f, 0.5f))).rgb;

    // Generate halo feature
    const float2 haloVec = normalize(ghostVec) * SphericalLensFlareFeaturesParams.HaloSize;
    const float haloWeight =
        pow(abs(1.f -
            length(
                float2(0.5f, 0.5f) -
                (input.FlippedTexCoord + haloVec)
                ) /
            length(float2(0.5f, 0.5f))),
            SphericalLensFlareFeaturesParams.HaloRadialWeightExp);
    color.rgb += FetchChromaShiftedTextureSample(SphericalLensFlareFeaturesSource, input.FlippedTexCoord + haloVec).rgb * haloWeight;
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL