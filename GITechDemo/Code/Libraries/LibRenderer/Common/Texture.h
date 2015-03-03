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
#ifndef TEXTURE_H
#define TEXTURE_H

#ifndef LIBRENDERER_DLL
#ifdef LIBRENDERER_EXPORTS
#define LIBRENDERER_DLL __declspec(dllexport) 
#else
#define LIBRENDERER_DLL __declspec(dllimport) 
#endif
#endif // LIBRENDERER_DLL

#include <gmtl\gmtl.h>
using namespace gmtl;

#include "Buffer.h"

namespace LibRendererDll
{
	// This is the platform independent texture class
	class Texture : public Buffer
	{
	public:
			#if __cplusplus >= 201103L // Check if the compiler supports C++11 standard
				// Replaced by GetMipmapLevelDimensions()
				LIBRENDERER_DLL const	unsigned int	GetElementCount() const = delete;
				// Replaced by GetPixelSize()
				LIBRENDERER_DLL const	unsigned int	GetElementSize() const = delete;
				// Replaced by GetMipmapLevelByteCount()
				LIBRENDERER_DLL const	unsigned int	GetSize() const = delete;
				// Replaced by GetMipmapLevelData()
				LIBRENDERER_DLL 		byte*			GetData() const = delete;
			#else
				// Replaced by GetMipmapLevelDimensions()
				LIBRENDERER_DLL const	unsigned int	GetElementCount() const { assert(false); return 0; }
				// Replaced by GetPixelSize()
				LIBRENDERER_DLL const	unsigned int	GetElementSize() const { assert(false); return 0; }
				// Replaced by GetMipmapLevelByteCount()
				LIBRENDERER_DLL const	unsigned int	GetSize() const { assert(false); return 0; }
				// Replaced by GetMipmapLevelData()
				LIBRENDERER_DLL 		byte*			GetData() const { assert(false); return nullptr; }
			#endif // __cplusplus >= 201103L

				// Get the format of the texture
				LIBRENDERER_DLL const	PixelFormat		GetTextureFormat() const { return m_eTexFormat; }
				// Get the type of texture
				LIBRENDERER_DLL const	TexType			GetTextureType() const { return m_eTexType; }
				// Get the number of mip levels
				LIBRENDERER_DLL const	unsigned int	GetMipmapLevelCount() const { return m_nMipmapLevelCount; }

				/* Get the width of the specified mip level */
				LIBRENDERER_DLL const	unsigned int	GetWidth(const unsigned int mipmapLevel = 0) const { return m_nDimension[mipmapLevel][0]; }
				/* Get the height of the specified mip level */
				LIBRENDERER_DLL const	unsigned int	GetHeight(const unsigned int mipmapLevel = 0) const { return m_nDimension[mipmapLevel][1]; }
				/* Get the depth of the specified mip level */
				LIBRENDERER_DLL const	unsigned int	GetDepth(const unsigned int mipmapLevel = 0) const { return m_nDimension[mipmapLevel][2]; }
				/* Get the number of dimensions (width/height/depth) that the texture supports (texture type dependent) */
				LIBRENDERER_DLL const	unsigned int	GetDimensionCount() const { return m_nDimensionCount; }

				/* Get the size in bytes of the specified mip level */
				LIBRENDERER_DLL const	unsigned int	GetMipmapLevelByteCount(const unsigned int mipmapLevel = 0) const { assert(mipmapLevel < TEX_MAX_MIPMAP_LEVELS); return m_nMipmapLevelByteCount[mipmapLevel]; }
				/* Get the offset in bytes from the beginning of the memory buffer in which the texture is stored to the specified mip level */
				LIBRENDERER_DLL const	unsigned int	GetMipmapLevelOffset(const unsigned int mipmapLevel = 0) const { assert(mipmapLevel < TEX_MAX_MIPMAP_LEVELS); return m_nMipmapLevelOffset[mipmapLevel]; }
				/* Get the offset in bytes of a cube face from the beginning of the memory buffer in which the texture is stored*/
				LIBRENDERER_DLL const	unsigned int	GetCubeFaceOffset() const { assert(m_eTexType == TT_CUBE); return m_nSize / 6; }
				/* Get the pixel size in bytes */
				LIBRENDERER_DLL const	unsigned int	GetPixelSize() const { return Buffer::GetElementSize(); }
				/* Returns true if the format of the texture is a compressed format */
				LIBRENDERER_DLL const	bool			IsCompressed() const { return m_eTexFormat == PF_DXT1 || m_eTexFormat == PF_DXT3 || m_eTexFormat == PF_DXT5; }
				/* Returns true if the format of the texture allows it to be mipmapable */
				LIBRENDERER_DLL const	bool			IsMipmapable() const { return ms_bIsMipmapable[m_eTexFormat]; }
				/* Get a pointer to the start of the specified mip level in the memory buffer in which the texture is stored */
				LIBRENDERER_DLL 		byte*			GetMipmapLevelData(const unsigned int mipmapLevel = 0) const { return m_pData + GetMipmapLevelOffset(mipmapLevel); }
				/* Get a pointer to the start of the specified face and mip level in the memory buffer in which a cube texture is stored */
				LIBRENDERER_DLL 		byte*			GetMipmapLevelData(const unsigned int cubeFace, const unsigned int mipmapLevel) const { assert(cubeFace >= 0 && cubeFace < 6); return m_pData + cubeFace * GetCubeFaceOffset() + GetMipmapLevelOffset(mipmapLevel); }

		// Enable the texture on the specified slot
		virtual LIBRENDERER_DLL void			Enable(const unsigned int texUnit) const = 0;
		// Disable the texture from the specified slot
		virtual LIBRENDERER_DLL void			Disable(const unsigned int texUnit) const = 0;
		// Lock the specified mipmap level for reading/writing
		virtual LIBRENDERER_DLL const	bool	Lock(const unsigned int mipmapLevel, const BufferLocking lockMode) { assert(!m_bIsLocked); m_bIsLocked = true; m_nLockedMipmap = mipmapLevel; m_nLockedCubeFace = 0; return true; }
		// Lock the specified mipmap level of the specified cube face for reading/writing (cubemaps only!)
		virtual LIBRENDERER_DLL const	bool	Lock(const unsigned int cubeFace, const unsigned int mipmapLevel, const BufferLocking lockMode) { assert(!m_bIsLocked); m_bIsLocked = true; m_nLockedMipmap = mipmapLevel; m_nLockedCubeFace = cubeFace; return true; }
		// Unlock the texture
		virtual LIBRENDERER_DLL void			Unlock() { assert(m_bIsLocked); m_bIsLocked = false; m_nLockedMipmap = -1; m_nLockedCubeFace = -1; }
		// Update the locked mipmap level (of the locked face, if cube texture) with the changes made
		virtual LIBRENDERER_DLL void			Update() = 0;

		// Create a corresponding platform specific resource
		virtual LIBRENDERER_DLL void			Bind() = 0;
		// Destroy the platform specific resource
		virtual LIBRENDERER_DLL void			Unbind() = 0;

				// Get lock status
				LIBRENDERER_DLL const	bool			IsLocked() const { return m_bIsLocked; }
				// Get the mipmap level which has been locked
				LIBRENDERER_DLL const	unsigned int	GetLockedMipmap() const { assert(m_bIsLocked); return m_nLockedMipmap; }
				// Get the cube face level which has been locked
				LIBRENDERER_DLL const	unsigned int	GetLockedCubeFace() const { assert(m_bIsLocked); return m_nLockedCubeFace; }

				// Generate mipmaps (SLOW!)
				LIBRENDERER_DLL const	bool	GenerateMipmaps();
				// Check for mipmap autogeneration
				LIBRENDERER_DLL const	bool	UsingHardwareAcceleratedMipmapGeneration() { return m_bAutogenMipmaps; }

		/* Get the number of dimensions a texture of the specified type has */
		static	LIBRENDERER_DLL const	unsigned int	GetDimensionCount(const TexType texType) { return ms_nDimensionCount[texType]; }
		/* Returns true if the specified texture format is mipmapable */
		static	LIBRENDERER_DLL const	bool			IsMipmapable(const PixelFormat texFormat) { return ms_bIsMipmapable[texFormat]; }
		/* Get the size in bytes of a pixel from a texture of the specified format */
		static	LIBRENDERER_DLL const	unsigned int	GetPixelSize(const PixelFormat texFormat) { return ms_nPixelSize[texFormat]; }

	protected:
		Texture(
			const PixelFormat texFormat, const TexType texType,
			const unsigned int sizeX, const unsigned int sizeY = 1, const unsigned int sizeZ = 1,
			const unsigned int mipmapLevelCount = 0, const BufferUsage usage = BU_TEXTURE);
		virtual							~Texture();

		// Computes the properties of the texture and its mipmaps
		void			ComputeMipmapProperties();

		PixelFormat		m_eTexFormat;			// Holds the format of the texture
		TexType			m_eTexType;				// Holds the type of texture
		unsigned int	m_nMipmapLevelCount;	// Holds the number of mips

		/* Holds the number of valid dimensions based on the type */
		unsigned int			m_nDimensionCount;
		/* Holds the dimensions of each mip */
		Vec<unsigned int, 3U>	m_nDimension[TEX_MAX_MIPMAP_LEVELS];
		/* Holds offsets in bytes from the start of the texture */
		unsigned int			m_nMipmapLevelByteCount[TEX_MAX_MIPMAP_LEVELS];
		/* Holds the offsets in bytes from the beginning of the texture and to the start */
		unsigned int			m_nMipmapLevelOffset[TEX_MAX_MIPMAP_LEVELS];

		/* Hardware accelerated mipmap generation */
		bool	m_bAutogenMipmaps;

		/* Lock status */
		bool			m_bIsLocked;
		unsigned int	m_nLockedMipmap;
		unsigned int	m_nLockedCubeFace;

		/* Holds the number of valid dimensions for the specified texture type */
		static	const unsigned int	ms_nDimensionCount[TT_MAX];
		/* Holds the size in bytes of a pixel for the specified texture format */
		static	const unsigned int	ms_nPixelSize[PF_MAX];
		/* Holds whether the specified texture format is mipmapable */
		static	const bool			ms_bIsMipmapable[PF_MAX];

		friend class ResourceManager;
	};
}

#endif // TEXTURE_H