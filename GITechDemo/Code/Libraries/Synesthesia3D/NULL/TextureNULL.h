/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	TextureNULL.h
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#ifndef TEXTURENULL_H
#define TEXTURENULL_H

#include "Texture.h"

namespace Synesthesia3D
{
	class TextureNULL : public Texture
	{
	public:
		void		Enable(const unsigned int /*texUnit*/) const {}
		void		Disable(const unsigned int /*texUnit*/) const {}
		const bool	Lock(const unsigned int mipmapLevel, const BufferLocking lockMode) { return Texture::Lock(mipmapLevel, lockMode); }
		const bool	Lock(const CubeFace cubeFace, const unsigned int mipmapLevel, const BufferLocking lockMode) { return Texture::Lock(cubeFace, mipmapLevel, lockMode); }
		void		Unlock() { Texture::Unlock(); }
		void		Update() {}

		void		Bind() {}
		void		Unbind() {}

		const unsigned int	GetCubeFaceIndex(const CubeFace cubeFace) const { return cubeFace; }

	private:
		TextureNULL(
			const PixelFormat pixelFormat, const TextureType texType,
			const unsigned int sizeX, const unsigned int sizeY = 1, const unsigned int sizeZ = 1,
			const unsigned int mipCount = 0, const BufferUsage usage = BU_TEXTURE)
			: Texture(pixelFormat, texType, sizeX, sizeY, sizeZ, mipCount, usage) {}
		~TextureNULL() {}

		friend class ResourceManagerNULL;
	};
}

#endif //TEXTURENULL_H