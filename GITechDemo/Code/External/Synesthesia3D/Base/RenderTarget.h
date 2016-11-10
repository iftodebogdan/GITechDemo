/**
 *	@file		RenderTarget.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *	@copyright
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	@copyright
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *	@copyright
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "ResourceData.h"

namespace Synesthesia3D
{
	class Texture;

	/**
	 * @brief	Render target resource class that manages color and depth surfaces.
	 */
	class RenderTarget
	{

	public:

		/**
		 * @brief	Enables rendering to this render target.
		 *
		 * @details	Every @ref Enable() call must be paired with a corresponding @ref Disable() call.
		 *			Every time a render target is enabled, it is pushed on the stack. As such, disabling
		 *			the last set render target will enable the second to last render target automatically.
		 *			If there are no more render targets on the stack, disabling a render target will cause
		 *			the back buffer to be set in its place.
		 *
		 * @warning	Calling the @ref Disable() function of another render target than the currently active one
		 *			can cause undefined behaviour. Make sure you always disable the <B>active</B> render target.
		 *			In order to safely pop from the render target stack, use @ref GetActiveRenderTarget().
		 */
		virtual	SYNESTHESIA3D_DLL			void		Enable();

		/**
		 * @brief	Disables rendering to this render target.
		 *
		 * @details	Every @ref Enable() call must be paired with a corresponding @ref Disable() call.
		 *			Every time a render target is enabled, it is pushed on the stack. As such, disabling
		 *			the last set render target will enable the second to last render target automatically.
		 *			If there are no more render targets on the stack, disabling a render target will cause
		 *			the back buffer to be set in its place.
		 *
		 * @warning	Calling the @ref Disable() function of another render target than the currently active one
		 *			can cause undefined behaviour. Make sure you always disable the <B>active</B> render target.
		 *			In order to safely pop from the render target stack, use @ref GetActiveRenderTarget().
		 */
		virtual	SYNESTHESIA3D_DLL			void		Disable();

		/**
		 * @brief	Copies the contents of the specified color buffer to a texture.
		 *
		 * @note	The provided texture must be a 2D texture and have the same width, height
		 *			and pixel format as the render target. Otherwise, nothing will be copied.
		 *			For render targets with automatic mip generation only mip 0 is copied.
		 *
		 * @warning	As an optimization for memory footprint, render targets do not allocate
		 *			memory for their shadow copy when they are created. Instead, that memory
		 *			is allocated only when required (for example, when using @ref CopyColorBuffer()).
		 *			If you wish to force the initialization of the shadow copy at a more convenient
		 *			time, retrieve the texture ID using @ref GetColorBuffer() and use it to retrieve
		 *			a pointer to the actual texture using @ref ResourceManager::GetTexture().
		 *			After retrieving the texture, a simple call to @ref Texture::GetMipData() will
		 *			allocate the memory for the render target's shadow copy.
		 *
		 * @param[in]		colorBufferIdx	The index of the render target's color buffer, for MRTs
		 * @param[in,out]	texture			The texture in which the specified render target will be copied.
		 */
		virtual	SYNESTHESIA3D_DLL			void		CopyColorBuffer(const unsigned int colorBufferIdx, Texture* texture) PURE_VIRTUAL;
		
		/**
		 * @brief	Creates the corresponding platform specific resource.
		 * @note	Not really of much interest to the average user. Mainly used by the @ref ResourceManager or in the device reset flow.
		 */
		virtual	SYNESTHESIA3D_DLL			void		Bind();
		
		/**
		 * @brief	Destroys the platform specific resource.
		 * @note	Not really of much interest to the average user. Mainly used by the @ref ResourceManager or in the device reset flow.
		 */
		virtual	SYNESTHESIA3D_DLL			void		Unbind();

		/**
		 * @brief	Retrieves the number of color surfaces in this render target, as is the case with MRTs.
		 */
				SYNESTHESIA3D_DLL	const unsigned int	GetTargetCount() const;

		/**
		 * @brief	Retrieves the pixel format of the color target.
		 */
				SYNESTHESIA3D_DLL	const PixelFormat	GetPixelFormat(const unsigned int colorBufferIdx = 0) const;

		/**
		 * @brief	Retrieves the width of the render target.
		 */
				SYNESTHESIA3D_DLL	const unsigned int	GetWidth() const;

		/**
		 * @brief	Retrieves the height of the render target.
		 */
				SYNESTHESIA3D_DLL	const unsigned int	GetHeight() const;

		/**
		 * @brief	Retrieves the width and height of the render target.
		 */
				SYNESTHESIA3D_DLL		const Vec2i		GetSize() const;

		/**
		 * @brief	Retrieves the ID for the texture corresponding to the specified color buffer.
		 *
		 * @param[in]	colorBufferIdx		Index of the color buffer to be retrieved.
		 *
		 * @return	The resource ID for the color buffer.
		 *
		 * @note	Use the @ref ResourceManager to retrieve a pointer to the texture.
		 */
				SYNESTHESIA3D_DLL	const unsigned int	GetColorBuffer(const unsigned int colorBufferIdx = 0) const;
				
		/**
		 * @brief	Retrieves the ID for the texture corresponding to the depth buffer.
		 *
		 * @return	The resource ID for the depth buffer.
		 *
		 * @note	Use the @ref ResourceManager to retrieve a pointer to the texture.
		 */
				SYNESTHESIA3D_DLL	const unsigned int	GetDepthBuffer() const;
				
		/**
		 * @brief	Determines if the render target has mipmaps.
		 *
		 * @return	Presence of mipmaps.
		 *
		 * @note	Use the @ref ResourceManager to retrieve a pointer to the texture.
		 */
				SYNESTHESIA3D_DLL		const bool		HasMipmaps() const;

		/**
		 * @brief	Determines if the render target has a depth buffer.
		 */
				SYNESTHESIA3D_DLL		const bool		HasDepthBuffer() const;

		/**
		 * @brief	Retrieves the currently active render target.
		 *
		 * @return	Pointer to the active render target.
		 *
		 * @note	Returns nullptr if the current active render target is the back buffer.
		 */
		static	SYNESTHESIA3D_DLL		RenderTarget*	GetActiveRenderTarget();

	protected:

		/**
		 * @brief	Constructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::CreateRenderTarget()
		 *
		 * @param[in]	targetCount			The number of color render targets.
		 * @param[in]	pixelFormat			The pixel format of the color render targets.
		 * @param[in]	width				The width of the render targets.
		 * @param[in]	height				The height of the render targets.
		 * @param[in]	hasMipmaps			The render targets have auto generated mip-maps.
		 * @param[in]	hasDepthStencil		A depth-stencil target is also present.
		 * @param[in]	depthStencilFormat	The pixel format of the depth-stencil target.
		 */
		RenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
			const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);
		
		/**
		 * @brief	Constructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::CreateRenderTarget()
		 *
		 * @param[in]	targetCount			The number of color render targets.
		 * @param[in]	pixelFormat			The pixel format of the color render targets.
		 * @param[in]	widthRatio			The width of the render targets as a ratio of the width of the back buffer.
		 * @param[in]	heightRatio			The height of the render targets as a ratio of the height of the back buffer.
		 * @param[in]	hasMipmaps			The render targets have auto generated mip-maps.
		 * @param[in]	hasDepthStencil		A depth-stencil target is also present.
		 * @param[in]	depthStencilFormat	The pixel format of the depth-stencil target.
		 */
		RenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
			const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);
		
		/**
		 * @brief	Constructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::CreateRenderTarget()
		 *
		 * @param[in]	targetCount			The number of color render targets.
		 * @param[in]	pixelFormatRT0		The pixel format of the first color render targets.
		 * @param[in]	pixelFormatRT1		The pixel format of the second color render targets.
		 * @param[in]	pixelFormatRT2		The pixel format of the third color render targets.
		 * @param[in]	pixelFormatRT3		The pixel format of the fourth color render targets.
		 * @param[in]	width				The width of the render targets.
		 * @param[in]	height				The height of the render targets.
		 * @param[in]	hasMipmaps			The render targets have auto generated mip-maps.
		 * @param[in]	hasDepthStencil		A depth-stencil target is also present.
		 * @param[in]	depthStencilFormat	The pixel format of the depth-stencil target.
		 */
		RenderTarget(const unsigned int targetCount,
			PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
			const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);
		
		/**
		 * @brief	Constructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::CreateRenderTarget()
		 *
		 * @param[in]	targetCount			The number of color render targets.
		 * @param[in]	pixelFormatRT0		The pixel format of the first color render targets.
		 * @param[in]	pixelFormatRT1		The pixel format of the second color render targets.
		 * @param[in]	pixelFormatRT2		The pixel format of the third color render targets.
		 * @param[in]	pixelFormatRT3		The pixel format of the fourth color render targets.
		 * @param[in]	widthRatio			The width of the render targets as a ratio of the width of the back buffer.
		 * @param[in]	heightRatio			The height of the render targets as a ratio of the height of the back buffer.
		 * @param[in]	hasMipmaps			The render targets have auto generated mip-maps.
		 * @param[in]	hasDepthStencil		A depth-stencil target is also present.
		 * @param[in]	depthStencilFormat	The pixel format of the depth-stencil target.
		 */
		RenderTarget(const unsigned int targetCount,
			PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
			const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat);

		/**
		 * @brief	Destructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::ReleaseRenderTarget()
		 */
		virtual ~RenderTarget();

		/**
		 * @brief	The number of color render targets.
		 */
		unsigned int	m_nTargetCount;
		


		unsigned int	m_nWidth;				/**< @brief	The width of the render targets. */
		unsigned int	m_nHeight;				/**< @brief	The height of the render targets. */
		float			m_fWidthRatio;			/**< @brief	The width of the render targets as a ratio of the width of the back buffer. */
		float			m_fHeightRatio;			/**< @brief	The height of the render targets as a ratio of the height of the back buffer. */
		bool			m_bHasMipmaps;			/**< @brief	The render target has auto generated mip-maps. */
		bool			m_bHasDepthStencil;		/**< @brief	A depth-stencil target is present. */
		unsigned int*	m_nColorBufferTexIdx;	/**< @brief	The array with texture IDs for each color target. */
		unsigned int	m_nDepthBufferTexIdx;	/**< @brief	The texture ID for the depth-stencil target. */
		Texture**		m_pColorBuffer;			/**< @brief	The array of pointers to each color textures. */
		Texture*		m_pDepthBuffer;			/**< @brief	The pointer to the depth-stencil texture. */

		static std::vector<RenderTarget*>	ms_pActiveRenderTarget;	/**< @brief	Static pointer to the currently active render target (or nullptr, if back buffer). */

		friend class ResourceManager;
	};
}

#endif // RENDERTARGET_H
