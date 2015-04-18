#include "Utils.hlsl"

// Vertex shader /////////////////////////////////////////////////
const float4x4 f44WorldViewProjMat;
const float4x4 f44WorldViewMat;

struct VSIn
{
	float4 f4Position	:	POSITION;
	float2 f2TexCoord	:	TEXCOORD;
	float3 f3Normal		:	NORMAL;
	float3 f3Tangent	:	TANGENT;
	float3 f3Binormal	:	BINORMAL;
};

struct VSOut
{
	float4 f4Position	:	SV_POSITION;
	float2 f2TexCoord	:	TEXCOORD0;
	float2 f2Depth		:	TEXCOORD1;
	float3 f3Normal		:	NORMAL;
	float3 f3Tangent	:	TANGENT;
	float3 f3Binormal	:	BINORMAL;
};

void vsmain(VSIn input, out VSOut output)
{
	output.f4Position	=	mul(f44WorldViewProjMat, input.f4Position);
	output.f2TexCoord	=	input.f2TexCoord;
	output.f2Depth		=	output.f4Position.zw;
	output.f3Normal		=	normalize(mul((float3x3)f44WorldViewMat, input.f3Normal));
	output.f3Tangent	=	normalize(mul((float3x3)f44WorldViewMat, input.f3Tangent));
	output.f3Binormal	=	normalize(mul((float3x3)f44WorldViewMat, input.f3Binormal));
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D texDiffuse;
const sampler2D texNormal;
const sampler2D texSpec;

const float fSpecIntensity;

const bool bHasNormalMap;
const bool bHasSpecMap;

struct PSOut
{
	float4 f4Diffuse	:	SV_TARGET0;
	float4 f4Normal		:	SV_TARGET1;
};

void psmain(VSOut input, out PSOut output)
{
	// sample the diffuse texture
	float4 f4DiffuseColor = tex2D(texDiffuse, input.f2TexCoord);
	
	// early alpha-test
	if(f4DiffuseColor.a < 0.5f)
		clip(-1);
	
	// sample the specular texture, if present
	if(bHasSpecMap)
		f4DiffuseColor.a = tex2D(texSpec, input.f2TexCoord).r;
	else
		f4DiffuseColor.a = fSpecIntensity;
	
	// sample the normal map, if present
	float3 f3NormalMap = float3(0.f, 0.f, 0.f);
	if(bHasNormalMap)
		f3NormalMap = tex2D(texNormal, input.f2TexCoord).rgb * 2.f - 1.f;
	float3 f3Normal = input.f3Normal + f3NormalMap.x * input.f3Tangent + f3NormalMap.y * input.f3Binormal;
	
	output.f4Diffuse = f4DiffuseColor;
	output.f4Normal = EncodeNormal(f3Normal);
}
////////////////////////////////////////////////////////////////////