#include "PostProcessingUtils.hlsl"
#include "Utils.hlsl"

// Vertex shader /////////////////////////////////////////////////
const float2 f2HalfTexelOffset;

struct VSOut
{
	float4	f4Position	:	SV_POSITION;
	float2	f2TexCoord	:	TEXCOORD0;
	float2	f2ScreenPos	:	TEXCOORD1;
};

void vsmain(float4 f4Position : POSITION, float2 f2TexCoord : TEXCOORD, out VSOut output)
{
	output.f4Position = f4Position;
	output.f2TexCoord = f4Position.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f) + f2HalfTexelOffset;
	output.f2ScreenPos = f4Position.xy;
}
////////////////////////////////////////////////////////////////////

// Pixel shader ///////////////////////////////////////////////////
const sampler2D	texNormalBuffer;	// View-space normals
const sampler2D	texDepthBuffer;		// Depth values

const float4x4 f44InvProjMat;	// Matrix for inversing the projection transform

const float fSSAOSampleRadius;	// Radius of the sampling pattern
const float fSSAOIntensity;		// Overall intensity of the SSAO effect
const float fSSAOScale;			// Scale for the occlusion attenuation with distance
const float fSSAOBias;			// Bias for the occlusion attenuation with normal differences

static const float2 f2Kernel[4] =
{
	float2(	 1.f,	 0.f),
	float2(	-1.f,	 0.f),
	float2(	 0.f,	 1.f),
	float2(	 0.f,	-1.f)
};

/*
	Simple Screen-Space Ambient Occlusion shader.
	Based on the work of José María Méndez:
	http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753

	More exotic / esoteric / physically-based methods of calculating
	ambient occlusion are subject for future research, if time permits.
	(e.g. http://sirkan.iit.bme.hu/~szirmay/ambient8.pdf)
*/
float AOCalc(const float2 f2TexCoord, const float2 f2Offset, const float3 f3Position, const float3 f3Normal)
{
	const float2 f2SampleTexCoord		= f2TexCoord + f2Offset;
	const float2 f2SampleScreenPos		= f2SampleTexCoord * float2(2.f, -2.f) - float2(1.f, -1.f);
	const float fSampleDepth			= tex2D(texDepthBuffer, f2SampleTexCoord).r;
	const float4 f4SampleProjPosition	= float4(f2SampleScreenPos, fSampleDepth, 1.f);
	const float4 f4SamplePositionPreW	= mul(f44InvProjMat, f4SampleProjPosition);
	const float3 f3SamplePosition		= f4SamplePositionPreW.xyz / f4SamplePositionPreW.w;

	const float3 f3PosDiff = f3SamplePosition - f3Position;
	const float3 f3Dir = normalize(f3PosDiff);
	const float f3Dist = length(f3PosDiff) * fSSAOScale;

	return max(0.f, dot(f3Normal, f3Dir) - fSSAOBias) * rcp(1.f + f3Dist) * fSSAOIntensity;
}

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	const float fDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;
	const float4 f4ProjPosition = float4(input.f2ScreenPos, fDepth, 1.f);
	const float4 f4PositionPreW = mul(f44InvProjMat, f4ProjPosition);
	const float3 f3Position = f4PositionPreW.xyz / f4PositionPreW.w;
	const float3 f3Normal = DecodeNormal(tex2D(texNormalBuffer, input.f2TexCoord));
	//const float2 f2Rand = float2(GenerateRandomNumber(input.f2TexCoord.xy), GenerateRandomNumber(input.f2TexCoord.yx));

	float fAO = 0.0f;
	const float fRad = fSSAOSampleRadius / f3Position.z;

	UNROLL for (int i = 0; i < 4; i++)
	{
		// Noise isn't really needed since we'll be blurring the AO mask
		// and it won't look biased towards any discernable direction.
		//const float2 f2Offset1 = reflect(f2Kernel[i], f2Rand) * fRad;
		const float2 f2Offset1 = f2Kernel[i] * fRad;
		const float2 f2Offset2 = float2(
			f2Offset1.x * 0.707f - f2Offset1.y * 0.707f,
			f2Offset1.x * 0.707f + f2Offset1.y * 0.707f
			);

		fAO += AOCalc(input.f2TexCoord, f2Offset1 * 0.25f, f3Position, f3Normal);
		fAO += AOCalc(input.f2TexCoord, f2Offset2 * 0.5f, f3Position, f3Normal);
		fAO += AOCalc(input.f2TexCoord, f2Offset1 * 0.75f, f3Position, f3Normal);
		fAO += AOCalc(input.f2TexCoord, f2Offset2, f3Position, f3Normal);
	}

	fAO *= 0.0625f;

	f4Color = float4(fAO, fAO, fAO, 1.f);
}
////////////////////////////////////////////////////////////////////
