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

float PCF3x3(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float percentLit = 0.f;
	
	for(int x = -1; x <= 1; x++)
		for(int y = -1; y <= 1; y++)
		{
			bool isLit =
				tex2D(
					shadowMap,
					texCoord +
					float2(x * oneOverShadowMapSize.x, y * oneOverShadowMapSize.y)
				).r > depthCompare;
			percentLit += isLit;
		}

	return percentLit * 0.111111f;
}

float PCF4x4Poisson(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float2 poissonDisk[4] =
	{
		float2( -0.94201624, -0.39906216 ),
		float2( 0.94558609, -0.76890725 ),
		float2( -0.094184101, -0.92938870 ),
		float2( 0.34495938, 0.29387760 )
	};
	float percentLit = 0.f;
	
	for(int i = 0; i < 4; i++)
	{
		bool isLit =
			tex2D(
				shadowMap,
				texCoord +
				poissonDisk[i] * oneOverShadowMapSize
			).r > depthCompare;
		percentLit += isLit * 0.2f;
	}

	return percentLit;
}

float PCF4x4Dithered(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	// use modulo to vary the sample pattern
	float2 offset = floor(texCoord.xy) % 2.0;

	float percentLit = 0.f;
	percentLit += tex2D(shadowMap, texCoord + (float2(-1.5, 1.5) + offset) * oneOverShadowMapSize).r > depthCompare;
	percentLit += tex2D(shadowMap, texCoord + (float2(0.5, 1.5) + offset) * oneOverShadowMapSize).r > depthCompare;
	percentLit += tex2D(shadowMap, texCoord + (float2(-1.5, -0.5) + offset) * oneOverShadowMapSize).r > depthCompare;
	percentLit += tex2D(shadowMap, texCoord + (float2(0.5, -0.5) + offset) * oneOverShadowMapSize).r > depthCompare;
	
	return percentLit * 0.25 ;
}

float PCF5x5Gaussian(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	const float GaussianKernel[5][5] =
	{
		0.00296901674395065, 0.013306209891014005, 0.02193823127971504, 0.013306209891014005, 0.00296901674395065, 
		0.013306209891014005, 0.05963429543618023, 0.09832033134884507, 0.05963429543618023, 0.013306209891014005, 
		0.02193823127971504, 0.09832033134884507, 0.16210282163712417, 0.09832033134884507, 0.02193823127971504, 
		0.013306209891014005, 0.05963429543618023, 0.09832033134884507, 0.05963429543618023, 0.013306209891014005, 
		0.00296901674395065, 0.013306209891014005, 0.02193823127971504, 0.013306209891014005, 0.00296901674395065
	};
	float percentLit = 0.f;
	
	for(int x = -2; x <= 2; x++)
		for(int y = -2; y <= 2; y++)
		{
			bool isLit =
				tex2D(
					shadowMap,
					texCoord +
					float2(x * oneOverShadowMapSize.x, y * oneOverShadowMapSize.y)
				).r > depthCompare;
			percentLit += isLit * GaussianKernel[x+2][y+2];
		}

	return percentLit;
}

float PCF8x8(sampler2D shadowMap, float2 oneOverShadowMapSize, float2 texCoord, float depthCompare)
{
	float x, y, percentLit = 0.f;
	for (y = -3.5 ; y <=3.5 ; y+=1.0)
		for (x = -3.5 ; x <=3.5 ; x+=1.0)
			percentLit += tex2D(shadowMap, texCoord + float2(x, y) * oneOverShadowMapSize).r > depthCompare;
	
	return percentLit / 64.0 ;
}
////////////////////////////////////////////////////////////////