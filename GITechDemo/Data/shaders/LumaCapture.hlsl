#include "PostProcessingUtils.hlsl"
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

// One of the two 1x1 textures with the current average luma value
const sampler2D	texLumaInput;

// Flags for deciding code path
const bool bInitialLumaPass;
const bool bFinalLumaPass;

// The 1x1 texture with the target average luma value
const sampler2D texLumaTarget;

// Minimum and maximum values for average luma
const float2 f2AvgLumaClamp;

// Size of a texel
const float2 f2TexelSize;

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 0.f);

	// The initial pass samples the luminance of the source average luma
	// texture using a 3x3 kernel of sample points, downscaling to an image
	// containing the log() of averages
	if (bInitialLumaPass)
	{
		const float2 f2Kernel = f2TexelSize * 0.33333333f;
		float fLogLumSum = 0.f;
		UNROLL for (float i = -1.f; i <= 1.f; i++)
			UNROLL for (float j = -1.f; j <= 1.f; j++)
			{
				// The average luma can be calculated from part of the source image,
				// hence the scale of the texture coordinates
				const float2 f2ScaledTexCoord = input.f2TexCoord;// *float2(0.4f, 0.6f) + float2(0.3f, 0.2f);
				const float3 f3Sample = tex2D(texLumaInput, f2ScaledTexCoord + f2Kernel * float2(i, j)).rgb;
				fLogLumSum += log(dot(f3Sample, LUMINANCE_VECTOR) + 0.0001f);
			}

		fLogLumSum *= 0.11111111f;
		f4Color = float4(fLogLumSum, fLogLumSum, fLogLumSum, 1.f);
	}
	// The rest of the passes further downscale the average luma texture
	else
	{
		float fAvgLuma = 0.f;
		UNROLL for (float i = -1.5f; i <= 1.5f; i += 1.f)
			UNROLL for (float j = -1.5f; j <= 1.5f; j += 1.f)
				fAvgLuma += tex2D(texLumaInput, input.f2TexCoord + f2TexelSize * float2(i, j)).r;

		// On the final pass, we do an exp() and store the value into
		// the final 1x1 average luma texture
		if (bFinalLumaPass)
		{
			fAvgLuma = exp(fAvgLuma * 0.0625f);
			fAvgLuma = clamp(fAvgLuma, f2AvgLumaClamp.x, f2AvgLumaClamp.y);
		}
		else
			fAvgLuma *= 0.0625f;

		f4Color = float4(fAvgLuma, fAvgLuma, fAvgLuma, 1.f);
	}
}
////////////////////////////////////////////////////////////////////