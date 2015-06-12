//////////////////////////////////////////////////////////////
// The ratio of a circle's circumference to its diameter	//
//////////////////////////////////////////////////////////////
#define PI		3.14159265f



//////////////////////////////////////////////////////////////
// Enable the unrolling of for loops, effectively trading	//
// off compile speed and shader size for runtime speed.		//
//////////////////////////////////////////////////////////////
#define ENABLE_UNROLLING (1)

#if ENABLE_UNROLLING
	#define UNROLL [unroll]
#else
	#define UNROLL
#endif



//////////////////////////////////////////////////////////////////
// Normal encoding / decoding using Stereographic Projection	//
// http://en.wikipedia.org/wiki/Stereographic_projection		//
//////////////////////////////////////////////////////////////////

float4 EncodeNormal(float3 n)
{
	n = normalize(n);
	const float scale = 1.7777f;
	float2 enc = n.xy / (n.z + 1.f);
	enc /= scale;
	enc = enc * 0.5f + 0.5f;
	return float4(enc, 0.f, 0.f);
}

float3 DecodeNormal(float4 enc)
{
	const float scale = 1.7777f;
	const float3 nn = enc.xyz * float3(2.f * scale, 2.f * scale, 0.f) + float3(-scale, -scale, 1.f);
	const float g = 2.f / dot(nn.xyz, nn.xyz);
	const float3 n = float3(g * nn.xy, g - 1.f);
	return normalize(n);
}
////////////////////////////////////////////////////////////////



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

	return f4Color / 16.f;
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



//////////////////////////////////////////////////////
// Random Number Generator							//
// Source: http://stackoverflow.com/a/10625698		//
// Input: texture coords as the random number seed.	//
// Output: Random number: [0,1)						//
//////////////////////////////////////////////////////
float GenerateRandomNumber(float2 p)
{
	const float2 r = float2(
		23.1406926327792690,
		2.6651441426902251);
	return frac(cos(fmod(123456789., 1e-7 + 256. * dot(p, r))));
}
//////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////
// Percentage Closer Filtering variations						//
// http://http.developer.nvidia.com/GPUGems/gpugems_ch11.html	//
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Poisson Disk kernel																										//
// https://electronicmeteor.wordpress.com/2013/02/05/poisson-disc-shadow-sampling-ridiculously-easy-and-good-looking-too/	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float2 poissonDisk[16];

float PCF2x2Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	UNROLL for (int i = 0; i < 4; i++)
	{
		const bool isLit =
			tex2D(
			shadowMap,
			texCoord +
			poissonDisk[i] * oneOverShadowMapSize
			).r > depthCompare;
		percentLit += isLit;
	}

	return percentLit * 0.25f;
}

float PCF3x3Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	UNROLL for (int i = 0; i < 9; i++)
	{
		const bool isLit =
			tex2D(
			shadowMap,
			texCoord +
			poissonDisk[i] * oneOverShadowMapSize
			).r > depthCompare;
		percentLit += isLit;
	}

	return percentLit * 0.1111111f;
}

float PCF12TapPoisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	UNROLL for (int i = 0; i < 12; i++)
	{
		const bool isLit =
			tex2D(
			shadowMap,
			texCoord +
			poissonDisk[i] * oneOverShadowMapSize
			).r > depthCompare;
		percentLit += isLit;
	}

	return percentLit * 0.0833333f;
}

float PCF4x4Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	UNROLL for (int i = 0; i < 16; i++)
	{
		const bool isLit =
			tex2D(
			shadowMap,
			texCoord +
			poissonDisk[i] * oneOverShadowMapSize
			).r > depthCompare;
		percentLit += isLit;
	}

	return percentLit * 0.0625f;
}

// Wow, this looks pretty good
float PCF4x4PoissonRotatedx4(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	[loop] for (int i = 0; i < 16; i++)
	{
		int isLit =
			tex2D(
				shadowMap,
				texCoord +
				poissonDisk[i].xy * oneOverShadowMapSize
				).r > depthCompare;
		isLit +=
			tex2D(
				shadowMap,
				texCoord +
				float2(1.f - poissonDisk[i].y, poissonDisk[i].x) * oneOverShadowMapSize
				).r > depthCompare;
		isLit +=
			tex2D(
				shadowMap,
				texCoord +
				(float2(1.f, 1.f) - poissonDisk[i].xy) * oneOverShadowMapSize
				).r > depthCompare;
		isLit +=
			tex2D(
				shadowMap,
				texCoord +
				float2(poissonDisk[i].y, 1.f - poissonDisk[i].x) * oneOverShadowMapSize
				).r > depthCompare;
		percentLit += isLit;
	}
	
	return percentLit * 0.015625f;
}

//////////////////////////
// Dithered patterns	//
//////////////////////////
float PCF3x3Dithered(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	// use modulo to vary the sample pattern
	float2 offset = floor(texCoord.xy) % 3.0;
	
	float percentLit = 0.f;
	UNROLL for(int x = -1; x <= 1; x++)
		UNROLL for(int y = -1; y <= 1; y++)
		{
			const bool isLit =
				tex2D(
					shadowMap,
					texCoord +
					float2(x, y) * oneOverShadowMapSize +
					offset
				).r > depthCompare;
			percentLit += isLit;
		}

	return percentLit * 0.111111f;
}

float PCF5x5Dithered(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	// use modulo to vary the sample pattern
	float2 offset = floor(texCoord.xy) % 5.0;
	
	float percentLit = 0.f;
	UNROLL for(int x = -2; x <= 2; x++)
		UNROLL for(int y = -2; y <= 2; y++)
		{
			const bool isLit =
				tex2D(
					shadowMap,
					texCoord +
					float2(x, y) * oneOverShadowMapSize +
					offset
				).r > depthCompare;
			percentLit += isLit;
		}

	return percentLit * 0.04f;
}

float PCF8x8Dithered(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	// use modulo to vary the sample pattern
	float2 offset = floor(texCoord.xy) % 8.0;

	float x, y, percentLit = 0.f;
	UNROLL for (y = -3.5f; y <= 3.5f; y += 1.0f)
		UNROLL for (x = -3.5f; x <= 3.5f; x += 1.0f)
			percentLit += tex2D(shadowMap, texCoord + float2(x, y) * oneOverShadowMapSize + offset).r > depthCompare;

	return percentLit * 0.015625f;
}

//////////////////////
// Gaussian kernel	//
//////////////////////
float PCF5x5Gaussian(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	const float GaussianKernel[5][5] =
	{
		0.00296901674395065f, 0.013306209891014005f, 0.02193823127971504f, 0.013306209891014005f, 0.00296901674395065f, 
		0.013306209891014005f, 0.05963429543618023f, 0.09832033134884507f, 0.05963429543618023f, 0.013306209891014005f,
		0.02193823127971504f, 0.09832033134884507f, 0.16210282163712417f, 0.09832033134884507f, 0.02193823127971504f,
		0.013306209891014005f, 0.05963429543618023f, 0.09832033134884507f, 0.05963429543618023f, 0.013306209891014005f,
		0.00296901674395065f, 0.013306209891014005f, 0.02193823127971504f, 0.013306209891014005f, 0.00296901674395065f
	};
	float percentLit = 0.f;
	
	UNROLL for(int x = -2; x <= 2; x++)
		UNROLL for(int y = -2; y <= 2; y++)
		{
			const bool isLit =
				tex2D(
					shadowMap,
					texCoord +
					float2(x, y) * oneOverShadowMapSize
				).r > depthCompare;
			percentLit += isLit * GaussianKernel[x+2][y+2];
		}

	return percentLit;
}

//////////////////
// Regular PCF	//
//////////////////
float PCF3x3(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	UNROLL for (int x = -1; x <= 1; x++)
		UNROLL for (int y = -1; y <= 1; y++)
		{
			const bool isLit =
				tex2D(
				shadowMap,
				texCoord +
				float2(x, y) * oneOverShadowMapSize
				).r > depthCompare;
			percentLit += isLit;
		}

	return percentLit * 0.111111f;
}

float PCF8x8(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;
	UNROLL for (float y = -3.5f ; y <=3.5f ; y += 1.0f)
		UNROLL for (float x = -3.5f ; x <=3.5f ; x += 1.0f)
			percentLit += tex2D(shadowMap, texCoord + float2(x, y) * oneOverShadowMapSize).r > depthCompare;
	
	return percentLit * 0.015625f;
}
////////////////////////////////////////////////////////////////
