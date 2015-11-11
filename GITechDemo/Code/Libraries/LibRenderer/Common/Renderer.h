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
#ifndef RENDERER_H
#define RENDERER_H

#include "ResourceData.h"

namespace LibRendererDll
{
	class RenderState;
	class SamplerState;

	// This is the platform independent renderer interface
	class Renderer
	{
	public:
		/* Create an instance of the Renderer object based on API of choice */
		static	LIBRENDERER_DLL	void				CreateInstance(API eApi);
		/* Destroys the instance of the Renderer object */
		static	LIBRENDERER_DLL	void				DestroyInstance();
		/* Retrieves the instance of the Renderer object */
		static	LIBRENDERER_DLL	Renderer* const		GetInstance();
		/* Retrieves the API of the currently instantiated renderer */
		static	LIBRENDERER_DLL	const API			GetAPI();

		/* After you create an application window, you are ready to initialize the graphics
		object that you will use to render the scene. This process includes creating the
		object, setting the presentation parameters, and finally creating the device. */
		virtual	LIBRENDERER_DLL	void				Initialize(void* hWnd) PURE_VIRTUAL

		/* Set backbuffer size and offset (useful when rendering in a part of a window) */
		virtual	LIBRENDERER_DLL	const Vec2i			GetScreenResolution() const PURE_VIRTUAL
		virtual	LIBRENDERER_DLL	const Vec2i			GetScreenOffset() const;
		virtual	LIBRENDERER_DLL	const bool			SetScreenResolution(const Vec2i size, const Vec2i offset = Vec2i(0, 0), const bool fullscreen = false) PURE_VIRTUAL
		virtual	LIBRENDERER_DLL	const PixelFormat	GetBackBufferFormat() const PURE_VIRTUAL
		/* Check full-screen mode */
		virtual LIBRENDERER_DLL	const bool			IsFullscreen() const PURE_VIRTUAL
		/* Validates the supplied backbuffer size, searching for highest res lower than supplied 'size') */
				LIBRENDERER_DLL	void				ValidateScreenResolution(Vec2i& size) const;

		/* Set viewport size and offset */
		virtual	LIBRENDERER_DLL	void				SetViewport(const Vec2i size, const Vec2i offset = Vec2i(0, 0)) PURE_VIRTUAL

		/* Create a projection matrix for the corresponding API */
		virtual LIBRENDERER_DLL	void				CreatePerspectiveMatrix(Matrix44f& matProj, const float fovYRad, const float aspectRatio, const float zNear, const float zFar) const PURE_VIRTUAL
		virtual LIBRENDERER_DLL	void				CreateOrthographicMatrix(Matrix44f& matProj, const float left, const float top, const float right, const float bottom, const float zNear, const float zFar) const PURE_VIRTUAL
		/* Convert a GMTL / OpenGL projection matrix to a DirectX compliant projection matrix */
		static	LIBRENDERER_DLL	void				ConvertOGLProjMatToD3D(Matrix44f& matProj);
		static	LIBRENDERER_DLL	void				ConvertOGLProjMatToD3D(Matrix44f* const matProj);

		/* Try to begin a new frame (don't try to draw anything if it fails) */
		virtual	LIBRENDERER_DLL	const bool			BeginFrame() PURE_VIRTUAL
		/* End the current frame */
		virtual LIBRENDERER_DLL	void				EndFrame() PURE_VIRTUAL
		/* Swap buffers, presenting the backbuffer to the monitor */
		virtual	LIBRENDERER_DLL	void				SwapBuffers() PURE_VIRTUAL
		/* Clear the currently set render target(s)*/
		virtual	LIBRENDERER_DLL	void				Clear(const Vec4f rgba, const float z, const unsigned int stencil) PURE_VIRTUAL
		/* Draw the contents of a vertex buffer */
		virtual	LIBRENDERER_DLL	void				DrawVertexBuffer(VertexBuffer* vb) PURE_VIRTUAL

		/* Mark the beginning of a user-defined event, viewable in graphical analysis tools (NB: color values are between 0-255) */
		virtual	LIBRENDERER_DLL	void				PushProfileMarker(const char* const label);
		/* Mark the end of a user-defined event, viewable in graphical analysis tools */
		virtual	LIBRENDERER_DLL	void				PopProfileMarker();

				/* Retrieve a pointer to the resource manager */
				LIBRENDERER_DLL	ResourceManager* const	GetResourceManager() const;
				/* Retrieve a pointer to the render state manager */
				LIBRENDERER_DLL	RenderState* const		GetRenderStateManager() const;
				/* Retrieve a pointer to the texture sampler state manager */
				LIBRENDERER_DLL	SamplerState* const		GetSamplerStateManager() const;
				/* Device capabilities */
				LIBRENDERER_DLL	const DeviceCaps		GetDeviceCaps() const;

	protected:
		Renderer();
		virtual	~Renderer();

				Vec2i			m_vBackBufferOffset;		// The backbuffer offset in pixels
			ResourceManager*	m_pResourceManager;			// Pointer to the resource manager
			RenderState*		m_pRenderStateManager;		// Pointer to the render state manager
			SamplerState*		m_pSamplerStateManager;		// Pointer to the texture sampler state manager
			DeviceCaps			m_tDeviceCaps;				// Pointer to the device capabilities

		static	Renderer*		ms_pInstance;				// Holds the current instance of the rendering class
		static	API				ms_eAPI;					// Holds the currently instanced rendering API
		static	int				ms_nProfileMarkerCounter;	// Keeps track of profiler marker start/end pairs
	};
}

#endif	//RENDERER_H
