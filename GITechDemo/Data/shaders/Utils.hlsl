//////////////////////////////////////////////////////////////
// Normal encoding / decoding								//
// using Stereographic Projection							//
// http://en.wikipedia.org/wiki/Stereographic_projection	//
//////////////////////////////////////////////////////////////

float4 EncodeNormal(float3 n)
{
	n = normalize(n);
	float scale = 1.7777f;
	float2 enc = n.xy / (n.z + 1.f);
	enc /= scale;
	enc = enc * 0.5f + 0.5f;
	return float4(enc, 0.f, 0.f);
}

float3 DecodeNormal(float4 enc)
{
	float scale = 1.7777f;
	float3 nn = enc.xyz * float3(2.f * scale, 2.f * scale, 0.f) + float3(-scale, -scale, 1.f);
	float g = 2.f / dot(nn.xyz, nn.xyz);
	float3 n;
	n.xy = g * nn.xy;
	n.z = g - 1.f;
	return normalize(n);
}

//////////////////////////////////////////////
// Percentage Closer Filtering variations	//
//////////////////////////////////////////////

const float2 poissonDisk[16];

float PCF2x2Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	for (int i = 0; i < 4; i++)
	{
		bool isLit =
			tex2D(
			shadowMap,
			texCoord +
			poissonDisk[i] * oneOverShadowMapSize
			).r > depthCompare;
		percentLit += isLit * 0.25f;
	}

	return percentLit;
}

float PCF3x3Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	for (int i = 0; i < 9; i++)
	{
		bool isLit =
			tex2D(
			shadowMap,
			texCoord +
			poissonDisk[i] * oneOverShadowMapSize
			).r > depthCompare;
		percentLit += isLit * 0.1111111f;
	}

	return percentLit;
}

float PCF12TapPoisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	for (int i = 0; i < 12; i++)
	{
		bool isLit =
			tex2D(
			shadowMap,
			texCoord +
			poissonDisk[i] * oneOverShadowMapSize
			).r > depthCompare;
		percentLit += isLit * 0.0833333f;
	}

	return percentLit;
}

float PCF4x4Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	for (int i = 0; i < 16; i++)
	{
		bool isLit =
			tex2D(
			shadowMap,
			texCoord +
			poissonDisk[i] * oneOverShadowMapSize
			).r > depthCompare;
		percentLit += isLit * 0.0625f;
	}

	return percentLit;
}

float PCF3x3Dithered(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	// use modulo to vary the sample pattern
	float2 offset = floor(texCoord.xy) % 3.0;
	
	float percentLit = 0.f;
	for(int x = -1; x <= 1; x++)
		for(int y = -1; y <= 1; y++)
		{
			bool isLit =
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
	for(int x = -2; x <= 2; x++)
		for(int y = -2; y <= 2; y++)
		{
			bool isLit =
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
	for (y = -3.5f; y <= 3.5f; y += 1.0f)
		for (x = -3.5f; x <= 3.5f; x += 1.0f)
			percentLit += tex2D(shadowMap, texCoord + float2(x, y) * oneOverShadowMapSize + offset).r > depthCompare;

	return percentLit * 0.015625f;
}

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
	
	for(int x = -2; x <= 2; x++)
		for(int y = -2; y <= 2; y++)
		{
			bool isLit =
				tex2D(
					shadowMap,
					texCoord +
					float2(x, y) * oneOverShadowMapSize
				).r > depthCompare;
			percentLit += isLit * GaussianKernel[x+2][y+2];
		}

	return percentLit;
}

float PCF3x3(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;

	for (int x = -1; x <= 1; x++)
		for (int y = -1; y <= 1; y++)
		{
			bool isLit =
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
	for (float y = -3.5f ; y <=3.5f ; y += 1.0f)
		for (float x = -3.5f ; x <=3.5f ; x += 1.0f)
			percentLit += tex2D(shadowMap, texCoord + float2(x, y) * oneOverShadowMapSize).r > depthCompare;
	
	return percentLit * 0.015625f;
}
////////////////////////////////////////////////////////////////