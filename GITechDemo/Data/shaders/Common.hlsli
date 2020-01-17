/*=============================================================================
 * This file is part of the "GITechDemo" application
 * Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *      File:   Common.hlsli
 *      Author: Bogdan Iftode
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#ifndef COMMON_HLSLI_
#define COMMON_HLSLI_

#ifdef HLSL

//////////////////////////////////////////////////////////////
// Enable the unrolling of for loops, effectively trading   //
// off compile speed and shader size for runtime speed.     //
//////////////////////////////////////////////////////////////
#define ENABLE_UNROLLING (1)
#define ENABLE_LOOP (1)

#if ENABLE_UNROLLING
#define UNROLL [unroll]
#else
#define UNROLL
#endif

#if ENABLE_LOOP
#define LOOP [loop]
#else
#define LOOP
#endif



//////////////////////////////////////////////////////////////
// Small optimization in the form of an early depth check.  //
// This is useful if you have an expensive pixel shader     //
// which you would not want (fully) executed on pixels that //
// exceed a certain depth (or other) threshold.             //
//////////////////////////////////////////////////////////////
//#define DEPTH_KILL(depth, threshold) if (depth >= threshold) clip(-1)
//#define DEPTH_KILL(depth, threshold) clip(-(depth >= threshold))
#define DEPTH_KILL(depth, threshold) clip(-step(threshold, depth))



//////////////////////////////////////////////////////////
// Similar to the above early depth test, but tailored  //
// specifically for alpha testing. If the alpha is less //
// the threshold, then the pixel is clipped.            //
//////////////////////////////////////////////////////////
#define ALPHA_TEST(alpha, threshold) clip(-step(alpha, threshold))



//////////////////////////////////////////////////////////////////////////////////////////////
// These typedefs help with achieving the same memory layout between HLSL and CPP structs.  //
// They are not required if you don't use structs.                                          //
// More details in ShaderProgramDX9::GetConstantType() in Synesthesia3D project.            //
//////////////////////////////////////////////////////////////////////////////////////////////

typedef float  GPU_bool;
typedef float1 GPU_bool1;
typedef float2 GPU_bool2;
typedef float3 GPU_bool3;
typedef float4 GPU_bool4;

typedef int  GPU_int;
typedef int1 GPU_int1;
typedef int2 GPU_int2;
typedef int4 GPU_int4;
typedef int3 GPU_int3;

typedef uint  GPU_uint;
typedef uint1 GPU_uint1;
typedef uint2 GPU_uint2;
typedef uint4 GPU_uint4;
typedef uint3 GPU_uint3;

typedef half  GPU_half;
typedef half1 GPU_half1;
typedef half2 GPU_half2;
typedef half3 GPU_half3;
typedef half4 GPU_half4;

typedef float  GPU_float;
typedef float1 GPU_float1;
typedef float2 GPU_float2;
typedef float3 GPU_float3;
typedef float4 GPU_float4;

typedef double  GPU_double;
typedef double1 GPU_double1;
typedef double2 GPU_double2;
typedef double3 GPU_double3;
typedef double4 GPU_double4;

typedef float1x1 GPU_bool1x1;
typedef float1x2 GPU_bool1x2;
typedef float1x3 GPU_bool1x3;
typedef float1x4 GPU_bool1x4;
typedef float2x1 GPU_bool2x1;
typedef float2x2 GPU_bool2x2;
typedef float2x3 GPU_bool2x3;
typedef float2x4 GPU_bool2x4;
typedef float3x1 GPU_bool3x1;
typedef float3x2 GPU_bool3x2;
typedef float3x3 GPU_bool3x3;
typedef float3x4 GPU_bool3x4;
typedef float4x1 GPU_bool4x1;
typedef float4x2 GPU_bool4x2;
typedef float4x3 GPU_bool4x3;
typedef float4x4 GPU_bool4x4;

typedef int1x1 GPU_int1x1;
typedef int1x2 GPU_int1x2;
typedef int1x3 GPU_int1x3;
typedef int1x4 GPU_int1x4;
typedef int2x1 GPU_int2x1;
typedef int2x2 GPU_int2x2;
typedef int2x3 GPU_int2x3;
typedef int2x4 GPU_int2x4;
typedef int3x1 GPU_int3x1;
typedef int3x2 GPU_int3x2;
typedef int3x3 GPU_int3x3;
typedef int3x4 GPU_int3x4;
typedef int4x1 GPU_int4x1;
typedef int4x2 GPU_int4x2;
typedef int4x3 GPU_int4x3;
typedef int4x4 GPU_int4x4;

typedef uint1x1 GPU_uint1x1;
typedef uint1x2 GPU_uint1x2;
typedef uint1x3 GPU_uint1x3;
typedef uint1x4 GPU_uint1x4;
typedef uint2x1 GPU_uint2x1;
typedef uint2x2 GPU_uint2x2;
typedef uint2x3 GPU_uint2x3;
typedef uint2x4 GPU_uint2x4;
typedef uint3x1 GPU_uint3x1;
typedef uint3x2 GPU_uint3x2;
typedef uint3x3 GPU_uint3x3;
typedef uint3x4 GPU_uint3x4;
typedef uint4x1 GPU_uint4x1;
typedef uint4x2 GPU_uint4x2;
typedef uint4x3 GPU_uint4x3;
typedef uint4x4 GPU_uint4x4;

typedef half1x1 GPU_half1x1;
typedef half1x2 GPU_half1x2;
typedef half1x3 GPU_half1x3;
typedef half1x4 GPU_half1x4;
typedef half2x1 GPU_half2x1;
typedef half2x2 GPU_half2x2;
typedef half2x3 GPU_half2x3;
typedef half2x4 GPU_half2x4;
typedef half3x1 GPU_half3x1;
typedef half3x2 GPU_half3x2;
typedef half3x3 GPU_half3x3;
typedef half3x4 GPU_half3x4;
typedef half4x1 GPU_half4x1;
typedef half4x2 GPU_half4x2;
typedef half4x3 GPU_half4x3;
typedef half4x4 GPU_half4x4;

typedef float1x1 GPU_float1x1;
typedef float1x2 GPU_float1x2;
typedef float1x3 GPU_float1x3;
typedef float1x4 GPU_float1x4;
typedef float2x1 GPU_float2x1;
typedef float2x2 GPU_float2x2;
typedef float2x3 GPU_float2x3;
typedef float2x4 GPU_float2x4;
typedef float3x1 GPU_float3x1;
typedef float3x2 GPU_float3x2;
typedef float3x3 GPU_float3x3;
typedef float3x4 GPU_float3x4;
typedef float4x1 GPU_float4x1;
typedef float4x2 GPU_float4x2;
typedef float4x3 GPU_float4x3;
typedef float4x4 GPU_float4x4;

typedef double1x1 GPU_double1x1;
typedef double1x2 GPU_double1x2;
typedef double1x3 GPU_double1x3;
typedef double1x4 GPU_double1x4;
typedef double2x1 GPU_double2x1;
typedef double2x2 GPU_double2x2;
typedef double2x3 GPU_double2x3;
typedef double2x4 GPU_double2x4;
typedef double3x1 GPU_double3x1;
typedef double3x2 GPU_double3x2;
typedef double3x3 GPU_double3x3;
typedef double3x4 GPU_double3x4;
typedef double4x1 GPU_double4x1;
typedef double4x2 GPU_double4x2;
typedef double4x3 GPU_double4x3;
typedef double4x4 GPU_double4x4;

#define TEXTURE_1D_RESOURCE(textureName) sampler1D textureName
#define TEXTURE_2D_RESOURCE(textureName) sampler2D textureName
#define TEXTURE_3D_RESOURCE(textureName) sampler3D textureName
#define TEXTURE_CUBE_RESOURCE(textureName) samplerCUBE textureName
#define CBUFFER_RESOURCE(CBUFFER_NAME, CBUFFER_BODY) \
struct CBUFFER_NAME##ConstantTable \
{ \
    CBUFFER_BODY \
}; \
\
cbuffer CBUFFER_NAME##ResourceTable \
{ \
    CBUFFER_NAME##ConstantTable CBUFFER_NAME##Params; \
};

#else // HLSL

typedef Vec4f GPU_bool;
typedef Vec4f GPU_bool1;
typedef Vec4f GPU_bool2;
typedef Vec4f GPU_bool3;
typedef Vec4f GPU_bool4;

typedef Vec4f GPU_int;
typedef Vec4f GPU_int1;
typedef Vec4f GPU_int2;
typedef Vec4f GPU_int4;
typedef Vec4f GPU_int3;

typedef Vec4f GPU_uint;
typedef Vec4f GPU_uint1;
typedef Vec4f GPU_uint2;
typedef Vec4f GPU_uint4;
typedef Vec4f GPU_uint3;

typedef Vec4f GPU_half;
typedef Vec4f GPU_half1;
typedef Vec4f GPU_half2;
typedef Vec4f GPU_half3;
typedef Vec4f GPU_half4;

typedef Vec4f GPU_float;
typedef Vec4f GPU_float1;
typedef Vec4f GPU_float2;
typedef Vec4f GPU_float3;
typedef Vec4f GPU_float4;

typedef Vec4f GPU_double;
typedef Vec4f GPU_double1;
typedef Vec4f GPU_double2;
typedef Vec4f GPU_double3;
typedef Vec4f GPU_double4;

typedef Matrix<float, 1u, 4u> GPU_bool1x1;
typedef Matrix<float, 1u, 4u> GPU_bool1x2;
typedef Matrix<float, 1u, 4u> GPU_bool1x3;
typedef Matrix<float, 1u, 4u> GPU_bool1x4;
typedef Matrix<float, 2u, 4u> GPU_bool2x1;
typedef Matrix<float, 2u, 4u> GPU_bool2x2;
typedef Matrix<float, 2u, 4u> GPU_bool2x3;
typedef Matrix<float, 2u, 4u> GPU_bool2x4;
typedef Matrix<float, 3u, 4u> GPU_bool3x1;
typedef Matrix<float, 3u, 4u> GPU_bool3x2;
typedef Matrix<float, 3u, 4u> GPU_bool3x3;
typedef Matrix<float, 3u, 4u> GPU_bool3x4;
typedef Matrix<float, 4u, 4u> GPU_bool4x1;
typedef Matrix<float, 4u, 4u> GPU_bool4x2;
typedef Matrix<float, 4u, 4u> GPU_bool4x3;
typedef Matrix<float, 4u, 4u> GPU_bool4x4;

typedef Matrix<float, 1u, 4u> GPU_int1x1;
typedef Matrix<float, 1u, 4u> GPU_int1x2;
typedef Matrix<float, 1u, 4u> GPU_int1x3;
typedef Matrix<float, 1u, 4u> GPU_int1x4;
typedef Matrix<float, 2u, 4u> GPU_int2x1;
typedef Matrix<float, 2u, 4u> GPU_int2x2;
typedef Matrix<float, 2u, 4u> GPU_int2x3;
typedef Matrix<float, 2u, 4u> GPU_int2x4;
typedef Matrix<float, 3u, 4u> GPU_int3x1;
typedef Matrix<float, 3u, 4u> GPU_int3x2;
typedef Matrix<float, 3u, 4u> GPU_int3x3;
typedef Matrix<float, 3u, 4u> GPU_int3x4;
typedef Matrix<float, 4u, 4u> GPU_int4x1;
typedef Matrix<float, 4u, 4u> GPU_int4x2;
typedef Matrix<float, 4u, 4u> GPU_int4x3;
typedef Matrix<float, 4u, 4u> GPU_int4x4;

typedef Matrix<float, 1u, 4u> GPU_uint1x1;
typedef Matrix<float, 1u, 4u> GPU_uint1x2;
typedef Matrix<float, 1u, 4u> GPU_uint1x3;
typedef Matrix<float, 1u, 4u> GPU_uint1x4;
typedef Matrix<float, 2u, 4u> GPU_uint2x1;
typedef Matrix<float, 2u, 4u> GPU_uint2x2;
typedef Matrix<float, 2u, 4u> GPU_uint2x3;
typedef Matrix<float, 2u, 4u> GPU_uint2x4;
typedef Matrix<float, 3u, 4u> GPU_uint3x1;
typedef Matrix<float, 3u, 4u> GPU_uint3x2;
typedef Matrix<float, 3u, 4u> GPU_uint3x3;
typedef Matrix<float, 3u, 4u> GPU_uint3x4;
typedef Matrix<float, 4u, 4u> GPU_uint4x1;
typedef Matrix<float, 4u, 4u> GPU_uint4x2;
typedef Matrix<float, 4u, 4u> GPU_uint4x3;
typedef Matrix<float, 4u, 4u> GPU_uint4x4;

typedef Matrix<float, 1u, 4u> GPU_half1x1;
typedef Matrix<float, 1u, 4u> GPU_half1x2;
typedef Matrix<float, 1u, 4u> GPU_half1x3;
typedef Matrix<float, 1u, 4u> GPU_half1x4;
typedef Matrix<float, 2u, 4u> GPU_half2x1;
typedef Matrix<float, 2u, 4u> GPU_half2x2;
typedef Matrix<float, 2u, 4u> GPU_half2x3;
typedef Matrix<float, 2u, 4u> GPU_half2x4;
typedef Matrix<float, 3u, 4u> GPU_half3x1;
typedef Matrix<float, 3u, 4u> GPU_half3x2;
typedef Matrix<float, 3u, 4u> GPU_half3x3;
typedef Matrix<float, 3u, 4u> GPU_half3x4;
typedef Matrix<float, 4u, 4u> GPU_half4x1;
typedef Matrix<float, 4u, 4u> GPU_half4x2;
typedef Matrix<float, 4u, 4u> GPU_half4x3;
typedef Matrix<float, 4u, 4u> GPU_half4x4;

typedef Matrix<float, 1u, 4u> GPU_float1x1;
typedef Matrix<float, 1u, 4u> GPU_float1x2;
typedef Matrix<float, 1u, 4u> GPU_float1x3;
typedef Matrix<float, 1u, 4u> GPU_float1x4;
typedef Matrix<float, 2u, 4u> GPU_float2x1;
typedef Matrix<float, 2u, 4u> GPU_float2x2;
typedef Matrix<float, 2u, 4u> GPU_float2x3;
typedef Matrix<float, 2u, 4u> GPU_float2x4;
typedef Matrix<float, 3u, 4u> GPU_float3x1;
typedef Matrix<float, 3u, 4u> GPU_float3x2;
typedef Matrix<float, 3u, 4u> GPU_float3x3;
typedef Matrix<float, 3u, 4u> GPU_float3x4;
typedef Matrix<float, 4u, 4u> GPU_float4x1;
typedef Matrix<float, 4u, 4u> GPU_float4x2;
typedef Matrix<float, 4u, 4u> GPU_float4x3;
typedef Matrix<float, 4u, 4u> GPU_float4x4;

typedef Matrix<float, 1u, 4u> GPU_double1x1;
typedef Matrix<float, 1u, 4u> GPU_double1x2;
typedef Matrix<float, 1u, 4u> GPU_double1x3;
typedef Matrix<float, 1u, 4u> GPU_double1x4;
typedef Matrix<float, 2u, 4u> GPU_double2x1;
typedef Matrix<float, 2u, 4u> GPU_double2x2;
typedef Matrix<float, 2u, 4u> GPU_double2x3;
typedef Matrix<float, 2u, 4u> GPU_double2x4;
typedef Matrix<float, 3u, 4u> GPU_double3x1;
typedef Matrix<float, 3u, 4u> GPU_double3x2;
typedef Matrix<float, 3u, 4u> GPU_double3x3;
typedef Matrix<float, 3u, 4u> GPU_double3x4;
typedef Matrix<float, 4u, 4u> GPU_double4x1;
typedef Matrix<float, 4u, 4u> GPU_double4x2;
typedef Matrix<float, 4u, 4u> GPU_double4x3;
typedef Matrix<float, 4u, 4u> GPU_double4x4;
//////////////////////////////////////////////////////////////////////////////////////////////

#endif // HLSL
#endif // COMMON_HLSLI_
