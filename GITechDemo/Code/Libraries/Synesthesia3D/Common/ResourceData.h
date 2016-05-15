/**
 *	@file		ResourceData.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2016 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef RESOURCEDATA_H
#define RESOURCEDATA_H

#ifndef SYNESTHESIA3D_DLL
#ifdef SYNESTHESIA3D_EXPORTS
#define SYNESTHESIA3D_DLL __declspec(dllexport)	/**< @brief Export/import directive keyword. */
#else
#define SYNESTHESIA3D_DLL __declspec(dllimport)	/**< @brief Export/import directive keyword. */
#endif
#endif // SYNESTHESIA3D_DLL

#include <string>
#include <vector>
#include <gmtl/gmtl.h>
using namespace gmtl;

// This header holds all defines, enums and
// structures used by the rendering library

#ifndef PURE_VIRTUAL
	#define PURE_VIRTUAL = 0	/**< @brief Used to mark pure virtual functions. */
#endif

#include "Utility/Debug.h"
#include "Utility/HalfFloat.h"

namespace Synesthesia3DTools
{
	class ModelCompiler;
	class TextureCompiler;
}

namespace Synesthesia3D
{
	// DATA TYPES //////////////////////////////////

	typedef HalfFloat			half;	/**< @brief 16 bit floating-point value. */
	typedef unsigned char		byte;	/**< @brief Byte (8 bits) value. */
	typedef unsigned short		word;	/**< @brief Word (16 bits) value. */
	typedef unsigned long		dword;	/**< @brief Double word (32 bits) value. */
	typedef unsigned long long	qword;	/**< @brief Quadword (64 bits) value. */

	typedef unsigned int		sampler;		/**< General texture resource ID */
	typedef sampler				sampler1D;		/**< 1D texture resource ID */
	typedef sampler				sampler2D;		/**< 2D texture resource ID */
	typedef sampler				sampler3D;		/**< 3D texture resource ID */
	typedef sampler				samplerCUBE;	/**< Cube texture resource ID */

	////////////////////////////////////////////////

	// APIS /////////////////////////////////////////////////////////////

	/**
	 * @brief	Available rendering APIs to be used when instantiating the renderer.
	 */
	enum API
	{
		API_NONE,	/**< @brief No renderer instantiated. */
		API_NULL,	/**< @brief Null render. */
		API_DX9		/**< @brief Direct3D 9. */
	};

	/////////////////////////////////////////////////////////////////////

	// BUFFERS /////////////////////////////////////////////////////////

	/**
	 * @brief	Usage options that identify how resources are to be used.
	 */
	enum BufferUsage
	{
		BU_NONE,			/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		BU_STATIC,			/**< @brief Informs the system that the application only writes once to the buffer. */
		BU_DYNAMIC,			/**< @brief Set to indicate that the buffer requires dynamic memory use. */
		BU_RENDERTAGET,		/**< @brief The resource will be a render target. */
		BU_DEPTHSTENCIL,	/**< @brief The resource will be a depth stencil buffer. */
		BU_TEXTURE,			/**< @brief The resource will be a texture. */

		BU_MAX				/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Locking options that identify how resources are locked for reading/writing.
	 */
	enum BufferLocking
	{
		BL_READ_ONLY,	/**< @brief The application will ONLY read the buffer. */
		BL_WRITE_ONLY,	/**< @brief The application will ONLY write to the buffer. */
		BL_READ_WRITE,	/**< @brief The application will both read and write to the buffer. */

		BL_MAX			/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Specifies the format of the index buffer.
	 */
	enum IndexBufferFormat
	{
		IBF_INDEX16,	/**< @brief 16bit index buffer. */
		IBF_INDEX32,	/**< @brief 32bit index buffer. */

		IBF_MAX			/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	///////////////////////////////////////////////////////////

	// RENDER STATES //////////////////////////////////////////

	/**
	 * @brief	Render states for color blend modes.
	 */
	enum Blend
	{
		BLEND = 0,				/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		BLEND_ZERO,				/**< @brief Blend factor is (0, 0, 0, 0). */
		BLEND_ONE,				/**< @brief Blend factor is (1, 1, 1, 1). */
		BLEND_SRCCOLOR,			/**< @brief Blend factor is (Rs, Gs, Bs, As). */
		BLEND_INVSRCCOLOR,		/**< @brief Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As). */
		BLEND_SRCALPHA,			/**< @brief Blend factor is (As, As, As, As). */
		BLEND_INVSRCALPHA,		/**< @brief Blend factor is (1 - As, 1 - As, 1 - As, 1 - As). */
		BLEND_DESTALPHA,		/**< @brief Blend factor is (Ad Ad Ad Ad). */
		BLEND_INVDESTALPHA,		/**< @brief Blend factor is (1 - Ad 1 - Ad 1 - Ad 1 - Ad). */
		BLEND_DESTCOLOR,		/**< @brief Blend factor is (Rd, Gd, Bd, Ad). */
		BLEND_INVDESTCOLOR,		/**< @brief Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad). */
		BLEND_SRCALPHASAT,		/**< @brief Blend factor is (f, f, f, 1), where f = min(As, 1 - Ad). */
		BLEND_BOTHSRCALPHA,		/**< @brief Same as setting the source and destination blend factors to D3DBLEND_SRCALPHA and D3DBLEND_INVSRCALPHA. */
		BLEND_BOTHINVSRCALPHA,	/**< @brief Source blend factor is (1 - As, 1 - As, 1 - As, 1 - As) and destination blend factor is (As, As, As, As). */
		BLEND_BLENDFACTOR,		/**< @brief Constant color blending factor. */
		BLEND_INVBLENDFACTOR,	/**< @brief Inverted constant color blending factor. */

		BLEND_MAX				/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Render states for comparison functions.
	 */
	enum Cmp
	{
		CMP = BLEND_MAX + 1,	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		CMP_NEVER,				/**< @brief Always fail the test. */
		CMP_LESS,				/**< @brief Accept the new pixel if its value is less than the value of the current pixel. */
		CMP_EQUAL,				/**< @brief Accept the new pixel if its value equals the value of the current pixel. */
		CMP_LESSEQUAL,			/**< @brief Accept the new pixel if its value is less than or equal to the value of the current pixel. */
		CMP_GREATER,			/**< @brief Accept the new pixel if its value is greater than the value of the current pixel. */
		CMP_NOTEQUAL,			/**< @brief Accept the new pixel if its value does not equal the value of the current pixel. */
		CMP_GREATEREQUAL,		/**< @brief Accept the new pixel if its value is greater than or equal to the value of the current pixel. */
		CMP_ALWAYS,				/**< @brief Always pass the test. */

		CMP_MAX,				/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Render states for culling.
	 */
	enum Cull
	{
		CULL = CMP_MAX + 1,	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		CULL_NONE,			/**< @brief Do not cull back faces. */
		CULL_CW,			/**< @brief Cull back faces with clockwise vertices. */
		CULL_CCW,			/**< @brief Cull back faces with counterclockwise vertices. */

		CULL_MAX			/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Render states for Z and W buffers.
	 */
	enum ZBuffer
	{
		ZB = CULL_MAX + 1,	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		ZB_ENABLED,			/**< @brief Enable z-buffering. */
		ZB_DISABLED,		/**< @brief Disable depth buffering. */
		ZB_USEW,			/**< @brief Enable w-buffering. */

		ZB_MAX,				/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Render states for stencil.
	 */
	enum StencilOp
	{
		STENCILOP = ZB_MAX + 1,	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		STENCILOP_KEEP,			/**< @brief Do not update the entry in the stencil buffer. This is the default value. */
		STENCILOP_ZERO,			/**< @brief Set the stencil-buffer entry to 0. */
		STENCILOP_REPLACE,		/**< @brief Replace the stencil-buffer entry with a reference value. */
		STENCILOP_INCRSAT,		/**< @brief Increment the stencil-buffer entry, clamping to the maximum value. */
		STENCILOP_DECRSAT,		/**< @brief Decrement the stencil-buffer entry, clamping to zero. */
		STENCILOP_INVERT,		/**< @brief Invert the bits in the stencil-buffer entry. */
		STENCILOP_INCR,			/**< @brief Increment the stencil-buffer entry, wrapping to zero if the new value exceeds the maximum value. */
		STENCILOP_DECR,			/**< @brief Decrement the stencil-buffer entry, wrapping to the maximum value if the new value is less than zero. */

		STENCILOP_MAX			/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Render states for geometry fill modes.
	 */
	enum Fill
	{
		FILL = STENCILOP_MAX + 1,	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		FILL_POINT,					/**< @brief Fill points. */
		FILL_WIREFRAME,				/**< @brief Fill wireframes. */
		FILL_SOLID,					/**< @brief Fill solids. */

		FILL_MAX					/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	DO NOT USE! INTERNAL USAGE ONLY!
	 */
	enum
	{
		RS_MAX = FILL_MAX + 1	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	///////////////////////////////////////////////////////////

	// SAMPLER STATES /////////////////////////////////////////

	/**
	 * @brief	Maximum anisotropy level.
	 */
	enum
	{
		MAX_ANISOTROPY = 16u	/**< @brief Maximum anisotropy level. */
	};

	/**
	 * @brief	Maximum number of texture samplers for each shader type.
	 */
	enum
	{
		MAX_NUM_VSAMPLERS = 4,	/**< @brief Maximum number of vertex shader texture samplers: vs3.0 has 4, vs2.0 has 0. */
		MAX_NUM_PSAMPLERS = 16	/**< @brief Maximum number of pixel shader texture samplers: ps2.0 and ps3.0 have 16. */
	};

	/**
	 * @brief	Texture sampling filters.
	 */
	enum SamplerFilter
	{
		SF_NONE,						/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		SF_MIN_MAG_POINT_MIP_NONE,		/**< @brief Choose nearest texel. No mips. */
		SF_MIN_MAG_LINEAR_MIP_NONE,		/**< @brief Linearly interpolate between neighbouring texels. No mips. */
		SF_MIN_MAG_POINT_MIP_POINT,		/**< @brief Choose nearest texel and nearest mip level. */
		SF_MIN_MAG_POINT_MIP_LINEAR,	/**< @brief Choose nearest texel and linearly interpolate between nearest mip levels. */
		SF_MIN_MAG_LINEAR_MIP_POINT,	/**< @brief Linearly interpolate between neighbouring texels and choose nearest mip level (bilinear). */
		SF_MIN_MAG_LINEAR_MIP_LINEAR,	/**< @brief Linearly interpolate between neighbouring texels and nearest mip levels (trilinear). */

		SF_MAX							/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Texture addressing mode.
	 */
	enum SamplerAddressingMode
	{
		SAM_NONE,	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		SAM_CLAMP,	/**< @brief Texture coordinates outside the range [0.0, 1.0] are set to the texture color at 0.0 or 1.0, respectively. */
		SAM_WRAP,	/**< @brief Tile the texture at every integer junction. No mirroring is performed. */
		SAM_MIRROR,	/**< @brief Similar to SAM_WRAP, except that the texture is flipped at every integer junction. */
		SAM_BORDER,	/**< @brief Texture coordinates outside the range [0.0, 1.0] are set to the border color. */

		SAM_MAX		/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	A structure that describes a texture sampler.
	 */
	struct SamplerStateDesc
	{
		enum
		{
			COORD_U = 0,
			COORD_V = 1,
			COORD_W = 2
		};

		unsigned int			nAnisotropy;		/**< @brief Anisotropic filter quality level. */
		float					fLodBias;			/**< @brief Offset for mip sampling. */
		SamplerFilter			eFilter;			/**< @brief Sampling filter used. */
		Vec4f					vBorderColor;		/**< @brief Color of bolder when using SAM_BORDER as the addressing mode. */
		SamplerAddressingMode	eAddressingMode[3];	/**< @brief Texture addressing mode for each axis. */
		bool					bSRGBEnabled;		/**< @brief Linear gamma conversion before presenting it to the pixel shader. */
	};

	//////////////////////////////////////////////////////////////

	// SHADERS //////////////////////////////////////////////////

	/**
	 * @brief	The type of a shader program.
	 */
	enum ShaderProgramType
	{
		SPT_NONE,	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		SPT_VERTEX,	/**< @brief Vertex shader. */
		SPT_PIXEL,	/**< @brief Pixel shader. */

		SPT_MAX		/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Shader input data types.
	 */
	enum InputType
	{
		IT_NONE,		/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		IT_BOOL,		/**< @brief Boolean data type. */
		IT_INT,			/**< @brief Integer data type. */
		IT_FLOAT,		/**< @brief Floating point data type. */
		IT_SAMPLER,		/**< @brief Generic texture. */
		IT_SAMPLER1D,	/**< @brief 1D texture. */
		IT_SAMPLER2D,	/**< @brief 2D texture. */
		IT_SAMPLER3D,	/**< @brief 3D texture. */
		IT_SAMPLERCUBE,	/**< @brief Cube texture. */

		IT_MAX			/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Shader input register types.
	 */
	enum RegisterType
	{
		RT_NONE,	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		RT_BOOL,	/**< @brief Boolean register type. */
		RT_INT4,	/**< @brief Integer register type. */
		RT_FLOAT4,	/**< @brief Floating point register type. */
		RT_SAMPLER,	/**< @brief Texture register type. */

		RT_MAX		/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	A structure describing a shader input.
	 */
	struct ShaderInputDesc
	{
		std::string		szName;			/**< @brief Input name (as appears in shader source). */
		unsigned int	nNameHash;		/**< @brief Hash of input name. */
		InputType		eInputType;		/**< @brief Input data type. */
		RegisterType	eRegisterType;	/**< @brief Input register type (not necessarily the same as it's data type). */
		unsigned int	nRegisterIndex;	/**< @brief Input starting register ID. */
		unsigned int	nRegisterCount;	/**< @brief Number of occupied registers. */
		unsigned int	nRows;			/**< @brief Number or rows. */
		unsigned int	nColumns;		/**< @brief Number of columns. */
		unsigned int	nArrayElements;	/**< @brief Number of elements in the array. */
		unsigned int	nBytes;			/**< @brief Size, in bytes, of the input. */
		unsigned int	nOffsetInBytes;	/**< @brief Input's offset in the shader input buffer. */
	};

	/////////////////////////////////////////////////////////

	// TEXTURES /////////////////////////////////////////////

	/**
	 * @brief	Specifies the format of the pixel.
	 */
	enum PixelFormat
	{
		// Null color format
		PF_NONE,			/**< @brief A format which does not have any memory allocated internally, but will satisfy validation requirements imposed by some runtimes. */

		// Small-bit color formats
		PF_R5G6B5,			/**< @brief A 16-bit RGB pixel format that uses 5 bits for red, 6 bits for green, and 5 bits for blue. */
		PF_A1R5G5B5,		/**< @brief A 16-bit pixel format in which 5 bits are reserved for each color and 1 bit is reserved for alpha. */
		PF_A4R4G4B4,		/**< @brief A 16-bit ARGB pixel format that uses 4 bits for each channel. */

		// 8-bit integer formats
		PF_A8,				/**< @brief An 8-bit format that uses alpha only. */
		PF_L8,				/**< @brief An 8-bit luminance-only format. */
		PF_A8L8,			/**< @brief A 16-bit format that uses 8 bits each for alpha and luminance. */
		PF_R8G8B8,			/**< @brief A 24-bit RGB pixel format that uses 8 bits per channel. */
		PF_A8R8G8B8,		/**< @brief A 32-bit ARGB pixel format, with alpha, that uses 8 bits per channel. */
		PF_A8B8G8R8,		/**< @brief A 32-bit ABGR pixel format, with alpha, that uses 8 bits per channel. */

		// 16-bit integer formats
		PF_L16,				/**< @brief A 16-bit luminance-only format. */
		PF_G16R16,			/**< @brief A 32-bit pixel format that uses 16 bits each for green and red. */
		PF_A16B16G16R16,	/**< @brief A 64-bit pixel format that uses 16 bits for each component. */

		// 16-bit floating-point formats ('half float' channels)
		PF_R16F,			/**< @brief A 16-bit floating-point format that uses 16 bits for the red channel. */
		PF_G16R16F,			/**< @brief A 32-bit floating-point format that uses 16 bits each for the red and green channels. */
		PF_A16B16G16R16F,	/**< @brief A 64-bit floating-point format that uses 16 bits for each channel (alpha, blue, green, and red). */

		// 32-bit floating-point formats ('float' channels)
		PF_R32F,			/**< @brief A 32-bit floating-point format that uses 32 bits for the red channel. */
		PF_G32R32F,			/**< @brief A 64-bit floating-point format that uses 32 bits each for the red and green channels. */
		PF_A32B32G32R32F,	/**< @brief A 128-bit floating-point format that uses 32 bits for each channel (alpha, blue, green, and red). */

		// DXT compressed formats
		PF_DXT1,			/**< @brief DXT1 compression texture format. */
		PF_DXT3,			/**< @brief DXT3 compression texture format. */
		PF_DXT5,			/**< @brief DXT5 compression texture format. */

		// Depth-stencil format
		PF_D24S8,			/**< @brief A 32-bit z-buffer bit depth that uses 24 bits for the depth channel and 8 bits for the stencil channel. */
		PF_INTZ,			/**< @brief A 32-bit z-buffer bit depth. */

		PF_MAX				/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Specifies the type of the texture
	 */
	enum TextureType
	{
		TT_1D,		/**< @brief One-dimensional texture. */
		TT_2D,		/**< @brief Two-dimensional texture. */
		TT_3D,		/**< @brief Three-dimensional texture. */
		TT_CUBE,	/**< @brief Cubic environment maps. */

		TT_MAX		/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	Specifies the face of a cubemap.
	 */
	enum CubeFace
	{
		FACE_NONE = ~0u,	//**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		FACE_XNEG = 0,		//**< @brief The left face of the cube. */
		FACE_YPOS,			//**< @brief The top face of the cube. */
		FACE_YNEG,			//**< @brief The bottom face of the cube. */
		FACE_ZPOS,			//**< @brief The front face of the cube. */
		FACE_XPOS,			//**< @brief The right face of the cube. */
		FACE_ZNEG,			//**< @brief The back face of the cube. */

		FACE_MAX			//**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	enum
	{
		/**
		 * @brief	This is the maximum number of mipmap levels for a texture.
		 * @note	A 16384x16384 texture has a maximum of 15 levels.
		 */
		TEX_MAX_MIPMAP_LEVELS = 15
	};

	/////////////////////////////////////////////////////////////////////

	// VERTEX FORMATS ///////////////////////////////////////////////////

	enum
	{
		VF_MAX_ATTRIBUTES	= 16,	/**< @brief The maximum number of attribues. */
		VF_MAX_TCOORD_UNITS	= 8,	/**< @brief The maximum number of texcoord units. */
		VF_MAX_COLOR_UNITS	= 2		/**< @brief The maximum number of vertex color units. */
	};

	/**
	 * @brief	These flags describe the type of data which compose the vertex attributes.
	 */
	enum VertexAttributeType
	{
		VAT_NONE,	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		VAT_FLOAT1,	/**< @brief Attribute is of a single channel floating point data type. */
		VAT_FLOAT2,	/**< @brief Attribute is of a dual channel floating point data type. */
		VAT_FLOAT3,	/**< @brief Attribute is of a triple channel floating point data type. */
		VAT_FLOAT4,	/**< @brief Attribute is of a quadruple channel floating point data type. */
		VAT_HALF2,	/**< @brief Attribute is of a dual channel half floating point data type. */
		VAT_HALF4,	/**< @brief Attribute is of a quadruple channel half floating point data type. */
		VAT_UBYTE4,	/**< @brief Attribute is of a quadruple channel unsigned byte data type. */
		VAT_SHORT2,	/**< @brief Attribute is of a dual channel short integer data type. */
		VAT_SHORT4,	/**< @brief Attribute is of a quadruple channel short integer data type. */

		VAT_MAX		/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	These flags describe an attribute's purpose
	 */
	enum VertexAttributeSemantic
	{
		VAS_NONE,			/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

		VAS_POSITION,		/**< @brief This attribute will be used as a position vector. */
		VAS_NORMAL,			/**< @brief This attribute will be used as a normal vector. */
		VAS_TANGENT,		/**< @brief This attribute will be used as a tangent vector. */
		VAS_BINORMAL,		/**< @brief This attribute will be used as a binormal vector. */
		VAS_TEXCOORD,		/**< @brief This attribute will be used as texture coordinates. */
		VAS_COLOR,			/**< @brief This attribute will be used as a color value. */
		VAS_BLENDINDICES,	/**< @brief This attribute will be used as bone indices. */
		VAS_BLENDWEIGHT,	/**< @brief This attribute will be used as skinning weights. */
		VAS_FOGCOORD,		/**< @brief This attribute will be used as a fog blend value. */
		VAS_PSIZE,			/**< @brief This attribute will be used as the size of a point sprite. */

		VAS_MAX				/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
	};

	/**
	 * @brief	A structure describing a vertex element.
	 */
	struct VertexElement
	{
		unsigned int			nOffset;		/**< @brief The element's offset in the vertex format. */
		VertexAttributeType		eType;			/**< @brief The data type of the element. */
		VertexAttributeSemantic	eSemantic;		/**< @brief The usage/semantic of the element. */
		unsigned int			nSemanticIdx;	/**< @brief The usage/semantic index of the element. */

		/**
		 * @brief	Serializes the vertex element object.
		 */
		friend std::ostream& operator<<(std::ostream& output_out, const VertexElement& ve_in);
		
		/**
		 * @brief	Deserializes the vertex element object.
		 */
		friend std::istream& operator>>(std::istream& s_in, VertexElement& ve_out);
	};

	//////////////////////////////////////////////////////////////////

	// MODELS ////////////////////////////////////////////////////////
	
	class VertexFormat;
	class VertexBuffer;
	class IndexBuffer;

	/**
	 * @brief	A structure describind a model and its meshes.
	 */
	struct Model
	{
		// Forward declarations
		struct Mesh;
		struct Material;
		struct TextureDesc;

		/**
		 * @brief	A structure describing a mesh and its properties.
		 */
		struct Mesh
		{
			std::string		szName;			/**< Name of mesh (not required). */
			unsigned int	nVfIdx;			/**< Index for the corresponding vertex format resource in the resource manager. */
			unsigned int	nIbIdx;			/**< Index for the corresponding index buffer resource in the resource manager. */
			unsigned int	nVbIdx;			/**< Index for the corresponding vertex buffer resource in the resource manager. */
			VertexFormat*	pVertexFormat;	/**< The vertex format for the vertex buffer in which the mesh's vertices are stored. */
			IndexBuffer*	pIndexBuffer;	/**< The index buffer for the vertex buffer in which the mesh's vertices are stored. */
			VertexBuffer*	pVertexBuffer;	/**< The vertex buffer in which the mesh's vertices are stored. */
			unsigned int	nMaterialIdx;	/**< The index for the material used by this mesh. */

			/**
			 * @brief	Serializes a mesh object.
			 */
			friend std::ostream& operator<<(std::ostream& output_out, const Mesh& mesh_in);
			
			/**
			 * @brief	Deserializes a mesh object.
			 */
			friend std::istream& operator>>(std::istream& s_in, Mesh& mesh_out);
			
			/**
			 * @brief	Grant access for model object deserialization operator.
			 */
			friend std::istream& operator>>(std::istream& s_in, Model& model_out);

		private:
			Mesh() {}
			~Mesh();
			friend struct Model;
			friend class Synesthesia3DTools::ModelCompiler;
		};

		/**
		 * @brief	A structure describing a texture used by the mesh's material.
		 *
		 * @note	The final color value from a texture stack (same @ref TextureType, different indices) is calculated as such:
		 *			ConstantBaseColor (eTexOp0) fTexBlend0 * Texture0 (eTexOp1) fTexBlend1 * Texture1 ...
		 */
		struct TextureDesc
		{
			/**
			 * @brief	Describes the contents of a texture.
			 */
			enum TextureType
			{
				TT_NONE			= 0,	/**< @brief No actual texture, but can be used to configure a sampler for a dynamic texture, for example. */
				TT_DIFFUSE		= 1,	/**< @brief To be combined with the result of the diffuse lighting equation. */
				TT_SPECULAR		= 2,	/**< @brief To be combined with the result of the specular lighting equation. */
				TT_AMBIENT		= 3,	/**< @brief To be combined with the result of the ambient lighting equation. */
				TT_EMISSIVE		= 4,	/**< @brief To be added to the result of the lighting equation. */
				TT_HEIGHT		= 5,	/**< @brief Height map. */
				TT_NORMALS		= 6,	/**< @brief Normal map. */
				TT_SHININESS	= 7,	/**< @brief Glossiness of the material (i.e. the exponent of the specular lighting equation). */
				TT_OPACITY		= 8,	/**< @brief Per-pixel opacity. */
				TT_DISPLACEMENT	= 9,	/**< @brief Displacement map (purpose is implementation dependent). */
				TT_LIGHTMAP		= 10,	/**< @brief Scaling value for the result of the lighting equation (e.g. ambient occlusion). */
				TT_REFLECTION	= 11,	/**< @brief Contains the color of the reflection. */
				TT_UNKNOWN		= 12	/**< @brief Custom texture (i.e. application specific usage). */
			};

			/**
			 * @brief	Texture blending operations.
			 */
			enum TextureOp
			{
				TO_MULTIPLY		= 0,	/**< @brief T = T1 * T2. */
				TO_ADD			= 1,	/**< @brief T = T1 + T2. */
				TO_SUBTRACT		= 2,	/**< @brief T = T1 - T2. */
				TO_DIVIDE		= 3,	/**< @brief T = T1 / T2. */
				TO_SMOOTHADD	= 4,	/**< @brief T = (T1 + T2) - (T1 * T2). */
				TO_SIGNEDADD	= 5		/**< @brief T = T1 + (T2 - 0.5). */
			};

			/**
			 * @brief	Texture mapping form.
			 */
			enum TextureMapping
			{
				TM_UV		= 0,	/**< @brief Mapping coordinates are taken from UV channel. */
				TM_SPHERE	= 1,	/**< @brief Spherical mapping. */
				TM_CYLINDER	= 2,	/**< @brief Cylindrical mapping. */
				TM_BOX		= 3,	/**< @brief Cubic mapping. */
				TM_PLANE	= 4,	/**< @brief Planar mapping. */
				TM_OTHER	= 5		/**< @brief Undefined or unknown mapping. */
			};

			/**
			 * @brief	Texture mapping mode.
			 */
			enum TextureMappingMode
			{
				TMM_WRAP	= 0,	/**< @brief Corresponds to @ref SAM_WRAP. */
				TMM_CLAMP	= 1,	/**< @brief Corresponds to @ref SAM_CLAMP. */
				TMM_DECAL	= 3,	/**< @brief If the tex coords are outside [0, 1], the texture is not applied (i.e. @ref SAM_BORDER addressing mode with transparent border color). */
				TMM_MIRROR	= 2		/**< @brief Corresponds to @ref SAM_MIRROR. */
			};

			std::string			szFilePath;		/**< @brief Path to the texture file. */
			TextureType			eTexType;		/**< @brief Type of texture. */
			unsigned int		nTexIndex;		/**< @brief Index in this texture type's stack. */
			unsigned int		nUVChannel;		/**< @brief Used texcoord unit. */
			TextureOp			eTexOp;			/**< @brief Blend operation. */
			TextureMapping		eTexMapping;	/**< @brief Defined how the mapping coords are generated. */
			float				fTexBlend;		/**< @brief Blend strength factor. */
			TextureMappingMode	eTexMapModeU;	/**< @brief Mapping mode on the X axis. */
			TextureMappingMode	eTexMapModeV;	/**< @brief Mapping mode on the Y axis. */

			/**
			 * @brief	Serializes texture description object.
			 */
			friend std::ostream& operator<<(std::ostream& output_out, const TextureDesc& tex_in);
			
			/**
			 * @brief	Deserializes texture description object.
			 */
			friend std::istream& operator>>(std::istream& s_in, TextureDesc& tex_out);
			
			/**
			 * @brief	Grant access for model object deserialization operator.
			 */
			friend std::istream& operator>>(std::istream& s_in, Material& mat_out);

		private:
			TextureDesc() {}
			~TextureDesc() {}
			friend struct Model;
			friend class Synesthesia3DTools::ModelCompiler;
		};

		/**
		 * @brief	A structure describing a material.
		 *
		 * @note	Applications can choose to ignore all or part of this information
		 *			if they provide their own properties, shaders, render schemes, etc.
		 */
		struct Material
		{
			/**
			 * @brief	The list of shading modes has been taken from Blender.
			 * @note	See Blender documentation for more information.
			 */
			enum ShadingModel
			{
				SM_FLAT			= 1,	/**< @brief Shading is done on per-face base, diffuse only. */
				SM_GOURAUD		= 2,	/**< @brief Simple Gouraud shading. */
				SM_PHONG		= 3,	/**< @brief Phong shading. */
				SM_BLINN		= 4,	/**< @brief Phong-Blinn shading. */
				SM_TOON			= 5,	/**< @brief Cel shading. */
				SM_ORENNAYAR	= 6,	/**< @brief Extension to standard Lambertian shading, taking the roughness of the material into account. */
				SM_MINNAERT		= 7,	/**< @brief Extension to standard Lambertian shading, taking the darkness of the material into account. */
				SM_COOKTORRANCE	= 8,	/**< @brief Special shader for metallic surfaces. */
				SM_NOSHADING	= 9,	/**< @brief No light influence. */
				SM_FRESNEL		= 10	/**< @brief Fresnel shading. */
			};

			/**
			 * @brief	These flags define how the final color value of a pixel is computed, based
			 *			on the previous color at that pixel and the new color value from the material.
			 */
			enum BlendMode
			{
				BM_DEFAULT	= 0,	/**< @brief SourceColor * SourceAlpha + DestColor * (1 - SourceAlpha). */
				BM_ADDITIVE	= 1		/**< @brief SourceColor * 1 + DestColor * 1. */
			};
			
			std::string					szName;				/**< @brief The name of the material (not required). */
			bool						bTwoSided;			/**< @brief Specifies whether this mesh must be rendered without culling. */
			ShadingModel				eShadingModel;		/**< @brief The shading model to use for rendering the mesh. */
			bool						bEnableWireframe;	/**< @brief Specifies whether this mesh should be rendered in wireframe mode. */
			BlendMode					eBlendMode;			/**< @brief Defines how the final color value is computed. */
			float						fOpacity;			/**< @brief The opacity of the material (i.e. if alpha testing is enabled). */
			float						fBumpScaling;		/**< @brief Scale value for normals. */
			float						fShininess;			/**< @brief Shininess of a phong-shaded material (i.e. the exponent of the phong specular equation). */
			float						fReflectivity;		/**< @brief Reflectivity of the material. */
			float						fShininessStrength;	/**< @brief Scale value for the specular color. */
			float						fRefractIndex;		/**< @brief The material's refraction index (useful for raytracing). */
			Vec3f						vColorDiffuse;		/**< @brief Diffuse base color; Normalized RGB color components (i.e. between [0, 1]). */
			Vec3f						vColorAmbient;		/**< @brief Ambient base color; Normalized RGB color components (i.e. between [0, 1]). */
			Vec3f						vColorSpecular;		/**< @brief Specular base color; Normalized RGB color components (i.e. between [0, 1]). */
			Vec3f						vColorEmissive;		/**< @brief The amount of light emitted by the object; Normalized RGB color components (i.e. between [0, 1]). */
			Vec3f						vColorTransparent;	/**< @brief The transparent color, to be multiplied with the color of translucent light; Normalized RGB color components (i.e. between [0, 1]). */
			Vec3f						vColorReflective;	/**< @brief The reflective color, to be multiplied with the color of the reflected light; Normalized RGB color components (i.e. between [0, 1]). */
			std::vector<TextureDesc*>	arrTexture;			/**< @brief Textures associated with the material. */

			/**
			 * @brief	Serializes a material object.
			 */
			friend std::ostream& operator<<(std::ostream& output_out, const Material& mat_in);
			
			/**
			 * @brief	Deserializes a material object.
			 */
			friend std::istream& operator>>(std::istream& s_in, Material& mat_out);

			/**
			 * @brief	Grant access for model object deserialization operator.
			 */
			friend std::istream& operator>>(std::istream& s_in, Model& model_out);

		private:
			Material() {}
			~Material();
			friend struct Model;
			friend class Synesthesia3DTools::ModelCompiler;
		};

		std::string				szName;			/**< Name of the model (not required). */
		std::vector<Mesh*>		arrMesh;		/**< Meshes associated with the model. */
		std::vector<Material*>	arrMaterial;	/**< Materials associated with the mesh. */

		std::string				szSourceFile;	/**< File from which model was loaded. */

		/**
		 * @brief	Serializes a model object.
		 */
		SYNESTHESIA3D_DLL friend std::ostream& operator<<(std::ostream& output_out, const Model& model_in);
		
		/**
		 * @brief	Deserializes a model object.
		 */
		friend std::istream& operator>>(std::istream& s_in, Model& model_out);

		private:
			Model() {}
			SYNESTHESIA3D_DLL ~Model();
			friend class ResourceManager;
			friend class Synesthesia3DTools::ModelCompiler;
	};

	//////////////////////////////////////////////////////////////////

	// DEVICE CAPS ///////////////////////////////////////////////////

	/**
	 * @brief	Structure describing device capabilities.
	 */
	struct DeviceCaps
	{
		bool bCanAutoGenMipmaps;					/**< @brief The driver is capable of automatically generating mipmaps. */
		bool bDynamicTextures;						/**< @brief The driver supports dynamic textures. */
		bool bPresentIntervalImmediate;				/**< @brief The driver supports an immediate presentation swap interval (no V-sync). */
		bool bPresentIntervalOne;					/**< @brief The driver supports a presentation swap interval of every screen refresh (usually 60Hz). */
		bool bPresentIntervalTwo;					/**< @brief The driver supports a presentation swap interval of every second screen refresh (usually 30Hz). */
		bool bPresentIntervalThree;					/**< @brief The driver supports a presentation swap interval of every third screen refresh (usually 20Hz). */
		bool bPresentIntervalFour;					/**< @brief The driver supports a presentation swap interval of every fourth screen refresh (usually 15Hz). */
		bool bMrtIndependentBitDepths;				/**< @brief Device supports different bit depths for multiple render targets. */
		bool bMrtPostPixelShaderBlending;			/**< @brief Device supports post-pixel shader operations for multiple render targets. */
		bool bAnisotropicFiltering;					/**< @brief Device supports anisotropic filtering */
		bool bDepthBias;							/**< @brief Device supports legacy depth bias. */
		bool bSlopeScaledDepthBias;					/**< @brief Device performs true slope-scale based depth bias. */
		bool bMipmapLodBias;						/**< @brief Device supports level-of-detail bias adjustments. */
		bool bWBuffer;								/**< @brief Device supports depth buffering using W. */
		bool bTextureNPOT;							/**< @brief Device supports textures with dimensions that are not powers of two. */
		unsigned int nMaxTextureWidth;				/**< @brief Maximum texture width for this device. */
		unsigned int nMaxTextureHeight;				/**< @brief Maximum texture height for this device. */
		unsigned int nMaxTextureDepth;				/**< @brief Maximum texture depth for this device. */
		unsigned int nVertexShaderVersionMajor;		/**< @brief Vertex shader main version. */
		unsigned int nVertexShaderVersionMinor;		/**< @brief Vertex shader sub version. */
		unsigned int nPixelShaderVersionMajor;		/**< @brief Pixel shader main version. */
		unsigned int nPixelShaderVersionMinor;		/**< @brief Pixel shader sub version. */
		unsigned int nNumSimultaneousRTs;			/**< @brief Number of simultaneous render targets. */

		/**
		 * @brief	Supported display formats.
		 */
		struct SupportedScreenFormat
		{
			PixelFormat		ePixelFormat;	/**< @brief Supported pixel format. */
			unsigned int	nWidth;			/**< @brief Supported width. */
			unsigned int	nHeight;		/**< @brief Supported height. */
			unsigned int	nRefreshRate;	/**< @brief Supported refresh rate. */
		};
		std::vector<const SupportedScreenFormat> arrSupportedScreenFormats;	/**< Array with supported display formats. */

		/**
		 * @brief	Supported pixel formats for textures.
		 */
		struct SupportedPixelFormat
		{
			TextureType	eTextureType;	/**< @brief Supported texture type. */
			PixelFormat	ePixelFormat;	/**< @brief Supported pixel format. */
			BufferUsage	eResourceUsage;	/**< @brief Supported usage mode. */
		};
		std::vector<const SupportedPixelFormat> arrSupportedPixelFormats;	/**< Array with supported texture formats. */
	};

	//////////////////////////////////////////////////////////////////
}

#endif // RESOURCEDATA_H