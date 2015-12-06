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
const sampler2D	texSource;	// The texture to be blurred
const float2 f2TexelSize;	// Size of a texel
const int nKernel;			// Kernel size for current pass

float4 KawaseBlurAnamorphic(const sampler2D texSource, const float2 f2TexelSize, const float2 f2TexCoord, const int nKernel)
{
	float4 f4Color = float4(0.f, 0.f, 0.f, 0.f);

	UNROLL for (int i = -1; i <= 1; i += 2)
	{
		UNROLL for (int j = -1; j <= 1; j += 2)
		{
			const float2 f2TexelOffset = f2TexelSize * float2(i, j);
			const float2 f2HalfTexelOffset = 0.5f * f2TexelOffset;
			const float2 f2HalfTexelSize = 0.5f * f2TexelSize;
			const float2 f2SampleCenter = f2TexCoord + f2HalfTexelOffset + f2TexelOffset * nKernel * float2(1.f, 0.f);

			UNROLL for (int x = -1; x <= 1; x += 2)
				UNROLL for (int y = -1; y <= 1; y += 2)
				f4Color += tex2D(texSource, f2SampleCenter + f2HalfTexelSize * float2(x, y));
		}
	}

	return f4Color * 0.0625f;
}

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = KawaseBlurAnamorphic(texSource, f2TexelSize, input.f2TexCoord, nKernel);
}
////////////////////////////////////////////////////////////////////