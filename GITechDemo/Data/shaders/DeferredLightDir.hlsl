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

const sampler2D	texShadowMap;			// Cascaded shadow maps
const float		fShadowDepthBias;		// Depth bias for shadowing
const float2	f2OneOverShadowMapSize;	// 1 / shadow map width/height

const float	fDiffuseFactor;	// Scale value for diffuse light
const float	fSpecFactor;	// Scale value for specular light

const float4x4	f44ViewMat;	// View matrix

const float3	f3LightDir;					// The direction of the light
const float4x4	f44ScreenToLightViewMat;	// Composite matrix for transforming screen-space coordinates to light-view space

#define	MAX_NUM_CASCADES (4)	// Maximum number of supported cascades (sync with nCascadeCount for optimal memory usage)
const bool		bDebugCascades;	// Visual cascade debug option
const float2	f2CascadeBoundsMin[MAX_NUM_CASCADES];	// Light-view space AABBs corresponding
const float2	f2CascadeBoundsMax[MAX_NUM_CASCADES];	// To each shadow cascade
const float4x4	f44CascadeProjMat[MAX_NUM_CASCADES];	// Light space projection matrix
const float		fCascadeBlendSize;	// Size of the blend band for blurring between cascade boundaries

// For performance reasons, we set these variables
// directly in the shader, instead of from the
// application (so that they are known at compile time)
// NB: Also, we don't want to waste ALU calculating them
static const int	nCascadeCount = 4;			// Number of cascades
static const int	nCascadesPerRow = 2;		// Number of cascades per row, i.e. ceil(sqrt(nCascadeCount))
static const float	fCascadeNormSize = 0.5f;	// Normalized size of a cascade, i.e. 1.f / nCascadesPerRow

// PCF method
#define PCF_SAMPLE	PCF4x4PoissonRotatedx4

struct PSOut
{
	float4 colorOut	:	SV_TARGET;
};

void psmain(VSOut input, out PSOut output)
{
	// Sample the diffuse buffer
	const float4 f4DiffuseColor = tex2D(texDiffuseBuffer, input.f2TexCoord);

	// Sample the normal buffer
	const float3 f3Normal = DecodeNormal(tex2D(texNormalBuffer, input.f2TexCoord));

	// Sample the depth buffer
	const float fDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;

	// Early depth check, so that we don't shade
	// the far plane (where the sky will be drawn)
	if (fDepth == 1.f)
		clip(-1);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Blinn-Phong directional lighting																			//
	// http://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model#High-Level_Shading_Language_code_sample	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const float3 f3LightDirView = normalize(mul((float3x3)f44ViewMat, f3LightDir));
	const float fNdotL = dot(f3Normal, -f3LightDirView);
	const float fDiffIntensity = saturate(fNdotL);
	const float3 f3Diffuse = smoothstep(0.f, 1.f, fDiffIntensity) * f4DiffuseColor.rgb * fDiffuseFactor;
	const float3 f3H = normalize(normalize( -input.f3ViewVec) - f3LightDirView);
	const float fNdotH = dot(f3Normal, f3H);
	const float fSpecIntensity = pow(saturate(fNdotH), f4DiffuseColor.a * 255.f);
	const float3 f3Specular = smoothstep(0.f, 1.f, fDiffIntensity) * fSpecIntensity * f4DiffuseColor.rgb * fSpecFactor;

	//////////////////////////////////////////////////////////////////////////////////////////
	// Cascaded Shadow Maps																	//
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ee416307%28v=vs.85%29.aspx	//
	//////////////////////////////////////////////////////////////////////////////////////////
	// Step 1: Calculate light-view space position of current pixel
	float4 f4LightViewPos = mul(f44ScreenToLightViewMat, float4(input.f2ScreenPos, fDepth, 1.f));
	f4LightViewPos /= f4LightViewPos.w;

	// Step 2: Find the best valid cascade
	int nValidCascade = 0;
	for(int cascade = nCascadeCount - 1; cascade >= 0; cascade--)
	{
		// Iterate through all AABBs and check if the point
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

	// Step 3: Sample from the cascade we found above
	float3 f3CascadeTexCoord = mul(f44CascadeProjMat[nValidCascade], f4LightViewPos).xyz;
	// After bringing our point in normalized light-view space, calculate
	// proper texture coordinates for shadow map sampling
	f3CascadeTexCoord.xy =
		(f3CascadeTexCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f)) *
		fCascadeNormSize +
		float2(fCascadeNormSize * fmod(nValidCascade, nCascadesPerRow), fCascadeNormSize * floor(nValidCascade / nCascadesPerRow));

	// PCF Poisson disc shadow sampling
	float fPercentLit = PCF_SAMPLE(texShadowMap, f2OneOverShadowMapSize, f3CascadeTexCoord.xy, f3CascadeTexCoord.z - fShadowDepthBias);
	//float fPercentLit = tex2D(texShadowMap, f3CascadeTexCoord.xy).r > f3CascadeTexCoord.z - fShadowDepthBias;

	// If required, blend between cascade seams
	if (fCascadeBlendSize != 0.f)
	{
		if (nValidCascade != nCascadeCount - 1)
		{
			// The blend amount depends on the position of the point inside the blend band of the current cascade
			const float fScaledBlendSize = fCascadeBlendSize * (nValidCascade * nValidCascade + 1);
			float fBlendAmount = 0.f;
			
			// Partition the cascade into 4 parts and only allow blending in the parts furthest from the camera
			// (i.e. the part that is closest to the camera is not adjacent to any valid cascade, because they
			// fit very tightly around the view frustum, so blending in that zone would result in artifacts)
			// Disclaimer: there probably exists an easier way to do this, but I'm just lazy
			const float4x2 Direction = {
				float2(0.707107f, 0.707107f),	//normalize(float2(0.5f, 0.5f));
				float2(0.707107f, -0.707107f),	//normalize(float2(0.5f, -0.5f));
				float2(-0.707107f, -0.707107f),	//normalize(float2(-0.5f, -0.5f));
				float2(-0.707107f, 0.707107f)	//normalize(float2(-0.5f, 0.5f));
			};

			const float2 f2CascadeSpaceViewDir = normalize(mul(f44CascadeProjMat[0], f4LightViewPos).xy);
			const float4 f4LightSpaceCameraDir = mul(f44CascadeProjMat[0], mul(f44ScreenToLightViewMat, float4(0.f, 0.f, 1.f, 1.f)));
			
			// Compact the individual 4 * 2 dot products
			// into 2 matrix - vector multiplications
			const float4 dotCascadeDir = mul(Direction, f2CascadeSpaceViewDir);
			const float4 dotCamDir = mul(Direction, f4LightSpaceCameraDir.xy);
			
			const bool dotNE = dotCascadeDir.x > 0.f;
			const bool dotSE = dotCascadeDir.y > 0.f;
			const bool dotSW = dotCascadeDir.z > 0.f;
			const bool dotNW = dotCascadeDir.w > 0.f;
			const bool dotCamDirNE = dotCamDir.x > 0.f;
			const bool dotCamDirSE = dotCamDir.y > 0.f;
			const bool dotCamDirSW = dotCamDir.z > 0.f;
			const bool dotCamDirNW = dotCamDir.w > 0.f;

			// These dynamic branches should be fine performance wise, since the code path would be coherent for large
			// portions of the screen. Of course, the GPU's pixel branching granularity has a large influence on the
			// overall cost of these branches, but current generation hardware should have <32-pixel granularity
			// NB: GeForce 8800 architecture had 32-pixel branching granularity
			// http://bps10.idav.ucdavis.edu/talks/03-fatahalian_gpuArchTeraflop_BPS_SIGGRAPH2010.pdf
			if (dotSW && dotNW && (dotCamDirSW || dotCamDirNW))
				fBlendAmount = max(fBlendAmount, (f2CascadeBoundsMin[nValidCascade].x + fScaledBlendSize) - f4LightViewPos.x);
			if (dotSW && dotSE && (dotCamDirSW || dotCamDirSE))
				fBlendAmount = max(fBlendAmount, (f2CascadeBoundsMin[nValidCascade].y + fScaledBlendSize) - f4LightViewPos.y);
			if (dotSE && dotNE && (dotCamDirSE || dotCamDirNE))
				fBlendAmount = max(fBlendAmount, f4LightViewPos.x - (f2CascadeBoundsMax[nValidCascade].x - fScaledBlendSize));
			if (dotNW && dotNE && (dotCamDirNW || dotCamDirNE))
				fBlendAmount = max(fBlendAmount, f4LightViewPos.y - (f2CascadeBoundsMax[nValidCascade].y - fScaledBlendSize));
			
			fBlendAmount /= fScaledBlendSize;

			// If our point is inside the blend band, we can continue with blending
			if (fBlendAmount > 0.f)
			{
				// Calculate texture coordinates for sampling from the cascade one order
				// higher than the cascade from which we sampled earlier
				float3 f3LQCascadeTexCoord = mul(f44CascadeProjMat[nValidCascade + 1], f4LightViewPos).xyz;
				f3LQCascadeTexCoord.xy =
					(f3LQCascadeTexCoord.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f)) *
					fCascadeNormSize +
					float2(fCascadeNormSize * fmod(nValidCascade + 1, nCascadesPerRow), fCascadeNormSize * floor((nValidCascade + 1) / nCascadesPerRow));

				// Sample from the lower quality cascade and blend between samples appropriately
				const float fPercentLitLQ = PCF_SAMPLE(texShadowMap, f2OneOverShadowMapSize, f3LQCascadeTexCoord.xy, f3LQCascadeTexCoord.z - fShadowDepthBias);
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