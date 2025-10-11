/**
 * @file        VertexFormatD3D9.h
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

#ifndef VERTEXFORMATD3D9_H
#define VERTEXFORMATD3D9_H

#include <d3d9.h>
#include "VertexFormat.h"

namespace Synesthesia3D
{
    //This is the D3D9 implementation of the VertexFormat class
    class VertexFormatD3D9 : public VertexFormat
    {
    public:
        void    Enable();
        void    Disable();
        void    Update();

        void    Bind();
        void    Unbind();

    private:
        VertexFormatD3D9(const unsigned int attributeCount);
        ~VertexFormatD3D9();

        D3DVERTEXELEMENT9 m_pVertexElements[VF_MAX_ATTRIBUTES + 1];
        IDirect3DVertexDeclaration9* m_pVertexDeclaration;

        friend class ResourceManagerD3D9;
    };
}

#endif //VERTEXFORMATD3D9_H
