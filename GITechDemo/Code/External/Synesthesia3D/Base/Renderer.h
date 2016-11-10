/**
 * @file        Renderer.h
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

#ifndef RENDERER_H
#define RENDERER_H

#include "ResourceData.h"

namespace Synesthesia3D
{
    class RenderState;
    class SamplerState;
    class Profiler;

    /**
     * @brief   Render context interface.
     *
     * @details This class is the interface to the underlying rendering hardware.
     *          It is from here that rendering commands can be issued and also
     *          where the @ref ResourceManager, @ref RenderState and @ref SamplerState
     *          managers can be accessed from.
     *
     * @see     CreateInstance() to get started.
     */
    class Renderer
    {

    public:

        /**
         * @brief   Creates a rendering context based on API of choice.
         * @see     Initialize()
         *
         * @param[in]   api     Underlying rendering API to be used.
         *
         * @note    At the moment, only one rendering context can be created,
         *          allowing a single thread to issue rendering commands.
         *
         * @todo    Add support for multiple rendering contexts for multithreaded rendering.
         */
        static  SYNESTHESIA3D_DLL           void        CreateInstance(API api);

        /**
         * @brief   Destroys the current rendering context.
         */
        static  SYNESTHESIA3D_DLL           void        DestroyInstance();

        /**
         * @brief   Retrieves the rendering context.
         */
        static  SYNESTHESIA3D_DLL   Renderer* const     GetInstance();

        /**
         * @brief   Retrieves the API/platform of the current render context.
         */
        static  SYNESTHESIA3D_DLL       const API       GetAPI();

        /**
         * @brief   Initializes the device.
         *
         * @details After creating a render context using @ref CreateInstance(), the user
         *          must call @ref Initialize(), providing some platform specific data
         *          required to create the underlying platform specific device.
         *
         * @param[in]   hWnd    Handle to the window in which to display the backbuffer (Windows/D3D only).
         */
        virtual SYNESTHESIA3D_DLL           void        Initialize(void* hWnd) PURE_VIRTUAL;

        /**
         * @brief   Sets backbuffer size, window offset and fullscreen mode.
         *
         * @param[in]   size            The size of the backbuffer, in number of pixels on each axis.
         * @param[in]   offset          An offset with respect to the window in which the backbuffer is being displayed.
         * @param[in]   fullscreen      Switch the application into fullscreen (true) or windowed (false) mode.
         * @param[in]   refreshRate     Set the specified display refresh rate (only affects fullscreen mode).
         * @param[in]   vsync           Switch VSync on or off.
         *
         * @return  Success of operation.
         *
         * @note    The window offset parameter is useful when the user wants to display the backbuffer in a part of the window.
         * @note    When in fullscreen mode, the window offset parameter has no effect.
         */
        virtual SYNESTHESIA3D_DLL       const bool      SetDisplayResolution(
            const Vec2i size,
            const Vec2i offset = Vec2i(0, 0),
            const bool fullscreen = false,
            const unsigned int refreshRate = 0,
            const bool vsync = true) PURE_VIRTUAL;

        /**
         * @brief   Returns the size of the backbuffer.
         */
        virtual SYNESTHESIA3D_DLL       const Vec2i     GetDisplayResolution() const PURE_VIRTUAL;

        /**
         * @brief   Returns the window offset.
         */
        virtual SYNESTHESIA3D_DLL       const Vec2i     GetDisplayOffset() const;
        
        /**
         * @brief   Checks VSync status
         */
        virtual SYNESTHESIA3D_DLL       const bool      GetVSyncStatus() const PURE_VIRTUAL;

        /**
         * @brief   Retrieves display refresh rate.
         */
        virtual SYNESTHESIA3D_DLL   const unsigned int  GetDisplayRefreshRate() const PURE_VIRTUAL;

        /**
         * @brief   Returns the backbuffer pixel format.
         */
        virtual SYNESTHESIA3D_DLL   const PixelFormat   GetBackBufferFormat() const PURE_VIRTUAL;

        /**
         * @brief   Returns whether the application is in fullscreen mode.
         */
        virtual SYNESTHESIA3D_DLL       const bool      IsFullscreen() const PURE_VIRTUAL;

        /**
         * @brief   Validates the supplied backbuffer size, searching for the highest resolution lower than it.
         *
         * @param[in,out]   size            The requested size of the backbuffer, in number of pixels on each axis.
         *                                  In case the requested resolution is not valid, the function will set this
         *                                  parameter to the highest valid resolution lower than the requested one.
         * @param[in,out]   refreshRate     The requested display refresh rate. Will be set to the closest possible valid value.
         *
         * @note    When in fullscreen mode, there are restrictions to the size of the backbuffer based on
         *          graphics card and monitor capabilities. This is not the case when in window mode.
         *
         * @see     GetDeviceCaps()
         */
                SYNESTHESIA3D_DLL           void        ValidateDisplayResolution(Vec2i& size, unsigned int& refreshRate) const;

        /**
         * @brief   Sets viewport size and offset.
         *
         * @param[in]   size    The size of the viewport.
         * @param[in]   offset  The offset for the viewport.
         *
         * @note    This function differs from @ref SetDisplayResolution() since the viewport transformation is
         *          applied when the geometry in clip space is converted to pixel coordinates (screen space).
         */
        virtual SYNESTHESIA3D_DLL           void        SetViewport(const Vec2i size, const Vec2i offset = Vec2i(0, 0)) PURE_VIRTUAL;

        /**
         * @brief   Creates a perspective projection matrix fit for the current platform standards.
         *
         * @param[out]  matProj     The calculated projection matrix.
         * @param[in]   fovYRad     The field of view angle in radians on the Y axis.
         * @param[in]   aspectRatio The display's aspect ratio (width/height).
         * @param[in]   zNear       The distance to the near clipping plane.
         * @param[in]   zFar        The distance to the far clipping plane.
         */
        virtual SYNESTHESIA3D_DLL           void        CreatePerspectiveMatrix(Matrix44f& matProj, const float fovYRad, const float aspectRatio, const float zNear, const float zFar) const PURE_VIRTUAL;

        /**
         * @brief   Creates an orthographic projection matrix fit for the current platform standards.
         *
         * @param[out]  matProj The calculated projection matrix.
         * @param[in]   left    The viewing volume's left coordinate.
         * @param[in]   top     The viewing volume's top coordinate.
         * @param[in]   right   The viewing volume's right coordinate.
         * @param[in]   bottom  The viewing volume's bottom coordinate.
         * @param[in]   zNear   The distance to the near clipping plane.
         * @param[in]   zFar    The distance to the far clipping plane.
         */
        virtual SYNESTHESIA3D_DLL           void        CreateOrthographicMatrix(Matrix44f& matProj, const float left, const float top, const float right, const float bottom, const float zNear, const float zFar) const PURE_VIRTUAL;

        /**
         * @brief   Converts a GMTL / OpenGL projection matrix to a DirectX compliant projection matrix.
         *
         * @param[in,out]   matProj     The converted projection matrix.
         */
        static  SYNESTHESIA3D_DLL           void        ConvertOGLProjMatToD3D(Matrix44f& matProj);
        
        /**
         * @brief   Converts a GMTL / OpenGL projection matrix to a DirectX compliant projection matrix.
         *
         * @param[in,out]   matProj     The converted projection matrix.
         */
        static  SYNESTHESIA3D_DLL           void        ConvertOGLProjMatToD3D(Matrix44f* const matProj);

        /**
         * @brief   Begins a new frame.
         *
         * @details An application must call @ref BeginFrame() before performing any rendering and must call
         *          @ref EndFrame() when rendering is complete and before calling @ref BeginFrame() again.
         *
         * @return  Success of operation.
         *
         * @note    Do not attempt to issue rendering commands if @ref BeginFrame() fails.
         *          Instead, periodically call @ref BeginFrame() until it succeeds.
         */
        virtual SYNESTHESIA3D_DLL       const bool      BeginFrame() PURE_VIRTUAL;

        /**
         * @brief   Ends the current frame.
         *
         * @details An application must call @ref BeginFrame() before performing any rendering and must call
         *          @ref EndFrame() when rendering is complete and before calling @ref BeginFrame() again.
         */
        virtual SYNESTHESIA3D_DLL           void        EndFrame() PURE_VIRTUAL;

        /**
         * @brief   Presents the contents of the backbuffer to the display.
         *
         * @note    @ref SwapBuffers() must be called after @ref EndFrame() and before @ref BeginFrame().
         *          It will also apply the window offset set by @ref SetDisplayResolution() if the application is in windowed mode.
         */
        virtual SYNESTHESIA3D_DLL           void        SwapBuffers() PURE_VIRTUAL;

        /**
         * @brief   Clears the currently set render target(s) / stencil buffer / depth buffer.
         *
         * @param[in]   rgba        Red, green, blue, and alpha values (range 0.0 - 1.0 per color channel).
         * @param[in]   z           Value for clearing depth buffer.
         * @param[in]   stencil     Value for clearing stencil buffer.
         */
        virtual SYNESTHESIA3D_DLL           void        Clear(const Vec4f rgba, const float z, const unsigned int stencil) PURE_VIRTUAL;

        /**
         * @brief   Renders the specified vertex buffer - index buffer pair.
         *
         * @note    The vertex buffer is not required to reference an index buffer, in which case it will be rendered without indexing.
         *
         * @param[in]   vb  A pointer to the vertex buffer resource.
         */
        virtual SYNESTHESIA3D_DLL           void        DrawVertexBuffer(VertexBuffer* const vb);

        /**
         * @brief   Retrieves a pointer to the resource manager.
         */
                SYNESTHESIA3D_DLL   ResourceManager* const  GetResourceManager() const;

        /**
         * @brief   Retrieves a pointer to the render state manager.
         */
                SYNESTHESIA3D_DLL   RenderState* const      GetRenderStateManager() const;

        /**
         * @brief   Retrieves a pointer to the texture sampler state manager.
         */
                SYNESTHESIA3D_DLL   SamplerState* const     GetSamplerStateManager() const;
                
        /**
         * @brief   Retrieves a pointer to the profiler, which allows for managing profile markers.
         */
                SYNESTHESIA3D_DLL   Profiler* const         GetProfiler() const;

        /**
         * @brief   Retrieves the device's capabilities.
         */
                SYNESTHESIA3D_DLL   const DeviceCaps&       GetDeviceCaps() const;

    protected:

        /**
         * @brief   Constructor.
         *
         * @details Meant to be used only by @ref CreateInstance().
         */
        Renderer();

        /**
         * @brief   Destructor.
         *
         * @details Meant to be used only by @ref DestroyInstance().
         */
        virtual ~Renderer();

                Vec2i           m_vBackBufferOffset;        /**< @brief The backbuffer offset in pixels. */
            ResourceManager*    m_pResourceManager;         /**< @brief Pointer to the resource manage.r */
            RenderState*        m_pRenderStateManager;      /**< @brief Pointer to the render state manager. */
            SamplerState*       m_pSamplerStateManager;     /**< @brief Pointer to the texture sampler state manager. */
            Profiler*           m_pProfiler;                /**< @brief Pointer to the profiler instance. */
            DeviceCaps          m_tDeviceCaps;              /**< @brief Pointer to the device capabilities. */

        static  Renderer*       ms_pInstance;               /**< @brief Holds the current instance of the rendering class. */
        static  API             ms_eAPI;                    /**< @brief Holds the currently instanced rendering API. */
    };
}

#endif  //RENDERER_H
