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

#include "TextureNULL.h"
using namespace LibRendererDll;

TextureNULL::TextureNULL(
	const PixelFormat texFormat, const TexType texType,
	const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ,
	const unsigned int mipmapLevelCount, const BufferUsage usage)
	: Texture(texFormat, texType, sizeX, sizeY, sizeZ, mipmapLevelCount, usage)
{}

TextureNULL::~TextureNULL()
{}

void TextureNULL::Enable(const unsigned int texUnit) const
{}

void TextureNULL::Disable(const unsigned int texUnit) const
{}

const bool TextureNULL::Lock(const unsigned int mipmapLevel, const BufferLocking lockMode)
{
	return Texture::Lock(mipmapLevel, lockMode);
}

const bool TextureNULL::Lock(const unsigned int cubeFace, const unsigned int mipmapLevel, const BufferLocking lockMode)
{
	return Texture::Lock(cubeFace, mipmapLevel, lockMode);
}

void TextureNULL::Unlock()
{
	Texture::Unlock();
}

void TextureNULL::Update()
{}

void TextureNULL::Bind()
{}

void TextureNULL::Unbind()
{}