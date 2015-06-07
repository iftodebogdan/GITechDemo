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
const float fBloomStrength;	// Intensity of bloom
const float fBloomPower;	// Exponent of bloom
const float2 f2TexelSize;	// Size of a texel
const int nKernel;			// Kernel size for current pass

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	f4Color = float4(0.f, 0.f, 0.f, 0.f);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Kawase blur, approximating a 35x35 Gaussian kernel																	//
	// in the nKernel = [0-1-2-2-3] format over 5 passes																	//
	// https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (int i = -1; i <= 1; i += 2)
	{
		for (int j = -1; j <= 1; j += 2)
		{
			const float2 f2TexelOffset = f2TexelSize * float2(i, j);
			const float2 f2HalfTexelOffset = 0.5f * f2TexelOffset;
			const float2 f2HalfTexelSize = 0.5f * f2TexelSize;
			const float2 f2SampleCenter = input.f2TexCoord + f2HalfTexelOffset + f2TexelOffset * nKernel;

			for (int x = -1; x <= 1; x += 2)
				for (int y = -1; y <= 1; y += 2)
					f4Color += tex2D(texSource, f2SampleCenter + f2HalfTexelSize * float2(x, y));
		}
	}

	f4Color = pow(abs(f4Color) / 16.f, fBloomPower) * fBloomStrength;
}
////////////////////////////////////////////////////////////////////