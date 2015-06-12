//////////////////////////////////////////////////////
// This file should be included before "Utils.hlsl"	//
// in order to leverage the tex2Dlod() optimization	//
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// Hardcoded mip level when sampling from textures to	//
// prevent the GPU from calculating ddx(), ddy() for	//
// every sample (post-processing effects work with		//
// textures with 1 mip level)							//
//////////////////////////////////////////////////////////
#define tex2D(tex, tc) tex2Dlod(tex, float4((tc).xy, 0.f, 0.f))

//////////////////////////////////////////////////////////////////////////////////////////
// Reconstruct the depth in world space coordinates from the hyperbolic depth			//
//--------------------------------------------------------------------------------------//
// LinearDepth = (znear * zfar / (znear - zfar)) / (Depth - (zfar / (zfar - znear)))	//
// f2LinearDepthEquation.x = znear * zfar / (znear - zfar)								//
// f2LinearDepthEquation.y = zfar / (zfar - znear)										//
//////////////////////////////////////////////////////////////////////////////////////////
static const float znear = 1.f;
static const float zfar = 5000.f;
static const float2 f2LinearDepthEquation = float2(znear * zfar / (znear - zfar), zfar / (zfar - znear));
float ReconstructDepth(float fHyperbolicDepth)
{
	return f2LinearDepthEquation.x / (fHyperbolicDepth - f2LinearDepthEquation.y);
}
