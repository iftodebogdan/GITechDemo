/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   LensFlareApply.hlsl
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

#include "Utils.hlsli"
#include "PostProcessingUtils.hlsli"

TEXTURE_2D_RESOURCE(LensFlareApply_Features); // Lens flare effect features
TEXTURE_2D_RESOURCE(LensFlareApply_Dirt); // Lens dirt texture
TEXTURE_2D_RESOURCE(LensFlareApply_StarBurst); // Lens star burst texture

CBUFFER_RESOURCE(LensFlareApply,
    GPU_float DirtIntensity; // Scale factor for lens dirt texture samples
    GPU_float StarBurstIntensity; // Scale factor for lens star burst texture samples

    // Transform matrix for the star burst texture coordinates
    // applying a camera dependent rotation
    GPU_float3x3 StarBurstMat;
);

#ifdef HLSL
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
    output.TexCoord = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

    PatchVSOutputPositionForHalfPixelOffset(output.Position);
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
#undef tex2D // PostProcessingUtils.hlsli defines tex2D as tex2Dlod, since we typically don't use mip-mapped textures in post processing passes

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    //////////////////////////////////////////////////////////////////////////////
    // Lens flare effect                                                        //
    // http://john-chapman-graphics.blogspot.ro/2013/02/pseudo-lens-flare.html  //
    //////////////////////////////////////////////////////////////////////////////

    // Sample lens flare features
    color  = float4(tex2D(LensFlareApply_Features, input.TexCoord).rgb, 0.f);
    
    // Sample dirt texture
    const float3 dirt = tex2D(LensFlareApply_Dirt, input.TexCoord).rgb * LensFlareApplyParams.DirtIntensity;

    // Sample star burst texture
    const float3 starBurst = tex2D(LensFlareApply_StarBurst, mul(LensFlareApplyParams.StarBurstMat, float3(input.TexCoord, 1.f)).xy).rgb * LensFlareApplyParams.StarBurstIntensity;

    // Modulate features by dirt and star burst textures
    color.rgb *= dirt + starBurst;
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL