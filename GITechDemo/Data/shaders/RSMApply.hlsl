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

const float4x4 f44ScreenToLightViewMat;
const float4x4 f44RSMProjMat;
const float4x4 f44RSMInvProjMat;
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
#define RSM_SAMPLES_PER_PASS (16)
const float3 f3RSMKernel[RSM_SAMPLES_PER_PASS];

const float fRSMIntensity;
const float fRSMKernelScale;

// For upsampling
const sampler2D	texIndirectLightAccumulationBuffer;
const bool bIsUpsamplePass;

struct PSOut
{
	float4 colorOut	:	SV_TARGET;
};

void psmain(VSOut input, out PSOut output)
{
	output.colorOut = float4(0.f, 0.f, 0.f, 0.f);

	// In order to avoid making a new shader just for this,
	// do an upscale of the indirect light accumulation buffer here
	if (bIsUpsamplePass)
	{
		// Perhaps a bilateral upscale where the samples are weighted based on depth and
		// normal differences would have been smarter/more accurate. Fortunately, artifacts
		// aren't too noticable when using this naive upscale method.
		output.colorOut = tex2D(texIndirectLightAccumulationBuffer, input.f2TexCoord);
		return;
	}

	// Early depth test so that we don't shade
	// at the far plane (where the sky is drawn)
	const float fDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;
	if (fDepth == 1.f)
		clip(-1);

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
	for (unsigned int i = 0; i < RSM_SAMPLES_PER_PASS; i++)
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
			f4RSMSampleFlux *
			max(0.f, dot(f3RSMSampleNormal, f3DeltaPos)) *
			max(0.f, dot(f3RSMViewSpaceNormal, -f3DeltaPos)) /
			pow(length(f3DeltaPos), 4.f);
	}

	output.colorOut *= fRSMIntensity;
}
////////////////////////////////////////////////////////////////////