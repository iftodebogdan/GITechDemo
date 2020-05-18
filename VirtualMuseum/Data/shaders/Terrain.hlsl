/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Terrain.hlsl
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
#include "BRDFUtils.hlsli"
#include "PostProcessingUtils.hlsli"

#undef tex2D // we don't want to override this here

TEXTURE_2D_RESOURCE(Terrain_Diffuse);   // Diffuse color
TEXTURE_2D_RESOURCE(Terrain_Normal);    // Normal map
TEXTURE_2D_RESOURCE(Terrain_MatType);   // Material type
TEXTURE_2D_RESOURCE(Terrain_Roughness); // Roughness

CBUFFER_RESOURCE(Terrain,
    GPU_float2 HalfTexelOffset;
    GPU_float3 TerrainPlanePoint;
    GPU_float3 TerrainPlaneNormal;
    GPU_float3 CameraWorldPos;
    GPU_float3 CameraWorldDir;
    GPU_float2 TextureMappingScale;
);

#ifdef HLSL
struct VSOut
{
                    float4 Position     : SV_POSITION;
                    float2 ScreenPos    : TEXCOORD0;
    nointerpolation float3 Normal       : NORMAL;
    nointerpolation float3 Tangent      : TANGENT;
    nointerpolation float3 Binormal     : BINORMAL;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, out VSOut output)
{
    output.Position = position;
    output.ScreenPos = position.xy;
    output.Normal = normalize(mul((float3x3)BRDFParams.ViewMat, TerrainParams.TerrainPlaneNormal));
    output.Tangent = normalize(cross(float3(0.f, 0.f, 1.f), output.Normal));
    output.Binormal = normalize(cross(output.Normal, output.Tangent));
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
    float  Depth        : SV_DEPTH;
};

float LinePlaneIntersection(float3 planePoint, float3 planeNormal, float3 linePoint, float3 lineDirection) {
    if (dot(planeNormal, lineDirection) == 0)
    {
        return false;
    }

    return (dot(planeNormal, planePoint) - dot(planeNormal, linePoint)) / dot(planeNormal, lineDirection);
    //return t > 0;// linePoint.plus(lineDirection.normalize().scale(t));
}

void psmain(VSOut input, out PSOut output)
{
    const float4 camViewDirWSPreW = mul(FrameParams.InvViewProjMat, float4(input.ScreenPos, 1.f, 1.f));
    const float3 camViewDirWS = normalize(camViewDirWSPreW.xyz / camViewDirWSPreW.w);

    const float t = LinePlaneIntersection(
        TerrainParams.TerrainPlanePoint, TerrainParams.TerrainPlaneNormal,
        TerrainParams.CameraWorldPos, camViewDirWS);

    if(t < 0 || t > PostProcessingParams.ZFar)
    {
        discard;
    }

    const float3 terrainPosWS = TerrainParams.CameraWorldPos + camViewDirWS * t;
    const float3 terrainPosVs = mul(BRDFParams.ViewMat, float4(terrainPosWS, 1.f)).xyz;
    const float4 terrainPosProjPreW = mul(FrameParams.ViewProjMat, float4(terrainPosWS, 1.f));
    const float3 terrainPosProj = terrainPosProjPreW.xyz / terrainPosProjPreW.w;

    const float2 texCoord = terrainPosWS.xz * TerrainParams.TextureMappingScale;

    const float3 normalMap = tex2D(Terrain_Normal, texCoord).rgb * 2.f - 1.f;
    const float3 normal = input.Normal + normalMap.x * input.Tangent + normalMap.y * input.Binormal;

    //const float distFactor = (1 - saturate(t / PostProcessingParams.ZFar));
    const float distFactor = smoothstep(PostProcessingParams.ZFar, 0, t);

    output.Diffuse = tex2D(Terrain_Diffuse, texCoord) * distFactor;
    output.Normal = EncodeNormal(lerp(float3(0, 1, 0), normal, distFactor));
    output.Material = lerp(float4(0, 1, 0, 0), float4(tex2D(Terrain_MatType, texCoord).r, tex2D(Terrain_Roughness, texCoord).r, 0.f, 0.f), distFactor);
    output.VertexNormal = EncodeNormal(lerp(float3(0, 1, 0), input.Normal, distFactor));
    output.Depth = terrainPosProj.z;
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
