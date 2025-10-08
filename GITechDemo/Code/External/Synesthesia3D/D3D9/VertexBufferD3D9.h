/**
 * @file        VertexBufferD3D9.h
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

#ifndef VERTEXBUFFERD3D9_H
#define VERTEXBUFFERD3D9_H

#include <d3d9.h>
#include "VertexBuffer.h"

namespace Synesthesia3D
{
    class VertexFormatD3D9;
    class IndexBufferD3D9;

    //This is the D3D9 implementation of the VertexBuffer class
    class VertexBufferD3D9 : public VertexBuffer
    {
    public:
        void    Enable(const unsigned int offset = 0);
        void    Disable();
        void    Lock(const BufferLocking lockMode);
        void    Unlock();
        void    Update();

        void    Bind();
        void    Unbind();

    private:
        VertexBufferD3D9(
            VertexFormatD3D9* const vertexFormat, const unsigned int vertexCount,
            IndexBufferD3D9* const indexBuffer = nullptr, const BufferUsage usage = BU_STATIC);
        ~VertexBufferD3D9();

        IDirect3DVertexBuffer9*     m_pVertexBuffer;

        // A temporary pointer used in the Lock->Update->Unlock flow
        void*                       m_pTempBuffer;

        friend class ResourceManagerD3D9;
    };
}

#endif //VERTEXBUFFERD3D9_H
