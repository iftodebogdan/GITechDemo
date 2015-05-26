#include "Utils.hlsl"

// Vertex shader /////////////////////////////////////////////////
const float4x4 f44LightWorldViewProjMat;
const float4x4 f44LightWorldViewMat;

struct VSIn
{
	float4 f4Position	:	POSITION;
	float2 f2TexCoord	:	TEXCOORD;
	float3 f3Normal		:	NORMAL;
};

struct VSOut
{
	float4 f4Position	:	SV_POSITION;
	float2 f2TexCoord	:	TEXCOORD0;
	float3 f3Normal		:	NORMAL;
};

void vsmain(VSIn input, out VSOut output)
{
	output.f4Position = mul(f44LightWorldViewProjMat, input.f4Position);
	output.f2TexCoord = input.f2TexCoord;
	output.f3Normal = normalize(mul((float3x3)f44LightWorldViewMat, input.f3Normal));
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D texDiffuse;

struct PSOut
{
	float4 flux		:	SV_TARGET0;
	float4 normal	:	SV_TARGET1;
};

void psmain(VSOut input, out PSOut output)
{
	// Sample the diffuse texture
	float4 f4DiffuseColor = tex2D(texDiffuse, input.f2TexCoord);
	output.flux = f4DiffuseColor;

	// Vertex normal will suffice
	output.normal = EncodeNormal(input.f3Normal);
}
////////////////////////////////////////////////////////////////////