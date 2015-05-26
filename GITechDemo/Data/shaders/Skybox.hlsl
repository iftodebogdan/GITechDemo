// Vertex shader /////////////////////////////////////////////////
const float4x4 f44SkyViewProjMat;

struct VSIn
{
	float4 f4Position	:	POSITION;
};

struct VSOut
{
	float4 f4Position	:	SV_POSITION;
	float3 f3TexCoord	:	TEXCOORD0;
};

void vsmain(VSIn input, out VSOut output)
{
	output.f4Position	=	mul(f44SkyViewProjMat, input.f4Position);
	output.f3TexCoord	=	normalize(input.f4Position.xyz);
	output.f4Position.z =	output.f4Position.w; // position sky at far plane
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const samplerCUBE texSkyTex;

const float3 f3LightDir;
const float fSunRadius;
const float fSunBrightness;

struct PSOut
{
	float4 f4SkyColor	:	SV_TARGET0;
};

void psmain(VSOut input, out PSOut output)
{
	const float3 f3SunDir	=	normalize(-f3LightDir);
	const float	fSunDot		=	dot(normalize(input.f3TexCoord), f3SunDir);

	output.f4SkyColor		=	texCUBE(texSkyTex, input.f3TexCoord);
	output.f4SkyColor		+=	pow(max(0.f, fSunDot), fSunRadius) * fSunBrightness;
}
////////////////////////////////////////////////////////////////////