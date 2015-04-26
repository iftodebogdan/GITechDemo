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

float PCF2x2Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float2 poissonDisk[4] =
	{
		float2(-0.94201624, -0.39906216),
		float2(0.94558609, -0.76890725),
		float2(-0.094184101, -0.92938870),
		float2(0.34495938, 0.29387760)
	};
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
	float2 poissonDisk[9] =
	{
		float2(0.4677864f, 1.0492188f),
		float2(0.8965628f, -0.3094058f),
		float2(-0.3828112f, 1.6226606f),
		float2(-0.214711f, -0.7530054f),
		float2(-0.6589904f, 0.4324332f),
		float2(1.480888f, 1.2347982f),
		float2(1.9457676f, -0.13916496f),
		float2(1.1015856f, -1.5681816f),
		float2(0.06198422f, -1.7864658f)
	};
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
	float2 poissonDisk[12] =
	{
		float2(-0.5946302f, 0.6008242f),
		float2(-1.762928f, 0.5199542f),
		float2(-1.56506f, -0.8692048f),
		float2(0.14760224f, 1.7710094f),
		float2(0.7065646f, 0.04265878f),
		float2(-0.5888886f, -0.429245f),
		float2(1.024129f, 1.1201256f),
		float2(-1.118836f, 1.6018892f),
		float2(-0.9557046f, -1.7215218f),
		float2(0.02812326f, -1.4003752f),
		float2(1.6575236f, -0.287287f),
		float2(1.3867802f, -1.375697f)
	};
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
	float2 poissonDisk[16] =
	{
		float2(2.688109f, 0.686785f),
		float2(3.0212565f, -1.1432755f),
		float2(1.9640215f, 1.5282315f),
		float2(3.293479f, 1.5044285f),
		float2(3.7991725f, 0.47122805f),
		float2(1.211579f, 0.587791f),
		float2(1.8459345f, -0.8353495f),
		float2(4.1637455f, -0.639698f),
		float2(2.3383775f, 2.6021615f),
		float2(4.58543f, 1.3013345f),
		float2(3.9300595f, 2.3949865f),
		float2(3.537996f, 3.44172f),
		float2(1.055484f, 2.1200395f),
		float2(1.9478455f, 3.7591355f),
		float2(1.0448675f, 3.2125375f),
		float2(-0.25313345f, 2.309396f)
	};
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