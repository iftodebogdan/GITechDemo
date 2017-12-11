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

// Vertex shader /////////////////////////////////////////////////
const float4x4 f44WorldViewProjMat;
const float4x4 f44WorldViewMat;

struct VSIn
{
    float4 f4Position   :   POSITION;
    float2 f2TexCoord   :   TEXCOORD;
    float3 f3Normal     :   NORMAL;
    float3 f3Tangent    :   TANGENT;
    float3 f3Binormal   :   BINORMAL;
};

struct VSOut
{
    float4 f4Position   :   SV_POSITION;
    float2 f2TexCoord   :   TEXCOORD0;
    float3 f3Normal     :   NORMAL;
    float3 f3Tangent    :   TANGENT;
    float3 f3Binormal   :   BINORMAL;
};

void vsmain(VSIn input, out VSOut output)
{
    output.f4Position   =   mul(f44WorldViewProjMat, input.f4Position);
    output.f2TexCoord   =   input.f2TexCoord;
    output.f3Normal     =   normalize(mul((float3x3)f44WorldViewMat, input.f3Normal));
    output.f3Tangent    =   normalize(mul((float3x3)f44WorldViewMat, input.f3Tangent));
    output.f3Binormal   =   normalize(mul((float3x3)f44WorldViewMat, input.f3Binormal));
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D texDiffuse;     // Diffuse color
const sampler2D texNormal;      // Normal map
const sampler2D texSpec;        // Specular map
const sampler2D texMatType;     // Material type
const sampler2D texRoughness;   // Roughness

const float fSpecIntensity; // In case there is no specular map available

const bool bHasNormalMap;   // Is normal map available?
const bool bHasSpecMap;     // Is specular map available?

struct PSOut
{
    float4 f4Diffuse        :   SV_TARGET0;
    float4 f4Normal         :   SV_TARGET1;
    float4 f4Material       :   SV_TARGET2;
    float4 f4VertexNormal   :   SV_TARGET3;
};

void psmain(VSOut input, out PSOut output)
{
    // Sample the diffuse texture
    float4 f4DiffuseColor = tex2D(texDiffuse, input.f2TexCoord);

    // Early alpha-test
    ALPHA_TEST(f4DiffuseColor.a, 0.5f);

    // Sample the specular texture, if present
    if(bHasSpecMap)
        f4DiffuseColor.a = tex2D(texSpec, input.f2TexCoord).r;
    else
        f4DiffuseColor.a = fSpecIntensity;

    // Sample the normal map, if present
    float3 f3NormalMap = float3(0.f, 0.f, 0.f);
    if(bHasNormalMap)
        f3NormalMap = tex2D(texNormal, input.f2TexCoord).rgb * 2.f - 1.f;
    const float3 f3Normal = input.f3Normal + f3NormalMap.x * input.f3Tangent + f3NormalMap.y * input.f3Binormal;

    // Sample the material type (metallic/dielectric) texture
    float fMatType = tex2D(texMatType, input.f2TexCoord).r;

    // Sample the roughness texture
    float fRoughness = tex2D(texRoughness, input.f2TexCoord).r;

    // Final values
    output.f4Diffuse = f4DiffuseColor;
    output.f4Normal = EncodeNormal(f3Normal);
    output.f4Material = float4(fMatType, fRoughness, 0.f, 0.f);
    output.f4VertexNormal = EncodeNormal(input.f3Normal); // low frequency vertex normals required to reduce noise from indirect lighting
}
////////////////////////////////////////////////////////////////////