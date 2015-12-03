#include "PostProcessingUtils.hlsl"
#include "Utils.hlsl"
#include "CSMUtils.hlsl"

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;
const float2 f2TexSize;	// Size, in texels, of destination texture

struct VSOut
{
	float4 f4Position	:	SV_POSITION;
	float2 f2TexCoord	:	TEXCOORD0;
	float2 f2ScreenPos	:	TEXCOORD2;
	float2 f2TexelIdx	:	TEXCOORD3;
};

void vsmain(float4 f4Position : POSITION, out VSOut output)
{
	output.f4Position	= f4Position;
	output.f2TexCoord	= f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
	output.f2ScreenPos	= f4Position.xy;
	output.f2TexelIdx	= f2TexSize * (f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f));
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////

////////////////////////
// Tunable parameters //
////////////////////////
#define			OFFSET_RAY_SAMPLES		// If defined, scatter multiple ray samples across grid of INTERLEAVED_GRID_SIZE x INTERLEAVED_GRID_SIZE pixels
#define			USE_BAYER_MATRIX		// If defined, sample grid offsets from texture, else use procedurally generated values
#define			FOG_DENSITY_MAP			// If defined, use 3D noise texture as a fog density map

const int		nSampleCount;			// Number of samples along ray
const float		fLightIntensity;		// Intensity of the effect when in direct light
const float		fMultScatterIntensity;	// Intensity of the effect when in indirect light
const float3	f3FogBox;				// Bounding box of fog texture in world space coordinates
const float3	f3FogSpeed;				// Fog speed modifier for each axis (world space units / sec)
const float		fFogVerticalFalloff;	// Vertical fallof factor for the volumetric fog effect
////////////////////////

// These must match dither map (Bayer matrix) texture dimensions
#define INTERLEAVED_GRID_SIZE			(8.f)
#define INTERLEAVED_GRID_SIZE_RCP		(0.125f)
#define INTERLEAVED_GRID_SIZE_SQR		(64.f)
#define INTERLEAVED_GRID_SIZE_SQR_RCP	(0.015625f)

// Radiative transport equation terms (http://sirkan.iit.bme.hu/~szirmay/lightshaft.pdf)
#define TAU		(0.0001f)	// Probability of collision
#define PHI		(100.f)		// Source power of the light

#ifdef USE_BAYER_MATRIX
// INTERLEAVED_GRID_SIZE x INTERLEAVED_GRID_SIZE texture containing sample offsets
const sampler2D	texDitherMap;
#endif

#ifdef FOG_DENSITY_MAP
// Noise texture (for fog)
const sampler3D	texNoise;
#endif

const sampler2D	texShadowMap;				// Cascaded shadow maps
const sampler2D	texDepthBuffer;				// Scene depth
const float4x4	f44ScreenToLightViewMat;	// Composite matrix for transforming screen-space coordinates to light-view space
const float4x4	f44InvLightViewMat;			// Light view space to world space matrix
const float3	f3CameraPositionLightVS;	// Light view space position of camera
const float		fRaymarchDistanceLimit;		// Fallback if we can't find a tighter limit
const float		fElapsedTime;				// Elapsed time

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Based on the work of Benjamin Glatzel <bglatzel@deck13.com>:																				//
	// http://bglatzel.movingblocks.net/wp-content/uploads/2014/05/Volumetric-Lighting-for-Many-Lights-in-Lords-of-the-Fallen-With-Notes.pdf	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	f4Color = float4(0.f, 0.f, 0.f, 1.f);
	
	// Intersect ray with scene
	const float fSceneDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;
	const float4 f4RayPositionLightVS = mul(f44ScreenToLightViewMat, float4(input.f2ScreenPos, fSceneDepth, 1.f));
	float3 f3RayPositionLightVS = f4RayPositionLightVS.xyz * rcp(f4RayPositionLightVS.w);

	// Reduce noisiness by truncating the starting position
	const float3 f3Ray = trunc(f3CameraPositionLightVS - f3RayPositionLightVS);
	const float3 f3RayDir = normalize(f3Ray);
	const float fRaymarchDistance = clamp(length(f3Ray), 0.f, fRaymarchDistanceLimit);

	// Calculate the size of each step
	const float fStepSize = fRaymarchDistance * rcp((float)nSampleCount);
	const float3 f3RayStep = fStepSize * f3RayDir;

	// Calculate the offsets on the ray according to the interleaved sampling pattern
#ifdef OFFSET_RAY_SAMPLES
#ifdef USE_BAYER_MATRIX
	const float fRayStartOffset = tex2D(texDitherMap, input.f2TexCoord * f2TexSize * INTERLEAVED_GRID_SIZE_RCP).r * fStepSize;
#else // USE_BAYER_MATRIX
	const float2 f2InterleavedPos = fmod(ceil(input.f2TexelIdx), INTERLEAVED_GRID_SIZE);
	const float fRayStartOffset = (f2InterleavedPos.y * INTERLEAVED_GRID_SIZE + f2InterleavedPos.x) * (fStepSize * INTERLEAVED_GRID_SIZE_SQR_RCP);
#endif // USE_BAYER_MATRIX
#else // OFFSET_RAY_SAMPLES
	const float fRayStartOffset = 0.f;
#endif // OFFSET_RAY_SAMPLES

	f3RayPositionLightVS += fRayStartOffset * f3RayDir;

	// Calculate ray position in fog texture space for fog effect.
	// NB: Avoid doing matrix multiplication inside 'for' loop. Rather, march along ray in world space in parallel.
	const float3 f3FogBoxRcp = rcp(f3FogBox);
	float3 f3RayPositionLightWS = mul(f44InvLightViewMat, float4(f3RayPositionLightVS, 1.f)).xyz * f3FogBoxRcp;
	const float3 f3RayStepWS = mul(f44InvLightViewMat, float4(f3RayStep, 1.f)).xyz * f3FogBoxRcp;
	const float3 f3FogSampleOffset = fElapsedTime * f3FogSpeed * rcp(f3FogBox);

	// Ray marching (could be optimized with a hardcoded sample count value and unrolling this 'for')
	LOOP for (
		float fRayDist = fRaymarchDistance;
		fRayDist > fStepSize;
		fRayDist -= fStepSize, f3RayPositionLightVS += f3RayStep, f3RayPositionLightWS += f3RayStepWS
		)
	{
		// Find the best valid cascade
		const unsigned int nValidCascade = GetCascadeIdx(f3RayPositionLightVS.xy);

		// Calculate texture coordinates, then sample from the cascade we found above
		const float3 f3CascadeTexCoord = GetCascadeSpacePos(f3RayPositionLightVS, nValidCascade);

		// Fetch whether the current position on the ray is visible from the light's perspective - or not
		const bool bLit = (tex2D(texShadowMap, f3CascadeTexCoord.xy).r > f3CascadeTexCoord.z);

		// Distance to the current position on the ray in light view-space
		// NB: We don't want to attenuate the directional light
		//const float fLightDist = length(f3RayPositionLightVS - float3(f3CameraPositionLightVS.xy, f3CameraPositionLightVS.z - fRaymarchDistanceLimit));
		//const float fLightDistRcp = rcp(fLightDist);

#ifdef FOG_DENSITY_MAP
		// Sample the fog/noise texture and apply an exponential vertical falloff
		// NB: ddx()/ddy() (i.e. any texture fetch operation with auto mip selection) requires loop unrolling, so go for tex3Dlod() instead
		const float fDensity = tex3Dlod(texNoise, float4(f3RayPositionLightWS + f3FogSampleOffset, 0.f)).r * exp(-f3RayPositionLightWS.y * f3RayPositionLightWS.y * fFogVerticalFalloff);
#else // FOG_DENSITY_MAP
		const float fDensity = exp(-f3RayPositionLightWS.y * f3RayPositionLightWS.y * fFogVerticalFalloff);
#endif

		// Calculate the final light contribution for the sample on the ray
		// and add it to the total contribution of the ray
		//f4Color.rgb += TAU * (bLit * (PHI * 0.25f * PI_RCP) * fLightDistRcp * fLightDistRcp) * exp(-fLightDist * TAU) * exp(-fRayDist * TAU) * fStepSize;
		f4Color.rgb += (fMultScatterIntensity + fLightIntensity * bLit) * exp(-fRayDist * TAU) * fDensity; // * (fLightDistRcp * fLightDistRcp) * exp(-fLightDist * TAU)
	}

	// Moved some calculations outside of the above 'for' as an optimization
	f4Color.rgb *= TAU * (PHI * 0.25f * PI_RCP) * fStepSize;
}
////////////////////////////////////////////////////////////////////
