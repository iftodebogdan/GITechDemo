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

#include "Buffer.h"

namespace LibRendererDll
{
	// This is the platform independent texture class
	// NB: for compressed texture formats, the smallest
	// addressable element is a block, instead of a texel
	class Texture : public Buffer
	{
	public:
				// Replaced by GetWidth/Height/Depth()
				const unsigned int GetElementCount() const { assert(false); return 0; }
				// Replaced by GetMipData()
				byte* GetData() const { assert(false); return nullptr; }

				// Get the format of the texture
				LIBRENDERER_DLL const	PixelFormat		GetPixelFormat() const;
				// Get the type of texture
				LIBRENDERER_DLL const	TextureType		GetTextureType() const;
				// Get the number of mip levels
				LIBRENDERER_DLL const	unsigned int	GetMipCount() const;

				/* Get the width of the specified mip level */
				LIBRENDERER_DLL const	unsigned int	GetWidth(const unsigned int mipmapLevel = 0) const;
				/* Get the height of the specified mip level */
				LIBRENDERER_DLL const	unsigned int	GetHeight(const unsigned int mipmapLevel = 0) const;
				/* Get the depth of the specified mip level */
				LIBRENDERER_DLL const	unsigned int	GetDepth(const unsigned int mipmapLevel = 0) const;
				/* Get the number of dimensions (width/height/depth) that the texture supports (texture type dependent) */
				LIBRENDERER_DLL const	unsigned int	GetDimensionCount() const;

				/* Get the size in bytes of the specified mip level */
				LIBRENDERER_DLL const	unsigned int	GetMipSizeBytes(const unsigned int mipmapLevel = 0) const;
				/* Get the offset in bytes from the beginning of the memory buffer in which the texture is stored to the specified mip level */
				LIBRENDERER_DLL const	unsigned int	GetMipOffset(const unsigned int mipmapLevel = 0) const;
				/* Get the offset in bytes of a cube face from the beginning of the memory buffer in which the texture is stored*/
				LIBRENDERER_DLL const	unsigned int	GetCubeFaceOffset() const;
		/* Get the index in the texture array of the specified cubemap face */
		virtual	LIBRENDERER_DLL const	unsigned int	GetCubeFaceIndex(const CubeFace cubeFace) const PURE_VIRTUAL
				/* Returns true if the format of the texture is a compressed format */
				LIBRENDERER_DLL const	bool			IsCompressed() const;
				/* Returns true if the format of the texture is a floating point format */
				LIBRENDERER_DLL const	bool			IsFloatingPoint() const;
				/* Returns true if the format of the texture is a depth stencil format */
				LIBRENDERER_DLL const	bool			IsDepthStencil() const;
				/* Returns true if the texture is a render target */
				LIBRENDERER_DLL	const	bool			IsRenderTarget() const;
				/* Returns true if the format of the texture allows it to be mipmapable */
				LIBRENDERER_DLL const	bool			IsMipmapable() const;
				/* Get a pointer to the start of the specified mip level in the memory buffer in which the texture is stored */
				LIBRENDERER_DLL 		byte*	const	GetMipData(const unsigned int mipmapLevel = 0);
				/* Get a pointer to the start of the specified face and mip level in the memory buffer in which a cube texture is stored */
				LIBRENDERER_DLL 		byte*	const	GetMipData(const CubeFace cubeFace, const unsigned int mipmapLevel = 0) const;

		// Enable the texture on the specified slot
		virtual LIBRENDERER_DLL void			Enable(const unsigned int texUnit) const PURE_VIRTUAL
		// Disable the texture from the specified slot
		virtual LIBRENDERER_DLL void			Disable(const unsigned int texUnit) const PURE_VIRTUAL
		// Lock the specified mipmap level for reading/writing
		virtual LIBRENDERER_DLL const	bool	Lock(const unsigned int mipmapLevel, const BufferLocking lockMode);
		// Lock the specified mipmap level of the specified cube face for reading/writing (cubemaps only!)
		virtual LIBRENDERER_DLL const	bool	Lock(const CubeFace cubeFace, const unsigned int mipmapLevel, const BufferLocking lockMode);
		// Unlock the texture
		virtual LIBRENDERER_DLL void			Unlock();
		// Update the locked mipmap level (of the locked face, if cube texture) with the changes made
		virtual LIBRENDERER_DLL void			Update() PURE_VIRTUAL

		// Create a corresponding platform specific resource
		virtual LIBRENDERER_DLL void			Bind();
		// Destroy the platform specific resource
		virtual LIBRENDERER_DLL void			Unbind() PURE_VIRTUAL

				// Get lock status
				LIBRENDERER_DLL const	bool			IsLocked() const;
				// Get the mipmap level which has been locked
				LIBRENDERER_DLL const	unsigned int	GetLockedMip() const;
				// Get the cube face level which has been locked
				LIBRENDERER_DLL const	CubeFace		GetLockedCubeFace() const;

				// Generate mipmaps (SLOW!)
				LIBRENDERER_DLL const	bool	GenerateMips();

				// Set various sampler states for when the texture is bound
				LIBRENDERER_DLL	void	SetAnisotropy(const float anisotropy);
				LIBRENDERER_DLL	void	SetMipLodBias(const float lodBias);
				LIBRENDERER_DLL	void	SetFilter(const SamplerFilter filter);
				LIBRENDERER_DLL	void	SetBorderColor(const Vec4f rgba);
				LIBRENDERER_DLL	void	SetAddressingModeU(const SamplerAddressingMode samU);
				LIBRENDERER_DLL	void	SetAddressingModeV(const SamplerAddressingMode samV);
				LIBRENDERER_DLL	void	SetAddressingModeW(const SamplerAddressingMode samW);
				LIBRENDERER_DLL	void	SetAddressingMode(const SamplerAddressingMode samUVW);
				LIBRENDERER_DLL	void	SetSRGBEnabled(const bool enabled);

				// Get various sampler states
				LIBRENDERER_DLL const float					GetAnisotropy() const;
				LIBRENDERER_DLL const float					GetMipLodBias() const;
				LIBRENDERER_DLL const SamplerFilter			GetFilter() const;
				LIBRENDERER_DLL const Vec4f					GetBorderColor() const;
				LIBRENDERER_DLL const SamplerAddressingMode	GetAddressingModeU() const;
				LIBRENDERER_DLL const SamplerAddressingMode	GetAddressingModeV() const;
				LIBRENDERER_DLL const SamplerAddressingMode	GetAddressingModeW() const;
				LIBRENDERER_DLL const SamplerAddressingMode	GetAddressingMode() const;
				LIBRENDERER_DLL	const bool					GetSRGBEnabled() const;

				// Retrieve the path to the file from which the texture was loaded
				LIBRENDERER_DLL	const	char*	GetSourceFileName() const;

		/* Get the number of dimensions a texture of the specified type has */
		static	LIBRENDERER_DLL const	unsigned int	GetDimensionCount(const TextureType texType);
		/* Returns true if the specified texture format is mipmapable */
		static	LIBRENDERER_DLL const	bool			IsMipmapable(const PixelFormat pixelFormat);
		/* Get the size in bytes of a pixel (or block, for compressed formats) from a texture of the specified format */
		static	LIBRENDERER_DLL const	unsigned int	GetBytesPerPixel(const PixelFormat pixelFormat);
		/* Validates the supplied pixel format against the device's capabilities and attempts to find the closest match if not compatible */
		static	LIBRENDERER_DLL void					ValidatePixelFormat(PixelFormat& pixelFormat, const TextureType texType, const BufferUsage usage);

	protected:
		Texture(
			const PixelFormat pixelFormat, const TextureType texType,
			const unsigned int sizeX, const unsigned int sizeY = 1, const unsigned int sizeZ = 1,
			const unsigned int mipCount = 0, const BufferUsage usage = BU_TEXTURE);
		virtual	~Texture();

		// Computes the properties of the texture and its mipmaps
		void			ComputeTextureProperties(const Vec3i dimensions);

		// Sets the width/height ratio relative to the backbuffer (for dynamic render targets)
		void			SetDynamicSizeRatios(const float widthRatio, const float heightRatio);

		PixelFormat		m_ePixelFormat;	// Holds the format of the texture
		TextureType			m_eTexType;		// Holds the type of texture
		unsigned int	m_nMipCount;	// Holds the number of mips

		/* Holds the number of valid dimensions based on the type */
		unsigned int			m_nDimensionCount;
		/* Holds the dimensions of each mip */
		Vec<unsigned int, 3U>	m_nDimension[TEX_MAX_MIPMAP_LEVELS];
		/* Holds the sizes in bytes of each mip level */
		unsigned int			m_nMipSizeBytes[TEX_MAX_MIPMAP_LEVELS];
		/* Holds the offsets in bytes of each mip */
		unsigned int			m_nMipOffset[TEX_MAX_MIPMAP_LEVELS];

		/* Lock status */
		bool			m_bIsLocked;
		unsigned int	m_nLockedMip;
		CubeFace		m_eLockedCubeFace;

		/* Sampler states */
		SamplerStateDesc	m_tSamplerStates;

		/* Path to the file from which the texture was loaded from */
		std::string		m_szSourceFile;

		/* This texture is a render target with dynamic resolution, sync'ed to the backbuffer's resolution */
		bool	m_bIsDynamicRT;
		float	m_fWidthRatio;
		float	m_fHeightRatio;

		friend class ResourceManager;
		friend class RenderTarget;

		LIBRENDERER_DLL friend std::ostream& operator<<(std::ostream& output_out, Texture &tex_in);
		friend std::istream& operator>>(std::istream& s_in, Texture &tex_out);
	};
}

#endif // TEXTURE_H