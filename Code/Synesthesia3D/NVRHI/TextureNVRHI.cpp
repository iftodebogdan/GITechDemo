/**
 * @file        TextureNVRHI.cpp
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

#include "TextureNVRHI.h"
using namespace Synesthesia3D;

TextureNVRHI::TextureNVRHI(
    const PixelFormat pixelFormat, const TextureType texType,
    const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
    const unsigned int mipCount, const BufferUsage usage)
    : Texture(pixelFormat, texType, sizeX, sizeY, sizeZ, mipCount, usage)
{
    // Support for deferred initialization (loading from file)
    if (usage != BU_NONE)
        Bind();
}

TextureNVRHI::~TextureNVRHI()
{
    Unbind();
}

void TextureNVRHI::Enable(const unsigned int texUnit) const
{

}

void TextureNVRHI::Disable(const unsigned int texUnit) const
{

}

const bool TextureNVRHI::Lock(const unsigned int mipmapLevel, const BufferLocking lockMode)
{
    if (m_eTexType == TT_CUBE)
    {
        assert(false);
        return false;
    }

    if (m_bIsLocked || m_eBufferUsage == BU_RENDERTAGET || m_eBufferUsage == BU_DEPTHSTENCIL)
        return false;

    return Texture::Lock(mipmapLevel, lockMode);
}

const bool TextureNVRHI::Lock(const CubeFace cubeFace, const unsigned int mipmapLevel, const BufferLocking lockMode)
{
    if (m_eTexType != TT_CUBE)
    {
        assert(false);
        return false;
    }

    if (m_bIsLocked)
        return false;

    return Texture::Lock(cubeFace, mipmapLevel, lockMode);
}

void TextureNVRHI::Unlock()
{
    Texture::Unlock();
}

void TextureNVRHI::Update()
{

}

void TextureNVRHI::Bind()
{
    Texture::Bind();

    switch (GetTextureType())
    {
    case TT_1D:
    case TT_2D:
    case TT_3D:
        for (unsigned int mip = 0; mip < GetMipCount(); mip++)
        {
            if (Lock(mip, BL_WRITE_ONLY))
            {
                Update();
                Unlock();
            }
            else
                if (m_eBufferUsage != BU_RENDERTAGET && m_eBufferUsage != BU_DEPTHSTENCIL)
                    assert(false);
        }
        break;
    case TT_CUBE:
        for (CubeFace face = FACE_XNEG; face < FACE_MAX; face = (CubeFace)(face + 1))
        {
            for (unsigned int mip = 0; mip < GetMipCount(); mip++)
            {
                if (Lock(face, mip, BL_WRITE_ONLY))
                {
                    Update();
                    Unlock();
                }
                else
                    assert(false);
            }
        }
    }
}

void TextureNVRHI::Unbind()
{

}

const unsigned int TextureNVRHI::GetCubeFaceIndex(const CubeFace cubeFace) const
{
    return 0;
}
