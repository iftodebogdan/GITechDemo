/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   GBufferGeneration.hlsl
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

TEXTURE_2D_RESOURCE(GBufferGeneration_Diffuse);     // Diffuse color
TEXTURE_2D_RESOURCE(GBufferGeneration_Normal);      // Normal map
TEXTURE_2D_RESOURCE(GBufferGeneration_Spec);        // Specular map
TEXTURE_2D_RESOURCE(GBufferGeneration_MatType);     // Material type
TEXTURE_2D_RESOURCE(GBufferGeneration_Roughness);   // Roughness

CBUFFER_RESOURCE(GBufferGeneration,
    GPU_float4x4 WorldViewProjMat;
    GPU_float4x4 WorldViewMat;
    GPU_float SpecIntensity; // In case there is no specular map available
    GPU_bool HasNormalMap;   // Is normal map available?
    GPU_bool HasSpecMap;     // Is specular map available?
);

#ifdef HLSL
struct VSOut
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Normal   : NORMAL;
    float3 Tangent  : TANGENT;
    float3 Binormal : BINORMAL;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
struct VSIn
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal   : NORMAL;
    float3 Tangent  : TANGENT;
    float3 Binormal : BINORMAL;
};

void vsmain(VSIn input, out VSOut output)
{
    output.Position = mul(GBufferGenerationParams.WorldViewProjMat, input.Position);
    output.TexCoord = input.TexCoord;
    output.Normal   = normalize(mul((float3x3)GBufferGenerationParams.WorldViewMat, input.Normal));
    output.Tangent  = normalize(mul((float3x3)GBufferGenerationParams.WorldViewMat, input.Tangent));
    output.Binormal = normalize(mul((float3x3)GBufferGenerationParams.WorldViewMat, input.Binormal));
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
struct PSOut
{
    float4 Diffuse      : SV_TARGET0;
    float4 Normal       : SV_TARGET1;
    float4 Material     : SV_TARGET2;
    float4 VertexNormal : SV_TARGET3;
};

void psmain(VSOut input, out PSOut output)
{
    // Sample the diffuse texture
    float4 diffuseColor = tex2D(GBufferGeneration_Diffuse, input.TexCoord);

    // Early alpha-test
    ALPHA_TEST(diffuseColor.a, 0.5f);

    // Sample the specular texture, if present
    if(GBufferGenerationParams.HasSpecMap)
        diffuseColor.a = tex2D(GBufferGeneration_Spec, input.TexCoord).r;
    else
        diffuseColor.a = GBufferGenerationParams.SpecIntensity;

    // Sample the normal map, if present
    float3 normalMap = float3(0.f, 0.f, 0.f);
    if(GBufferGenerationParams.HasNormalMap)
        normalMap = tex2D(GBufferGeneration_Normal, input.TexCoord).rgb * 2.f - 1.f;
    const float3 normal = input.Normal + normalMap.x * input.Tangent + normalMap.y * input.Binormal;

    // Sample the material type (metallic/dielectric) texture
    float matType = tex2D(GBufferGeneration_MatType, input.TexCoord).r;

    // Sample the roughness texture
    float roughness = tex2D(GBufferGeneration_Roughness, input.TexCoord).r;

    // Final values
    output.Diffuse = diffuseColor;
    output.Normal = EncodeNormal(normal);
    output.Material = float4(matType, roughness, 0.f, 0.f);
    output.VertexNormal = EncodeNormal(input.Normal); // low frequency vertex normals required to reduce noise from indirect lighting
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
