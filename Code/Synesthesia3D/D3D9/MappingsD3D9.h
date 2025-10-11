/**
 * @file        MappingsD3D9.h
 *
 * @note        This file is part of the "Synesthesia3D" graphics engine
 *
 * @copyright   Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * @copyright
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * @copyright
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAPPINGSD3D9_H
#define MAPPINGSD3D9_H

#include <d3d9.h>

#include "ResourceData.h"

namespace Synesthesia3D
{
    //These are used to translate platform independent flags to D3D9 format
    extern const DWORD BufferUsageD3D9[BU_MAX];
    extern const DWORD BufferLockingD3D9[BL_MAX];
    extern const BYTE VertexAttributeTypeD3D9[VAT_MAX];
    extern const BYTE VertexAttributeSemanticD3D9[VAS_MAX];
    extern const D3DFORMAT IndexBufferFormatD3D9[IBF_MAX];
    extern const D3DFORMAT PixelFormatD3D9[PF_MAX];
    extern const DWORD RenderStateMappingD3D9[RS_MAX];
    extern const DWORD TextureAddressingModeD3D9[SAM_MAX];
    extern const DWORD MinMagFilterD3D9[SF_MAX];
    extern const DWORD MipFilterD3D9[SF_MAX];
    extern const D3DRESOURCETYPE TextureTypeD3D9[TT_MAX];
    extern const unsigned int CubeFaceIndexD3D9[FACE_MAX];

    const SamplerFilter         MatchFilterType(const DWORD min, const DWORD mag, const DWORD mip);
    const SamplerAddressingMode MatchAddressingMode(const DWORD sam);
    const PixelFormat           MatchPixelFormat(const D3DFORMAT fmt);
}

#endif //MAPPINGD3D9_H
