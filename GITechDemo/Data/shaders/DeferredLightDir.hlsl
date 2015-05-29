#include "Utils.hlsl"

// Vertex shader /////////////////////////////////////////////////
const float4x4 f44InvProjMat;
const float2 f2HalfTexelOffset;

struct VSOut
{
	float4 f4Position	:	SV_POSITION;
	float2 f2TexCoord	:	TEXCOORD0;
	float3 f3ViewVec	:	TEXCOORD1;
};

void vsmain(float4 f4Position : POSITION, out VSOut output)
{
	output.f4Position	=	f4Position;
	output.f2TexCoord	=	f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
	output.f3ViewVec	=	mul(f44InvProjMat, float4(f4Position.xy, 1.f, 1.f)).xyz;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D	texDiffuseBuffer; // diffuse color
const sampler2D	texNormalBuffer; // view-space normals
const sampler2D	texDepthBuffer; // depth values

const sampler2D	texShadowMap; // cascaded shadow maps
const float		fShadowDepthBias; // depth bias for shadowing
const float2	f2OneOverShadowMapSize; // 1 / shadow map width/height

const float	fDiffuseFactor; // scale value for diffuse light
const float	fSpecFactor; // scale value for specular light

const float4x4	f44ViewMat; // view matrix

const float3	f3LightDir; // the direction of the light
const float4x4	f44ScreenToLightViewMat; // composite matrix for transforming screen-space coordinates to light-view space

#define	MAX_NUM_CASCADES (4) // maximum number of supported cascades (sync with nCascadeCount for optimal memory usage)
const bool		bDebugCascades; // visual cascade debug option
const float2	f2CascadeBoundsMin[MAX_NUM_CASCADES]; // light-view space AABBs corresponding
const float2	f2CascadeBoundsMax[MAX_NUM_CASCADES]; // to each shadow cascade
const float4x4	f44CascadeProjMat[MAX_NUM_CASCADES]; // light space projection matrix
const float		fCascadeBlendSize; // size of the blend band for blurring between cascade boundaries

// for performance reasons, we set these variables
// directly in the shader, instead of from the
// application (so that they are known at compile time)
// NB: also, we don't want to waste ALU calculating them
static const int	nCascadeCount = 4; // number of cascades
static const int	nCascadesPerRow = 2; // number of cascades per row, i.e. ceil(sqrt(nCascadeCount))
static const float	fCascadeNormSize = 0.5f; // normalized size of a cascade, i.e. 1.f / nCascadesPerRow

// PCF method
#define PCF_SAMPLE	PCF4x4PoissonRotatedx4

struct PSOut
{
	float4 colorOut	:	SV_TARGET;
};

void psmain(VSOut input, out PSOut output)
{
	// sample the diffuse buffer
	float4 f4DiffuseColor = tex2D(texDiffuseBuffer, input.f2TexCoord);

	// sample the normal buffer
	float3 f3Normal = DecodeNormal(tex2D(texNormalBuffer, input.f2TexCoord));

	// sample the depth buffer
	float fDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;

	// early depth check, so that we don't shade
	// the far plane (where the sky will be drawn)
	if (fDepth == 1.f)
		clip(-1);
	
	// Blinn-Phong directional lighting
	float3 f3LightDirView = normalize(mul((float3x3)f44ViewMat, f3LightDir));
	float fNdotL = dot(f3Normal, -f3LightDirView);
	float fDiffIntensity = saturate(fNdotL) * fDiffuseFactor;
	float3 f3Diffuse = smoothstep(0.f, 1.f, fDiffIntensity) * f4DiffuseColor.rgb;
	float3 f3H = normalize(normalize( -input.f3ViewVec) - f3LightDirView);
	float fNdotH = dot(f3Normal, f3H);
	float fSpecIntensity = f4DiffuseColor.a * pow(saturate(fNdotH), fSpecFactor);
	float3 f3Specular = smoothstep(0.f, 1.f, fDiffIntensity) * fSpecIntensity * f4DiffuseColor.rgb;

	// CSM
	// step 1: calculate light-view space position of current pixel
	float4 f4LightViewPos = mul(f44ScreenToLightViewMat, float4(input.f2TexCoord * float2(2.f, -2.f) - float2(1.f, -1.f), fDepth, 1.f));
	f4LightViewPos /= f4LightViewPos.w;

	// step 2: find the best valid cascade
	bool bValidCascade[MAX_NUM_CASCADES];
	bool bValidCascadeTexCoord = false;
	int nValidCascade = 0;
	for(int cascade = nCascadeCount - 1; cascade >= 0; cascade--)
	{
		// iterate through all AABBs and check if the point
		// is inside of one of them
		if (f4LightViewPos.x >= f2CascadeBoundsMin[cascade].x &&
			f4LightViewPos.y >= f2CascadeBoundsMin[cascade].y &&
			f4LightViewPos.x <= f2CascadeBoundsMax[cascade].x &&
			f4LightViewPos.y <= f2CascadeBoundsMax[cascade].y)
		{
			nValidCascade = cascade;
			//break;
		}
	}

	// step 3: sample from the cascade we found above
	float3 f3CascadeTexCoord = mul(f44CascadeProjMat[nValidCascade], f4LightViewPos).xyz;
	// after bringing our point in normalized light-view space, calculate
	// proper texture coordinates for shadow map sampling
	f3CascadeTexCoord.xy =
		(f3CascadeTexCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f)) *
		fCascadeNormSize +
		float2(fCascadeNormSize * fmod(nValidCascade, nCascadesPerRow), fCascadeNormSize * floor(nValidCascade / nCascadesPerRow));

	// PCF Poisson disc shadow sampling
	float fPercentLit = PCF_SAMPLE(texShadowMap, f2OneOverShadowMapSize, f3CascadeTexCoord.xy, f3CascadeTexCoord.z - fShadowDepthBias);
	//float fPercentLit = tex2D(texShadowMap, f3CascadeTexCoord.xy).r > f3CascadeTexCoord.z - fShadowDepthBias;

	// if required, blend between cascade seams
	if (fCascadeBlendSize != 0.f)
	{
		if (nValidCascade != nCascadeCount - 1)
		{
			// the blend amount depends on the position of the point inside the blend band of the current cascade
			float fScaledBlendSize = fCascadeBlendSize * (nValidCascade * nValidCascade + 1);
			float fBlendAmount = 0.f;
			
			// partition the cascade into 4 parts and only allow blending in the parts furthest from the camera
			// (i.e. the part that is closest to the camera is not adjacent to any valid cascade, because they
			// fit very tightly around the view frustum, so blending in that zone would result in artifacts)
			// Disclaimer: there probably exists an easier way to do this, but I'm just lazy
			float2 NE = float2(0.707107f, 0.707107f); //normalize(float2(0.5f, 0.5f));
			float2 SE = float2(0.707107f, -0.707107f); //normalize(float2(0.5f, -0.5f));
			float2 SW = float2(-0.707107f, -0.707107f); //normalize(float2(-0.5f, -0.5f));
			float2 NW = float2(-0.707107f, 0.707107f); //normalize(float2(-0.5f, 0.5f));

			float2 f2CascadeSpaceViewDir = normalize(mul(f44CascadeProjMat[0], f4LightViewPos).xy);
			float4 f4LightSpaceCameraDir = mul(f44CascadeProjMat[0], mul(f44ScreenToLightViewMat, float4(0.f, 0.f, 1.f, 1.f)));
			bool dotNE = dot(f2CascadeSpaceViewDir, NE) > 0.f;
			bool dotSE = dot(f2CascadeSpaceViewDir, SE) > 0.f;
			bool dotSW = dot(f2CascadeSpaceViewDir, SW) > 0.f;
			bool dotNW = dot(f2CascadeSpaceViewDir, NW) > 0.f;
			bool dotCamDirNE = dot(f4LightSpaceCameraDir.xy, NE) > 0.f;
			bool dotCamDirSE = dot(f4LightSpaceCameraDir.xy, SE) > 0.f;
			bool dotCamDirSW = dot(f4LightSpaceCameraDir.xy, SW) > 0.f;
			bool dotCamDirNW = dot(f4LightSpaceCameraDir.xy, NW) > 0.f;

			if (dotSW && dotNW && (dotCamDirSW || dotCamDirNW))
				fBlendAmount = max(fBlendAmount, (f2CascadeBoundsMin[nValidCascade].x + fScaledBlendSize) - f4LightViewPos.x);
			if (dotSW && dotSE && (dotCamDirSW || dotCamDirSE))
				fBlendAmount = max(fBlendAmount, (f2CascadeBoundsMin[nValidCascade].y + fScaledBlendSize) - f4LightViewPos.y);
			if (dotSE && dotNE && (dotCamDirSE || dotCamDirNE))
				fBlendAmount = max(fBlendAmount, f4LightViewPos.x - (f2CascadeBoundsMax[nValidCascade].x - fScaledBlendSize));
			if (dotNW && dotNE && (dotCamDirNW || dotCamDirNE))
				fBlendAmount = max(fBlendAmount, f4LightViewPos.y - (f2CascadeBoundsMax[nValidCascade].y - fScaledBlendSize));
			
			fBlendAmount /= fScaledBlendSize;

			// if our point is inside the blend band, we can continue with blending
			if (fBlendAmount > 0.f)
			{
				// calculate texture coordinates for sampling from the cascade one order
				// higher than the cascade from which we sampled earlier
				float3 f3LQCascadeTexCoord = mul(f44CascadeProjMat[nValidCascade + 1], f4LightViewPos).xyz;
				f3LQCascadeTexCoord.xy =
					(f3LQCascadeTexCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f)) *
					fCascadeNormSize +
					float2(fCascadeNormSize * fmod(nValidCascade + 1, nCascadesPerRow), fCascadeNormSize * floor((nValidCascade + 1) / nCascadesPerRow));

				// sample from the lower quality cascade and blend between samples appropriately
				float fPercentLitLQ = PCF_SAMPLE(texShadowMap, f2OneOverShadowMapSize, f3LQCascadeTexCoord.xy, f3LQCascadeTexCoord.z - fShadowDepthBias);
				//float fPercentLitLQ = tex2D(texShadowMap, f3LQCascadeTexCoord.xy).r > f3LQCascadeTexCoord.z - fShadowDepthBias;
				if (fPercentLitLQ > 0.f && fPercentLitLQ < 1.f) // only blend at shadow edges
					fPercentLit = lerp(fPercentLit, fPercentLitLQ, fBlendAmount);
			}
		}
	}

	// Final color
	output.colorOut = float4((f3Diffuse + f3Specular) * fPercentLit, 1.f);

	// CSM debug
	if (bDebugCascades)
		output.colorOut += float4(nValidCascade % 3 == 0, nValidCascade % 3 == 1, nValidCascade % 3 == 2, 0.f) * 0.5f;
}
////////////////////////////////////////////////////////////////////