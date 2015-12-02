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
const sampler2D	texSource;		// The texture to be upsampled
const sampler2D	texDepthBuffer;	// Scene depth values

const float2 f2TexelSize;	// Normalized size of source texel
const float2 f2TexSize;		// Size of source texture

// Set a threshold which controls the level of sensitivity of the edge detection.
const float fUpsampleDepthThreshold;

// Retrieves the coordinates for each the 4 bilinear samples corresponding to the current fragment
const float2 GetBilinearSampleCoords(const float2 f2TexCoord, const int nIdx);

// True if volumetric light accumulation buffer has a single channel
const bool bSingleChannelCopy;

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Based on "Nearest-Depth Filter" from the paper														//
	// "Fast rendering of opacity mapped particles using DirectX 11 tessellation and mixed resolutions":	//
	// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/OpacityMappingSDKWhitePaper.pdf		//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Avoid blocky artefacts by using bilinear upsampling,
	// if the current fragment is not located on an edge
	f4Color = float4(tex2D(texSource, input.f2TexCoord).rgb, 1.f);

	// If the sample is black we can safely clip this
	// pixel in an attempt to save some bandwidth
	clip(-!any(f4Color.rgb));

	// Fetch the reference depth for the current high-resolution fragment
	const float fRefDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;

	float fMinDepthDiff = 1.f;
	int nNearestDepthIndex = 0;
	bool bEdgeSample = false;

	UNROLL for (int i = 0; i < 4; i++)
	{
		// Fetch the downsampled depth of the i-th bilinear sample corresponding to the current fragment
		const float fSampleDepth = GetDownsampledDepth(texDepthBuffer, GetBilinearSampleCoords(input.f2TexCoord, i)).r;

		// Calculate the difference to the reference depth value
		const float fCurrentDepthDiff = abs(fSampleDepth - fRefDepth);

		// Reject the sample, if required
		bEdgeSample = bEdgeSample || (fCurrentDepthDiff > fUpsampleDepthThreshold);

		// Mark the best match we've gotten so far
		if (fCurrentDepthDiff < fMinDepthDiff)
		{
			fMinDepthDiff = fCurrentDepthDiff;
			nNearestDepthIndex = i;
		}
	}

	// For edges, simply use the color of the low resolution sample that
	// best matches the full resolution depth value for the current pixel
	if (bEdgeSample)
		f4Color.rgb = tex2D(texSource, GetBilinearSampleCoords(input.f2TexCoord, nNearestDepthIndex)).rgb;

	if (bSingleChannelCopy)
		f4Color.rgb = f4Color.r;
}

const float2 GetBilinearSampleCoords(const float2 f2TexCoord, const int nIdx)
{
	const float2 f2SampleOffset[] =
	{
		float2(-1.f, -1.f),
		float2( 1.f, -1.f),
		float2(-1.f,  1.f),
		float2( 1.f,  1.f)
	};

	// Calculate the offset for the center of the 4 bilinear samples
	// from the low-resolution texture corresponding to the high-resolution
	// fragment, relative to the current high-resolution fragment's coordinates
	const float2 f2CenterOffset = (step(float2(0.5f, 0.5f), frac(f2TexCoord * f2TexSize)) * 2.f - 1.f) * f2HalfTexelOffset;

	// Offset the current coordinates to the center of the 4 bilinear samples, then offset
	// it again to the center of the bilinear sample corresponding to the provided index
	return f2TexCoord + f2CenterOffset + f2TexelSize * 0.5f * f2SampleOffset[nIdx];
}
////////////////////////////////////////////////////////////////////