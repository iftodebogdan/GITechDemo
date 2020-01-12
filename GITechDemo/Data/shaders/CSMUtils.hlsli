/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   CSMUtils.hlsli
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

#ifndef CSMUTILS_HLSLI
#define CSMUTILS_HLSLI

#include "Common.hlsli"

struct CSM
{
    static const unsigned int CascadeCount = 4; // Number of supported cascades

#ifdef HLSL
    // For performance reasons, we set these variables
    // directly in the shader, instead of from the
    // application (so that they are known at compile time)
    // NB: Also, we don't want to waste ALU calculating them
    static const int CascadesPerRow = ceil(sqrt(CascadeCount)); // Number of cascades per row
    static const float CascadeNormSize = 1.f / CascadeCount;    // Normalized size of a cascade, i.e. 1.f / nCascadesPerRow
#endif
};

CBUFFER_RESOURCE(CSM,
    GPU_float2    CascadeBoundsMin[CSM::CascadeCount]; // Light-view space AABBs corresponding
    GPU_float2    CascadeBoundsMax[CSM::CascadeCount]; // to each shadow cascade
    GPU_float4x4  CascadeProjMat[CSM::CascadeCount]; // light space projection matrix
    GPU_float     CascadeBlendSize; // Size of the blend band for blurring between cascade boundaries
);

#ifdef HLSL
// Calculates the best (i.e. highest detail) valid cascade index from
// the light view space (not normalized) position of the current sample
const unsigned int GetCascadeIdx(const float2 lightViewPos)
{
    unsigned int validCascade = 0;

    UNROLL for (int cascade = CSM::CascadeCount - 1; cascade >= 0; cascade--)
    {
        // If lightViewPos > CSMParams.CascadeBoundsMin[nCascade] then step() returns 1, else 0 (per component evaluation).
        // After evaluating the second step(), we perform a per component scalar multiply which will result in either
        // 0 or 1 on either components. If both components are 1, then all() will return true, meaning that this cascade is valid.
        // On a higher level, the first step() determines whether the current's pixel light-view space position is above and
        // to the right of the lower left corner of the cascade, whereas the second step() determines whether it is below and
        // to the left of the upper right corner of the cascade. If all the conditions are met, then the cascade is valid.
        const bool valid =
            all(
                step(
                    CSMParams.CascadeBoundsMin[cascade],
                    lightViewPos
                    ) *
                step(
                    lightViewPos,
                    CSMParams.CascadeBoundsMax[cascade]
                    )
                );
        validCascade = lerp(validCascade, cascade, valid);
    }

    return validCascade;
}

// Converts the provided light view space (not normalized) position of the sample
// to the requested cascade's texture space coordinates (i.e. '.xy' holds the
// coordinates for sampling the shadow map and '.z' holds the sample's depth for
// comparison with the value extracted from the cascaded shadow map)
const float3 GetCascadeSpacePos(const float3 lightViewPos, const unsigned int validCascade)
{
    float3 cascadeTexCoord = mul(CSMParams.CascadeProjMat[validCascade], float4(lightViewPos, 1.f)).xyz;

    // After bringing our point in normalized light-view space, calculate
    // proper texture coordinates for shadow map sampling
    cascadeTexCoord.xy =
        (cascadeTexCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f)) *
        CSM::CascadeNormSize +
        float2(CSM::CascadeNormSize * fmod(validCascade, CSM::CascadesPerRow), CSM::CascadeNormSize * floor(validCascade * CSM::CascadeNormSize));

    return cascadeTexCoord;
}
#endif // HLSL

#endif // CSMUTILS_HLSLI
