/**
 * @file        VertexBufferNVRHI.h
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

#ifndef VERTEXBUFFERNVRHI_H
#define VERTEXBUFFERNVRHI_H

#include "ResourceData.h"

#if ENABLE_NVRHI

#include "VertexBuffer.h"

namespace Synesthesia3D
{
    class VertexFormatNVRHI;
    class IndexBufferNVRHI;

    class VertexBufferNVRHI : public VertexBuffer
    {
    public:
        void Enable(const unsigned int offset = 0) override;
        void Disable() override;
        void Lock(const BufferLocking lockMode) override;
        void Unlock() override;
        void Update() override;

        void Bind() override;
        void Unbind() override;

    private:
        VertexBufferNVRHI(
            VertexFormatNVRHI* const vertexFormat, const unsigned int vertexCount,
            IndexBufferNVRHI* const indexBuffer = nullptr, const BufferUsage usage = BU_STATIC);
        ~VertexBufferNVRHI();

        friend class ResourceManagerNVRHI;
    };
}

#endif // ENABLE_NVRHI

#endif // VERTEXBUFFERNVRHI_H
