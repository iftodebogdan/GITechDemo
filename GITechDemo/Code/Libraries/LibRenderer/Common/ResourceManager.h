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
#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#ifndef LIBRENDERER_DLL
#ifdef LIBRENDERER_EXPORTS
#define LIBRENDERER_DLL __declspec(dllexport) 
#else
#define LIBRENDERER_DLL __declspec(dllimport) 
#endif
#endif // LIBRENDERER_DLL

#include "VertexFormat.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "ShaderTemplate.h"
#include "ShaderInput.h"
#include "RenderTarget.h"

namespace LibRendererDll
{
	// This class manages all allocated resources
	class ResourceManager
	{
	public:
				// Destroy all resources
				LIBRENDERER_DLL	void ReleaseAll();
				// Create all platform specific resources
				LIBRENDERER_DLL	void BindAll();
				// Destroy all platform specific resources
				LIBRENDERER_DLL	void UnbindAll();

		// Support for deferred creation of a vertex format
		virtual	LIBRENDERER_DLL	const unsigned int CreateVertexFormat(const unsigned int attributeCount) = 0;
		// Create a vertex format
		virtual	LIBRENDERER_DLL	const unsigned int CreateVertexFormat(const unsigned int attributeCount, const VertexAttributeUsage usage, const VertexAttributeType type, const unsigned int usageIdx, ...) = 0;

		// Create an index buffer
		virtual	LIBRENDERER_DLL const unsigned int CreateIndexBuffer(const unsigned int indexCount, const IndexBufferFormat indexFormat = IBF_INDEX16, const BufferUsage usage = BU_STATIC) = 0;

		// Create a vertex buffer
		virtual	LIBRENDERER_DLL const unsigned int CreateVertexBuffer(VertexFormat* const vertexFormat, const unsigned int vertexCount, IndexBuffer* const indexBuffer = nullptr, const BufferUsage usage = BU_STATIC) = 0;

				// Create a shader input helper
				LIBRENDERER_DLL const unsigned int CreateShaderInput(ShaderTemplate* const shaderTemplate);

		// Support for deferred creation of shader programs
		virtual	LIBRENDERER_DLL const unsigned int CreateShaderProgram(const ShaderProgramType programType) = 0;
				// Create a shader program
				LIBRENDERER_DLL const unsigned int CreateShaderProgram(const ShaderProgramType programType, const char* srcData, char* const errors = nullptr, const char* entryPoint = "", const char* profile = "");

				// Create a shader template from a shader program
				// NB: TODO: shader templates will be extended in the future to encompass multiple passes / techniques
				LIBRENDERER_DLL const unsigned int CreateShaderTemplate(ShaderProgram* const shaderProgram);

		// Create a texture
		virtual	LIBRENDERER_DLL const unsigned int		CreateTexture(const PixelFormat texFormat, const TexType texType, const unsigned int sizeX, const unsigned int sizeY = 1, const unsigned int sizeZ = 1, const unsigned int mipmapLevelCount = 0, const BufferUsage usage = BU_TEXTURE) = 0;
				// Create a texture and load data from an image file
				LIBRENDERER_DLL const unsigned int		CreateTexture(const char* pathToFile, const unsigned int mipmapLevelCount = 0, const bool convertToARGB = true);

		// Create a render target
		// NB: The depth buffer is always PF_D24S8
		virtual	LIBRENDERER_DLL const unsigned int		CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat, const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil) = 0;

				LIBRENDERER_DLL	const unsigned int		CreateModel(const char* pathToFile);

				// Getters for various resource types
				LIBRENDERER_DLL VertexFormat*	const	GetVertexFormat(const unsigned int idx)		const	{ assert(idx < m_arrVertexFormat.size()); return m_arrVertexFormat[idx]; }
				LIBRENDERER_DLL IndexBuffer*	const	GetIndexBuffer(const unsigned int idx)		const	{ assert(idx < m_arrIndexBuffer.size()); return m_arrIndexBuffer[idx]; }
				LIBRENDERER_DLL VertexBuffer*	const	GetVertexBuffer(const unsigned int idx)		const	{ assert(idx < m_arrVertexBuffer.size()); return m_arrVertexBuffer[idx]; }
				LIBRENDERER_DLL ShaderInput*	const	GetShaderInput(const unsigned int idx)		const	{ assert(idx < m_arrShaderInput.size()); return m_arrShaderInput[idx]; }
				LIBRENDERER_DLL ShaderProgram*	const	GetShaderProgram(const unsigned int idx)	const	{ assert(idx < m_arrShaderProgram.size()); return m_arrShaderProgram[idx]; }
				LIBRENDERER_DLL ShaderTemplate*	const	GetShaderTemplate(const unsigned idx)		const	{ assert(idx < m_arrShaderTemplate.size()); return m_arrShaderTemplate[idx]; }
				LIBRENDERER_DLL Texture*		const	GetTexture(const unsigned int idx)			const	{ assert(idx < m_arrTexture.size()); return m_arrTexture[idx]; }
				LIBRENDERER_DLL RenderTarget*	const	GetRenderTarget(const unsigned int idx)		const	{ assert(idx < m_arrRenderTarget.size()); return m_arrRenderTarget[idx]; }
				LIBRENDERER_DLL Model*			const	GetModel(const unsigned int idx)			const	{ assert(idx < m_arrModel.size()); return m_arrModel[idx]; }

				// Destroy various resource types, freeing memory
				// NB: Destroying a high level resource also destroys the associated low level resources
				LIBRENDERER_DLL		void				ReleaseVertexFormat(const unsigned int idx)				{ assert(idx < m_arrVertexFormat.size()); delete m_arrVertexFormat[idx]; m_arrVertexFormat[idx] = nullptr; }
				LIBRENDERER_DLL		void				ReleaseIndexBuffer(const unsigned int idx)				{ assert(idx < m_arrIndexBuffer.size()); delete m_arrIndexBuffer[idx]; m_arrIndexBuffer[idx] = nullptr; }
				LIBRENDERER_DLL		void				ReleaseVertexBuffer(const unsigned int idx)				{ assert(idx < m_arrVertexBuffer.size()); delete m_arrVertexBuffer[idx]; m_arrVertexBuffer[idx] = nullptr; }
				LIBRENDERER_DLL		void				ReleaseShaderInput(const unsigned int idx)				{ assert(idx < m_arrShaderInput.size()); delete m_arrShaderInput[idx]; m_arrShaderInput[idx] = nullptr; }
				LIBRENDERER_DLL		void				ReleaseShaderProgram(const unsigned int idx)			{ assert(idx < m_arrShaderProgram.size()); delete m_arrShaderProgram[idx]; m_arrShaderProgram[idx] = nullptr; }
				LIBRENDERER_DLL		void				ReleaseShaderTemplate(const unsigned idx)				{ assert(idx < m_arrShaderTemplate.size()); delete m_arrShaderTemplate[idx]; m_arrShaderTemplate[idx] = nullptr; }
				LIBRENDERER_DLL		void				ReleaseTexture(const unsigned int idx)					{ assert(idx < m_arrTexture.size()); delete m_arrTexture[idx]; m_arrTexture[idx] = nullptr; }
				LIBRENDERER_DLL		void				ReleaseRenderTarget(const unsigned int idx)				{ assert(idx < m_arrRenderTarget.size()); delete m_arrRenderTarget[idx]; m_arrRenderTarget[idx] = nullptr; }
				LIBRENDERER_DLL		void				ReleaseModel(const unsigned int idx)					{ assert(idx < m_arrRenderTarget.size()); delete m_arrModel[idx]; m_arrModel[idx] = nullptr; }
				
	protected:
		ResourceManager();
		virtual ~ResourceManager();

		// Low level resources
		std::vector<VertexFormat*>		m_arrVertexFormat;
		std::vector<IndexBuffer*>		m_arrIndexBuffer;
		std::vector<VertexBuffer*>		m_arrVertexBuffer;
		std::vector<ShaderInput*>		m_arrShaderInput;
		std::vector<ShaderProgram*>		m_arrShaderProgram;
		std::vector<ShaderTemplate*>	m_arrShaderTemplate;
		std::vector<Texture*>			m_arrTexture;
		std::vector<RenderTarget*>		m_arrRenderTarget;

		// High level resources (comprised of low level resources)
		std::vector<Model*>				m_arrModel;

		friend class Renderer;
	};
}

#endif // RESOURCEMANAGER_H