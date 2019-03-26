/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Common.hlsli
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

#ifndef BRDFUTILS_HLSLI_
#define BRDFUTILS_HLSLI_

const samplerCUBE   texIrradianceMap;   // Irradiance map for Cook-Torrance BRDF
const samplerCUBE   texEnvMap;          // Environment map for Cook-Torrance BRDF

const float fDiffuseFactor;     // Scale value for diffuse light
const float fSpecFactor;        // Scale value for specular light
const float fAmbientFactor;     // Scale value for ambient light
const float fIrradianceFactor;  // Scale value for irradiance (Cook-Torrance BRDF only)
const float fReflectionFactor;  // Scale value for reflected light (Cook-Torrance BRDF only)

const float4x4  f44ViewMat;     // View matrix
const float4x4  f44InvViewMat;  // The inverse view matrix

const float3    f3LightDir;     // The direction of the light

// BRDF model
#define BLINN_PHONG (0)
#define COOK_TORRANCE_GGX (1)
#define COOK_TORRANCE_BECKMANN (2)
#define ASHIKHMIN_SHIRLEY (3)
#define WARD (4)
const unsigned int nBRDFModel;

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

// Mip computation based on roughness (e.g. for environment cubemap)
float ComputeMipFromRoughness(const float fRoughness, const int nMipCount)
{
    float fLevel = 3.f - 1.15f * log2(fRoughness);
    return (float)nMipCount - 1.f - fLevel;
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
    const float3 f3EnvAlbedo = texCUBElod(texEnvMap, float4(mul((float3x3)f44InvViewMat, reflect(f3ViewVec, f3Normal)), ComputeMipFromRoughness(fRoughness, ENVIRONMENT_MAP_MIP_COUNT))).rgb;
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

#endif // BRDFUTILS_HLSLI_
