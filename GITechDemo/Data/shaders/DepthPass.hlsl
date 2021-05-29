/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   DepthPass.hlsl
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

CBUFFER_RESOURCE(DepthPass,
    GPU_float4x4 WorldViewProjMat;
);

#ifdef HLSL
// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
float4 vsmain(float4 position : POSITION) : SV_POSITION
{
    float4 outputPosition = mul(DepthPassParams.WorldViewProjMat, position);
    PatchVSOutputPositionForHalfPixelOffset(outputPosition);
    return outputPosition;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
float4 psmain(float4 position : SV_POSITION) : SV_TARGET
{
    return float4(1.f, 0.f, 1.f, 1.f);
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
