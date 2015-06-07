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
const sampler2D	texLumaCalcInput;

// Flags for deciding code path
const bool bInitialLumaPass;
const bool bFinalLumaPass;
const bool bLumaAdaptationPass;

// The 1x1 texture with the target average luma value
const sampler2D texLumaTarget;

// The speed of the animation
const float fLumaAdaptSpeed;

// Last frame's duration in seconds
const float fFrameTime;

// Minimum and maximum values for average luma
const float2 f2AvgLumaClamp;

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 0.f);

	// The initial pass samples the luminance of the source average luma
	// texture using a 3x3 kernel of sample points, downscaling to an image
	// containing the log() of averages
	if (bInitialLumaPass)
	{
		const float2 f2Kernel = 2.f * f2HalfTexelOffset / 3.f;
		float fLogLumSum = 0.f;
		for (float i = -1.f; i <= 1.f; i++)
			for (float j = -1.f; j <= 1.f; j++)
			{
				// The average luma can be calculated from part of the source image,
				// hence the scale of the texture coordinates
				const float2 f2ScaledTexCoord = input.f2TexCoord * float2(0.4f, 0.6f) + float2(0.3f, 0.2f);
				const float3 f3Sample = tex2D(texLumaCalcInput, f2ScaledTexCoord + f2Kernel * float2(i, j)).rgb;
				fLogLumSum += log(dot(f3Sample, LUMINANCE_VECTOR) + 0.0001f);
			}

		fLogLumSum /= 9.f;
		f4Color = float4(fLogLumSum, fLogLumSum, fLogLumSum, 1.f);
	}
	// The rest of the passes further downscale the average luma texture
	else if(!bLumaAdaptationPass)
	{
		float fAvgLuma = 0.f;
		for (float i = -3.f; i <= 3.f; i += 2.f)
			for (float j = -3.f; j <= 3.f; j += 2.f)
				fAvgLuma += tex2D(texLumaCalcInput, input.f2TexCoord + f2HalfTexelOffset * float2(i, j)).r;

		// On the final pass, we do an exp() and store the value into
		// the final 1x1 average luma texture
		if (bFinalLumaPass)
		{
			fAvgLuma = exp(fAvgLuma / 16.f);
			fAvgLuma = clamp(fAvgLuma, f2AvgLumaClamp.x, f2AvgLumaClamp.y);
		}
		else
			fAvgLuma /= 16.f;

		f4Color = float4(fAvgLuma, fAvgLuma, fAvgLuma, 1.f);
	}
	// Two 1x1 textures containing last and current frames' average lumas are used to slowly
	// adjust the exposure of the HDR image when tone mapping. The target luma is the one
	// calculated just above.
	else if (bLumaAdaptationPass)
	{
		const float fCurrLuma = tex2D(texLumaCalcInput, float2(0.5f, 0.5f)).r;
		const float fTargetLuma = tex2D(texLumaTarget, float2(0.5f, 0.5f)).r;
		const float fNewLuma = fCurrLuma + (fTargetLuma - fCurrLuma) * (fLumaAdaptSpeed * fFrameTime);
		f4Color = float4(fNewLuma, fNewLuma, fNewLuma, 1.f);
	}
}
////////////////////////////////////////////////////////////////////