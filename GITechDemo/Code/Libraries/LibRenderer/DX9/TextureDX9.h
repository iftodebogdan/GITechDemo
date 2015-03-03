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
#ifndef TEXTUREDX9_H
#define TEXTUREDX9_H

#include <d3d9.h>
#include "Texture.h"

namespace LibRendererDll
{
	class TextureDX9 : public Texture
	{
	public:
		void		Enable(const unsigned int texUnit) const;
		void		Disable(const unsigned int texUnit) const;
		const bool	Lock(const unsigned int mipmapLevel, const BufferLocking lockMode);
		const bool	Lock(const unsigned int cubeFace, const unsigned int mipmapLevel, const BufferLocking lockMode);
		void		Unlock();
		void		Update();

		void		Bind();
		void		Unbind();

		IDirect3DBaseTexture9* const	GetTextureDX9() const { return m_pTexture; }

	private:
		TextureDX9(
			const PixelFormat texFormat, const TexType texType,
			const unsigned int sizeX, const unsigned int sizeY = 1, const unsigned int sizeZ = 1,
			const unsigned int mipmapLevelCount = 0, const BufferUsage usage = BU_TEXTURE);
		~TextureDX9();

		IDirect3DBaseTexture9* m_pTexture;

		// A temporary pointer used in the Lock->Update->Unlock flow
		void*			m_pTempBuffer;
		unsigned int	m_nRowPitch;
		unsigned int	m_nDepthPitch;

		friend class ResourceManagerDX9;
	};
}

#endif //TEXTUREDX9_H