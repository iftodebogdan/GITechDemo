#include "PostProcessUtils.hlsl"
#include "Utils.hlsl"

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;

struct VSOut
{
	float4 f4Position	:	SV_POSITION;
	float2 f2TexCoord	:	TEXCOORD0;
};

void vsmain(float4 f4Position : POSITION, out VSOut output)
{
	output.f4Position = f4Position;
	output.f2TexCoord = f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D	texRSMFluxBuffer;	// RSM flux data
const sampler2D texRSMNormalBuffer;	// RSM normal data
const sampler2D texRSMDepthBuffer;	// RSM depth data

const sampler2D	texNormalBuffer;	// GBuffer view-space normals
const sampler2D	texDepthBuffer;		// GBuffer depth values

// Composite matrix for transforming coordinates from render
// camera projective space to light projective space
// NB: perspective divide (i.e. w-divide) required
const float4x4 f44ScreenToLightViewMat;

// Matrix for transforming coordinates from light
// view space to light projective space
const float4x4 f44RSMProjMat;

// Matrix for transforming coordinates from
// light projective space to light view space
const float4x4 f44RSMInvProjMat;

// Composite matrix for transforming from render
// camera view space to light view space
const float4x4 f44ViewToRSMViewMat;

// This kernel is based on a Poisson Disk kernel.
// The samples' coordinates are "pushed" towards the exterior of the disk
// by calculating the square root of the sample coordinates.
// The sample weights are in the z component. Their value are actually
// the distance of each sample (before pushing them outward) from the
// center of the kernel. In other words, the sample density is higher
// towards the margin of the kernel, but the weights do not vary linearly
// with that density but instead have a much smoother slope.
// It is this combination of (former) linear weights combined with non-linear
// sample density that has led to good quality with less noise.
#define RSM_NUM_PASSES (8)			// The number of passes
#define RSM_SAMPLES_PER_PASS (16)	// The number of samples from the RSM in each pass
const float3 f3RSMKernel[RSM_SAMPLES_PER_PASS];	// The kernel for the current pass

// Intensity of the indirect light
const float fRSMIntensity;

// Scale the kernel for tweaking between more
// "concentrated" bounces and larger bounce distances
const float fRSMKernelScale;

// For upsampling
const sampler2D	texIndirectLightAccumulationBuffer;	// The texture to be upsampled
const bool bIsUpscalePass;	// Flag for the upscale pass

struct PSOut
{
	float4 colorOut	:	SV_TARGET;
};

bool DoUpscale(const float2 f2TexCoord, const float fDepth, out float4 colorOut);

void psmain(VSOut input, out PSOut output)
{
	output.colorOut = float4(0.f, 0.f, 0.f, 0.f);

	// Early depth test so that we don't shade
	// at the far plane (where the sky is drawn)
	const float fDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;
	if (fDepth == 1.f)
		clip(-1);

	// In order to avoid making a new shader just for this,
	// do an upscale of the indirect light accumulation buffer here
	if (bIsUpscalePass)
		if (DoUpscale(input.f2TexCoord, fDepth, output.colorOut))
			return;

	//////////////////////////////////////////////////////////////////
	// Reflective Shadow Map										//
	// http://www.vis.uni-stuttgart.de/~dachsbcn/download/rsm.pdf	//
	//////////////////////////////////////////////////////////////////

	// Calculate normalized device coordinates (NDC) space position of currently shaded pixel
	const float4 f4ScreenProjSpacePos = float4(input.f2TexCoord * float2(2.f, -2.f) - float2(1.f, -1.f), fDepth, 1.f);
	// Transform pixel coordinates from NDC space to RSM view space
	const float4 f4RSMViewSpacePos = mul(f44ScreenToLightViewMat, f4ScreenProjSpacePos);
	// Perspective w-divide
	const float3 f3RSMViewSpacePos = f4RSMViewSpacePos.xyz / f4RSMViewSpacePos.w;
	// Sample normal for currently shaded pixel and transform to RSM view space
	const float3 f3RSMViewSpaceNormal = mul((float3x3)f44ViewToRSMViewMat, DecodeNormal(tex2D(texNormalBuffer, input.f2TexCoord)));
	// Transform point to RSM NDC space
	const float3 f3RSMProjSpacePos = mul(f44RSMProjMat, float4(f3RSMViewSpacePos, 1.f)).xyz;
	// Calculate texture coordinates for the currently shaded pixel in the RSM texture
	const float2 f2RSMTexCoord = f3RSMProjSpacePos.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);

	// Sample around the corresponding location in the RSM
	// and accumulate light contribution from each VPL
	UNROLL for (unsigned int i = 0; i < RSM_SAMPLES_PER_PASS; i++)
	{
		// Add a bias to the texture coordinate calculated above
		const float2 f2RSMSampleTexCoord = f2RSMTexCoord + f3RSMKernel[i].xy * fRSMKernelScale;
		// The radiant flux for the current sample
		const float4 f4RSMSampleFlux = tex2D(texRSMFluxBuffer, f2RSMSampleTexCoord) * f3RSMKernel[i].z;
		// The normal for the current sample
		const float3 f3RSMSampleNormal = DecodeNormal(tex2D(texRSMNormalBuffer, f2RSMSampleTexCoord));
		// Sample coordinates in RSM NDC space
		const float4 f4RSMSampleProjSpacePos =
			float4(
				f2RSMSampleTexCoord * float2(2.f, -2.f) - float2(1.f, -1.f),
				tex2D(texRSMDepthBuffer, f2RSMSampleTexCoord).r, 1.f
			);
		// Sample coordinates in RSM view space
		const float3 f3RSMSamplePos = mul(f44RSMInvProjMat, f4RSMSampleProjSpacePos).xyz;
		// Distance vector (RSM view space coordinates between shaded pixel and RSM sample
		const float3 f3DeltaPos = f3RSMViewSpacePos - f3RSMSamplePos;
		// Calculate the light contribution from this VPL
		output.colorOut +=
			f4RSMSampleFlux * fRSMIntensity *
			max(0.f, dot(f3RSMSampleNormal, f3DeltaPos)) *
			max(0.f, dot(f3RSMViewSpaceNormal, -f3DeltaPos)) /
			pow(length(f3DeltaPos), 4.f);
	}
}

bool DoUpscale(const float2 f2TexCoord, const float fDepth, out float4 colorOut)
{

	// Sample the color from the quarter-resolution render target
	const float4 f4Color = tex2D(texIndirectLightAccumulationBuffer, f2TexCoord);

	// If the sample is black we can safely clip this pixel
	// so that the GPU doesn't do color blending anymore
	if (!any(f4Color.xyz))
		clip(-1);

	// Easier to write f3TexelOffset.xz or f3TexelOffset.zy than
	// float2(f3TexelOffset.x, 0.f) or float2(0.f, f3TexelOffset.y);
	const float3	f3TexelOffset = float3(f2HalfTexelOffset * 2.f, 0.f);

	// Sample the normal for our reference pixel (i.e. the one we're shading)
	const float3 f3RefNormal = DecodeNormal(tex2D(texNormalBuffer, f2TexCoord));
	// Sample the neighbours' normals
	const float3 f3NeighbourNormalN = DecodeNormal(tex2D(texNormalBuffer, f2TexCoord - f3TexelOffset.zy));
	const float3 f3NeighbourNormalS = DecodeNormal(tex2D(texNormalBuffer, f2TexCoord + f3TexelOffset.zy));
	const float3 f3NeighbourNormalW = DecodeNormal(tex2D(texNormalBuffer, f2TexCoord - f3TexelOffset.xz));
	const float3 f3NeighbourNormalE = DecodeNormal(tex2D(texNormalBuffer, f2TexCoord + f3TexelOffset.xz));

	// Sample the depth for our reference pixel
	const float fRefDepth = fDepth;
	// Sample the neighbours' depths
	const float fNeighbourDepthN = tex2D(texDepthBuffer, f2TexCoord - f3TexelOffset.zy).r;
	const float fNeighbourDepthS = tex2D(texDepthBuffer, f2TexCoord + f3TexelOffset.zy).r;
	const float fNeighbourDepthW = tex2D(texDepthBuffer, f2TexCoord - f3TexelOffset.xz).r;
	const float fNeighbourDepthE = tex2D(texDepthBuffer, f2TexCoord + f3TexelOffset.xz).r;

	// Calculate a weight based on normal differences between the reference pixel and its neighbours
	const float fWeightNormalN = pow(saturate(dot(f3RefNormal, f3NeighbourNormalN)), 32);
	const float fWeightNormalS = pow(saturate(dot(f3RefNormal, f3NeighbourNormalS)), 32);
	const float fWeightNormalW = pow(saturate(dot(f3RefNormal, f3NeighbourNormalW)), 32);
	const float fWeightNormalE = pow(saturate(dot(f3RefNormal, f3NeighbourNormalE)), 32);

	// Calculate a weight based on depth differences between the reference pixel and its neighbours
	const float fWeightDepthN = 1.f / (abs(fRefDepth - fNeighbourDepthN) + 0.00001f);
	const float fWeightDepthS = 1.f / (abs(fRefDepth - fNeighbourDepthS) + 0.00001f);
	const float fWeightDepthW = 1.f / (abs(fRefDepth - fNeighbourDepthW) + 0.00001f);
	const float fWeightDepthE = 1.f / (abs(fRefDepth - fNeighbourDepthE) + 0.00001f);

	// Put all weights into a float4
	const float4 f4Weight =
		normalize(float4(
			fWeightNormalN * fWeightDepthN,
			fWeightNormalS * fWeightDepthS,
			fWeightNormalW * fWeightDepthW,
			fWeightNormalE * fWeightDepthE));

	// Set a threshold which controls the level of sensitivity of the edge detection.
	// A value of 0 provides good quality and also reduces the number of invalidated
	// pixels, that would otherwise require resampling from the RSM.
	const float fWeightThreshold = 0.f;
	// If none of the weights fall below the threshold, or if the pixel is black,
	// then the pixel is valid and it doesn't require reshading using the RSM algorithm.
	if (all(saturate(f4Weight - fWeightThreshold.xxxx)) || !all(f4Color.rgb))
	{
		colorOut = f4Color;
		return true;
	}

	return false;
}
////////////////////////////////////////////////////////////////////