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

#ifndef CSMUTILS_HLSLI_
#define CSMUTILS_HLSLI_

#include "Common.hlsli"

#define NUM_CASCADES (4)        // Number of supported cascades
const float2    f2CascadeBoundsMin[NUM_CASCADES];   // Light-view space AABBs corresponding
const float2    f2CascadeBoundsMax[NUM_CASCADES];   // to each shadow cascade
const float4x4  f44CascadeProjMat[NUM_CASCADES];    // light space projection matrix
const float     fCascadeBlendSize;  // Size of the blend band for blurring between cascade boundaries

// For performance reasons, we set these variables
// directly in the shader, instead of from the
// application (so that they are known at compile time)
// NB: Also, we don't want to waste ALU calculating them
static const unsigned int nCascadeCount = NUM_CASCADES;                 // Number of cascades
static const unsigned int nCascadesPerRow = ceil(sqrt(nCascadeCount));  // Number of cascades per row
static const float fCascadeNormSize = rcp(nCascadesPerRow);             // Normalized size of a cascade, i.e. 1.f / nCascadesPerRow

// Calculates the best (i.e. highest detail) valid cascade index from
// the light view space (not normalized) position of the current sample
const unsigned int GetCascadeIdx(const float2 f2LightViewPos)
{
    unsigned int nValidCascade = 0;

    UNROLL for (int nCascade = nCascadeCount - 1; nCascade >= 0; nCascade--)
    {
        // If f2LightViewPos > f2CascadeBoundsMin[nCascade] then step() returns 1, else 0 (per component evaluation).
        // After evaluating the second step(), we perform a per component scalar multiply which will result in either
        // 0 or 1 on either components. If both components are 1, then all() will return true, meaning that this cascade is valid.
        // On a higher level, the first step() determines whether the current's pixel light-view space position is above and
        // to the right of the lower left corner of the cascade, whereas the second step() determines whether it is below and
        // to the left of the upper right corner of the cascade. If all the conditions are met, then the cascade is valid.
        const bool bValid =
            all(
                step(
                    f2CascadeBoundsMin[nCascade],
                    f2LightViewPos
                    ) *
                step(
                    f2LightViewPos,
                    f2CascadeBoundsMax[nCascade]
                    )
                );
        nValidCascade = lerp(nValidCascade, nCascade, bValid);
    }

    return nValidCascade;
}

// Converts the provided light view space (not normalized) position of the sample
// to the requested cascade's texture space coordinates (i.e. '.xy' holds the
// coordinates for sampling the shadow map and '.z' holds the sample's depth for
// comparison with the value extracted from the cascaded shadow map)
const float3 GetCascadeSpacePos(const float3 f3LightViewPos, const unsigned int nValidCascade)
{
    float3 f3CascadeTexCoord = mul(f44CascadeProjMat[nValidCascade], float4(f3LightViewPos, 1.f)).xyz;

    // After bringing our point in normalized light-view space, calculate
    // proper texture coordinates for shadow map sampling
    f3CascadeTexCoord.xy =
        (f3CascadeTexCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f)) *
        fCascadeNormSize +
        float2(fCascadeNormSize * fmod(nValidCascade, nCascadesPerRow), fCascadeNormSize * floor(nValidCascade * fCascadeNormSize));

    return f3CascadeTexCoord;
}

#endif // CSMUTILS_HLSLI_
