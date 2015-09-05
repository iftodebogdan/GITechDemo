// This define will modify the behaviour of the RSMApply()
// function so that it is aware of the multi-pass approach.
#define RSM_APPLY_PASS (1)
#include "RSMCommon.hlsl"

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
const sampler2D	texDepthBuffer;	// G-Buffer depth values

struct PSOut
{
	float4 colorOut	:	SV_TARGET;
};

void psmain(VSOut input, out PSOut output)
{
	output.colorOut = float4(0.f, 0.f, 0.f, 1.f);

	// Early depth test so that we don't shade
	// at the far plane (where the sky is drawn)
	const float fDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;
	DEPTH_KILL(fDepth, 1.f);

	ApplyRSM(input.f2TexCoord, fDepth, output.colorOut);
}
////////////////////////////////////////////////////////////////////