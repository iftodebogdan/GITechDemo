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

#include "VertexFormatNULL.h"
#include "VertexBufferNULL.h"
#include "IndexBufferNULL.h"
using namespace LibRendererDll;

VertexBufferNULL::VertexBufferNULL(VertexFormatNULL* const vertexFormat, const unsigned int vertexCount, IndexBufferNULL* const indexBuffer, const BufferUsage usage)
	: VertexBuffer(vertexFormat, vertexCount, indexBuffer, usage)
{}

VertexBufferNULL::~VertexBufferNULL()
{}

void VertexBufferNULL::Enable(const unsigned int offset)
{}

void VertexBufferNULL::Disable()
{}

void VertexBufferNULL::Lock(const BufferLocking lockMode)
{}

void VertexBufferNULL::Unlock()
{}

void VertexBufferNULL::Update()
{}

void VertexBufferNULL::Bind()
{}

void VertexBufferNULL::Unbind()
{}