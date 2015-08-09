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
const sampler2D	texSource;	// Source HDR texture
const sampler2D texAvgLuma;	// 1x1 average luma texture

const float fExposureBias;	// Exposure amount

const float fShoulderStrength;	// = 0.15;
const float fLinearStrength;	// = 0.50;
const float fLinearAngle;		// = 0.10;
const float fToeStrength;		// = 0.20;
const float fToeNumerator;		// = 0.02;
const float fToeDenominator;	// = 0.30;
const float fLinearWhite;		// = 11.2;

float3 ReinhardTonemap(const float3 f3Color, const float fAvgLuma)
{
	return f3Color * rcp(1 + fAvgLuma);
}

float3 DuikerOptimizedTonemap(const float3 f3Color)
{
	float3 x = max(0, f3Color - 0.004);
	return (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
}

//////////////////////////////////////////////////////////////////////////////
// Filmic tone mapping operator used in Uncharted 2							//
// http://filmicgames.com/Downloads/GDC_2010/Uncharted2-Hdr-Lighting.pptx	//
//////////////////////////////////////////////////////////////////////////////
float3 FilmicTonemap(const float3 f3Color)
{
	return
		(
			(
				f3Color * (fShoulderStrength * f3Color + fLinearStrength * fLinearAngle)
				+ fToeStrength * fToeNumerator
			) /
			(
				f3Color * (fShoulderStrength * f3Color + fLinearStrength)
				+ fToeStrength * fToeDenominator
			)
		) - fToeNumerator / fToeDenominator;
}

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	//////////////////////////////////////////////////////////////////
	// Convert color from gamma space to linear space				//
	// http://http.developer.nvidia.com/GPUGems3/gpugems3_ch24.html	//
	//////////////////////////////////////////////////////////////////

	// Linear gamma conversion done by SamplerState::SetSRGBEnabled()
	//float3 f3Color = pow(abs(tex2D(texSource, input.f2TexCoord)), 2.2f).rgb;
	float3 f3Color = tex2D(texSource, input.f2TexCoord).rgb;

	float fAvgLuma = tex2D(texAvgLuma, float2(0.5f, 0.5f)).r;
	f3Color /= fAvgLuma;

	//////////////////////////////////////////
	// Apply tone mapping operator			//
	// http://filmicgames.com/archives/75	//
	//////////////////////////////////////////

	// Reinhard
	//float3 f3FinalColor = ReinhardTonemap(f3Color * fExposureBias, fAvgLuma);

	// Duiker
	//float3 f3FinalColor = DuikerOptimizedTonemap(f3Color * fExposureBias);

	// Uncharted 2
	float3 f3FinalColor = FilmicTonemap(f3Color * fExposureBias);
	const float3 f3WhiteScale = 1.0f / FilmicTonemap(fLinearWhite);
	f3FinalColor *= f3WhiteScale;

	// Convert back to gamma space (not required for Duiker tonemap)
	// NB: Gamma correction done by RenderState::SetSRGBWriteEnabled()
	//f4Color = float4(pow(abs(f3FinalColor), rcp(2.2f)), 1);
	// Encode gamma-corrected luma in the alpha channel for FXAA
	f4Color = float4(f3FinalColor, pow(abs(dot(f3FinalColor, LUMINANCE_VECTOR)), rcp(2.2f)));
}
////////////////////////////////////////////////////////////////////