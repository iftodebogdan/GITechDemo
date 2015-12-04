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

// Ghost features
const sampler1D texGhostColorLUT;
const int nGhostSamples;
const float fGhostDispersal;
const float fGhostRadialWeightExp;

// Halo feature
const float fHaloSize;
const float fHaloRadialWeightExp;

// Chromatic abberation feature
const bool bChromaShift;
const float fShiftFactor;

// Anamorphic or spherical lens?
const bool bAnamorphic;
const float fAnamorphicIntensity;

// Replacement for tex2D() which adds a
// chromatic abberation effect to texture samples
float4 FetchChromaShiftedTextureSample(sampler2D tex, float2 texCoord)
{
	if (bChromaShift && !bAnamorphic)
	{
		const float3 f2ShiftAmount = float3(
			-f2TexelSize.x * fShiftFactor,
			0.f,
			f2TexelSize.x * fShiftFactor);
		const float2 f2Dir = normalize(float2(0.5f, 0.5f) - texCoord);
		return float4(
			tex2D(tex, texCoord + f2Dir * f2ShiftAmount.r).r,
			tex2D(tex, texCoord + f2Dir * f2ShiftAmount.g).g,
			tex2D(tex, texCoord + f2Dir * f2ShiftAmount.b).b,
			1.f);
	}
	else
		return tex2D(tex, texCoord);
}

// Convert tex2D() calls to FetchChromaShiftedTextureSample() calls
#ifdef tex2D
#undef tex2D
#endif
#define tex2D(tex, texCoord) FetchChromaShiftedTextureSample(tex, texCoord)

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	//////////////////////////////////////////////////////////////////////////////
	// Lens flare effect														//
	// http://john-chapman-graphics.blogspot.ro/2013/02/pseudo-lens-flare.html	//
	//////////////////////////////////////////////////////////////////////////////

	f4Color = float4(0.f, 0.f, 0.f, 1.f);

	if (!bAnamorphic)
	{
		// Ghost vector to image center
		const float2 f2GhostVec = (float2(0.5f, 0.5f) - input.f2FlippedTexCoord) * fGhostDispersal;

		// Generate ghost features
		for (int i = 0; i < nGhostSamples; i++)
		{
			const float2 f2Offset = input.f2FlippedTexCoord + f2GhostVec * float(i);
			const float fGhostWeight =
				pow(abs(1.f -
					length(float2(0.5f, 0.5f) - f2Offset) *
					rcp(length(float2(0.5f, 0.5f)))),
					fGhostRadialWeightExp);
			f4Color.rgb += tex2D(texSource, f2Offset).rgb * fGhostWeight;
		}

		// Adjust ghosts' color using a LUT
		f4Color.rgb *= tex1D(texGhostColorLUT, length(float2(0.5f, 0.5f) - input.f2FlippedTexCoord) * rcp(length(float2(0.5f, 0.5f)))).rgb;

		// Generate halo feature
		const float2 f2HaloVec = normalize(f2GhostVec) * fHaloSize;
		const float fHaloWeight =
			pow(abs(1.f -
				length(
					float2(0.5f, 0.5f) -
					(input.f2FlippedTexCoord + f2HaloVec)
					) *
				rcp(length(float2(0.5f, 0.5f)))),
				fHaloRadialWeightExp);
		f4Color.rgb += tex2D(texSource, input.f2FlippedTexCoord + f2HaloVec).rgb * fHaloWeight;
	}
	else
	{
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
}
////////////////////////////////////////////////////////////////////