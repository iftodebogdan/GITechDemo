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
const sampler2D texLensFlareFeatures; // Lens flare effect features
const sampler2D texLensFlareDirt; // Lens dirt texture
const sampler2D texLensFlareStarBurst; // Lens star burst texture

const float fLensDirtIntensity; // Scale factor for lens dirt texture samples
const float fLensStarBurstIntensity; // Scale factor for lens star burst texture samples

// Transform matrix for the star burst texture coordinates
// applying a camera dependent rotation
const float3x3 f33LensFlareStarBurstMat;

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	//////////////////////////////////////////////////////////////////////////////
	// Lens flare effect														//
	// http://john-chapman-graphics.blogspot.ro/2013/02/pseudo-lens-flare.html	//
	//////////////////////////////////////////////////////////////////////////////

	// Sample lens flare features
	f4Color  = float4(tex2D(texLensFlareFeatures, input.f2TexCoord).rgb, 0.f);
	
	// Sample dirt texture
	const float3 f3Dirt = tex2D(texLensFlareDirt, input.f2TexCoord).rgb * fLensDirtIntensity;

	// Sample star burst texture
	const float3 f3StarBurst = tex2D(texLensFlareStarBurst, mul(f33LensFlareStarBurstMat, float3(input.f2TexCoord, 1.f)).xy).rgb * fLensStarBurstIntensity;

	// Modulate features by dirt and star burst textures
	f4Color.rgb *= f3Dirt + f3StarBurst;
}
////////////////////////////////////////////////////////////////////