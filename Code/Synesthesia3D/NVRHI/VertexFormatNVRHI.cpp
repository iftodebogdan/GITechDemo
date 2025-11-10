/**
 * @file        VertexFormatNVRHI.cpp
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

#include "VertexFormatNVRHI.h"
using namespace Synesthesia3D;

#if ENABLE_NVRHI

#include "RendererNVRHI.h"
#include "MappingsNVRHI.h"

VertexFormatNVRHI::VertexFormatNVRHI(const unsigned int attributeCount)
    : VertexFormat(attributeCount)
{

}

VertexFormatNVRHI::~VertexFormatNVRHI()
{
    Unbind();
}

void VertexFormatNVRHI::Enable()
{

}

void VertexFormatNVRHI::Disable()
{

}

void VertexFormatNVRHI::Update()
{
    std::vector<nvrhi::VertexAttributeDesc> attributes;
    attributes.resize(m_nAttributeCount);

    for (unsigned int i = 0; i < m_nAttributeCount; i++)
    {
        attributes[i]
            .setName(VertexAttributeSemanticNVRHI[GetAttributeSemantic(i)] + std::to_string(GetSemanticIndex(i)))
            .setFormat(VertexAttributeTypeNVRHI[GetAttributeType(i)])
            .setOffset(GetOffset(i))
            .setBufferIndex(0)
            .setElementStride(GetStride());
    }

    // TODO: D3D11 requires vertex shader ptr
    m_pInputLayout = RendererNVRHI::GetInstance()->GetDevice()->createInputLayout(attributes.data(), GetAttributeCount(), nullptr);
}

void VertexFormatNVRHI::Bind()
{
    Update();
}

void VertexFormatNVRHI::Unbind()
{

}

#endif // ENABLE_NVRHI
