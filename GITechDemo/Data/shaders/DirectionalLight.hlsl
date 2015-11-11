#include "PostProcessingUtils.hlsl"
#include "Utils.hlsl"

// Vertex shader /////////////////////////////////////////////////
const float4x4 f44InvProjMat;
const float2 f2HalfTexelOffset;

struct VSOut
{
	float4 f4Position	:	SV_POSITION;
	float2 f2TexCoord	:	TEXCOORD0;
	float3 f3ViewVec	:	TEXCOORD1;
	float2 f2ScreenPos	:	TEXCOORD2;
};

void vsmain(float4 f4Position : POSITION, out VSOut output)
{
	output.f4Position	=	f4Position;
	output.f2TexCoord	=	f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
	output.f3ViewVec	=	mul(f44InvProjMat, float4(f4Position.xy, 1.f, 1.f)).xyz;
	output.f2ScreenPos	=	f4Position.xy;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D	texDiffuseBuffer;	// Diffuse color
const sampler2D	texNormalBuffer;	// View-space normals
const sampler2D	texDepthBuffer;		// Depth values
const sampler2D	texMaterialBuffer;	// Roughness and material type (metallic/dielectric)

#define ENVIRONMENT_MAP_MIP_COUNT	(9)
const samplerCUBE texIrradianceMap;	// Irradiance map for Cook-Torrance BRDF
const samplerCUBE texEnvMap;		// Environment map for Cook-Torrance BRDF

const sampler2D	texShadowMap;			// Cascaded shadow maps
const float2	f2OneOverShadowMapSize;	// 1 / shadow map width/height

const float	fDiffuseFactor;		// Scale value for diffuse light
const float	fSpecFactor;		// Scale value for specular light
const float	fAmbientFactor;		// Scale value for ambient light
const float fIrradianceFactor;	// Scale value for irradiance (Cook-Torrance BRDF only)
const float fReflectionFactor;	// Scale value for reflected light (Cook-Torrance BRDF only)

const float4x4	f44ViewMat;		// View matrix
const float4x4 f44InvViewMat;	// The inverse view matrix

const float3	f3LightDir;					// The direction of the light
const float4x4	f44ScreenToLightViewMat;	// Composite matrix for transforming screen-space coordinates to light-view space

#define	NUM_CASCADES (4)		// Number of supported cascades
const bool		bDebugCascades;	// Visual cascade debug option
const float2	f2CascadeBoundsMin[NUM_CASCADES];	// Light-view space AABBs corresponding
const float2	f2CascadeBoundsMax[NUM_CASCADES];	// to each shadow cascade
const float4x4	f44CascadeProjMat[NUM_CASCADES];	// light space projection matrix
const float		fCascadeBlendSize;	// Size of the blend band for blurring between cascade boundaries

// For performance reasons, we set these variables
// directly in the shader, instead of from the
// application (so that they are known at compile time)
// NB: Also, we don't want to waste ALU calculating them
static const unsigned int nCascadeCount = NUM_CASCADES;					// Number of cascades
static const unsigned int nCascadesPerRow = ceil(sqrt(nCascadeCount));	// Number of cascades per row
static const float fCascadeNormSize = rcp(nCascadesPerRow);				// Normalized size of a cascade, i.e. 1.f / nCascadesPerRow

// Conditional PCF shadow sampling for different sampling methods for each cascade
// NB: PCF_SAMPLE0 corresponds to most detailed cascade (highest resolution),
// whereas PCF_SAMPLE3 corresponds to the least detailed cascade (lowest resolution)
#define USE_CONDITIONAL_PCF (0)
#if USE_CONDITIONAL_PCF
	#define PCF_SAMPLE0	PCF4x4Poisson
	#define PCF_SAMPLE1	PCF12TapPoisson
	#define PCF_SAMPLE2	PCF3x3Poisson
	#define PCF_SAMPLE3	PCF2x2Poisson
#else
	#define PCF_SAMPLE	PCF4x4PoissonRotatedx4
#endif

// BRDF model
#define BLINN_PHONG (0)
#define COOK_TORRANCE_GGX (1)
#define COOK_TORRANCE_BECKMANN (2)
const unsigned int nBRDFModel;

struct PSOut
{
	float4 colorOut	:	SV_TARGET;
};

float3 BlinnPhong(const float3 f3DiffuseColor, const float fSpecularPower, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit);
float3 CookTorranceGGX(const float3 f3MaterialColor, const float fMaterialType, const float fRoughness, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit);
float3 CookTorranceBeckmann(const float3 f3MaterialColor, const float fMaterialType, const float fRoughness, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit);

void psmain(VSOut input, out PSOut output)
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
	// Cascaded Shadow Maps																	//
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ee416307%28v=vs.85%29.aspx	//
	//////////////////////////////////////////////////////////////////////////////////////////
	// Step 1: Calculate light-view space position of current pixel
	float4 f4LightViewPos = mul(f44ScreenToLightViewMat, float4(input.f2ScreenPos, fDepth, 1.f));
	f4LightViewPos *= rcp(f4LightViewPos.w);

	// Step 2: Find the best valid cascade
	unsigned int nValidCascade = 0;
	UNROLL for(int nCascade = nCascadeCount - 1; nCascade >= 0; nCascade--)
	{
		// If f4LightViewPos.xy > f2CascadeBoundsMin[nCascade] then step() returns 1, else 0 (per component evaluation).
		// After evaluating the second step(), we perform a per component scalar multiply which will result in either
		// 0 or 1 on either components. If both components are 1, then all() will return true, meaning that this cascade is valid.
		// On a higher level, the first step() determines whether the current's pixel light-view space position is above and
		// to the right of the lower left corner of the cascade, whereas the second step() determines whether it is below and
		// to the left of the upper right corner of the cascade. If all the conditions are met, then the cascade is valid.
		const bool bValid =
			all(
				step(
					f2CascadeBoundsMin[nCascade],
					f4LightViewPos.xy
				) *
				step(
					f4LightViewPos.xy,
					f2CascadeBoundsMax[nCascade]
				)
			);
		nValidCascade = lerp(nValidCascade, nCascade, bValid);
	}

	// Step 3: Calculate texture coordinates, then sample from the cascade we found above
	float3 f3CascadeTexCoord = mul(f44CascadeProjMat[nValidCascade], f4LightViewPos).xyz;
	float2 origsign = sign(f3CascadeTexCoord.xy);
	// After bringing our point in normalized light-view space, calculate
	// proper texture coordinates for shadow map sampling
	f3CascadeTexCoord.xy =
		(f3CascadeTexCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f)) *
		fCascadeNormSize +
		float2(fCascadeNormSize * fmod(nValidCascade, nCascadesPerRow), fCascadeNormSize * floor(nValidCascade * rcp(nCascadesPerRow)));

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
			// True		- the edge on the X axis (the vertical one)
			// False	- the edge on the Y axis (the horizontal one)
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
					) * rcp(fScaledBlendSize);

			// If our point is inside the blend band, we can continue with blending
			if (fBlendAmount > 0.f)
			{
				// Calculate texture coordinates for sampling from the cascade one order
				// higher than the cascade from which we sampled earlier
				f3LQCascadeTexCoord.xy =
					(f3LQCascadeTexCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f)) *
					fCascadeNormSize +
					float2(fCascadeNormSize * fmod(nValidCascade + 1, nCascadesPerRow), fCascadeNormSize * floor((nValidCascade + 1) * rcp(nCascadesPerRow)));

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
		output.colorOut = float4(BlinnPhong(f4DiffuseColor.rgb, f4DiffuseColor.a, f3Normal, input.f3ViewVec, fPercentLit), 1.f);
		break;
	case COOK_TORRANCE_GGX:
		output.colorOut = float4(CookTorranceGGX(f4DiffuseColor.rgb, f2Material.r, f2Material.g, f3Normal, input.f3ViewVec, fPercentLit), 1.f);
		break;
	case COOK_TORRANCE_BECKMANN:
		output.colorOut = float4(CookTorranceBeckmann(f4DiffuseColor.rgb, f2Material.r, f2Material.g, f3Normal, input.f3ViewVec, fPercentLit), 1.f);
		break;
		/*
	default:
		output.colorOut = f4DiffuseColor * fPercentLit + f4DiffuseColor * fAmbientFactor;
		break;
		*/
	}
	
	// CSM debug
	if (bDebugCascades)
	{
		const float fBrightness = dot(output.colorOut.rgb, LUMINANCE_VECTOR);
		output.colorOut += float4(nValidCascade % 3 == 0, nValidCascade % 3 == 1, nValidCascade % 3 == 2, 0.f) * fBrightness * 0.5f;
		if (fBlendAmount > 0.f)
		{
			output.colorOut = float4(nValidCascade % 3 == 0, nValidCascade % 3 == 1, nValidCascade % 3 == 2, 0.f);
			output.colorOut += float4((nValidCascade + 1) % 3 == 0, (nValidCascade + 1) % 3 == 1, (nValidCascade + 1) % 3 == 2, 0.f);
			output.colorOut *= fBrightness * fBlendAmount;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Blinn-Phong BRDF model																					//
// http://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model#High-Level_Shading_Language_code_sample	//
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

//////////////////////////////////////////////////////////////////////////////////
// Cook-Torrance BRDF model														//
// https://en.wikipedia.org/wiki/Specular_highlight#Cook.E2.80.93Torrance_model	//
//////////////////////////////////////////////////////////////////////////////////

// Fresnel term
float3 FresnelRoughnessTerm(float3 f3SpecularAlbedo, float fRoughness, float3 f3HalfVec, float3 f3LightDir)
{
	// Sclick's approximation using roughness to attenuate fresnel.
	const float fLdotH = saturate(dot(f3LightDir, f3HalfVec));
	return f3SpecularAlbedo + (max(1.f - fRoughness, f3SpecularAlbedo) - f3SpecularAlbedo) * pow(1.f - fLdotH, 5.f);
}

float3 FresnelTerm(const float3 f3SpecularAlbedo, const float3 f3HalfVec, const float3 f3LightDir)
{
	return FresnelRoughnessTerm(f3SpecularAlbedo, 0.f, f3HalfVec, f3LightDir);
}

// GGX visibility term
float GGXVisibilityTerm(const float fRoughness2, const float fNdotX)
{
	return rcp((fNdotX + sqrt(fRoughness2 + (1.f - fRoughness2) * fNdotX * fNdotX)));
}

// Cook-Torrance BRDF using a GGX microfacet distribution
float3 CookTorranceGGX(const float3 f3MaterialColor, const float fMaterialType, const float fRoughness, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit)
{
	const float3 f3LightDirView = normalize(mul((float3x3)f44ViewMat, f3LightDir));
	const float fNdotL = saturate(dot(f3Normal, -f3LightDirView));

	const float3 f3DiffuseAlbedo = lerp(f3MaterialColor, 0.f, fMaterialType);
	const float3 f3SpecularAlbedo = lerp(0.03f, f3MaterialColor, fMaterialType);

	const float3 f3H = normalize(normalize(-f3ViewVec) - f3LightDirView);
	const float fNdotH = saturate(dot(f3Normal, f3H));
	const float fNdotV = max(dot(f3Normal, f3ViewVec), 0.0001f);
	const float fNdotH2 = fNdotH * fNdotH;
	const float fRoughness2 = fRoughness * fRoughness;

	// Distribution term
	const float fP = fNdotH * fNdotH * (fRoughness2 - 1.f) + 1.f;
	const float fDistrib = fRoughness2 * rcp(PI * fP * fP);

	// Calculate the matching visibility term
	const float fV1i = GGXVisibilityTerm(fRoughness2, fNdotL);
	const float fV1o = GGXVisibilityTerm(fRoughness2, fNdotV);
	const float fVis = fV1i * fV1o;

	// Fresnel term
	const float3 f3Fresnel = FresnelTerm(f3SpecularAlbedo, f3H, -f3LightDirView);

	// Color components
	const float3 f3DiffuseColor = f3DiffuseAlbedo * rcp(PI * (1.f - f3Fresnel));
	const float3 f3SpecularColor = fDistrib * f3Fresnel * fVis;

	const float3 f3EnvAlbedo = texCUBEbias(texEnvMap, float4(mul((float3x3)f44InvViewMat, reflect(f3ViewVec, f3Normal)), fRoughness2 * ENVIRONMENT_MAP_MIP_COUNT)).rgb;
	const float3 f3EnvFresnel = FresnelRoughnessTerm(f3SpecularAlbedo, fRoughness2, f3Normal, -f3ViewVec);

	const float3 f3Irradiance = texCUBE(texIrradianceMap, mul((float3x3)f44InvViewMat, f3Normal)).rgb;

	// Final color
	return fNdotL * (f3DiffuseColor * (1.f - f3SpecularColor) + f3SpecularColor) * fDiffuseFactor * fPercentLit +
		f3EnvFresnel * f3EnvAlbedo * fReflectionFactor + f3DiffuseAlbedo * f3Irradiance * fIrradianceFactor;
}

// Beckmann geometry term
float BeckmannGeometricTerm(const float fRoughness, const float fNdotX)
{
	float fNdotX2 = fNdotX * fNdotX;
	float fTanTheta = sqrt((1.f - fNdotX2) * rcp(fNdotX2));
	float fAlpha = rcp((fRoughness * fTanTheta));
	float fAlpha2 = fAlpha * fAlpha;
	float fGeom = 1.f;
	if (fAlpha < 1.6f)
		fGeom *= (3.535f * fAlpha + 2.181f * fAlpha2) * rcp(1.f + 2.276f * fAlpha + 2.577f * fAlpha2);
	return fGeom;
}

// Cook-Torrance BRDF using a Beckmann microfacet distribution
float3 CookTorranceBeckmann(const float3 f3MaterialColor, const float fMaterialType, const float fRoughness, const float3 f3Normal, const float3 f3ViewVec, const float fPercentLit)
{
	const float3 f3LightDirView = normalize(mul((float3x3)f44ViewMat, f3LightDir));
	const float fNdotL = saturate(dot(f3Normal, -f3LightDirView));

	const float3 f3DiffuseAlbedo = f3MaterialColor - f3MaterialColor * fMaterialType;
	const float3 f3SpecularAlbedo = lerp(0.03f, f3MaterialColor, fMaterialType);

	const float3 f3H = normalize(normalize(-f3ViewVec) - f3LightDirView);
	const float fNdotH = saturate(dot(f3Normal, f3H));
	const float fNdotV = max(dot(f3Normal, f3ViewVec), 0.0001f);
	const float fNdotH2 = fNdotH * fNdotH;
	const float fNdotH4 = fNdotH2 * fNdotH2;
	const float fRoughness2 = fRoughness * fRoughness;

	// Distribution term
	const float fTanTheta2 = (1.f - fNdotH2) * rcp(fNdotH2);
	const float fExpTerm = exp(-fTanTheta2 * rcp(fRoughness2));
	const float fDistrib = fExpTerm * rcp(PI * fRoughness2 * fNdotH4);

	// Geometric term
	const float fG1i = BeckmannGeometricTerm(fRoughness, fNdotL);
	const float fG1o = BeckmannGeometricTerm(fRoughness, fNdotV);
	const float fGeom = fG1i * fG1o;

	// Fresnel term
	const float3 f3Fresnel = FresnelTerm(f3SpecularAlbedo, f3H, -f3LightDirView);
	
	// Color components
	const float3 f3DiffuseColor = f3DiffuseAlbedo * rcp(PI * (1.f - f3Fresnel));
	const float3 f3SpecularColor = fDistrib * fGeom * f3Fresnel * rcp(4.f * fNdotL * fNdotV);

	const float3 f3EnvAlbedo = texCUBEbias(texEnvMap, float4(mul((float3x3)f44InvViewMat, reflect(f3ViewVec, f3Normal)), fRoughness2 * ENVIRONMENT_MAP_MIP_COUNT)).rgb;
	const float3 f3EnvFresnel = FresnelRoughnessTerm(f3SpecularAlbedo, fRoughness2, f3Normal, -f3ViewVec);

	const float3 f3Irradiance = texCUBE(texIrradianceMap, mul((float3x3)f44InvViewMat, f3Normal)).rgb;

	// Final color
	return fNdotL * (f3DiffuseColor * (1.f - f3SpecularColor) + f3SpecularColor) * fDiffuseFactor * fPercentLit +
		f3EnvFresnel * f3EnvAlbedo * fReflectionFactor + f3DiffuseAlbedo * f3Irradiance * fIrradianceFactor;
}
////////////////////////////////////////////////////////////////////