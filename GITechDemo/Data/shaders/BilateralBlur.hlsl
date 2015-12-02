#include "PostProcessingUtils.hlsl"

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;

struct VSOut
{
	float4	f4Position	:	SV_POSITION;
	float2	f2TexCoord	:	TEXCOORD0;
};

void vsmain(float4 f4Position : POSITION, float2 f2TexCoord : TEXCOORD, out VSOut output)
{
	output.f4Position = f4Position;
	output.f2TexCoord = f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D	texSource;		// The texture to be blurred
const sampler2D	texDepthBuffer;	// Scene depth

const float2 f2BlurDir;				// Horizontal or vertical blur
const float2 f2TexelSize;			// Normalized size of a texel

// Depth threshold for edge detection
const float fBlurDepthFalloff;

// Number of samples on each side of the kernel
#define NUM_SAMPLES_HALF	(7)

static const float fGaussianFilterWeight[] =
{
	0.14446445f, 0.13543542f,
	0.11153505f, 0.08055309f,
	0.05087564f, 0.02798160f,
	0.01332457f, 0.00545096f
};

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 1.f);
	float fTotalWeight = 0.f;

	// Get reference downsampled depth (center of kernel)
	const float fRefDepth = ReconstructDepth(GetDownsampledDepth(texDepthBuffer, input.f2TexCoord));

	UNROLL for (int i = -NUM_SAMPLES_HALF; i <= NUM_SAMPLES_HALF; i++)
	{
		// Calculate coordinates for sampling source texture
		const float2 f2Offset = i * f2BlurDir * f2TexelSize;
		const float2 f2SampleTexCoord = input.f2TexCoord + f2Offset;

		// Sample source color
		const float3 f3SampleColor = tex2D(texSource, f2SampleTexCoord).rgb;
		// Calculate downsampled depth for the above sample
		const float fSampleDepth = ReconstructDepth(GetDownsampledDepth(texDepthBuffer, f2SampleTexCoord).r);

		// Simple depth-aware filtering
		const float fDepthDiff = fBlurDepthFalloff * abs(fSampleDepth - fRefDepth);
		const float fWeight = exp(-fDepthDiff * fDepthDiff) * fGaussianFilterWeight[abs(i)];

		f4Color.rgb += fWeight * f3SampleColor;
		fTotalWeight += fWeight;
	}

	f4Color.rgb *= rcp(fTotalWeight);

	// Attempt to save some bandwidth
	clip(-!any(f4Color.rgb));
}
////////////////////////////////////////////////////////////////////
