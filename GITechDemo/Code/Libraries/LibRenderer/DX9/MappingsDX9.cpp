//////////////////////////////////////////////////////////////////////////
// This file is part of the "LibRenderer" 3D graphics library           //
//                                                                      //
// Copyright (C) 2014 - Iftode Bogdan-Marius <iftode.bogdan@gmail.com>  //
//                                                                      //
// This program is free software: you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program. If not, see <http://www.gnu.org/licenses/>. //
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "MappingsDX9.h"

namespace LibRendererDll
{
	//Translates buffer usage flags from platform independent format to D3D9 format
	const DWORD BufferUsageDX9[BU_MAX] =
	{
		0,							// BU_NONE
		D3DUSAGE_WRITEONLY,			// BU_STATIC
		D3DUSAGE_DYNAMIC,			// BU_DYNAMIC
		D3DUSAGE_RENDERTARGET,		// BU_RENDERTARGET
		D3DUSAGE_DEPTHSTENCIL,		// BU_DEPTHSTENCIL
		0,							// BU_TEXTURE
	};

	//Translates buffer locking flags from platform independent format to D3D9 format
	const DWORD BufferLockingDX9[BL_MAX] =
	{
		D3DLOCK_READONLY,		// BL_READ_ONLY
		0,						// BL_WRITE_ONLY
		0						// BL_READ_WRITE
	};

	//Translates vertex attribute type flags from platform independent format to D3D9 format
	const BYTE VertexAttributeTypeDX9[VAT_MAX] =
	{
		255,						// VAT_NONE
		D3DDECLTYPE_FLOAT1,			// VAT_FLOAT1
		D3DDECLTYPE_FLOAT2,			// VAT_FLOAT2
		D3DDECLTYPE_FLOAT3,			// VAT_FLOAT3
		D3DDECLTYPE_FLOAT4,			// VAT_FLOAT4
		D3DDECLTYPE_FLOAT16_2,		// VAT_HALF2
		D3DDECLTYPE_FLOAT16_4,		// VAT_HALF4
		D3DDECLTYPE_D3DCOLOR,		// VAT_UBYTE4
		D3DDECLTYPE_SHORT2,			// VAT_SHORT2
		D3DDECLTYPE_SHORT4			// VAT_SHORT4
	};

	//Translates vertex attribute usage flags from platform independent format to D3D9 format
	const BYTE VertexAttributeUsageDX9[VAU_MAX] =
	{
		255,						// VAU_NONE
		D3DDECLUSAGE_POSITION,		// VAU_POSITION
		D3DDECLUSAGE_NORMAL,		// VAU_NORMAL
		D3DDECLUSAGE_TANGENT,		// VAU_TANGENT
		D3DDECLUSAGE_BINORMAL,		// VAU_BINORMAL
		D3DDECLUSAGE_TEXCOORD,		// VAU_TEXCOORD
		D3DDECLUSAGE_COLOR,			// VAU_COLOR
		D3DDECLUSAGE_BLENDINDICES,	// VAU_BLENDINDICES
		D3DDECLUSAGE_BLENDWEIGHT,	// VAU_BLENDWEIGHT
		D3DDECLUSAGE_TESSFACTOR,	// VAU_FOGCOORD
		D3DDECLUSAGE_PSIZE			// VAU_PSIZE
	};

	const D3DFORMAT IndexBufferFormatDX9[IBF_MAX] =
	{
		D3DFMT_INDEX16,				// IBF_INDEX16
		D3DFMT_INDEX32				// IBF_INDEX32
	};

	const D3DFORMAT TextureFormatDX9[PF_MAX] =
	{
		D3DFMT_UNKNOWN,				// PF_NONE
		D3DFMT_R5G6B5,				// PF_R5G6B5
		D3DFMT_A1R5G5B5,			// PF_A1R5G5B5
		D3DFMT_A4R4G4B4,			// PF_A4R4G4B4
		D3DFMT_A8,					// PF_A8
		D3DFMT_L8,					// PF_L8
		D3DFMT_A8L8,				// PF_A8L8
		D3DFMT_R8G8B8,				// PF_R8G8B8
		D3DFMT_A8R8G8B8,			// PF_A8R8G8B8
		D3DFMT_A8B8G8R8,			// PF_A8B8G8R8
		D3DFMT_L16,					// PF_L16
		D3DFMT_G16R16,				// PF_G16R16
		D3DFMT_A16B16G16R16,		// PF_A16B16G16R16
		D3DFMT_R16F,				// PF_R16F
		D3DFMT_G16R16F,				// PF_G16R16F
		D3DFMT_A16B16G16R16F,		// PF_A16B16G16R16F
		D3DFMT_R32F,				// PF_R32F
		D3DFMT_G32R32F,				// PF_G32R32F
		D3DFMT_A32B32G32R32F,		// PF_A32B32G32R32F
		D3DFMT_DXT1,				// PF_DXT1
		D3DFMT_DXT3,				// PF_DXT3
		D3DFMT_DXT5,				// PF_DXT5
		D3DFMT_D24S8				// PF_D24S8
	};

	const DWORD RenderStateMappingDX9[RS_MAX] =
	{
		0,							// BLEND
		D3DBLEND_ZERO,				// BLEND_ZERO
		D3DBLEND_ONE,				// BLEND_ONE
		D3DBLEND_SRCCOLOR,			// BLEND_SRCCOLOR
		D3DBLEND_INVSRCCOLOR,		// BLEND_INVSRCCOLOR
		D3DBLEND_SRCALPHA,			// BLEND_SRCALPHA
		D3DBLEND_INVSRCALPHA,		// BLEND_INVSRCALPHA
		D3DBLEND_DESTALPHA,			// BLEND_DESTALPHA
		D3DBLEND_INVDESTALPHA,		// BLEND_INVDESTALPHA
		D3DBLEND_DESTCOLOR,			// BLEND_DESTCOLOR
		D3DBLEND_INVDESTCOLOR,		// BLEND_INVDESTCOLOR
		D3DBLEND_SRCALPHASAT,		// BLEND_SRCALPHASAT
		D3DBLEND_BOTHSRCALPHA,		// BLEND_BOTHSRCALPHA
		D3DBLEND_BOTHINVSRCALPHA,	// BLEND_BOTHINVSRCALPHA
		D3DBLEND_BLENDFACTOR,		// BLEND_BLENDFACTOR
		D3DBLEND_INVBLENDFACTOR,	// BLEND_INVBLENDFACTOR
		0,							// BLEND_END

		0,							// CMP
		D3DCMP_NEVER,				// CMP_NEVER
		D3DCMP_LESS,				// CMP_LESS
		D3DCMP_EQUAL,				// CMP_EQUAL
		D3DCMP_LESSEQUAL,			// CMP_LESSEQUAL
		D3DCMP_GREATER,				// CMP_GREATER
		D3DCMP_NOTEQUAL,			// CMP_NOTEQUAL
		D3DCMP_GREATEREQUAL,		// CMP_GREATEQUAL
		D3DCMP_ALWAYS,				// CMP_ALWAYS
		0,							// CMP_END

		0,							// CULL
		D3DCULL_NONE,				// CULL_NONE
		D3DCULL_CW,					// CULL_CW
		D3DCULL_CCW,				// CULL_CCW
		0,							// CULL_END

		0,							// ZB
		D3DZB_TRUE,					// ZB_ENABLED
		D3DZB_FALSE,				// ZB_DISABLED
		D3DZB_USEW,					// ZB_USEW
		0,							// ZB_END

		0,							// STENCILOP
		D3DSTENCILOP_KEEP,			// STENCILOP_KEEP
		D3DSTENCILOP_ZERO,			// STENCILOP_ZERO
		D3DSTENCILOP_REPLACE,		// STENCILOP_REPLACE
		D3DSTENCILOP_INCRSAT,		// STENCILOP_INCRSAT
		D3DSTENCILOP_DECRSAT,		// STENCILOP_DECRSAT
		D3DSTENCILOP_INVERT,		// STENCILOP_INVERT
		D3DSTENCILOP_INCR,			// STENCILOP_INCR
		D3DSTENCILOP_DECR,			// STENCILOP_DECR
		0,							// STENCILOP_END

		0,							// FILL
		D3DFILL_POINT,				// FILL_POINT
		D3DFILL_WIREFRAME,			// FILL_WIREFRAME
		D3DFILL_SOLID,				// FILL_SOLID
		0							// FILL_END
	};

	const DWORD TextureAddressingModeDX9[SAM_MAX] =
	{
		0,                          // SAM_NONE
		D3DTADDRESS_CLAMP,          // SAM_CLAMP
		D3DTADDRESS_WRAP,           // SAM_WRAP
		D3DTADDRESS_MIRROR,         // SAM_MIRROR
		D3DTADDRESS_BORDER,         // SAM_BORDER
	};
	
	const DWORD MinMagFilterDX9[SF_MAX] =
	{
		D3DTEXF_NONE,               // SF_NONE
		D3DTEXF_POINT,              // SF_MIN_MAG_POINT_MIP_NONE
		D3DTEXF_LINEAR,             // SF_MIN_MAG_LINEAR_MIP_NONE
		D3DTEXF_POINT,              // SF_MIN_MAG_POINT_MIP_POINT
		D3DTEXF_POINT,              // SF_MIN_MAG_POINT_MIP_LINEAR
		D3DTEXF_LINEAR,             // SF_MIN_MAG_LINEAR_MIP_POINT
		D3DTEXF_LINEAR              // SF_MIN_MAG_LINEAR_MIP_LINEAR
	};
	
	const DWORD MipFilterDX9[SF_MAX] =
	{
		D3DTEXF_NONE,               // SF_NONE
		D3DTEXF_NONE,               // SF_MIN_MAG_POINT_MIP_NONE
		D3DTEXF_NONE,               // SF_MIN_MAG_LINEAR_MIP_NONE
		D3DTEXF_POINT,              // SF_MIN_MAG_POINT_MIP_POINT
		D3DTEXF_LINEAR,             // SF_MIN_MAG_POINT_MIP_LINEAR
		D3DTEXF_POINT,              // SF_MIN_MAG_LINEAR_MIP_POINT
		D3DTEXF_LINEAR              // SF_MIN_MAG_LINEAR_MIP_LINEAR
	};
}
