/**
 * @file        TextureNVRHI.h
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

#ifndef TEXTURENVRHI_H
#define TEXTURENVRHI_H

#include "Texture.h"

namespace Synesthesia3D
{
    class TextureNVRHI : public Texture
    {
    public:
        void Enable(const unsigned int texUnit) const override;
        void Disable(const unsigned int texUnit) const override;
        const bool Lock(const unsigned int mipmapLevel, const BufferLocking lockMode) override;
        const bool Lock(const CubeFace cubeFace, const unsigned int mipmapLevel, const BufferLocking lockMode) override;
        void Unlock() override;
        void Update() override;

        void Bind() override;
        void Unbind() override;

        const unsigned int GetCubeFaceIndex(const CubeFace cubeFace) const override;

    private:
        TextureNVRHI(
            const PixelFormat pixelFormat, const TextureType texType,
            const unsigned int sizeX, const unsigned int sizeY = 1, const unsigned int sizeZ = 1,
            const unsigned int mipCount = 0, const BufferUsage usage = BU_TEXTURE);
        ~TextureNVRHI();

        friend class ResourceManagerNVRHI;
    };
}

#endif // TEXTURENVRHI_H
