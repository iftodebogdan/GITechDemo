#include "PostProcessUtils.hlsl"
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
const sampler2D	texSource;	// The texture to be blurred
const float fBloomStrength;	// Intensity of bloom
const float fBloomPower;	// Exponent of bloom
const float2 f2TexelSize;	// Size of a texel
const int nKernel;			// Kernel size for current pass

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = KawaseBlur(texSource, f2TexelSize, input.f2TexCoord, nKernel);
	f4Color = pow(abs(f4Color), fBloomPower) * fBloomStrength;
}
////////////////////////////////////////////////////////////////////