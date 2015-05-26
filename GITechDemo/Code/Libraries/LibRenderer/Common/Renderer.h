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

#ifndef LIBRENDERER_DLL
#ifdef LIBRENDERER_EXPORTS
#define LIBRENDERER_DLL __declspec(dllexport) 
#else
#define LIBRENDERER_DLL __declspec(dllimport) 
#endif
#endif // LIBRENDERER_DLL

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
		static	LIBRENDERER_DLL void				CreateInstance(API eApi);
		/* Destroys the instance of the Renderer object */
		static	LIBRENDERER_DLL void				DestroyInstance();
		/* Retrieves the instance of the Renderer object */
		static	LIBRENDERER_DLL Renderer*			GetInstance();
		/* Retrieves the API of the currently instantiated renderer */
		static	LIBRENDERER_DLL API					GetAPI();

		/* After you create an application window, you are ready to initialize the graphics
		object that you will use to render the scene. This process includes creating the
		object, setting the presentation parameters, and finally creating the device. */
		virtual	LIBRENDERER_DLL void				Initialize(void* hWnd) = 0;

		/* Set backbuffer size and offset (useful when rendering in a part of a window) */
		virtual	LIBRENDERER_DLL void				SetBackBufferSize(const Vec2i size, const Vec2i offset = Vec2i(0, 0));
		virtual	LIBRENDERER_DLL	Vec2i				GetBackBufferSize();
		virtual	LIBRENDERER_DLL	Vec2i				GetBackBufferOffset();
		/* Set viewport size and offset */
		virtual	LIBRENDERER_DLL	void				SetViewport(const Vec2i size, const Vec2i offset = Vec2i(0, 0)) = 0;
		/* Create a projection matrix for the corresponding API */
		virtual LIBRENDERER_DLL void				CreatePerspectiveMatrix(Matrix44f& matProj, float fovYRad, float aspectRatio, float zNear, float zFar) = 0;
		virtual LIBRENDERER_DLL void				CreateOrthographicMatrix(Matrix44f& matProj, float left, float top, float right, float bottom, float zNear, float zFar) = 0;
		/* Convert a GMTL / OpenGL projection matrix to a DirectX compliant projection matrix */
		static	LIBRENDERER_DLL	void				ConvertOGLProjMatToD3D(Matrix44f& matProj);
		static	LIBRENDERER_DLL	void				ConvertOGLProjMatToD3D(Matrix44f* const matProj);

		/* Try to begin a new frame (don't try to draw anything if it fails) */
		virtual	LIBRENDERER_DLL const bool			BeginFrame() = 0;
		/* End the current frame */
		virtual LIBRENDERER_DLL void				EndFrame() = 0;
		/* Swap buffers, presenting the backbuffer to the monitor */
		virtual	LIBRENDERER_DLL void				SwapBuffers() = 0;
		/* Clear the currently set render target(s)*/
		virtual	LIBRENDERER_DLL void				Clear(const Vec4f rgba, const float z, const unsigned int stencil) = 0;
		/* Draw the contents of a vertex buffer (for the moment, only supports triangle lists with valid index buffers) */
		virtual	LIBRENDERER_DLL void				DrawVertexBuffer(VertexBuffer* vb) = 0;

		/* Mark the beginning of a user-defined event, viewable in graphical analysis tools (NB: color values are between 0-255) */
		virtual	LIBRENDERER_DLL	void				PushProfileMarker(const char* const label);
		/* Mark the end of a user-defined event, viewable in graphical analysis tools */
		virtual	LIBRENDERER_DLL	void				PopProfileMarker();

				/* Retrieve a pointer to the resource manager */
				LIBRENDERER_DLL ResourceManager*	GetResourceManager();
				/* Retrieve a pointer to the render state manager */
				LIBRENDERER_DLL RenderState*		GetRenderStateManager();
				/* Retrieve a pointer to the texture sampler state manager */
				LIBRENDERER_DLL SamplerState*		GetSamplerStateManager();
				/* Device capabilities */
				LIBRENDERER_DLL DeviceCaps			GetDeviceCaps();

	protected:
		Renderer();
		virtual	~Renderer();

				Vec2i			m_vBackBufferSize;		// The backbuffer size in pixels
				Vec2i			m_vBackBufferOffset;	// The backbuffer offset in pixels
			ResourceManager*	m_pResourceManager;		// Pointer to the resource manager
			RenderState*		m_pRenderState;			// Pointer to the render state manager
			SamplerState*		m_pSamplerState;		// Pointer to the texture sampler state manager
			DeviceCaps			m_tDeviceCaps;			// Pointer to the device capabilities

		static	Renderer*		ms_pInstance;			// Holds the current instance of the rendering class
		static	API				ms_eAPI;				// Holds the currently instanced rendering API
		static	int				ms_nProfileMarkerCounter;
	};
}

#endif	//RENDERER_H
