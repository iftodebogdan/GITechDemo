#include "PostProcessingUtils.hlsl"
#include "Utils.hlsl"

// Disable "warning X4122: sum of ... and ... cannot be represented accurately in double precision"
#pragma warning (disable: 4122)

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
const sampler2D texSource;		// Source texture to be blurred
const sampler2D texDepthBuffer;	// Source depth buffer
const sampler2D texTargetFocus;	// Used for autofocus

const float2 f2TexSize;		// Size, in texels, of source image
const float2 f2TexelSize;	// Size of a single texel of source image
//------------------------------------------

//------------------------------------------
/*
	Depth of Field with Bokeh shader

	Based on the work of Martins Upitis
	http://devlog-martinsh.blogspot.com/
*/
//------------------------------------------

//------------------------------------------
/*
	DoF quality

	'nRingCount'
		Effectively determines the blur kernel size, with every concentric ring becoming larger and larger.

	'nSampleCount'
		The number of samples taken on the smallest ring, with every larger ring taking 'nSampleCount' more samples than the previous.

	The total number of samples, excluding the ones for other effects
	(e.g. chromatic aberration, depth blur, etc.) can be calculated as follows:

		TotalNumSamples = nSampleCount * nRingCount * (nRingCount + 1) / 2

	Observation:
		As can be seen from the formula above, the total number of samples exhibits linear growth with 'nSampleCount'
		and exponential growth with 'nRingCount'. In other words, increasing 'nRingCount' will have a drastic effect
		on the performance of the shader. For visualization, see example below:
	
	Examples:
		 3 rings with  3 samples -  18 texture accesses
		 5 rings with  5 samples -  75 texture accesses
		10 rings with  5 samples - 275 texture accesses
		10 rings with 10 samples - 550 texture accesses
		15 rings with  5 samples - 600 texture accesses

	NB: These values are hardcoded inside the shader so that they are known at compile time, for loop unrolling
*/
#define	nRingCount		(9)
#define	nSampleCount	(3)

// Set up loop unrolling such that we don't exceed the
// maximum number of constant registers (224). This
// behaviour can be disabled in order to speed up shader
// loading times, but at the price of reduced performance.
#define DOF_LOOP_UNROLLING (1)

#if DOF_LOOP_UNROLLING
	#if ((nSampleCount * nRingCount * (nRingCount + 1) / 2) <= 112)
	#define DOF_UNROLL_LV1 [unroll]
	#define DOF_UNROLL_LV2 [unroll]
	#elif (nRingCount < 16)
	#define DOF_UNROLL_LV1 [unroll]
	#define DOF_UNROLL_LV2
	#else
	#define DOF_UNROLL_LV1
	#define DOF_UNROLL_LV2
	#endif
#else
	#define DOF_UNROLL_LV1
	#define DOF_UNROLL_LV2
#endif
//------------------------------------------

//------------------------------------------
/*
	DoF tuning
*/
//------------------------------------------
// Version 1: Variables for a physically based depth of field effect
const float	fFocalDepth;	// Focal distance value in meters (overridden by 'bAutofocus')
const float	fFocalLength;	// Focal length in mm
const float	fFStop;			// F-stop value
const float	fCoC;			// Circle of confusion size in mm (35mm film = 0.03mm)

// Version 2: Variables for an artist friendly depth of field effect
const float	fNearDofStart;		// Near plane DoF blur start
const float	fNearDofFalloff;	// Near plane DoF blur falloff distance
const float	fFarDofStart;		// Far plane DoF blur start
const float	fFarDofFalloff;		// Far plane DoF blur falloff distance

// Version switch
const bool	bManualDof;		// true - artist friendly verison, false - physically based version
//------------------------------------------

//------------------------------------------
// The following variables are common for both the artist friendly and physically based versions

// General options
const bool		bDebugFocus;	// Show debug auto focus point and focal range
const bool		bAutofocus;		// Use auto focus? (overrides 'fFocalDepth')
const float		fMaxBlur;		// Blurriness factor (0.f = no blur, 1.f default)

// Brightness filter (directly affects bokeh abundance and prominence)
const float	fHighlightThreshold;	// Brightness-pass filter threshold (higher = sparser bokeh)
const float	fHighlightGain;			// Brightness gain (higher = more prominent bokeh)

// Bokeh appearance
const float	fBokehBias;		// Bokeh edge bias (shift weights towards edges)
const float	fBokehFringe;	// Bokeh chromatic aberration (any value except 0 will effectively triple the number of texture accesses)

// Bokeh shape
const bool	bPentagonBokeh;		// Use pentagon as bokeh shape?
const float	fPentagonFeather;	// Pentagon edge feathering

// Sampling pattern
const bool	bUseNoise;		// Use noise instead of pattern for sample dithering
const float	fNoiseAmount;	// Dither amount

// Blur depth (for softer edges between in and out of focus objects)
const bool	bBlurDepth;		// Blur the depth buffer?
const float	fDepthBlurSize;	// Depth blur kernel size

// Vignetting effect
const bool	bVignetting;	//	Use optical lens vignetting?
const float	fVignOut;		//	Vignetting outer border
const float	fVignIn;		//	Vignetting inner border
const float	fVignFade;		//	F-stops until vignette fades
//------------------------------------------

//------------------------------------------
/*
	Forward function declarations
*/
//------------------------------------------
float GeneratePentagon(float2 f2Coords);
float BlurDepth(float2 f2Coords);
float3 CalculateColor(float2 f2Coords, float fDofBlurFactor);
float2 GenerateNoise(float2 f2Coords);
float3 DebugFocus(float3 f3Color, float fDofBlurFactor, float fLinearDepth);
float CalculateVignetting(float2 f2Coords);
//------------------------------------------

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	// Calculate the scene depth in world space coordinates
	float fLinearDepth;

	if (bBlurDepth)
		fLinearDepth = ReconstructDepth(BlurDepth(input.f2TexCoord));
	else
		fLinearDepth = ReconstructDepth(tex2D(texDepthBuffer, input.f2TexCoord).r);

	// Calculate focal plane
	float fFocalPoint;

	if (bAutofocus)
		fFocalPoint = ReconstructDepth(tex2D(texTargetFocus, float2(0.5f, 0.5f)).r);
	else
		fFocalPoint = fFocalDepth;

	// Calculate DoF blur factor
	float fDofBlurFactor = 0.f;

	if (bManualDof)
	{
		const float fFocalPlane	= fLinearDepth - fFocalPoint;
		const float fFarPlane	= (fFocalPlane - fFarDofStart) * rcp(fFarDofFalloff);
		const float fNearPlane	= (-fFocalPlane - fNearDofStart) * rcp(fNearDofFalloff);
		//fDofBlurFactor		= (fFocalPlane > 0.f) ? fFarPlane : fNearPlane;
		fDofBlurFactor			= lerp(fNearPlane, fFarPlane, fFocalPlane > 0.f);	// Avoid dynamic branching
	}
	else
	{
		const float fFocalPointMm	= fFocalPoint	*	1000.f;	// Focal point in mm
		const float fLinearDepthMm	= fLinearDepth	*	1000.f;	// Linear depth in mm

		const float a	= (fLinearDepthMm	*	fFocalLength)	/	(fLinearDepthMm	-	fFocalLength);
		const float b	= (fFocalPointMm	*	fFocalLength)	/	(fFocalPointMm	-	fFocalLength);
		const float c	= (fFocalPointMm	-	fFocalLength)	/	(fFocalPointMm	*	fFStop	*	fCoC);

		fDofBlurFactor	= abs(a - b) * c;
	}

	fDofBlurFactor = saturate(fDofBlurFactor);

	// Calculate dithering pattern
	const float2 f2Noise = GenerateNoise(input.f2TexCoord) * fNoiseAmount * fDofBlurFactor;

	// Getting blur X and Y step factor
	const float2 f2BlurStepFactor = f2TexelSize * fDofBlurFactor * fMaxBlur + f2Noise;

	// Begin calculating final color
	float3 f3Color = tex2D(texSource, input.f2TexCoord).rgb;

	//----------------------------------------------------------------------//
	// Dynamic branching versus some extra expensive ALU ops (sin/cos)...	//
	//----------------------------------------------------------------------//
	// Since fDofBlurFactor >= 0.05f is coherent across the screen			//
	// (i.e. the focal plane has a coherent coverage), dynamic branching	//
	// isn't such a big deal when compared to a bunch of sin() and cos().	//
	// NB: I have tested it both ways and the dynamic branching version		//
	// is faster than extra ALU by about 13% on AMD Mobility Radeon HD 5650	//
	//----------------------------------------------------------------------//
	if (fDofBlurFactor >= 0.05f)
	{
		float fSampleDiv = 1.f;

		DOF_UNROLL_LV1 for (int i = 1; i <= nRingCount; i++)
		{
			const int nRingSampleCount = i * nSampleCount;

			DOF_UNROLL_LV2 for (int j = 0; j < nRingSampleCount; j++)
			{
				// Distribute the samples across the ring's edge evenly
				const float		fStep			=	PI * 2.f * rcp(nRingSampleCount);
				const float		fAngle			=	j * fStep;
				const float2	f2RingPattern	=	float2(cos(fAngle), sin(fAngle)) * i;

				// Shift sampling weights toward bokeh edge according to the value of 'fBokehBias'
				float3	f3ColorAdd		=	CalculateColor(input.f2TexCoord + f2RingPattern * f2BlurStepFactor, fDofBlurFactor) * lerp(1.f, i * rcp(nRingCount), fBokehBias);
				float	fSampleDivAdd	=	lerp(1.f, i * rcp(nRingCount), fBokehBias);

				// Optionally, use pentagon shape for bokeh
				if (bPentagonBokeh)
				{
					const float fPentagonPattern = GeneratePentagon(f2RingPattern);
					f3ColorAdd *= fPentagonPattern;
					fSampleDivAdd *= fPentagonPattern;
				}

				f3Color		+=	f3ColorAdd;
				fSampleDiv	+=	fSampleDivAdd;
			}
		}

		f3Color *= rcp(fSampleDiv);
	}

	// Apply vignetting
	if (bVignetting)
		f3Color *= CalculateVignetting(input.f2TexCoord);

	// Debug option for visualizing the near, far and focal planes
	if (bDebugFocus)
		f3Color = DebugFocus(f3Color, fDofBlurFactor, fLinearDepth);
	
	f4Color = float4(f3Color, fDofBlurFactor);
}
////////////////////////////////////////////////////////////////////

// Generate pentagon pattern
float GeneratePentagon(float2 f2Coords)
{
	const float fScale = nRingCount - 1.3f;

	const float4 f4HS0 = float4( 1.f,			 0.f,			0.f,	1.f);
	const float4 f4HS1 = float4( 0.309016994f,	 0.951056516f,	0.f,	1.f);
	const float4 f4HS2 = float4(-0.809016994f,	 0.587785252f,	0.f,	1.f);
	const float4 f4HS3 = float4(-0.809016994f,	-0.587785252f,	0.f,	1.f);
	const float4 f4HS4 = float4( 0.309016994f,	-0.951056516f,	0.f,	1.f);
	const float4 f4HS5 = float4( 0.f,			 0.f,			1.f,	1.f);

	const float4 f4One = float4(1.f, 1.f, 1.f, 1.f);

	const float4 f4P = float4(f2Coords.xy, fScale.xx);

	float4 f4Dist = float4(0.f, 0.f, 0.f, 0.f);
	float fInOrOut = -4.f;

	f4Dist.x = dot(f4P, f4HS0);
	f4Dist.y = dot(f4P, f4HS1);
	f4Dist.z = dot(f4P, f4HS2);
	f4Dist.w = dot(f4P, f4HS3);

	f4Dist = smoothstep(-fPentagonFeather, fPentagonFeather, f4Dist);

	fInOrOut += dot(f4Dist, f4One);

	f4Dist.x = dot(f4P, f4HS4);
	f4Dist.y = f4HS5.w - abs(f4P.z);

	f4Dist = smoothstep(-fPentagonFeather, fPentagonFeather, f4Dist);
	fInOrOut += f4Dist.x;

	return saturate(fInOrOut);
}

// Blur the depth buffer
float BlurDepth(float2 f2Coords)
{
	//------------------------------------------
	/*
	Hardcoded blur kernel and weights for depth blurring
	*/
	//------------------------------------------
	#define DEPTH_BLUR_SAMPLE_COUNT (9)
	const float fKernelWeight[DEPTH_BLUR_SAMPLE_COUNT] =
	{
		1.f / 16.f, 2.f / 16.f, 1.f / 16.f,
		2.f / 16.f, 4.f / 16.f, 2.f / 16.f,
		1.f / 16.f, 2.f / 16.f, 1.f / 16.f
	};
	const float2 f2KernelOffset[DEPTH_BLUR_SAMPLE_COUNT] =
	{
		float2(-1.f, -1.f),
		float2(0.f, -1.f),
		float2(1.f, -1.f),

		float2(-1.f,  0.f),
		float2(0.f,  0.f),
		float2(1.f,  0.f),

		float2(-1.f,  1.f),
		float2(0.f,  1.f),
		float2(1.f,  1.f)
	};
	//------------------------------------------

	// We don't want f2KernelSize to be resolution dependant, since the soft halos around
	// overlapping in and out of focus objects would be bigger as the resolution got lower.
	// Just maintain a proper aspect ratio so that the kernel is always square.
	//float2 f2KernelSize = f2TexelSize * fDepthBlurSize;
	const float2 f2KernelSize = float2(fDepthBlurSize, f2TexelSize.y * fDepthBlurSize * rcp(f2TexelSize.x));

	float fDepth = 0.f;
	UNROLL for (int i = 0; i < DEPTH_BLUR_SAMPLE_COUNT; i++)
		fDepth += tex2D(texDepthBuffer, f2Coords + f2KernelOffset[i] * f2KernelSize).r * fKernelWeight[i];

	return fDepth;
}

// Calculate the color of the sample
float3 CalculateColor(float2 f2Coords, float fDofBlurFactor)
{
	float3 f3Color;

	if (fBokehFringe)
	{
		// The effect is more intense towards the exterior of the screen
		// NB: The distance of the screen coordinates from the screen center is normalized
		// by multiplying to the inverse of the length of the screen diagonal in normalized
		// screen space units: 
		// distance * (1.f / (0.5f * sqrt(2.f))) = distance * (2.f / sqrt(2.f)) = distance * sqrt(2.f) = distance * 1.414213562f
		const float fFringeFactor = smoothstep(0.f, 1.f, distance(f2Coords, float2(0.5f, 0.5f)) * 1.414213562f);

		// Apply chromatic aberration effect
		f3Color.r = tex2D(texSource, f2Coords + fFringeFactor * float2(0.f, 1.f)		* f2TexelSize * fBokehFringe * fDofBlurFactor).r;
		f3Color.g = tex2D(texSource, f2Coords + fFringeFactor * float2(-0.866f, -0.5f)	* f2TexelSize * fBokehFringe * fDofBlurFactor).g;
		f3Color.b = tex2D(texSource, f2Coords + fFringeFactor * float2( 0.866f,	-0.5f)	* f2TexelSize * fBokehFringe * fDofBlurFactor).b;
	}
	else
		f3Color = tex2D(texSource, f2Coords).rgb;

	// Brightness-pass filter
	const float fLuma		= dot(saturate(f3Color.rgb), LUMINANCE_VECTOR);
	const float fThreshold	= max((fLuma - fHighlightThreshold) * fHighlightGain, 0.f);

	return f3Color * (fThreshold * fDofBlurFactor + 1.f);	// f3Color + lerp(0.f, f3Color, fThreshold * fDofBlurFactor)
}

// Generate noise for dithering
float2 GenerateNoise(float2 f2Coords)
{
	float2 f2Noise;

	if (bUseNoise)
	{
		f2Noise.x = saturate(frac(sin(dot(f2Coords, float2(12.9898f, 78.233f)))			* 43758.5453f)) * 2.f - 1.f;
		f2Noise.y = saturate(frac(sin(dot(f2Coords, float2(12.9898f, 78.233f) * 2.f))	* 43758.5453f)) * 2.f - 1.f;
	}
	else
	{
		f2Noise.x = ((frac(1.f - f2Coords.x * (f2TexSize.x * 0.5f)) * 0.25f) + (frac(f2Coords.y * (f2TexSize.y * 0.5f)) * 0.75f)) * 2.f - 1.f;
		f2Noise.y = ((frac(1.f - f2Coords.x * (f2TexSize.x * 0.5f)) * 0.75f) + (frac(f2Coords.y * (f2TexSize.y * 0.5f)) * 0.25f)) * 2.f - 1.f;
	}

	return f2Noise;
}

// Debugging utility for visualizing far, near and focus planes
float3 DebugFocus(float3 f3Color, float fDofBlurFactor, float fLinearDepth)
{
	const float fEdge	=	0.002f * fLinearDepth;
	const float fNear	=	saturate(smoothstep(0.f, fEdge, fDofBlurFactor));
	const float fFar	=	saturate(smoothstep(1.f - fEdge, 1.f, fDofBlurFactor));

	f3Color		=	lerp(f3Color, float3(1.f, 0.5f, 0.f), (1.f - fNear) * 0.6f);
	f3Color		=	lerp(f3Color, float3(0.f, 0.5f, 1.f), ((1.f - fFar) - (1.f - fNear)) * 0.2f);

	return f3Color;
}

// Calculate vignetting effect factor of a pixel
float CalculateVignetting(float2 f2Coords)
{
	const float fFadeFactor = fFStop * rcp(fVignFade);
	return smoothstep(fVignOut + fFadeFactor, fVignIn + fFadeFactor, distance(f2Coords, float2(0.5f, 0.5f)));
}
