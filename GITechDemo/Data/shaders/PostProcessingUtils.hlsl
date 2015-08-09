#include "Common.hlsl"

//////////////////////////////////////////////////////////
// Hardcoded mip level when sampling from textures to	//
// prevent the GPU from calculating ddx(), ddy() for	//
// every sample (post-processing effects work with		//
// textures with 1 mip level)							//
//////////////////////////////////////////////////////////
#define tex2D(tex, tc) tex2Dlod(tex, float4((tc).xy, 0.f, 0.f))



//////////////////////////////////////////////////////////////////////////////////////////////
// Reconstruct the depth in world space coordinates from the hyperbolic depth				//
//------------------------------------------------------------------------------------------//
// LinearDepth = (fZNear * fZFar / (fZNear - fZFar)) / (Depth - (fZFar / (fZFar - fZNear)))	//
// f2LinearDepthEquation.x = fZNear * fZFar / (fZNear - fZFar)								//
// f2LinearDepthEquation.y = fZFar / (fZFar - fZNear)										//
//////////////////////////////////////////////////////////////////////////////////////////////
//static const float fZNear = 1.f;
//static const float fZFar = 5000.f;
//static const float2 f2LinearDepthEquation = float2(fZNear * fZFar / (fZNear - fZFar), fZFar / (fZFar - fZNear));
const float fZNear;
const float fZFar;
const float2 f2LinearDepthEquation;
float ReconstructDepth(float fHyperbolicDepth)
{
	return f2LinearDepthEquation.x / (fHyperbolicDepth - f2LinearDepthEquation.y);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Kawase blur, approximating a 35x35 Gaussian kernel																	//
// in the nKernel = [0-1-2-2-3] format over 5 passes																	//
// https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float4 KawaseBlur(const sampler2D texSource, const float2 f2TexelSize, const float2 f2TexCoord, const int nKernel)
{
	float4 f4Color = float4(0.f, 0.f, 0.f, 0.f);

	UNROLL for (int i = -1; i <= 1; i += 2)
	{
		UNROLL for (int j = -1; j <= 1; j += 2)
		{
			const float2 f2TexelOffset = f2TexelSize * float2(i, j);
			const float2 f2HalfTexelOffset = 0.5f * f2TexelOffset;
			const float2 f2HalfTexelSize = 0.5f * f2TexelSize;
			const float2 f2SampleCenter = f2TexCoord + f2HalfTexelOffset + f2TexelOffset * nKernel;

			UNROLL for (int x = -1; x <= 1; x += 2)
				UNROLL for (int y = -1; y <= 1; y += 2)
					f4Color += tex2D(texSource, f2SampleCenter + f2HalfTexelSize * float2(x, y));
		}
	}

	return f4Color * 0.0625f;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////
// Downsampling helpers	//
//////////////////////////
float4 Downsample4x4(sampler2D tex, float2 texCoord, float2 texelSize)
{
	float4 color = float4(0.f, 0.f, 0.f, 0.f);

	UNROLL for (float i = -1.5f; i <= 1.5f; i += 1.f)
		UNROLL for (float j = -1.5f; j <= 1.5f; j += 1.f)
		color += tex2D(tex, texCoord + texelSize * float2(i, j));

	return color * 0.0625f;
}

float4 Downsample2x2(sampler2D tex, float2 texCoord, float2 texelSize)
{
	float4 color = float4(0.f, 0.f, 0.f, 0.f);

	UNROLL for (float i = -0.5f; i <= 0.5f; i += 1.f)
		UNROLL for (float j = -0.5f; j <= 0.5f; j += 1.f)
			color += tex2D(tex, texCoord + texelSize * float2(i, j));

	return color * 0.25f;
}
////////////////////////////////////////////////////////////////



//////////////////////////////////////////
// Luminance vectors					//
// http://stackoverflow.com/a/24213274	//
//////////////////////////////////////////
#define ITUR_LUMA_VEC (float3(0.2125f, 0.7154f, 0.0721f))
#define CCIR601_LUMA_VEC (float3(0.299f, 0.587f, 0.114f))
#define LUMINANCE_VECTOR CCIR601_LUMA_VEC
////////////////////////////////////////////////////////////////
