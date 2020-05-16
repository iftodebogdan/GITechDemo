/*=============================================================================
 * This file is part of the "VirtualMuseum" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   DirectionalLight.hlsl
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
#include "CSMUtils.hlsli"
#include "BRDFUtils.hlsli"

TEXTURE_2D_RESOURCE(DirectionalLight_DiffuseBuffer);    // Diffuse color
TEXTURE_2D_RESOURCE(DirectionalLight_NormalBuffer);     // View-space normals
TEXTURE_2D_RESOURCE(DirectionalLight_DepthBuffer);      // Depth values
TEXTURE_2D_RESOURCE(DirectionalLight_MaterialBuffer);   // Roughness and material type (metallic/dielectric)
TEXTURE_2D_RESOURCE(DirectionalLight_ShadowMap);        // Cascaded shadow maps

CBUFFER_RESOURCE(DirectionalLight,
    GPU_float2 HalfTexelOffset;
    GPU_float2 OneOverShadowMapSize;    // 1 / shadow map width/height
    GPU_bool DebugCascades;             // Visual cascade debug option
);

#ifdef HLSL
struct VSOut
{
    float4 Position   :   SV_POSITION;
    float2 TexCoord   :   TEXCOORD0;
    float3 ViewVec    :   TEXCOORD1;
    float2 ScreenPos  :   TEXCOORD2;
};

// Vertex shader /////////////////////////////////////////////////
#ifdef VERTEX
void vsmain(float4 position : POSITION, out VSOut output)
{
    output.Position   = position;
    output.TexCoord   = position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + DirectionalLightParams.HalfTexelOffset;
    output.ViewVec    = mul(FrameParams.InvProjMat, float4(position.xy, 0.f, 1.f)).xyz;
    output.ScreenPos  = position.xy;
}
#endif // VERTEX
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
#ifdef PIXEL
// Conditional PCF shadow sampling for different sampling methods for each cascade
// NB: PCF_SAMPLE0 corresponds to most detailed cascade (highest resolution),
// whereas PCF_SAMPLE3 corresponds to the least detailed cascade (lowest resolution)
#define USE_CONDITIONAL_PCF (0)
#if USE_CONDITIONAL_PCF
    #define PCF_SAMPLE0 PCF4x4Poisson
    #define PCF_SAMPLE1 PCF12TapPoisson
    #define PCF_SAMPLE2 PCF3x3Poisson
    #define PCF_SAMPLE3 PCF2x2Poisson
#else
    #define PCF_SAMPLE  PCF4x4PoissonRotatedx4
#endif

void psmain(VSOut input, out float4 color : SV_TARGET)
{
    // Sample the depth buffer
    const float depth = tex2D(DirectionalLight_DepthBuffer, input.TexCoord).r;

    // Early depth check, so that we don't shade
    // the far plane (where the sky will be drawn)
    DEPTH_KILL(depth, 1.f);

    // Sample the diffuse buffer
    const float4 diffuseColor = tex2D(DirectionalLight_DiffuseBuffer, input.TexCoord);

    // Sample the normal buffer
    const float3 normal = DecodeNormal(tex2D(DirectionalLight_NormalBuffer, input.TexCoord));

    // Sample the material buffer
    // R = material type (metallic/dielectric)
    // G = roughness
    const float2 material = tex2D(DirectionalLight_MaterialBuffer, input.TexCoord).rg;

    //////////////////////////////////////////////////////////////////////////////////////////
    // Cascaded Shadow Maps                                                                 //
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ee416307%28v=vs.85%29.aspx  //
    //////////////////////////////////////////////////////////////////////////////////////////
    // Step 1: Calculate light-view space position of current pixel
    float4 lightViewPos = mul(FrameParams.ScreenToLightViewMat, float4(input.ScreenPos, depth, 1.f));
    lightViewPos /= lightViewPos.w;

    // Step 2: Find the best valid cascade
    unsigned int validCascade = GetCascadeIdx(lightViewPos.xy);

    // Step 3: Calculate texture coordinates, then sample from the cascade we found above
    const float3 cascadeTexCoord = GetCascadeSpacePos(lightViewPos.xyz, validCascade);

    // Conditional PCF shadow sampling
#if USE_CONDITIONAL_PCF
    float percentLit;
    switch (validCascade)
    {
    case 0:
        percentLit = PCF_SAMPLE0(DirectionalLight_ShadowMap, DirectionalLightParams.OneOverShadowMapSize, cascadeTexCoord.xy, cascadeTexCoord.z);
        break;
    case 1:
        percentLit = PCF_SAMPLE1(DirectionalLight_ShadowMap, DirectionalLightParams.OneOverShadowMapSize, cascadeTexCoord.xy, cascadeTexCoord.z);
        break;
    case 2:
        percentLit = PCF_SAMPLE2(DirectionalLight_ShadowMap, DirectionalLightParams.OneOverShadowMapSize, cascadeTexCoord.xy, cascadeTexCoord.z);
        break;
    case 3:
        percentLit = PCF_SAMPLE3(DirectionalLight_ShadowMap, DirectionalLightParams.OneOverShadowMapSize, cascadeTexCoord.xy, cascadeTexCoord.z);
        break;
    }
#else
    float percentLit = PCF_SAMPLE(DirectionalLight_ShadowMap, DirectionalLightParams.OneOverShadowMapSize, cascadeTexCoord.xy, cascadeTexCoord.z);
    //float percentLit = tex2D(DirectionalLight_ShadowMap, cascadeTexCoord.xy).r >= cascadeTexCoord.z;
#endif
    
    // If required, blend between cascade seams
    float blendAmount = 0.f;
    if (CSMParams.CascadeBlendSize != 0.f)
    {
        float3 cascadeLQTexCoord = mul(CSMParams.CascadeProjMat[validCascade + 1], lightViewPos).xyz;

        if (validCascade != CSM::CascadeCount - 1 &&
            all(
                step(float2(-0.99f, -0.99f), cascadeLQTexCoord.xy) *
                step(cascadeLQTexCoord.xy, float2(0.99f, 0.99f))
                )
            )
        {
            // Check to see if the blend band size is larger than the
            // cascade overlap size, which would result in artifacts.
            // We start by finding in which section of the next cascade
            // lies the pixel we are shading.
            // (-1, -1) - lower-left
            // (-1,  1) - upper-left
            // ( 1, -1) - lower-right
            // ( 1,  1) - upper-right
            const int2 section = sign(cascadeLQTexCoord.xy);
                
            // Next, we determine the distances to the nearest vertical and horizontal edges.
            const float2 distFromEdge = (float2(1.f, 1.f) - section * cascadeLQTexCoord.xy);

            // Finally, we see which edge (horizontal or vertical) is closer.
            // True     - the edge on the X axis (the vertical one)
            // False    - the edge on the Y axis (the horizontal one)
            const bool isXorY = step(distFromEdge.x, distFromEdge.y);

            // We can now calculate the overlap area for the current pixel.
            // To measure the size of an overlapping area, we have to calculate
            // the distance between nValidCascade's right/left/upper/lower edge
            // and (nValidCascade+1)'s left/right/lower/upper edge, respectively.
            // NB: This complicated form is for avoiding more dynamic branching.
            const float maxBlendSize =
                isXorY *
                (CSMParams.CascadeBoundsMax[validCascade + (section.x == 1)].x -
                 CSMParams.CascadeBoundsMin[validCascade + (section.x == -1)].x)
                +
                !isXorY *
                (CSMParams.CascadeBoundsMax[validCascade + (section.y == -1)].y -
                 CSMParams.CascadeBoundsMin[validCascade + (section.y == 1)].y);
                
            // The blend band size depends on the index of the cascade, but it can't be larger than the overlap area.
            const float scaledBlendSize = min(maxBlendSize, CSMParams.CascadeBlendSize * (validCascade * validCascade + 1));

            // The final blend amount value will be used to lerp between cascades.
            const float4 blendAmountPacked = float4(
                (CSMParams.CascadeBoundsMin[validCascade] + scaledBlendSize.xx) - lightViewPos.xy,
                lightViewPos.xy - (CSMParams.CascadeBoundsMax[validCascade] - scaledBlendSize.xx)
                );
            blendAmount = 
                max(
                    max(
                        blendAmountPacked.x,
                        blendAmountPacked.y
                        ),
                    max(
                        blendAmountPacked.z,
                        blendAmountPacked.w
                        )
                    ) / scaledBlendSize;

            // If our point is inside the blend band, we can continue with blending
            if (blendAmount > 0.f)
            {
                // Calculate texture coordinates for sampling from the cascade one order
                // higher than the cascade from which we sampled earlier
                cascadeLQTexCoord.xy =
                    (cascadeLQTexCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f)) *
                    CSM::CascadeNormSize +
                    float2(CSM::CascadeNormSize * fmod(validCascade + 1, CSM::CascadesPerRow), CSM::CascadeNormSize * floor((validCascade + 1) * CSM::CascadeNormSize));
                cascadeLQTexCoord.z = saturate(cascadeLQTexCoord.z);

                // Sample from the lower quality cascade and blend between samples appropriately
            #if USE_CONDITIONAL_PCF
                float percentLitLQ = 1.f;
                switch (validCascade)
                {
                case 0:
                    percentLitLQ = PCF_SAMPLE1(DirectionalLight_ShadowMap, DirectionalLightParams.OneOverShadowMapSize, cascadeLQTexCoord.xy, cascadeLQTexCoord.z);
                    break;
                case 1:
                    percentLitLQ = PCF_SAMPLE2(DirectionalLight_ShadowMap, DirectionalLightParams.OneOverShadowMapSize, cascadeLQTexCoord.xy, cascadeLQTexCoord.z);
                    break;
                case 2:
                    percentLitLQ = PCF_SAMPLE3(DirectionalLight_ShadowMap, DirectionalLightParams.OneOverShadowMapSize, cascadeLQTexCoord.xy, cascadeLQTexCoord.z);
                    break;
                }
            #else
                const float percentLitLQ = PCF_SAMPLE(DirectionalLight_ShadowMap, DirectionalLightParams.OneOverShadowMapSize, cascadeLQTexCoord.xy, cascadeLQTexCoord.z);
                //float percentLitLQ = tex2D(DirectionalLightParams.ShadowMap, cascadeLQTexCoord.xy).r > cascadeLQTexCoord.z;
            #endif

                // This causes some issues with lower quality cascades where there might be some light bleeding
                //if (fPercentLitLQ > 0.f && fPercentLitLQ < 1.f) // Only blend at shadow edges (i.e. the penumbra region)
                percentLit = lerp(percentLit, percentLitLQ, blendAmount);
            }
        }
    }

    // Final color
    switch (BRDFParams.BRDFModel)
    {
    case BRDF::BlinnPhong:
        color = float4(BlinnPhong(diffuseColor.rgb, diffuseColor.a, normal, input.ViewVec, percentLit), 1.f);
        break;
    case BRDF::CookTorranceGGX:
    case BRDF::CookTorranceBeckmann:
        color = float4(CookTorrance(diffuseColor.rgb, material.r, material.g, normal, input.ViewVec, percentLit, BRDFParams.BRDFModel), 1.f);
        break;
    case BRDF::AshikhminShirley:
        color = float4(AshikhminShirley(diffuseColor.rgb, material.r, material.g, normal, input.ViewVec, percentLit), 1.f);
        break;
    case BRDF::Ward:
        color = float4(Ward(diffuseColor.rgb, material.r, material.g, normal, input.ViewVec, percentLit), 1.f);
        break;
    default:
        color = diffuseColor * percentLit + diffuseColor * BRDFParams.AmbientFactor;
        break;
    }
    
    // CSM debug
    if (DirectionalLightParams.DebugCascades)
    {
        const float brightness = dot(color.rgb, PostProcessingUtils::LumaCoef);
        color += float4(validCascade % 3 == 0, validCascade % 3 == 1, validCascade % 3 == 2, 0.f) * brightness * 0.5f;
        if (blendAmount > 0.f)
        {
            color  = float4(validCascade % 3 == 0, validCascade % 3 == 1, validCascade % 3 == 2, 0.f);
            color += float4((validCascade + 1) % 3 == 0, (validCascade + 1) % 3 == 1, (validCascade + 1) % 3 == 2, 0.f);
            color *= brightness * blendAmount;
        }
    }
}
#endif // PIXEL
////////////////////////////////////////////////////////////////////
#endif // HLSL
