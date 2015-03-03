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
#ifndef MAPPINGSDX9_H
#define MAPPINGSDX9_H

#include <d3d9.h>

#include "ResourceData.h"

namespace LibRendererDll
{
	//These are used to translate platform independent flags to D3D9 format
	extern const DWORD BufferUsageDX9[BU_MAX];
	extern const DWORD BufferLockingDX9[BL_MAX];
	extern const BYTE VertexAttributeTypeDX9[VAT_MAX];
	extern const BYTE VertexAttributeUsageDX9[VAU_MAX];
	extern const D3DFORMAT IndexBufferFormatDX9[IBF_MAX];
	extern const D3DFORMAT TextureFormatDX9[PF_MAX];
	extern const DWORD RenderStateMappingDX9[RS_MAX];
	extern const DWORD TextureAddressingModeDX9[SAM_MAX];
	extern const DWORD MinMagFilterDX9[SF_MAX];
	extern const DWORD MipFilterDX9[SF_MAX];
}

#endif //MAPPINGDX9_H
