//////////////////////////////////////////////////////////////
// The ratio of a circle's circumference to its diameter	//
//////////////////////////////////////////////////////////////
#define PI		3.14159265f



//////////////////////////////////////////////////////////////
// Enable the unrolling of for loops, effectively trading	//
// off compile speed and shader size for runtime speed.		//
//////////////////////////////////////////////////////////////
#define ENABLE_UNROLLING (1)
#define ENABLE_FORCE_LOOP (1)

#if ENABLE_UNROLLING
#define UNROLL [unroll]
#else
#define UNROLL
#endif

#if ENABLE_FORCE_LOOP
#define FORCE_LOOP [loop]
#else
#define FORCE_LOOP
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
