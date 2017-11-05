/*=============================================================================
 * This file is part of the "GITechDemo" application
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

// Vertex shader /////////////////////////////////////////////////
const float4x4 f44InvProjMat;
const float2 f2HalfTexelOffset;

struct VSOut
{
    float4 f4Position   :   SV_POSITION;
    float2 f2TexCoord   :   TEXCOORD0;
    float3 f3ViewVec    :   TEXCOORD1;
    float2 f2ScreenPos  :   TEXCOORD2;
};

void vsmain(float4 f4Position : POSITION, out VSOut output)
{
    output.f4Position   =   f4Position;
    output.f2TexCoord   =   f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
    output.f3ViewVec    =   mul(f44InvProjMat, float4(f4Position.xy, 0.f, 1.f)).xyz;
    output.f2ScreenPos  =   f4Position.xy;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D texDiffuseBuffer;   // Diffuse color
const sampler2D texNormalBuffer;    // View-space normals
const sampler2D texDepthBuffer;     // Depth values
const sampler2D texMaterialBuffer;  // Roughness and material type (metallic/dielectric)

const samplerCUBE   texIrradianceMap;   // Irradiance map for Cook-Torrance BRDF
const samplerCUBE   texEnvMap;      // Environment map for Cook-Torrance BRDF

const sampler2D texShadowMap;           // Cascaded shadow maps
const float2    f2OneOverShadowMapSize; // 1 / shadow map width/height

const float fDiffuseFactor;     // Scale value for diffuse light
const float fSpecFactor;        // Scale value for specular light
const float fAmbientFactor;     // Scale value for ambient light
const float fIrradianceFactor;  // Scale value for irradiance (Cook-Torrance BRDF only)
const float fReflectionFactor;  // Scale value for reflected light (Cook-Torrance BRDF only)

const float4x4  f44ViewMat;     // View matrix
const float4x4  f44InvViewMat;  // The inverse view matrix

const float3    f3LightDir;                 // The direction of the light
const float4x4  f44ScreenToLightViewMat;    // Composite matrix for transforming screen-space coordinates to light-view space

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

// BRDF model
#define BLINN_PHONG (0)
#define COOK_TORRANCE_GGX (1)
#define COOK_TORRANCE_BECKMANN (2)
#define ASHIKHMIN_SHIRLEY (3)
#define WARD (4)
const unsigned int nBRDFModel;

const bool  bDebugCascades; // Visual cascade debug option

float3 BlinnPhong(const float3 f3DiffuseColor, const float fSpecularPower, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit);
float3 CookTorrance(const float3 f3MaterialColor, const float fMaterialType, const float fRoughness, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit, const unsigned int nBRDFModel);
float3 AshikhminShirley(const float3 f3MaterialColor, const float fMaterialType, const float fRoughness, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit);
float3 Ward(const float3 f3MaterialColor, const float fMaterialType, const float fRoughness, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit);

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
    // Sample the depth buffer
    const float fDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;

    // Early depth check, so that we don't shade
    // the far plane (where the sky will be drawn)
    DEPTH_KILL(fDepth, 1.f);

    // Sample the diffuse buffer
    const float4 f4DiffuseColor = tex2D(texDiffuseBuffer, input.f2TexCoord);

    // Sample the normal buffer
    const float3 f3Normal = DecodeNormal(tex2D(texNormalBuffer, input.f2TexCoord));

    // Sample the material buffer
    // R = material type (metallic/dielectric)
    // G = roughness
    const float2 f2Material = tex2D(texMaterialBuffer, input.f2TexCoord).rg;

    //////////////////////////////////////////////////////////////////////////////////////////
    // Cascaded Shadow Maps                                                                 //
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ee416307%28v=vs.85%29.aspx  //
    //////////////////////////////////////////////////////////////////////////////////////////
    // Step 1: Calculate light-view space position of current pixel
    float4 f4LightViewPos = mul(f44ScreenToLightViewMat, float4(input.f2ScreenPos, fDepth, 1.f));
    f4LightViewPos /= f4LightViewPos.w;

    // Step 2: Find the best valid cascade
    unsigned int nValidCascade = GetCascadeIdx(f4LightViewPos.xy);

    // Step 3: Calculate texture coordinates, then sample from the cascade we found above
    const float3 f3CascadeTexCoord = GetCascadeSpacePos(f4LightViewPos.xyz, nValidCascade);

    // Conditional PCF shadow sampling
#if USE_CONDITIONAL_PCF
    float fPercentLit;
    switch (nValidCascade)
    {
    case 0:
        fPercentLit = PCF_SAMPLE0(texShadowMap, f2OneOverShadowMapSize, f3CascadeTexCoord.xy, f3CascadeTexCoord.z);
        break;
    case 1:
        fPercentLit = PCF_SAMPLE1(texShadowMap, f2OneOverShadowMapSize, f3CascadeTexCoord.xy, f3CascadeTexCoord.z);
        break;
    case 2:
        fPercentLit = PCF_SAMPLE2(texShadowMap, f2OneOverShadowMapSize, f3CascadeTexCoord.xy, f3CascadeTexCoord.z);
        break;
    case 3:
        fPercentLit = PCF_SAMPLE3(texShadowMap, f2OneOverShadowMapSize, f3CascadeTexCoord.xy, f3CascadeTexCoord.z);
        break;
    }
#else
    float fPercentLit = PCF_SAMPLE(texShadowMap, f2OneOverShadowMapSize, f3CascadeTexCoord.xy, f3CascadeTexCoord.z);
    //float fPercentLit = tex2D(texShadowMap, f3CascadeTexCoord.xy).r > f3CascadeTexCoord.z;
#endif
    
    // If required, blend between cascade seams
    float fBlendAmount = 0.f;
    if (fCascadeBlendSize != 0.f)
    {
        float3 f3LQCascadeTexCoord = mul(f44CascadeProjMat[nValidCascade + 1], f4LightViewPos).xyz;

        if (nValidCascade != nCascadeCount - 1 &&
            all(
                step(float2(-0.99f, -0.99f), f3LQCascadeTexCoord.xy) *
                step(f3LQCascadeTexCoord.xy, float2(0.99f, 0.99f))
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
            const int2 n2Section = sign(f3LQCascadeTexCoord.xy);
                
            // Next, we determine the distances to the nearest vertical and horizontal edges.
            const float2 f2DistFromEdge = (float2(1.f, 1.f) - n2Section * f3LQCascadeTexCoord.xy);

            // Finally, we see which edge (horizontal or vertical) is closer.
            // True     - the edge on the X axis (the vertical one)
            // False    - the edge on the Y axis (the horizontal one)
            const bool bXorY = step(f2DistFromEdge.x, f2DistFromEdge.y);

            // We can now calculate the overlap area for the current pixel.
            // To measure the size of an overlapping area, we have to calculate
            // the distance between nValidCascade's right/left/upper/lower edge
            // and (nValidCascade+1)'s left/right/lower/upper edge, respectively.
            // NB: This complicated form is for avoiding more dynamic branching.
            const float fMaxBlendSize =
                bXorY *
                (f2CascadeBoundsMax[nValidCascade + (n2Section.x == 1)].x -
                f2CascadeBoundsMin[nValidCascade + (n2Section.x == -1)].x)
                +
                !bXorY *
                (f2CascadeBoundsMax[nValidCascade + (n2Section.y == -1)].y -
                f2CascadeBoundsMin[nValidCascade + (n2Section.y == 1)].y);
                
            // The blend band size depends on the index of the cascade, but it can't be larger than the overlap area.
            const float fScaledBlendSize = min(fMaxBlendSize, fCascadeBlendSize * (nValidCascade * nValidCascade + 1));

            // The final blend amount value will be used to lerp between cascades.
            const float4 f4BlendAmount = float4(
                (f2CascadeBoundsMin[nValidCascade] + fScaledBlendSize.xx) - f4LightViewPos.xy,
                f4LightViewPos.xy - (f2CascadeBoundsMax[nValidCascade] - fScaledBlendSize.xx)
                );
            fBlendAmount = 
                max(
                    max(
                        f4BlendAmount.x,
                        f4BlendAmount.y
                        ),
                    max(
                        f4BlendAmount.z,
                        f4BlendAmount.w
                        )
                    ) / fScaledBlendSize;

            // If our point is inside the blend band, we can continue with blending
            if (fBlendAmount > 0.f)
            {
                // Calculate texture coordinates for sampling from the cascade one order
                // higher than the cascade from which we sampled earlier
                f3LQCascadeTexCoord.xy =
                    (f3LQCascadeTexCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f)) *
                    fCascadeNormSize +
                    float2(fCascadeNormSize * fmod(nValidCascade + 1, nCascadesPerRow), fCascadeNormSize * floor((nValidCascade + 1) * fCascadeNormSize));

                // Sample from the lower quality cascade and blend between samples appropriately
            #if USE_CONDITIONAL_PCF
                float fPercentLitLQ = 1.f;
                switch (nValidCascade)
                {
                case 0:
                    fPercentLitLQ = PCF_SAMPLE1(texShadowMap, f2OneOverShadowMapSize, f3LQCascadeTexCoord.xy, f3LQCascadeTexCoord.z);
                    break;
                case 1:
                    fPercentLitLQ = PCF_SAMPLE2(texShadowMap, f2OneOverShadowMapSize, f3LQCascadeTexCoord.xy, f3LQCascadeTexCoord.z);
                    break;
                case 2:
                    fPercentLitLQ = PCF_SAMPLE3(texShadowMap, f2OneOverShadowMapSize, f3LQCascadeTexCoord.xy, f3LQCascadeTexCoord.z);
                    break;
                }
            #else
                const float fPercentLitLQ = PCF_SAMPLE(texShadowMap, f2OneOverShadowMapSize, f3LQCascadeTexCoord.xy, f3LQCascadeTexCoord.z);
                //float fPercentLitLQ = tex2D(texShadowMap, f3LQCascadeTexCoord.xy).r > f3LQCascadeTexCoord.z;
            #endif

                // This causes some issues with lower quality cascades where there might be some light bleeding
                //if (fPercentLitLQ > 0.f && fPercentLitLQ < 1.f) // Only blend at shadow edges (i.e. the penumbra region)
                fPercentLit = lerp(fPercentLit, fPercentLitLQ, fBlendAmount);
            }
        }
    }

    // Final color
    switch (nBRDFModel)
    {
    case BLINN_PHONG:
        f4Color = float4(BlinnPhong(f4DiffuseColor.rgb, f4DiffuseColor.a, f3Normal, input.f3ViewVec, fPercentLit), 1.f);
        break;
    case COOK_TORRANCE_GGX:
    case COOK_TORRANCE_BECKMANN:
        f4Color = float4(CookTorrance(f4DiffuseColor.rgb, f2Material.r, f2Material.g, f3Normal, input.f3ViewVec, fPercentLit, nBRDFModel), 1.f);
        break;
    case ASHIKHMIN_SHIRLEY:
        f4Color = float4(AshikhminShirley(f4DiffuseColor.rgb, f2Material.r, f2Material.g, f3Normal, input.f3ViewVec, fPercentLit), 1.f);
        break;
    case WARD:
        f4Color = float4(Ward(f4DiffuseColor.rgb, f2Material.r, f2Material.g, f3Normal, input.f3ViewVec, fPercentLit), 1.f);
        break;
    default:
        f4Color = f4DiffuseColor * fPercentLit + f4DiffuseColor * fAmbientFactor;
        break;
    }
    
    // CSM debug
    if (bDebugCascades)
    {
        const float fBrightness = dot(f4Color.rgb, LUMINANCE_VECTOR);
        f4Color += float4(nValidCascade % 3 == 0, nValidCascade % 3 == 1, nValidCascade % 3 == 2, 0.f) * fBrightness * 0.5f;
        if (fBlendAmount > 0.f)
        {
            f4Color  = float4(nValidCascade % 3 == 0, nValidCascade % 3 == 1, nValidCascade % 3 == 2, 0.f);
            f4Color += float4((nValidCascade + 1) % 3 == 0, (nValidCascade + 1) % 3 == 1, (nValidCascade + 1) % 3 == 2, 0.f);
            f4Color *= fBrightness * fBlendAmount;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Blinn-Phong BRDF model                                                                                   //
// http://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model#High-Level_Shading_Language_code_sample   //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 BlinnPhong(const float3 f3DiffuseColor, const float fSpecularPower, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit)
{
    const float3 f3LightDirView = normalize(mul((float3x3)f44ViewMat, f3LightDir));
    const float fNdotL = dot(f3Normal, -f3LightDirView);
    const float fDiffIntensity = saturate(fNdotL);
    const float3 f3Diffuse = smoothstep(0.f, 1.f, fDiffIntensity) * f3DiffuseColor;
    const float3 f3H = normalize(normalize(-f3ViewVec) - f3LightDirView);
    const float fNdotH = dot(f3Normal, f3H);
    const float fSpecIntensity = pow(saturate(fNdotH), fSpecularPower * 255.f);
    const float3 f3Specular = smoothstep(0.f, 1.f, fDiffIntensity) * fSpecIntensity;

    return (f3Diffuse * fDiffuseFactor + f3Specular * fSpecFactor) * fPercentLit + f3DiffuseColor * fAmbientFactor;
}

//////////////////////////////////////////////////////////////////////////
// Cook-Torrance BRDF model                                             //
// http://graphicrants.blogspot.ro/2013/08/specular-brdf-reference.html //
//////////////////////////////////////////////////////////////////////////

// Custom remapping of roughness parameter for Beckmann.
float AlphaBeckmann(const float fRoughness)
{
    //return fRoughness;
    return fRoughness * fRoughness;
}

// Disney version of roughness parameter remapping for GGX normal distribution fuction
float AlphaGGX_D(const float fRoughness)
{
    return fRoughness * fRoughness;
}

// Disney version of roughness parameter remapping for GGX geometric shadowing term
float AlphaGGX_G(const float fRoughness)
{
    //return fRoughness * fRoughness;
    const float fAlpha = (fRoughness + 1.f) * 0.5f;
    return fAlpha * fAlpha;
}

// Fresnel term
float3 FresnelTerm(const float3 f3F0, const float3 f3HalfVec, const float3 f3LightDir)
{
    const float fLdotH = safe_saturate_dot(-f3LightDir, f3HalfVec);
    return f3F0 + (1.f - f3F0) * pow(1.f - fLdotH, 5.f);
}

// Sclick's approximation using roughness to attenuate fresnel.
float3 FresnelRoughnessTerm(float3 f3F0, float fRoughness, float3 f3Normal, float3 f3ViewDir)
{
    const float fVdotN = safe_saturate_dot(-f3ViewDir, f3Normal);
    return f3F0 + (max(1.f - fRoughness, f3F0) - f3F0) * pow(1.f - fVdotN, 5.f);
}

// GGX normal distribution function
float GGXDistributionTerm(const float fNdotH, const float fAlpha)
{
    const float fAlpha2 = fAlpha * fAlpha;
    const float fNdotH2 = fNdotH * fNdotH;
    const float fP = fNdotH2 * (fAlpha2 - 1.f) + 1.f;
    return fAlpha2 / (PI * fP * fP);
}

// GGX geometric shadowing term
float GGXGeometricTerm(const float fAlpha, const float fNdotX)
{
    const float fAlpha2 = fAlpha * fAlpha;
    const float fNdotX2 = fNdotX * fNdotX;
    return 2.f * fNdotX / (fNdotX + sqrt(fAlpha2 + (1.f - fAlpha2) * fNdotX2));
}

// Beckmann normal distribution function
float BeckmannDistributionTerm(const float fNdotH, const float fAlpha)
{
    const float fAlpha2 = fAlpha * fAlpha;
    const float fNdotH2 = fNdotH * fNdotH;
    const float fNdotH4 = fNdotH2 * fNdotH2;
    const float fExpTerm = exp((fNdotH2 - 1.f) / (fAlpha2 * fNdotH2));
    return fExpTerm / (PI * fAlpha2 * fNdotH4);
}

// Beckmann geometric shadowing term
float BeckmannGeometricTerm(const float fAlpha, const float fNdotX)
{
    const float fNdotX2 = fNdotX * fNdotX;
    const float fC = fNdotX / (fAlpha * sqrt(1.f - fNdotX2));
    const float fC2 = fC * fC;

    if (fC < 1.6f)
        return (3.535f * fC + 2.181f * fC2) / (1.f + 2.276f * fC + 2.577f * fC2);
    else
        return 1.f;
}

// Cook-Torrance BRDF
float3 CookTorrance(const float3 f3MaterialColor, const float fMaterialType, const float fRoughness, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit, const unsigned int nBRDFModel)
{
    const float3 f3LightDirView = normalize(mul((float3x3)f44ViewMat, f3LightDir));
    const float fNdotL = safe_saturate_dot(f3Normal, -f3LightDirView);

    const float3 f3DiffuseAlbedo = f3MaterialColor - f3MaterialColor * fMaterialType;
    const float3 f3SpecularAlbedo = lerp(0.03f, f3MaterialColor, fMaterialType);

    const float3 f3H = normalize(normalize(-f3ViewVec) - f3LightDirView);
    const float fNdotH = safe_saturate_dot(f3Normal, f3H);
    const float fNdotV = max(dot(f3Normal, f3ViewVec), 0.0001f);

    // Fresnel term
    const float3 f3Fresnel = FresnelTerm(f3SpecularAlbedo, f3H, f3LightDirView);

    float fDistrib, fGeom;
    switch (nBRDFModel)
    {
    case COOK_TORRANCE_GGX:
    {
        const float fAlphaD = AlphaGGX_D(fRoughness);
        const float fAlphaG = AlphaGGX_G(fRoughness);
        fDistrib = GGXDistributionTerm(fNdotH, fAlphaD); // Distribution term
        fGeom = GGXGeometricTerm(fAlphaG, fNdotL) * GGXGeometricTerm(fAlphaG, fNdotV); // Geometric term
        break;
    }

    case COOK_TORRANCE_BECKMANN:
    {
        const float fAlpha = AlphaBeckmann(fRoughness);
        fDistrib = BeckmannDistributionTerm(fNdotH, fAlpha); // Distribution term
        fGeom = BeckmannGeometricTerm(fAlpha, fNdotL) * BeckmannGeometricTerm(fAlpha, fNdotV); // Geometric term
        break;
    }

    default:
        fDistrib = fGeom = 0;
        break;
    }

    // Color components
    const float3 f3DiffuseColor = f3DiffuseAlbedo / (PI * (1.f - f3Fresnel));
    const float3 f3SpecularColor = fDistrib * fGeom * f3Fresnel / (4.f * fNdotL * fNdotV);

    // Handled in ScreenSpaceReflection.hlsl as well, so be careful not to have them both active at the same time
    // (i.e. set fReflectionFactor to 0 for this shader when SSR is active and restore it when rendering with SSR shader)
    const float3 f3EnvAlbedo = texCUBEbias(texEnvMap, float4(mul((float3x3)f44InvViewMat, reflect(f3ViewVec, f3Normal)), fRoughness * fRoughness * ENVIRONMENT_MAP_MIP_COUNT)).rgb;
    const float3 f3EnvFresnel = FresnelRoughnessTerm(f3SpecularAlbedo, fRoughness, f3Normal, f3ViewVec);

    const float3 f3Irradiance = texCUBE(texIrradianceMap, mul((float3x3)f44InvViewMat, f3Normal)).rgb;

    // Final color
    return fNdotL * (f3DiffuseColor * (1.f - f3SpecularColor) + f3SpecularColor) * fDiffuseFactor * fPercentLit +
        f3EnvFresnel * f3EnvAlbedo * fReflectionFactor + f3DiffuseAlbedo * f3Irradiance * fIrradianceFactor;
}

//////////////////////////////////////////////////////////
// Ashikhmin-Shirley BRDF model                         //
// http://www.cs.utah.edu/~michael/brdfs/jgtbrdf.pdf    //
//////////////////////////////////////////////////////////
float3 AshikhminShirley(const float3 f3MaterialColor, const float fMaterialType, const float fRoughness, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit)
{
    const float3 f3LightDirView = normalize(mul((float3x3)f44ViewMat, f3LightDir));
    const float fNdotL = safe_saturate_dot(f3Normal, -f3LightDirView);

    const float3 f3DiffuseAlbedo = f3MaterialColor - f3MaterialColor * fMaterialType;
    const float3 f3SpecularAlbedo = lerp(0.03f, f3MaterialColor, fMaterialType);

    const float3 f3H = normalize(normalize(-f3ViewVec) - f3LightDirView);
    const float fNdotH = safe_saturate_dot(f3Normal, f3H);
    const float fNdotV = max(dot(f3Normal, -f3ViewVec), 0.0001f);

    const float3 f3Epsilon = float3(1.f, 0.f, 0.f);
    const float3 f3Tangent = normalize(cross(f3Normal, f3Epsilon));
    const float3 f3Bitangent = normalize(cross(f3Normal, f3Tangent));

    const float fLdotH = dot(-f3LightDirView, f3H);
    const float fTdotH = dot(f3Tangent, f3H);
    const float fBdotH = dot(f3Bitangent, f3H);

    // The specular exponents can theoretically go up to infinity (thus the very high valued scale factor),
    // representing a perfectly smooth surface, which is not physically based.
    // This remapping of the roughness term allows for more perceptual consistency between Cook-Torrance and Ashikhmin-Shirley BRDFs.
    const float fScale = 10000.f;
    const float fNu = pow(1.f - fRoughness * 0.7f, 6.f) * fScale;
    const float fNv = fNu; // There is support for anisotropic materials, however, we don't make use of it at the moment

    // Diffuse term
    float fPd = 28.f / (23.f * PI) *
                (1.f - pow(1.f - fNdotL * 0.5f, 5.f)) *
                (1.f - pow(1.f - fNdotV * 0.5f, 5.f));

    // Fresnel term
    const float3 f3Fresnel = FresnelTerm(f3SpecularAlbedo, f3H, f3LightDirView);

    // Specular term
    const float fPsNum = sqrt((fNu + 1.f) * (fNv + 1.f)) * pow(fNdotH, fNu * fTdotH * fTdotH + fNv * fBdotH * fBdotH);
    const float fPsDen = 8.f * PI * fLdotH * max(fNdotL, fNdotV);
    const float fPs = fPsNum / fPsDen;

    // Ambient term
    const float3 f3Irradiance = texCUBE(texIrradianceMap, mul((float3x3)f44InvViewMat, f3Normal)).rgb;

    // Final value
    return (f3DiffuseAlbedo * fPd * (1.f - f3SpecularAlbedo) + f3SpecularAlbedo * fPs * f3Fresnel) * fDiffuseFactor * fPercentLit + f3DiffuseAlbedo * f3Irradiance * fIrradianceFactor;
}

//////////////////////////////////////////////////////////////
// Ward BRDF model                                          //
// https://cseweb.ucsd.edu/~ravir/6998/papers/p265-ward.pdf //
//////////////////////////////////////////////////////////////
float3 Ward(const float3 f3MaterialColor, const float fMaterialType, const float fRoughness, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit)
{
    const float3 f3LightDirView = normalize(mul((float3x3)f44ViewMat, f3LightDir));
    const float fNdotL = safe_saturate_dot(f3Normal, -f3LightDirView);

    const float3 f3DiffuseAlbedo = f3MaterialColor - f3MaterialColor * fMaterialType;
    const float3 f3SpecularAlbedo = lerp(0.03f, f3MaterialColor, fMaterialType);

    const float3 f3H = normalize(normalize(-f3ViewVec) - f3LightDirView);
    const float fNdotH = safe_saturate_dot(f3Normal, f3H);
    const float fNdotV = max(dot(f3Normal, -f3ViewVec), 0.0001f);

    const float fAlpha = pow(fRoughness * 0.7f + 0.3f, 6.f) + 1e-5;

    // Specular term
    const float fPsNum = exp(-pow(tan(acos(fNdotH)), 2) / fAlpha);
    const float fPsDen = 4.f * PI * fAlpha * sqrt(fNdotL * fNdotV);
    const float fPs = fPsNum / fPsDen;

    // Ambient term
    const float3 f3Irradiance = texCUBE(texIrradianceMap, mul((float3x3)f44InvViewMat, f3Normal)).rgb;

    // Final value
    return fNdotL * (f3DiffuseAlbedo + f3SpecularAlbedo * fPs) * fDiffuseFactor * fPercentLit + f3DiffuseAlbedo * f3Irradiance * fIrradianceFactor;
}

////////////////////////////////////////////////////////////////////
