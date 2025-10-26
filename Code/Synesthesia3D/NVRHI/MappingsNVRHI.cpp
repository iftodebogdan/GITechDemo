/**
 * @file        MappingsNVRHI.cpp
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

#include "stdafx.h"

#include "MappingsNVRHI.h"

#if ENABLE_NVRHI

namespace Synesthesia3D
{
    const nvrhi::Format PixelFormatNVRHI[PF_MAX] =
    {
        nvrhi::Format::UNKNOWN,                 // PF_NONE
        nvrhi::Format::B5G6R5_UNORM,            // PF_R5G6B5
        nvrhi::Format::B5G5R5A1_UNORM,          // PF_A1R5G5B5
        nvrhi::Format::BGRA4_UNORM,             // PF_A4R4G4B4
        nvrhi::Format::R8_UNORM,                // PF_A8
        nvrhi::Format::R8_UNORM,                // PF_L8
        nvrhi::Format::RG8_UNORM,               // PF_A8L8
        nvrhi::Format::BGRX8_UNORM,             // PF_R8G8B8
        nvrhi::Format::BGRX8_UNORM,             // PF_X8R8G8B8
        nvrhi::Format::BGRA8_UNORM,             // PF_A8R8G8B8
        nvrhi::Format::RGBA8_UNORM,             // PF_A8B8G8R8
        nvrhi::Format::R16_UNORM,               // PF_L16
        nvrhi::Format::RG16_UNORM,              // PF_G16R16
        nvrhi::Format::RGBA16_UNORM,            // PF_A16B16G16R16
        nvrhi::Format::R16_FLOAT,               // PF_R16F
        nvrhi::Format::RG16_FLOAT,              // PF_G16R16F
        nvrhi::Format::RGBA16_FLOAT,            // PF_A16B16G16R16F
        nvrhi::Format::R32_FLOAT,               // PF_R32F
        nvrhi::Format::RG32_FLOAT,              // PF_G32R32F
        nvrhi::Format::RGBA32_FLOAT,            // PF_A32B32G32R32F
        nvrhi::Format::BC1_UNORM,               // PF_DXT1
        nvrhi::Format::BC2_UNORM,               // PF_DXT3
        nvrhi::Format::BC3_UNORM,               // PF_DXT5
        nvrhi::Format::D24S8,                   // PF_D24S8
        nvrhi::Format::D32,                     // PF_INTZ
    };

    const PixelFormat MatchPixelFormat(const nvrhi::Format fmt)
    {
        for (unsigned int i = 0; i < PF_MAX; i++)
        {
            if (PixelFormatNVRHI[i] == fmt)
                return (PixelFormat)i;
        }

        return PF_NONE;
    }

    const nvrhi::TextureDimension TextureTypeNVRHI[TT_MAX] =
    {
        nvrhi::TextureDimension::Texture1D,     // TT_1D
        nvrhi::TextureDimension::Texture2D,     // TT_2D
        nvrhi::TextureDimension::Texture3D,     // TT_3D
        nvrhi::TextureDimension::TextureCube,   // TT_CUBE
    };

    const TextureType MatchTextureType(const nvrhi::TextureDimension dim)
    {
        for (unsigned int i = 0; i < TT_MAX; i++)
        {
            if (TextureTypeNVRHI[i] == dim)
                return (TextureType)i;
        }
        return TT_MAX;
    }

    // TODO: Verify this mapping
    const unsigned int CubeFaceIndexNVRHI[FACE_MAX] =
    {
        1,  // FACE_XNEG
        2,  // FACE_YPOS
        3,  // FACE_YNEG
        4,  // FACE_ZPOS
        0,  // FACE_XPOS
        5   // FACE_ZNEG
    };
}

#endif // ENABLE_NVRHI
