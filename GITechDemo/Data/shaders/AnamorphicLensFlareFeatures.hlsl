#include "PostProcessingUtils.hlsl"

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;

struct VSOut
{
	float4	f4Position			:	SV_POSITION;
	float2	f2TexCoord			:	TEXCOORD0;
	float2	f2FlippedTexCoord	:	TEXCOORD1;
};

void vsmain(float4 f4Position : POSITION, float2 f2TexCoord : TEXCOORD, out VSOut output)
{
	output.f4Position = f4Position;
	output.f2TexCoord = f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
	// Flip texture coordinates horizontally/vertically
	output.f2FlippedTexCoord = float2(1.f, 1.f) - (f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset);
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D	texSource;	// Source texture
const float2 f2TexelSize;	// Size of source texture texel

const float fAnamorphicIntensity;

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 1.f);

	const float fWeight[] =
	{
		0.044256f,
		0.044036f, 0.043381f, 0.042311f, 0.040857f,
		0.03906f, 0.036972f, 0.034647f, 0.032145f,
		0.029528f, 0.026854f, 0.02418f, 0.021555f,
		0.019024f, 0.016624f, 0.014381f, 0.012318f
	};

	UNROLL for (int i = -16; i <= 16; i++)
	{
		// Calculate coordinates for sampling source texture
		const float2 f2Offset = float2(i * f2TexelSize.x, 0.f);
		const float2 f2SampleTexCoord = input.f2TexCoord + f2Offset;

		// Sample the texture and give it a bluish tint
		float3 f3SampleColor = tex2D(texSource, f2SampleTexCoord).rgb;
		f3SampleColor.b += f3SampleColor.r + f3SampleColor.g;

		f4Color.rgb += fWeight[abs(i)] * f3SampleColor * fAnamorphicIntensity;
	}
}