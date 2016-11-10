/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	BokehDoF.hlsl
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "PostProcessingUtils.hlsli"
#include "Utils.hlsli"

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

const float4 f4TexSize;	// xy: size, in texels, of source image; zw: normalized size of a texel
//------------------------------------------

//------------------------------------------
/*
	Depth of Field with Bokeh shader

	Based on ideas gathered from several sources including GTA V, Prototype 2, Skylanders, etc.
	http://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study-part-3/
	https://mynameismjp.wordpress.com/2011/02/28/bokeh/
	http://casual-effects.blogspot.ro/2013/09/the-skylanders-swap-force-depth-of.html
*/
//------------------------------------------

// Camera properties
#define APERTURE_BLADE_COUNT (6)
const float	fFocalDepth;	// Focal distance value in meters (overridden by 'bAutofocus')
const float	fFocalLength;	// Focal length in mm
const float	fFStop;			// F-stop value
const float	fCoC;			// Circle of confusion size in mm (35mm film = 0.03mm)
const float	fApertureSize;	// Determines size of bokeh
const bool	bAutofocus;		// Overrides fFocalDepth value with value taken from depth buffer

// Brightness filter (directly affects bokeh abundance and prominence)
const float	fHighlightThreshold;	// Brightness-pass filter threshold (higher = sparser bokeh)
const float	fHighlightGain;			// Brightness gain (higher = more prominent bokeh)

// Vignetting effect
const bool	bVignetting;	//	Use optical lens vignetting
const float	fVignOut;		//	Vignetting outer border
const float	fVignIn;		//	Vignetting inner border
const float	fVignFade;		//	F-stops until vignette fades

// Chromatic aberration
const float fChromaShiftAmount;	// Color separation factor

// Lens distortion
const float fQuarticDistortionCoef;
const float fCubicDistortionModifier;
const float fDistortionScale;

void ApplyLensDistortion(in out float2 f2TexCoord)
{
	if (fQuarticDistortionCoef != 0 || fCubicDistortionModifier != 0 || fDistortionScale != 0)
	{
		const float fAspectRatioScale = f4TexSize.y * f4TexSize.z; // y / (1 / x)
		f2TexCoord -= 0.5f;
		const float r2 = fAspectRatioScale * fAspectRatioScale * f2TexCoord.x * f2TexCoord.x + f2TexCoord.y * f2TexCoord.y;
		const float f = 1.f + r2 * (fQuarticDistortionCoef + fCubicDistortionModifier * sqrt(r2));
		f2TexCoord *= f * fDistortionScale;
		f2TexCoord += 0.5f;
	}
}

void psmain(VSOut input, out float4 f4Color : SV_TARGET)
{
	// Offsets for aperture blade shaped blur kernel
	const float2 f2KernelOffset[APERTURE_BLADE_COUNT] =
	{
		sin(0.f / APERTURE_BLADE_COUNT * 6.283f), cos(0.f / APERTURE_BLADE_COUNT * 6.283f),
		sin(1.f / APERTURE_BLADE_COUNT * 6.283f), cos(1.f / APERTURE_BLADE_COUNT * 6.283f),
		sin(2.f / APERTURE_BLADE_COUNT * 6.283f), cos(2.f / APERTURE_BLADE_COUNT * 6.283f),
		sin(3.f / APERTURE_BLADE_COUNT * 6.283f), cos(3.f / APERTURE_BLADE_COUNT * 6.283f),
		sin(4.f / APERTURE_BLADE_COUNT * 6.283f), cos(4.f / APERTURE_BLADE_COUNT * 6.283f),
		sin(5.f / APERTURE_BLADE_COUNT * 6.283f), cos(5.f / APERTURE_BLADE_COUNT * 6.283f),
		//sin(6.f / APERTURE_BLADE_COUNT * 6.283f), cos(6.f / APERTURE_BLADE_COUNT * 6.283f),
		//sin(7.f / APERTURE_BLADE_COUNT * 6.283f), cos(7.f / APERTURE_BLADE_COUNT * 6.283f)
	};

	// Apply lens distortion equation to texture coordinates
	ApplyLensDistortion(input.f2TexCoord);

	// Initial sample
	const float4	f4CenterSample	= tex2D(texSource, input.f2TexCoord);
	float			fDofBlurFactor = f4CenterSample.a * 2.f - 1.f;
	float3			f3AccumSamples	= f4CenterSample.rgb;
	float			fWeightTotal	= 1.f;

	// Accumulate the rest of the samples, if required. Otherwise, just calculate blur factor and output it in alpha.
	if (fApertureSize > 0.f)
	{
		// Fix aspect ratio of blur kernel
		const float fAspectRatioScale = f4TexSize.y * f4TexSize.z;

		for (int i = 0; i < APERTURE_BLADE_COUNT; i++)
		{
			// Retrieve sample color and CoC value
			const float4 f4Sample = tex2D(texSource, input.f2TexCoord + f2KernelOffset[i] * fApertureSize * fDofBlurFactor * float2(fAspectRatioScale, 1.f));
			const float fSampleDofBlurFactor = f4Sample.a * 2.f - 1.f;

			// Calculate sample weight
			const float fLuma	= dot(f4Sample.rgb, LUMINANCE_VECTOR);
			const float fWeight =
				max((fLuma > fHighlightThreshold) * fHighlightGain * abs(fDofBlurFactor), 1.f) *	// Luma threshold for applying highlight gain (more prominent bokeh)
				saturate(fSampleDofBlurFactor * fDofBlurFactor);			// Weight based on CoC values of center and sampled points,
																			// saturated in order to avoid blending front and back out-of-focus areas

			// Accumulate current weighted sample
			f3AccumSamples	+= f4Sample.rgb * fWeight;
			fWeightTotal	+= fWeight;
		}
	}
	else
	{
		// Calculate the scene depth in world space coordinates
		const float fLinearDepth = tex2D(texDepthBuffer, input.f2TexCoord).r;

		// Calculate focal plane
		const float fFocalPoint = bAutofocus ? ReconstructDepth(tex2D(texTargetFocus, float2(0.5f, 0.5f)).r) : fFocalDepth;

		// Calculate DoF blur factor
		const float fFocalPointMm	= fFocalPoint	*	1000.f;	// Focal point in mm
		const float fLinearDepthMm	= fLinearDepth	*	1000.f;	// Linear depth in mm
		const float a	=	(fLinearDepthMm	*	fFocalLength)	/	(fLinearDepthMm	-	fFocalLength);
		const float b	=	(fFocalPointMm	*	fFocalLength)	/	(fFocalPointMm	-	fFocalLength);
		const float c	=	(fFocalPointMm	-	fFocalLength)	/	(fFocalPointMm	*	fFStop	*	fCoC);
		fDofBlurFactor	=	saturate(abs(a - b) * c) * sign(fLinearDepth - fFocalPoint);

		// Apply chromatic aberration effect
		if (fChromaShiftAmount > 0.f)
		{
			// The effect is more intense towards the exterior of the screen
			// NB: The distance of the screen coordinates from the screen center is normalized
			// by multiplying to the inverse of the length of the screen diagonal in normalized
			// screen space units: 
			// distance * (1.f / (0.5f * sqrt(2.f))) = distance * (2.f / sqrt(2.f)) = distance * sqrt(2.f) = distance * 1.414213562f
			const float fChromaShiftFactor = smoothstep(0.f, 1.f, distance(input.f2TexCoord, float2(0.5f, 0.5f)) * 1.414213562f);

			// Apply chromatic aberration effect
			f3AccumSamples.r = tex2D(texSource, input.f2TexCoord + fChromaShiftFactor * float2(0.f, 1.f)		* f4TexSize.zw * fChromaShiftAmount * fDofBlurFactor).r;
			f3AccumSamples.g = tex2D(texSource, input.f2TexCoord + fChromaShiftFactor * float2(-0.866f, -0.5f)	* f4TexSize.zw * fChromaShiftAmount * fDofBlurFactor).g;
			f3AccumSamples.b = tex2D(texSource, input.f2TexCoord + fChromaShiftFactor * float2(0.866f, -0.5f)	* f4TexSize.zw * fChromaShiftAmount * fDofBlurFactor).b;
		}

		// Apply vignetting
		if (bVignetting)
		{
			const float fFadeFactor = fFStop / fVignFade;
			f3AccumSamples *= smoothstep(fVignOut + fFadeFactor, fVignIn + fFadeFactor, distance(input.f2TexCoord, float2(0.5f, 0.5f)));
		}
	}

	// Output color
	f4Color = float4(f3AccumSamples / fWeightTotal, fDofBlurFactor * 0.5f + 0.5f);
}
////////////////////////////////////////////////////////////////////
