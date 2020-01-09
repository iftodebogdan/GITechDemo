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

#include "Common.hlsli"

// BRDF model
struct BRDF
{
    static const unsigned int BlinnPhong = 0;
    static const unsigned int CookTorranceGGX = 1;
    static const unsigned int CookTorranceBeckmann = 2;
    static const unsigned int AshikhminShirley = 3;
    static const unsigned int Ward = 4;
    static const unsigned int BRDFModelCount = 5;
};

struct BRDFConstantTable
{
    GPU_float DiffuseFactor;     // Scale value for diffuse light
    GPU_float SpecFactor;        // Scale value for specular light
    GPU_float AmbientFactor;     // Scale value for ambient light
    GPU_float IrradianceFactor;  // Scale value for irradiance (Cook-Torrance BRDF only)
    GPU_float ReflectionFactor;  // Scale value for reflected light (Cook-Torrance BRDF only)

    GPU_float4x4 ViewMat;     // View matrix
    GPU_float4x4 InvViewMat;  // The inverse view matrix
    
    GPU_float3 LightDir;     // The direction of the light

    GPU_uint BRDFModel;

};

#ifdef HLSL
cbuffer BRDFResourceTable
{
    samplerCUBE   BRDF_IrradianceMap;   // Irradiance map for Cook-Torrance BRDF
    samplerCUBE   BRDF_EnvMap;          // Environment map for Cook-Torrance BRDF

    BRDFConstantTable BRDFParams;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Blinn-Phong BRDF model                                                                                   //
// http://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model#High-Level_Shading_Language_code_sample   //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
float3 BlinnPhong(const float3 diffuseColor, const float specularPower, const float3 normal, const float3 viewVec, const float percentLit)
{
    const float3 lightDirView = normalize(mul((float3x3)BRDFParams.ViewMat, BRDFParams.LightDir));
    const float NdotL = dot(normal, -lightDirView);
    const float diffIntensity = saturate(NdotL);
    const float3 diffuse = smoothstep(0.f, 1.f, diffIntensity) * diffuseColor;
    const float3 H = normalize(normalize(-viewVec) - lightDirView);
    const float NdotH = dot(normal, H);
    const float specIntensity = pow(saturate(NdotH), specularPower * 255.f);
    const float3 specular = smoothstep(0.f, 1.f, diffIntensity) * specIntensity;

    return (diffuse * BRDFParams.DiffuseFactor + specular * BRDFParams.SpecFactor) * percentLit + diffuseColor * BRDFParams.AmbientFactor;
}

//////////////////////////////////////////////////////////////////////////
// Cook-Torrance BRDF model                                             //
// http://graphicrants.blogspot.ro/2013/08/specular-brdf-reference.html //
//////////////////////////////////////////////////////////////////////////

// Custom remapping of roughness parameter for Beckmann.
float AlphaBeckmann(const float roughness)
{
    //return roughness;
    return roughness * roughness;
}

// Disney version of roughness parameter remapping for GGX normal distribution fuction
float AlphaGGX_D(const float roughness)
{
    return roughness * roughness;
}

// Disney version of roughness parameter remapping for GGX geometric shadowing term
float AlphaGGX_G(const float roughness)
{
    //return fRoughness * fRoughness;
    const float alpha = (roughness + 1.f) * 0.5f;
    return alpha * alpha;
}

// Fresnel term
float3 FresnelTerm(const float3 F0, const float3 halfVec, const float3 lightDir)
{
    const float LdotH = safe_saturate_dot(-lightDir, halfVec);
    return F0 + (1.f - F0) * pow(1.f - LdotH, 5.f);
}

// Sclick's approximation using roughness to attenuate fresnel.
float3 FresnelRoughnessTerm(float3 F0, float roughness, float3 normal, float3 viewDir)
{
    const float VdotN = safe_saturate_dot(-viewDir, normal);
    return F0 + (max(1.f - roughness, F0) - F0) * pow(1.f - VdotN, 5.f);
}

// GGX normal distribution function
float GGXDistributionTerm(const float NdotH, const float alpha)
{
    const float alpha2 = alpha * alpha;
    const float NdotH2 = NdotH * NdotH;
    const float P = NdotH2 * (alpha2 - 1.f) + 1.f;
    return alpha2 / (PI * P * P);
}

// GGX geometric shadowing term
float GGXGeometricTerm(const float alpha, const float NdotX)
{
    const float alpha2 = alpha * alpha;
    const float NdotX2 = NdotX * NdotX;
    return 2.f * NdotX / (NdotX + sqrt(alpha2 + (1.f - alpha2) * NdotX2));
}

// Beckmann normal distribution function
float BeckmannDistributionTerm(const float NdotH, const float alpha)
{
    const float alpha2 = alpha * alpha;
    const float NdotH2 = NdotH * NdotH;
    const float NdotH4 = NdotH2 * NdotH2;
    const float expTerm = exp((NdotH2 - 1.f) / (alpha2 * NdotH2));
    return expTerm / (PI * alpha2 * NdotH4);
}

// Beckmann geometric shadowing term
float BeckmannGeometricTerm(const float alpha, const float NdotX)
{
    const float NdotX2 = NdotX * NdotX;
    const float C = NdotX / (alpha * sqrt(1.f - NdotX2));
    const float C2 = C * C;

    if (C < 1.6f)
        return (3.535f * C + 2.181f * C2) / (1.f + 2.276f * C + 2.577f * C2);
    else
        return 1.f;
}

// Mip computation based on roughness (e.g. for environment cubemap)
float ComputeMipFromRoughness(const float roughness, const int mipCount)
{
    float level = 3.f - 1.15f * log2(roughness);
    return (float)mipCount - 1.f - level;
}

// Cook-Torrance BRDF
float3 CookTorrance(const float3 materialColor, const float materialType, const float roughness, const float3 normal, const float3 viewVec, const float percentLit, const unsigned int BRDFModel)
{
    const float3 lightDirView = normalize(mul((float3x3)BRDFParams.ViewMat, BRDFParams.LightDir));
    const float NdotL = safe_saturate_dot(normal, -lightDirView);

    const float3 diffuseAlbedo = materialColor - materialColor * materialType;
    const float3 specularAlbedo = lerp(0.03f, materialColor, materialType);

    const float3 H = normalize(normalize(-viewVec) - lightDirView);
    const float NdotH = safe_saturate_dot(normal, H);
    const float NdotV = max(dot(normal, viewVec), 0.0001f);

    // Fresnel term
    const float3 fresnel = FresnelTerm(specularAlbedo, H, lightDirView);

    float distrib, geom;
    switch (BRDFModel)
    {
    case BRDF::CookTorranceGGX:
    {
        const float alphaD = AlphaGGX_D(roughness);
        const float alphaG = AlphaGGX_G(roughness);
        distrib = GGXDistributionTerm(NdotH, alphaD); // Distribution term
        geom = GGXGeometricTerm(alphaG, NdotL) * GGXGeometricTerm(alphaG, NdotV); // Geometric term
        break;
    }

    case BRDF::CookTorranceBeckmann:
    {
        const float alpha = AlphaBeckmann(roughness);
        distrib = BeckmannDistributionTerm(NdotH, alpha); // Distribution term
        geom = BeckmannGeometricTerm(alpha, NdotL) * BeckmannGeometricTerm(alpha, NdotV); // Geometric term
        break;
    }

    default:
        distrib = geom = 0;
        break;
    }

    // Color components
    const float3 diffuseColor = diffuseAlbedo / (PI * (1.f - fresnel));
    const float3 specularColor = distrib * geom * fresnel / (4.f * NdotL * NdotV);

    // Handled in ScreenSpaceReflection.hlsl as well, so be careful not to have them both active at the same time
    // (i.e. set fReflectionFactor to 0 for this shader when SSR is active and restore it when rendering with SSR shader)
    const float3 envAlbedo = texCUBElod(BRDF_EnvMap, float4(mul((float3x3)BRDFParams.InvViewMat, reflect(viewVec, normal)), ComputeMipFromRoughness(roughness, ENVIRONMENT_MAP_MIP_COUNT))).rgb;
    const float3 envFresnel = FresnelRoughnessTerm(specularAlbedo, roughness, normal, viewVec);

    const float3 irradiance = texCUBE(BRDF_IrradianceMap, mul((float3x3)BRDFParams.InvViewMat, normal)).rgb;

    // Final color
    return NdotL * (diffuseColor * (1.f - specularColor) + specularColor) * BRDFParams.DiffuseFactor * percentLit +
        envFresnel * envAlbedo * BRDFParams.ReflectionFactor + diffuseAlbedo * irradiance * BRDFParams.IrradianceFactor;
}

//////////////////////////////////////////////////////////
// Ashikhmin-Shirley BRDF model                         //
// http://www.cs.utah.edu/~michael/brdfs/jgtbrdf.pdf    //
//////////////////////////////////////////////////////////
float3 AshikhminShirley(const float3 materialColor, const float materialType, const float roughness, const float3 normal, const float3 viewVec, const float percentLit)
{
    const float3 lightDirView = normalize(mul((float3x3)BRDFParams.ViewMat, BRDFParams.LightDir));
    const float NdotL = safe_saturate_dot(normal, -lightDirView);

    const float3 diffuseAlbedo = materialColor - materialColor * materialType;
    const float3 specularAlbedo = lerp(0.03f, materialColor, materialType);

    const float3 H = normalize(normalize(-viewVec) - lightDirView);
    const float NdotH = safe_saturate_dot(normal, H);
    const float NdotV = max(dot(normal, -viewVec), 0.0001f);

    const float3 epsilon = float3(1.f, 0.f, 0.f);
    const float3 tangent = normalize(cross(normal, epsilon));
    const float3 bitangent = normalize(cross(normal, tangent));

    const float LdotH = dot(-lightDirView, H);
    const float TdotH = dot(tangent, H);
    const float BdotH = dot(bitangent, H);

    // The specular exponents can theoretically go up to infinity (thus the very high valued scale factor),
    // representing a perfectly smooth surface, which is not physically based.
    // This remapping of the roughness term allows for more perceptual consistency between Cook-Torrance and Ashikhmin-Shirley BRDFs.
    const float scale = 10000.f;
    const float Nu = pow(1.f - roughness * 0.7f, 6.f) * scale;
    const float Nv = Nu; // There is support for anisotropic materials, however, we don't make use of it at the moment

    // Diffuse term
    float Pd = 28.f / (23.f * PI) *
                (1.f - pow(1.f - NdotL * 0.5f, 5.f)) *
                (1.f - pow(1.f - NdotV * 0.5f, 5.f));

    // Fresnel term
    const float3 fresnel = FresnelTerm(specularAlbedo, H, lightDirView);

    // Specular term
    const float PsNum = sqrt((Nu + 1.f) * (Nv + 1.f)) * pow(NdotH, Nu * TdotH * TdotH + Nv * BdotH * BdotH);
    const float PsDen = 8.f * PI * LdotH * max(NdotL, NdotV);
    const float Ps = PsNum / PsDen;

    // Ambient term
    const float3 irradiance = texCUBE(BRDF_IrradianceMap, mul((float3x3)BRDFParams.InvViewMat, normal)).rgb;

    // Final value
    return (diffuseAlbedo * Pd * (1.f - specularAlbedo) + specularAlbedo * Ps * fresnel) * BRDFParams.DiffuseFactor * percentLit + diffuseAlbedo * irradiance * BRDFParams.IrradianceFactor;
}

//////////////////////////////////////////////////////////////
// Ward BRDF model                                          //
// https://cseweb.ucsd.edu/~ravir/6998/papers/p265-ward.pdf //
//////////////////////////////////////////////////////////////
float3 Ward(const float3 materialColor, const float materialType, const float roughness, const float3 normal, const float3 viewVec, const float percentLit)
{
    const float3 lightDirView = normalize(mul((float3x3)BRDFParams.ViewMat, BRDFParams.LightDir));
    const float NdotL = safe_saturate_dot(normal, -lightDirView);

    const float3 diffuseAlbedo = materialColor - materialColor * materialType;
    const float3 specularAlbedo = lerp(0.03f, materialColor, materialType);

    const float3 H = normalize(normalize(-viewVec) - lightDirView);
    const float NdotH = safe_saturate_dot(normal, H);
    const float NdotV = max(dot(normal, -viewVec), 0.0001f);

    const float alpha = pow(roughness * 0.7f + 0.3f, 6.f) + 1e-5;

    // Specular term
    const float PsNum = exp(-pow(tan(acos(NdotH)), 2) / alpha);
    const float PsDen = 4.f * PI * alpha * sqrt(NdotL * NdotV);
    const float Ps = PsNum / PsDen;

    // Ambient term
    const float3 irradiance = texCUBE(BRDF_IrradianceMap, mul((float3x3)BRDFParams.InvViewMat, normal)).rgb;

    // Final value
    return NdotL * (diffuseAlbedo + specularAlbedo * Ps) * BRDFParams.DiffuseFactor * percentLit + diffuseAlbedo * irradiance * BRDFParams.IrradianceFactor;
}

////////////////////////////////////////////////////////////////////
#endif // HLSL

#endif // BRDFUTILS_HLSLI_
