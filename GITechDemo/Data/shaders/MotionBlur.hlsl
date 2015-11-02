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
const sampler2D texSource;		// Color texture
const sampler2D texDepthBuffer;	// Depth buffer

const float4x4 f44InvViewProjMat;	// Inverse view-projection matrix
const float4x4 f44PrevViewProjMat;	// Previous frame's view-projection matrix
const float fFrameTime;				// Frame duration
const float fMotionBlurIntensity;	// Intensity of the motion blur effect
const int nMotionBlurNumSamples;	// The number of samples along the velocity vector

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	//////////////////////////////////////////////////////////////////
	// Motion blur effect											//
	// http://http.developer.nvidia.com/GPUGems3/gpugems3_ch27.html	//
	//////////////////////////////////////////////////////////////////

	f4Color = float4(0.f, 0.f, 0.f, 1.f);

	// Compute NDC space position
	const float fDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;
	const float4 f4NDCPos = float4(input.f2ScreenPos, fDepth, 1.f);

	// Compute world space position
	const float4 f4WorldPosPreW = mul(f44InvViewProjMat, f4NDCPos);
	const float4 f4WorldPos = f4WorldPosPreW * rcp(f4WorldPosPreW.w);

	// Compute last frame NDC space position
	const float4 f4PrevNDCPosPreW = mul(f44PrevViewProjMat, f4WorldPos);
	const float4 f4PrevNDCPos = f4PrevNDCPosPreW * rcp(f4PrevNDCPosPreW.w);

	// Compute pixel velocity
	const float2 f2Velocity =
		(f4NDCPos.xy - f4PrevNDCPos.xy) * rcp(fFrameTime) // Speed (NDC units / s)
		* fMotionBlurIntensity // Scale velocity by intensity value
		* rcp(nMotionBlurNumSamples); // Divide by the number of samples (so that there would be no
										// change in perceived intensity if sample count varies)

	// Sample along the velocity vector and average the result
	for (int i = 0; i < nMotionBlurNumSamples; i++)
		f4Color.rgb += tex2D(texSource, input.f2TexCoord + f2Velocity * i).rgb;
	f4Color.rgb *= rcp(nMotionBlurNumSamples);
}
////////////////////////////////////////////////////////////////////
