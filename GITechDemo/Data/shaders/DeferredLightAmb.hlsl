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
const sampler2D	texDiffuseBuffer; // diffuse color
const float	fAmbientFactor; // scale value for ambient light

struct PSOut
{
	float4 colorOut	:	SV_TARGET;
};

void psmain(VSOut input, out PSOut output)
{
	// sample the diffuse buffer
	float4 f4DiffuseColor = tex2D(texDiffuseBuffer, input.f2TexCoord);

	// Final color
	output.colorOut = float4(f4DiffuseColor.rgb * fAmbientFactor, 1.f);
}
////////////////////////////////////////////////////////////////////