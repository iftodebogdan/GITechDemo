/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	Common.hlsl
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

//////////////////////////////////////////////////////////////
// The ratio of a circle's circumference to its diameter	//
//////////////////////////////////////////////////////////////
#define PI		(3.14159265f)
#define PI_RCP	(0.31830988618379067153776752674503f)



//////////////////////////////////////////////////////////////
// Enable the unrolling of for loops, effectively trading	//
// off compile speed and shader size for runtime speed.		//
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
// Small optimization in the form of an early depth check.	//
// This is useful if you have an expensive pixel shader		//
// which you would not want (fully) executed on pixels that	//
// exceed a certain depth (or other) threshold.				//
//////////////////////////////////////////////////////////////
//#define DEPTH_KILL(depth, threshold) if (depth >= threshold) clip(-1)
//#define DEPTH_KILL(depth, threshold) clip(-(depth >= threshold))
#define DEPTH_KILL(depth, threshold) clip(-step(threshold, depth))



//////////////////////////////////////////////////////////
// Similar to the above early depth test, but tailored	//
// specifically for alpha testing. If the alpha is less	//
// the threshold, then the pixel is clipped.			//
//////////////////////////////////////////////////////////
#define ALPHA_TEST(alpha, threshold) clip(-step(alpha, threshold))



//////////////////////////////////////////////////
// Mip count for texture bound to texEnvMap.	//
// Used in multiple shaders, so defined here.	//
//////////////////////////////////////////////////
#define ENVIRONMENT_MAP_MIP_COUNT	(9)
