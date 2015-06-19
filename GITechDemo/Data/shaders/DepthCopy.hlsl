#include "PostProcessUtils.hlsl"

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;

struct VSOut
{
	float4	f4Position	:	SV_POSITION;
	float2	f2TexCoord	:	TEXCOORD0;
};

void vsmain(float4 f4Position : POSITION, out VSOut output)
{
	output.f4Position	= f4Position;
	output.f2TexCoord	= f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D	texDepthBuffer;	// Source depth texture

struct PSOut
{
	float4	f4DummyColor	: SV_TARGET;
	float	fDepth			: SV_DEPTH;
};

void psmain(VSOut input, out PSOut output)
{
	output.f4DummyColor	= float4(1.f, 0.f, 1.f, 1.f);
	output.fDepth		= tex2D(texDepthBuffer, input.f2TexCoord).r;
}
////////////////////////////////////////////////////////////////////