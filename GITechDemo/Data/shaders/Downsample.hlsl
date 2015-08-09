#include "PostProcessingUtils.hlsl"
#include "Utils.hlsl"

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
const sampler2D	texSource;	// Source texture for downsampling
const float2 f2TexelSize;	// Size of source texture texel
const int nDownsampleFactor;		// Switch for 2x2 or 4x4 downsampling
const bool bApplyBrightnessFilter;	// Switch for whether to apply a brightness pass filter
const float fBrightnessThreshold;	// Threshold for the brightness pass filter

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 0.f);

	if (nDownsampleFactor == 16)
		f4Color = Downsample4x4(texSource, input.f2TexCoord, f2TexelSize);

	if (nDownsampleFactor == 4)
		f4Color = Downsample2x2(texSource, input.f2TexCoord, f2TexelSize);

	if (nDownsampleFactor == 1)
		f4Color = tex2D(texSource, input.f2TexCoord);

	if (bApplyBrightnessFilter)
		f4Color.rgb = max(0.f, f4Color.rgb - fBrightnessThreshold);
}
////////////////////////////////////////////////////////////////////