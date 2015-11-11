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

// The 1x1 texture with the target average luma value
const sampler2D texLumaTarget;

// The speed of the animation
const float fLumaAdaptSpeed;

// Last frame's duration in seconds
const float fFrameTime;

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 0.f);

	// Two 1x1 textures containing last and current frames' average lumas are used
	// to slowly adjust the exposure of the HDR image when tone mapping.
	const float fCurrLuma = tex2D(texLumaInput, float2(0.5f, 0.5f)).r;
	const float fTargetLuma = tex2D(texLumaTarget, float2(0.5f, 0.5f)).r;
	float fNewLuma = fCurrLuma + (fTargetLuma - fCurrLuma) * (rcp(clamp(fLumaAdaptSpeed, 0.01f, 3.402823466e+38f)) * fFrameTime);

	if (fCurrLuma < fTargetLuma)
		fNewLuma = clamp(fNewLuma, fCurrLuma, fTargetLuma);
	else
		fNewLuma = clamp(fNewLuma, fTargetLuma, fCurrLuma);

	f4Color = float4(fNewLuma, fNewLuma, fNewLuma, 1.f);
}
////////////////////////////////////////////////////////////////////