/**
 *	@file		ResourceManager.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "ResourceData.h"

namespace Synesthesia3D
{
	class VertexFormat;
	class VertexBuffer;
	class IndexBuffer;
	class Texture;
	class ShaderProgram;
	class ShaderProgram;
	class ShaderInput;
	class RenderTarget;

	/**
	 * @brief	Manages all allocated resources.
	 */
	class ResourceManager
	{

	public:

		/**
		 * @brief	Destroys all resources.
		 */
				SYNESTHESIA3D_DLL	void	ReleaseAll();

		/**
		 * @brief	Creates all platform specific resources.
		 *
		 * @note	Not of much interest to the average user. Mainly used in the device reset mechanic.
		 */
				SYNESTHESIA3D_DLL	void	BindAll();

		/**
		 * @brief	Destroys all platform specific resources.
		 *
		 * @note	Not of much interest to the average user. Mainly used in the device reset mechanic.
		 */
				SYNESTHESIA3D_DLL	void	UnbindAll();

		/**
		 * @brief	Describes a vertex format.
		 * @note	Support for deferred creation of a vertex format.
		 *
		 * @param[in]	attributeCount	The number of vertex format attributes.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		VertexFormat
		 */
		virtual	SYNESTHESIA3D_DLL	const unsigned int		CreateVertexFormat(const unsigned int attributeCount) PURE_VIRTUAL;

		/**
		 * @brief	Describes a vertex format.
		 *
		 * @param[in]	attributeCount	The number of vertex format attributes.
		 * @param[in]	semantic		Vertex semantic.
		 * @param[in]	type			Data type.
		 * @param[in]	semanticIdx		Semantic index.
		 * @param[in]	...				attributeCount - 1 pairs of semantic and semanticIdx.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		VertexFormat
		 */
		virtual	SYNESTHESIA3D_DLL	const unsigned int		CreateVertexFormat(const unsigned int attributeCount, const VertexAttributeSemantic semantic, const VertexAttributeType type, const unsigned int semanticIdx, ...) PURE_VIRTUAL;

		/**
		 * @brief	Creates an index buffer.
		 *
		 * @param[in]	indexCount		Number of indices.
		 * @param[in]	indexFormat		Bit size of an index.
		 * @param[in]	usage			Memory access type for buffer.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see IndexBuffer
		 */
		virtual	SYNESTHESIA3D_DLL	const unsigned int		CreateIndexBuffer(const unsigned int indexCount, const IndexBufferFormat indexFormat = IBF_INDEX16, const BufferUsage usage = BU_STATIC) PURE_VIRTUAL;

		/**
		 * @brief	Creates a vertex buffer.
		 *
		 * @param[in]	vertexFormat	Format of the vertex buffer.
		 * @param[in]	vertexCount		Number of vertices.
		 * @param[in]	indexBuffer		Optional index buffer.
		 * @param[in]	usage			Memory access type for buffer.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		VertexBuffer
		 */
		virtual	SYNESTHESIA3D_DLL	const unsigned int		CreateVertexBuffer(VertexFormat* const vertexFormat, const unsigned int vertexCount, IndexBuffer* const indexBuffer = nullptr, const BufferUsage usage = BU_STATIC) PURE_VIRTUAL;

		/**
		 * @brief	Creates a shader input helper.
		 *
		 * @param[in]	shaderProgram		Description of shader inputs.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		ShaderInput
		 */
				SYNESTHESIA3D_DLL	const unsigned int		CreateShaderInput(ShaderProgram* const shaderProgram);

		/**
		 * @brief	Creates a shader program.
		 *
		 * @param[in]	filePath		Path to shader file.
		 * @param[in]	programType		Shader stage.
		 * @param[out]	errors			Optional output string with compilation errors.
		 * @param[in]	entryPoint		Optional entry function name.
		 * @param[in]	profile			Optional shader model version.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		ShaderProgram
		 */
		virtual	SYNESTHESIA3D_DLL	const unsigned int		CreateShaderProgram(const char* filePath, const ShaderProgramType programType, const char* entryPoint = "") PURE_VIRTUAL;

		/**
		 * @brief	Creates a texture.
		 *
		 * @param[in]	pixelFormat		Pixel bit depth.
		 * @param[in]	texType			Type of texture (2D/cube/etc.).
		 * @param[in]	sizeX			Width of texture.
		 * @param[in]	sizeY			Height of texture.
		 * @param[in]	sizeZ			Depth of texture.
		 * @param[in]	mipCount		Number of mips (0 for max).
		 * @param[in]	usage			Memory access type.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		Texture
		 */
		virtual	SYNESTHESIA3D_DLL	const unsigned int		CreateTexture(const PixelFormat pixelFormat, const TextureType texType, const unsigned int sizeX, const unsigned int sizeY = 1, const unsigned int sizeZ = 1, const unsigned int mipCount = 0, const BufferUsage usage = BU_TEXTURE) PURE_VIRTUAL;

		/**
		 * @brief	Creates a texture and load data from an image file.
		 *
		 * @param[in]	pathToFile	Path to texture file (*.s3dtex)
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		Texture
		 */
				SYNESTHESIA3D_DLL	const unsigned int		CreateTexture(const char* pathToFile);

		/**
		 * @brief	Creates a render target.
		 *
		 * @param[in]	targetCount			Number of color render targets.
		 * @param[in]	pixelFormat			Pixel format of color render targets.
		 * @param[in]	width				Width of render target.
		 * @param[in]	height				Height of render target.
		 * @param[in]	hasMipmaps			Automatic generation of mips.
		 * @param[in]	hasDepthStencil		Has depth target.
		 * @param[in]	depthStencilFormat	Pixel format of depth target.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		RenderTarget
		 */
		virtual	SYNESTHESIA3D_DLL	const unsigned int		CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat, const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat) PURE_VIRTUAL;

		/**
		 * @brief	Creates a render target.
		 *
		 * @note		The widthRatio and heightRatio parameters are relative
		 *			to the backbuffer's resolution, for dynamic render target sizes.
		 *
		 * @param[in]	targetCount			Number of color targets.
		 * @param[in]	pixelFormat			Pixel format of color render targets.
		 * @param[in]	widthRatio			Width ratio relative to back buffer size.
		 * @param[in]	heightRatio			Height ratio relative to back buffer size.
		 * @param[in]	hasMipmaps			Automatic generation of mips.
		 * @param[in]	hasDepthStencil		Has depth target.
		 * @param[in]	depthStencilFormat	Pixel format of depth target.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		RenderTarget
		 */
		virtual	SYNESTHESIA3D_DLL	const unsigned int		CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormat, const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat) PURE_VIRTUAL;

		/**
		 * @brief	Creates a render target.
		 *
		 * @param[in]	targetCount			Number of color targets.
		 * @param[in]	pixelFormatRT0		Pixel format of first render target.
		 * @param[in]	pixelFormatRT1		Pixel format of second render target, if applicable.
		 * @param[in]	pixelFormatRT2		Pixel format of third render target, if applicable.
		 * @param[in]	pixelFormatRT3		Pixel format of fourth render target, if applicable.
		 * @param[in]	width				Width of render target.
		 * @param[in]	height				Height of render target.
		 * @param[in]	hasMipmaps			Automatic generation of mips.
		 * @param[in]	hasDepthStencil		Has depth target.
		 * @param[in]	depthStencilFormat	Pixel format of depth target.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		RenderTarget
		 */
		virtual	SYNESTHESIA3D_DLL	const unsigned int		CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3, const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat) PURE_VIRTUAL;

		/**
		 * @brief	Creates a render target.
		 *
		 * @note		The widthRatio and heightRatio parameters are relative
		 *			to the backbuffer's resolution, for dynamic render target sizes.
		 *
		 * @param[in]	targetCount			Number of color targets.
		 * @param[in]	pixelFormatRT0		Pixel format of first render target.
		 * @param[in]	pixelFormatRT1		Pixel format of second render target, if applicable.
		 * @param[in]	pixelFormatRT2		Pixel format of third render target, if applicable.
		 * @param[in]	pixelFormatRT3		Pixel format of fourth render target, if applicable.
		 * @param[in]	widthRatio			Width of render target.
		 * @param[in]	heightRatio			Height of render target.
		 * @param[in]	hasMipmaps			Automatic generation of mips.
		 * @param[in]	hasDepthStencil		Has depth target.
		 * @param[in]	depthStencilFormat	Pixel format of depth target.
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see		RenderTarget
		 */
		virtual	SYNESTHESIA3D_DLL	const unsigned int		CreateRenderTarget(const unsigned int targetCount, PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1, PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3, const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat) PURE_VIRTUAL;

		/**
		 * @brief	Loads a model file.
		 *
		 * @param[in]	pathToFile	Path to model file (*.s3dmdl).
		 *
		 * @return	Resource ID corresponding to the created resource.
		 *
		 * @see Model
		 */
				SYNESTHESIA3D_DLL	const unsigned int		CreateModel(const char* pathToFile);

		/**
		 * @brief	Gets a vertex format.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @return	Vertex format object corresponding to the provided resource ID.
		 *
		 * @see	CreateVertexFormat()
		 */
				SYNESTHESIA3D_DLL VertexFormat*		const	GetVertexFormat(const unsigned int idx)		const;
				
		/**
		 * @brief	Gets a index buffer.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @return	Index buffer object corresponding to the provided resource ID.
		 *
		 * @see	CreateIndexBuffer()
		 */
				SYNESTHESIA3D_DLL IndexBuffer*		const	GetIndexBuffer(const unsigned int idx)		const;
				
		/**
		 * @brief	Gets a vertex buffer.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @return	Vertex buffer object corresponding to the provided resource ID.
		 *
		 * @see	CreateVertexBuffer()
		 */
				SYNESTHESIA3D_DLL VertexBuffer*		const	GetVertexBuffer(const unsigned int idx)		const;

		/**
		 * @brief	Gets a shader input.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @return	Shader input object corresponding to the provided resource ID.
		 *
		 * @see	CreateShaderInput()
		 */
				SYNESTHESIA3D_DLL ShaderInput*		const	GetShaderInput(const unsigned int idx)		const;

		/**
		 * @brief	Gets a shader program.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @return	Shader program object corresponding to the provided resource ID.
		 *
		 * @see	CreateShaderProgram()
		 */
				SYNESTHESIA3D_DLL ShaderProgram*	const	GetShaderProgram(const unsigned int idx)	const;

		/**
		 * @brief	Gets a texture.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @return	Texture object corresponding to the provided resource ID.
		 *
		 * @see	CreateTexture()
		 */
				SYNESTHESIA3D_DLL Texture*			const	GetTexture(const unsigned int idx)			const;
				
		/**
		 * @brief	Gets a render target.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @return	Render target object corresponding to the provided resource ID.
		 *
		 * @see	CreateRenderTarget()
		 */
				SYNESTHESIA3D_DLL RenderTarget*		const	GetRenderTarget(const unsigned int idx)		const;
				
		/**
		 * @brief	Gets a model.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @return	Model object corresponding to the provided resource ID.
		 *
		 * @see	CreateModel()
		 */
				SYNESTHESIA3D_DLL Model*			const	GetModel(const unsigned int idx)			const;
				
		/**
		 * @brief	Gets the number of vertex formats.
		 *
		 * @return	The number of vertex format objects.
		 *
		 * @see	GetVertexFormat()
		 */
				SYNESTHESIA3D_DLL	const unsigned int		GetVertexFormatCount()		const;
				
		/**
		 * @brief	Gets the number of index buffers.
		 *
		 * @return	The number of index buffer objects.
		 *
		 * @see	GetIndexBuffer()
		 */
				SYNESTHESIA3D_DLL	const unsigned int		GetIndexBufferCount()		const;
				
		/**
		 * @brief	Gets the number of vertex buffers.
		 *
		 * @return	The number of vertex buffer objects.
		 *
		 * @see	GetVertexBuffer()
		 */
				SYNESTHESIA3D_DLL	const unsigned int		GetVertexBufferCount()		const;
				
		/**
		 * @brief	Gets the number of shader inputs.
		 *
		 * @return	The number of shader input objects.
		 *
		 * @see	GetShaderInput()
		 */
				SYNESTHESIA3D_DLL	const unsigned int		GetShaderInputCount()		const;
				
		/**
		 * @brief	Gets the number of shader program.
		 *
		 * @return	The number of shader program objects.
		 *
		 * @see	GetShaderProgram()
		 */
				SYNESTHESIA3D_DLL	const unsigned int		GetShaderProgramCount()		const;

		/**
		 * @brief	Gets the number of texture.
		 *
		 * @return	The number of texture objects.
		 *
		 * @see	GetTexture()
		 */
				SYNESTHESIA3D_DLL	const unsigned int		GetTextureCount()			const;
				
		/**
		 * @brief	Gets the number of render targets.
		 *
		 * @return	The number of render target objects.
		 *
		 * @see	GetRenderTarget()
		 */
				SYNESTHESIA3D_DLL	const unsigned int		GetRenderTargetCount()		const;
				
		/**
		 * @brief	Gets the number of models.
		 *
		 * @return	The number of model objects.
		 *
		 * @see	GetModel()
		 */
				SYNESTHESIA3D_DLL	const unsigned int		GetModelCount()				const;

		/**
		 * @brief	Finds a texture by its original file name from which it was loaded.
		 *
		 * @note	If the strict parameter is set to false, it will first try to find an exact match, then try
		 *			to find a substring in the resource's original file path; default behaviour (i.e. strict = true)
		 *			is to only search for an exact match
		 *
		 * @param[in]	pathToFile	Texture file name.
		 * @param[in]	strict		Searching mode.
		 *
		 * @return	Resource ID corresponding to the texture.
		 */
				SYNESTHESIA3D_DLL	const unsigned int		FindTexture(const char* pathToFile, const bool strict = true);
				
		/**
		 * @brief	Finds a model by its original file name from which it was loaded.
		 *
		 * @note	If the strict parameter is set to false, it will first try to find an exact
		 *			match, then try to find a substring in the resource's original file path.
		 *			Default behaviour (i.e. strict = true) is to only search for an exact match.
		 *
		 * @param[in]	pathToFile	Model file name.
		 * @param[in]	strict		Searching mode.
		 *
		 * @return	Resource ID corresponding to the model.
		 */
				SYNESTHESIA3D_DLL	const unsigned int		FindModel(const char* pathToFile, const bool strict = true);

		/**
		 * @brief	Destroys a vertex format.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @see CreateVertexFormat()
		 */
				SYNESTHESIA3D_DLL			void			ReleaseVertexFormat(const unsigned int idx);
				
		/**
		 * @brief	Destroys an index buffer.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @see CreateIndexBuffer()
		 */
				SYNESTHESIA3D_DLL			void			ReleaseIndexBuffer(const unsigned int idx);
				
		/**
		 * @brief	Destroys a vertex buffer.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @see CreateVertexBuffer()
		 */
				SYNESTHESIA3D_DLL			void			ReleaseVertexBuffer(const unsigned int idx);
				
		/**
		 * @brief	Destroys a shader input.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @see CreateShaderInput()
		 */
				SYNESTHESIA3D_DLL			void			ReleaseShaderInput(const unsigned int idx);
				
		/**
		 * @brief	Destroys a shader program.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @see CreateShaderProgram()
		 */
				SYNESTHESIA3D_DLL			void			ReleaseShaderProgram(const unsigned int idx);

		/**
		 * @brief	Destroys a texture.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @see CreateTexture()
		 */
				SYNESTHESIA3D_DLL			void			ReleaseTexture(const unsigned int idx);
				
		/**
		 * @brief	Destroys a render target.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @see CreateRenderTarget()
		 */
				SYNESTHESIA3D_DLL			void			ReleaseRenderTarget(const unsigned int idx);
				
		/**
		 * @brief	Destroys a model.
		 * @note	Destroying a model will also destroy underlying resource such as vertex buffers,
		 *			vertex formats, index buffers and textures.
		 *
		 * @param[in]	idx		Resource ID.
		 *
		 * @see CreateModel()
		 */
				SYNESTHESIA3D_DLL			void			ReleaseModel(const unsigned int idx);
				
	protected:
		
		/**
		 * @brief Constructor.
		 *
		 * @details	Meant to be used only by @ref Renderer::Initialize().
		 */
		ResourceManager();
		
		/**
		 * @brief Destructor.
		 *
		 * @details	Meant to be used only by @ref Renderer::DestroyInstance().
		 */
		virtual ~ResourceManager();

		std::vector<VertexFormat*>		m_arrVertexFormat;		/**< @brief Array of vertex formats created by the resource manager. */
		std::vector<IndexBuffer*>		m_arrIndexBuffer;		/**< @brief Array of index buffers created by the resource manager. */
		std::vector<VertexBuffer*>		m_arrVertexBuffer;		/**< @brief Array of vertex buffers created by the resource manager. */
		std::vector<ShaderInput*>		m_arrShaderInput;		/**< @brief Array of shader inputs created by the resource manager */
		std::vector<ShaderProgram*>		m_arrShaderProgram;		/**< @brief Array of shader programs created by the resource manager. */
		std::vector<Texture*>			m_arrTexture;			/**< @brief Array of textures created by the resource manager. */
		std::vector<RenderTarget*>		m_arrRenderTarget;		/**< @brief Array of render targets created by the resource manager. */
		std::vector<Model*>				m_arrModel;				/**< @brief Array of models created by the resource manager. */

		friend class Renderer;
	};
}

#endif // RESOURCEMANAGER_H