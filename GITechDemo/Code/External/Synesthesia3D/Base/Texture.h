/**
 * @file        Texture.h
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

#ifndef TEXTURE_H
#define TEXTURE_H

#include "Buffer.h"

namespace Synesthesia3D
{
    /**
     * @brief   Describes a texture/surface object.
     */
    class Texture : public Buffer
    {

    public:

        /**
         * @warning     Use @ref GetWidth(), @ref GetHeight() and @ref GetDepth() instead.
         *
         * @return  The number of pixels.
         */
                const unsigned int GetElementCount() const { assert(false); return Buffer::GetElementCount(); }

        /**
         * @warning     Use @ref GetMipData() instead.
         *
         * @return  Pointer to the texture data.
         */
                s3dByte* GetData()      const { assert(false); return Buffer::GetData(); }

        /**
         * @brief   Retrieves the format of the texture.
         *
         * @return  The texture's pixel format.
         */
                SYNESTHESIA3D_DLL   const   PixelFormat     GetPixelFormat() const;

        /**
         * @brief   Retrieves the type of texture.
         *
         * @return  The type of the texture.
         */
                SYNESTHESIA3D_DLL   const   TextureType     GetTextureType() const;

        /**
         * @brief   Retrieves the number of mip levels.
         *
         * @return  The number of mip levels.
         */
                SYNESTHESIA3D_DLL   const   unsigned int    GetMipCount() const;



        /**
         * @brief   Retrieves the width of the specified mip level.
         *
         * @param[in]   mipmapLevel     The mip level for which to retrieve the information.
         *
         * @return  The width of the specified mip level.
         */
                SYNESTHESIA3D_DLL   const   unsigned int    GetWidth(const unsigned int mipmapLevel = 0) const;

        /**
         * @brief   Retrieves the height of the specified mip level
         *
         * @param[in]   mipmapLevel     The mip level for which to retrieve the information.
         *
         * @return  The height of the specified mip level.
         */
                SYNESTHESIA3D_DLL   const   unsigned int    GetHeight(const unsigned int mipmapLevel = 0) const;

        /**
         * @brief   Retrieves the depth of the specified mip level.
         *
         * @param[in]   mipmapLevel     The mip level for which to retrieve the information.
         *
         * @return  The depth of the specified mip level.
         */
                SYNESTHESIA3D_DLL   const   unsigned int    GetDepth(const unsigned int mipmapLevel = 0) const;

        /**
         * @brief   Retrieves the number of dimensions (width/height/depth) that the texture supports (texture type dependent).
         *
         * @return  The number of dimensions supported by the texture type.
         */
                SYNESTHESIA3D_DLL   const   unsigned int    GetDimensionCount() const;



        /**
         * @brief   Retrieves the size in bytes of the specified mip level.
         *
         * @param[in]   mipmapLevel     The mip level for which to retrieve the information.
         *
         * @return  The size, in bytes, of the specified mip level.
         */
                SYNESTHESIA3D_DLL   const   unsigned int    GetMipSizeBytes(const unsigned int mipmapLevel = 0) const;

        /**
         * @brief   Retrieves the offset in bytes from the beginning of the memory buffer in which the texture is stored to the specified mip level.
         *
         * @param[in]   mipmapLevel     The mip level for which to retrieve the information.
         *
         * @return  The offset, in bytes, of a mip level.
         */
                SYNESTHESIA3D_DLL   const   unsigned int    GetMipOffset(const unsigned int mipmapLevel = 0) const;



        /**
         * @brief   Retrieves the offset in bytes of a cube face from the beginning of the memory buffer in which the texture is stored.
         *
         * @return  The offset, in bytes, of a cube face.
         */
                SYNESTHESIA3D_DLL   const   unsigned int    GetCubeFaceOffset() const;

        /**
         * @brief   Retrieves the index in the texture array of the specified cubemap face.
         *
         * @param[in]   cubeFace    The cube face for which to retrieve the information.
         *
         * @return  The index in the texture array of the specified cubemap face.
         */
        virtual SYNESTHESIA3D_DLL   const   unsigned int    GetCubeFaceIndex(const CubeFace cubeFace) const PURE_VIRTUAL;

        

        /**
         * @brief   Returns true if the format of the texture is a compressed format.
         *
         * @return  Whether the texture is compressed.
         */
                SYNESTHESIA3D_DLL   const   bool            IsCompressed() const;

        /**
         * @brief   Returns true if the format of the texture is of a floating point format.
         *
         * @return  Whether the texture is of a floating point format.
         */
                SYNESTHESIA3D_DLL   const   bool            IsFloatingPoint() const;



        /**
         * @brief   Returns true if the format of the texture is of a depth stencil format.
         *
         * @return  Whether the texture is of a depth-stencil format.
         */
                SYNESTHESIA3D_DLL   const   bool            IsDepthStencil() const;

        /**
         * @brief   Returns true if the texture is a render target.
         *
         * @return  Whether the texture is a render target.
         */
                SYNESTHESIA3D_DLL   const   bool            IsRenderTarget() const;



        /**
         * @brief   Returns true if the format of the texture allows it to be mipmapable.
         *
         * @return  Whether the texture can have a mip chain.
         */
                SYNESTHESIA3D_DLL   const   bool            IsMipmapable() const;

        /**
         * @brief   Retrieves a pointer to the start of the specified mip level in the memory buffer in which the texture is stored.
         *
         * @param[in]   mipmapLevel     The mip level for which to retrieve the information.
         *
         * @return  Pointer to the specified mip level data.
         */
                SYNESTHESIA3D_DLL       s3dByte*    const   GetMipData(const unsigned int mipmapLevel = 0);

        /**
         * @brief   Retrieves a pointer to the start of the specified face and mip level in the memory buffer in which a cube texture is stored.
         *
         * @param[in]   cubeFace        The cube face for which to retrieve the information.
         * @param[in]   mipmapLevel     The mip level for which to retrieve the information.
         *
         * @return  Pointer to the specified mip level data of the specified cube face.
         */
                SYNESTHESIA3D_DLL       s3dByte*    const   GetMipData(const CubeFace cubeFace, const unsigned int mipmapLevel = 0) const;



        /**
         * @brief   Binds the texture on the specified slot.
         *
         * @param[in]   texUnit     The texture slot to which to bind the texture.
         */
        virtual SYNESTHESIA3D_DLL void          Enable(const unsigned int texUnit) const PURE_VIRTUAL;

        /**
         * @brief   Unbinds the texture from the specified slot.
         *
         * @param[in]   texUnit     The texture slot from which to unbind the texture.
         */
        virtual SYNESTHESIA3D_DLL void          Disable(const unsigned int texUnit) const PURE_VIRTUAL;

        /**
         * @brief   Locks the specified mipmap level for reading/writing.
         * @note    For use with non-cubemap textures only!
         *
         * @param[in]   mipmapLevel     The mip level to lock.
         * @param[in]   lockMode        The locking method. See @ref BufferLocking.
         *
         * @return  Success of operation.
         */
        virtual SYNESTHESIA3D_DLL const bool    Lock(const unsigned int mipmapLevel, const BufferLocking lockMode);

        /**
         * @brief   Lock the specified mipmap level of the specified cube face for reading/writing.
         * @note    For use with cubemaps only!
         *
         * @param[in]   cubeFace        The cube face to lock.
         * @param[in]   mipmapLevel     The mip level to lock.
         * @param[in]   lockMode        The locking method. See @ref BufferLocking.
         *
         * @return  Success of operation.
         */
        virtual SYNESTHESIA3D_DLL const bool    Lock(const CubeFace cubeFace, const unsigned int mipmapLevel, const BufferLocking lockMode);

        /**
         * @brief   Unlocks the texture, flushing the changes to memory.
         */
        virtual SYNESTHESIA3D_DLL void          Unlock();

        /**
         * @brief   Update the locked mipmap level (of the locked face, if cube texture) with the changes made.
         * @note    This operates on a temporary buffer. Use @ref Unlock() to commit changes to memory.
         */
        virtual SYNESTHESIA3D_DLL void          Update() PURE_VIRTUAL;

        /**
         * @brief   Creates a corresponding platform specific resource.
         */
        virtual SYNESTHESIA3D_DLL void          Bind();

        /**
         * @brief   Destroys the platform specific resource.
         */
        virtual SYNESTHESIA3D_DLL void          Unbind() PURE_VIRTUAL;

        /**
         * @brief   Retrieves the lock status.
         */
                SYNESTHESIA3D_DLL const bool            IsLocked() const;

        /**
         * @brief   Retrieves the mipmap level which has been locked.
         */
                SYNESTHESIA3D_DLL const unsigned int    GetLockedMip() const;

        /**
         * @brief   Retrieves the cube face level which has been locked.
         */
                SYNESTHESIA3D_DLL const CubeFace        GetLockedCubeFace() const;


        /**
         * @brief   Generates mipmaps.
         * @warning As it is implemented on the CPU, it can be very slow!
         *
         * @return  Success of operation.
         */
                SYNESTHESIA3D_DLL const bool    GenerateMips();
                


        /**
         * @brief   Sets the anisotropic filter quality level.
         *
         * @param[in]   anisotropy  Anisotropic filtering level.
         */
                SYNESTHESIA3D_DLL   void    SetAnisotropy(const unsigned int anisotropy);

        /**
         * @brief   Sets the mip LOD bias.
         *
         * @param[in]   lodBias     Texture LOD bias.
         */
                SYNESTHESIA3D_DLL   void    SetMipLodBias(const float lodBias);

        /**
         * @brief   Sets the texture filtering type.
         *
         * @param[in]   filter      Filtering type.
         *
         * @see     SamplerFilter
         */
                SYNESTHESIA3D_DLL   void    SetFilter(const SamplerFilter filter);

        /**
         * @brief   Sets the color of the border.
         * @note    Has effect if addressing mode is @ref SAM_BORDER.
         *
         * @param[in]   rgba        Color of border.
         */
                SYNESTHESIA3D_DLL   void    SetBorderColor(const Vec4f rgba);

        /**
         * @brief   Sets the texture addressing mode on the U axis.
         *
         * @param[in]   samU        Addressing mode on the U axis.
         */
                SYNESTHESIA3D_DLL   void    SetAddressingModeU(const SamplerAddressingMode samU);

        /**
         * @brief   Sets the texture addressing mode on the V axis.
         *
         * @param[in]   samV        Addressing mode on the V axis.
         */
                SYNESTHESIA3D_DLL   void    SetAddressingModeV(const SamplerAddressingMode samV);

        /**
         * @brief   Sets the texture addressing mode on the W axis.
         *
         * @param[in]   samW        Addressing mode on the W axis.
         */
                SYNESTHESIA3D_DLL   void    SetAddressingModeW(const SamplerAddressingMode samW);

        /**
         * @brief   Sets the texture addressing mode.
         *
         * @param[in]   samUVW      Addressing mode on all axes.
         */
                SYNESTHESIA3D_DLL   void    SetAddressingMode(const SamplerAddressingMode samUVW);

        /**
         * @brief   Enables gamma correction when sampling from the texture.
         * @note    Textures that have content encoded in gamma space (sRGB encoded)
         *          should have this state enabled so as to linearize it when sampling.
         *
         * @param[in]   enabled     Enable sRGB for texture.
         */
                SYNESTHESIA3D_DLL   void    SetSRGBEnabled(const bool enabled);



        /**
         * @brief   Retrieves the anisotropic filter quality level.
         */
                SYNESTHESIA3D_DLL const unsigned int            GetAnisotropy() const;

        /**
         * @brief   Retrieves the mip LOD bias.
         */
                SYNESTHESIA3D_DLL const float                   GetMipLodBias() const;

        /**
         * @brief   Retrieves the texture filtering type.
         */
                SYNESTHESIA3D_DLL const SamplerFilter           GetFilter() const;

        /**
         * @brief   Retrieves the color of the border.
         */
                SYNESTHESIA3D_DLL const Vec4f                   GetBorderColor() const;

        /**
         * @brief   Retrieves the texture addressing mode on the U axis.
         */
                SYNESTHESIA3D_DLL const SamplerAddressingMode   GetAddressingModeU() const;

        /**
         * @brief   Retrieves the texture addressing mode on the V axis.
         */
                SYNESTHESIA3D_DLL const SamplerAddressingMode   GetAddressingModeV() const;

        /**
         * @brief   Retrieves the texture addressing mode on the W axis.
         */
                SYNESTHESIA3D_DLL const SamplerAddressingMode   GetAddressingModeW() const;

        /**
         * @brief   Retrieves the texture addressing mode.
         */
                SYNESTHESIA3D_DLL const SamplerAddressingMode   GetAddressingMode() const;

        /**
         * @brief   Checks whether the texture is sRGB encoded.
         */
                SYNESTHESIA3D_DLL   const bool                  GetSRGBEnabled() const;



        /**
         * @brief   Retrieves the path to the file from which the texture was loaded.
         */
                SYNESTHESIA3D_DLL   const   char*   GetSourceFileName() const;



        /**
         * @brief   Retrieves the number of dimensions a texture of the specified type has.
         *
         * @param[in]   texType     The type of the texture. See @ref TextureType.
         *
         * @return  The number of dimensions of the specified texture type.
         */
        static  SYNESTHESIA3D_DLL const unsigned int    GetDimensionCount(const TextureType texType);

        /**
         * @brief   Returns true if the specified texture format is mipmapable.
         *
         * @param[in]   pixelFormat     The format of the texture. See @ref PixelFormat.
         *
         * @return  Whether the texture can have a mip chain.
         */
        static  SYNESTHESIA3D_DLL const bool            IsMipmapable(const PixelFormat pixelFormat);

        /**
         * @brief   Retrieves the size in bytes of a pixel (or block, for compressed formats) from a texture of the specified format.
         *
         * @param[in]   pixelFormat     The format of the texture. See @ref PixelFormat.
         *
         * @return  Number of bytes required to represent a single pixel of the specified pixel format.
         */
        static  SYNESTHESIA3D_DLL const unsigned int    GetBytesPerPixel(const PixelFormat pixelFormat);

        /**
         * @brief   Validates the supplied pixel format against the device's capabilities and attempts to find the closest match if not compatible
         *
         * @param[in]   pixelFormat     The format of the texture. See @ref PixelFormat.
         * @param[in]   texType         The type of the texture. See @ref TextureType.
         * @param[in]   usage           The usage mode. See @ref BufferUsage.
         */
        static  SYNESTHESIA3D_DLL void                  ValidatePixelFormat(PixelFormat& pixelFormat, const TextureType texType, const BufferUsage usage);

    protected:
        
        /**
         * @brief   Constructor.
         *
         * @details Meant to be used only by @ref ResourceManager.
         * @see     ResourceManager::CreateTexture()
         *
         * @param[in]   pixelFormat     The format of the texture. See @ref PixelFormat.
         * @param[in]   texType         The type of the texture. See @ref TextureType.
         * @param[in]   sizeX           The width of the texture.
         * @param[in]   sizeY           The height of the texture.
         * @param[in]   sizeZ           The depth of the texture.
         * @param[in]   mipCount        The number of mips. Use 0 for max, 1 for no mips etc.
         * @param[in]   usage           The usage mode. See @ref BufferUsage.
         */
        Texture(
            const PixelFormat pixelFormat, const TextureType texType,
            const unsigned int sizeX, const unsigned int sizeY = 1, const unsigned int sizeZ = 1,
            const unsigned int mipCount = 0, const BufferUsage usage = BU_TEXTURE);
        
        /**
         * @brief   Destructor.
         *
         * @details Meant to be used only by @ref ResourceManager.
         * @see     ResourceManager::ReleaseTexture()
         */
        virtual ~Texture();

        /**
         * @brief   Computes the properties of the texture and its mipmaps.
         *
         * @param[in]   dimensions  The width, height and depth of the texture.
         */
        void            ComputeTextureProperties(const Vec3i dimensions);

        /**
         * @brief   Sets the width/height ratio relative to the backbuffer (for dynamic render targets).
         *
         * @param[in]   widthRatio      The texture's width ratio relative to the back buffer's width.
         * @param[in]   heightRatio     The texture's height ratio relative to the back buffer's height.
         */
        void            SetDynamicSizeRatios(const float widthRatio, const float heightRatio);

        PixelFormat     m_ePixelFormat; /**< @brief Holds the format of the texture. */
        TextureType     m_eTexType;     /**< @brief Holds the type of texture. */
        unsigned int    m_nMipCount;    /**< @brief Holds the number of mips. */

        unsigned int            m_nDimensionCount; /**< @brief Holds the number of valid dimensions based on the type. */
        Vec<unsigned int, 3U>   m_nDimension[TEX_MAX_MIPMAP_LEVELS]; /**< @brief Holds the dimensions of each mip. */
        unsigned int            m_nMipSizeBytes[TEX_MAX_MIPMAP_LEVELS]; /**< @brief Holds the sizes in bytes of each mip level. */
        unsigned int            m_nMipOffset[TEX_MAX_MIPMAP_LEVELS]; /**< @brief Holds the offsets in bytes of each mip. */

        bool            m_bIsLocked;        /**< @brief Lock status. */
        unsigned int    m_nLockedMip;       /**< @brief Locked mip. */
        CubeFace        m_eLockedCubeFace;  /**< @brief Locked cube face. */

        SamplerStateDesc    m_tSamplerStates; /**< @brief Sampler states to be set when texture is bound. */

        std::string     m_szSourceFile; /**< @brief Path to the file from which the texture was loaded from. */

        bool    m_bIsDynamicRT; /**< @brief Whether this texture is a render target with dynamic resolution, sync'ed to the back buffer's resolution. */
        float   m_fWidthRatio;  /**< @brief Ratio of width of texture to width of back buffer. */
        float   m_fHeightRatio; /**< @brief Ratio of height of texture to height of back buffer. */

        friend class ResourceManager;
        friend class RenderTarget;

        /**
         * @brief   Serializes the texture object.
         */
        SYNESTHESIA3D_DLL friend std::ostream& operator<<(std::ostream& output_out, Texture &tex_in);
        
        /**
         * @brief   Deserializes the texture object.
         */
        friend std::istream& operator>>(std::istream& s_in, Texture &tex_out);
    };
}

#endif // TEXTURE_H