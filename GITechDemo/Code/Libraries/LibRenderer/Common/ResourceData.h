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
#ifndef RESOURCEDATA_H
#define RESOURCEDATA_H

#ifndef LIBRENDERER_DLL
#ifdef LIBRENDERER_EXPORTS
#define LIBRENDERER_DLL __declspec(dllexport) 
#else
#define LIBRENDERER_DLL __declspec(dllimport) 
#endif
#endif // LIBRENDERER_DLL

#include <string>
#include <vector>
#include <gmtl/gmtl.h>
using namespace gmtl;

// This header holds all defines, enums and
// structures used by the rendering library

#ifndef assert
	#define assert(_Expression)     ((void)0)
#endif

#ifndef PURE_VIRTUAL
	#define PURE_VIRTUAL = 0;
#endif

#ifndef ENABLE_PROFILE_MARKERS
	#if defined(_DEBUG) || defined(_PROFILE)
		#define ENABLE_PROFILE_MARKERS (1)
	#else
		#define ENABLE_PROFILE_MARKERS (0)
	#endif
#endif

#if ENABLE_PROFILE_MARKERS
	extern int g_nProfileMarkerCounter;
	#ifndef PUSH_PROFILE_MARKER
		#define PUSH_PROFILE_MARKER(label) \
			if(LibRendererDll::Renderer::GetInstance()) \
				LibRendererDll::Renderer::GetInstance()->PushProfileMarker(label);
	#endif
	#ifndef POP_PROFILE_MARKER
		#define POP_PROFILE_MARKER() \
			if(LibRendererDll::Renderer::GetInstance()) \
				LibRendererDll::Renderer::GetInstance()->PopProfileMarker();
	#endif
#else
	#ifndef PUSH_PROFILE_MARKER
		#define PUSH_PROFILE_MARKER(label) ((void)0)
	#endif
	#ifndef POP_PROFILE_MARKER
		#define POP_PROFILE_MARKER() ((void)0)
	#endif
#endif

#include "../Utility/HalfFloat.h"

namespace LibRendererTools
{
	class ModelCompiler;
	class TextureCompiler;
}

namespace LibRendererDll
{
	// DATA TYPES //////////////////////////////////

	typedef HalfFloat			half;
	typedef unsigned char		byte;
	typedef unsigned short		word;
	typedef unsigned long		dword;
	typedef unsigned long long	qword;

	typedef unsigned int		Sampler1D;
	typedef unsigned int		Sampler2D;
	typedef unsigned int		Sampler3D;
	typedef unsigned int		SamplerCUBE;

	////////////////////////////////////////////////

	// APIS /////////////////////////////////////////////////////////////

	// Available rendering APIs to be used when instantiating the renderer
	enum API
	{
		API_NONE,	// No renderer instantiated
		API_NULL,	// Null render
		API_DX9		// Direct3D 9
	};

	/////////////////////////////////////////////////////////////////////

	// BUFFERS /////////////////////////////////////////////////////////

	// Usage options that identify how resources are to be used
	enum BufferUsage
	{
		BU_NONE,

		BU_STATIC,			// Informs the system that the application only writes once to the buffer
		BU_DYNAMIC,			// Set to indicate that the buffer requires dynamic memory use
		BU_RENDERTAGET,		// The resource will be a render target
		BU_DEPTHSTENCIL,	// The resource will be a depth stencil buffer
		BU_TEXTURE,			// The resource will be a texture

		BU_MAX				// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Locking options that identify how resources are locked for reading/writing
	enum BufferLocking
	{
		BL_READ_ONLY,	// The application will ONLY read the buffer
		BL_WRITE_ONLY,	// The application will ONLY write to the buffer
		BL_READ_WRITE,	// The application will both read and write to the buffer

		BL_MAX			// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Specifies the format of the index buffer
	enum IndexBufferFormat
	{
		IBF_INDEX16,	// 16bit index buffer
		IBF_INDEX32,	// 32bit index buffer

		IBF_MAX			// DO NOT USE! INTERNAL USAGE ONLY!
	};

	///////////////////////////////////////////////////////////

	// RENDER STATES //////////////////////////////////////////

	// Render states for color blend modes
	enum Blend
	{
		BLEND = 0,	// DO NOT USE! INTERNAL USAGE ONLY!

		BLEND_ZERO,
		BLEND_ONE,
		BLEND_SRCCOLOR,
		BLEND_INVSRCCOLOR,
		BLEND_SRCALPHA,
		BLEND_INVSRCALPHA,
		BLEND_DESTALPHA,
		BLEND_INVDESTALPHA,
		BLEND_DESTCOLOR,
		BLEND_INVDESTCOLOR,
		BLEND_SRCALPHASAT,
		BLEND_BOTHSRCALPHA,
		BLEND_BOTHINVSRCALPHA,
		BLEND_BLENDFACTOR,
		BLEND_INVBLENDFACTOR,

		BLEND_MAX	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Render states for comparison functions
	enum Cmp
	{
		CMP = BLEND_MAX + 1,	// DO NOT USE! INTERNAL USAGE ONLY!

		CMP_NEVER,
		CMP_LESS,
		CMP_EQUAL,
		CMP_LESSEQUAL,
		CMP_GREATER,
		CMP_NOTEQUAL,
		CMP_GREATEREQUAL,
		CMP_ALWAYS,

		CMP_MAX,	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Render states for culling
	enum Cull
	{
		CULL = CMP_MAX + 1,	// DO NOT USE! INTERNAL USAGE ONLY!

		CULL_NONE,
		CULL_CW,
		CULL_CCW,

		CULL_MAX	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Render states for Z and W buffers
	enum ZBuffer
	{
		ZB = CULL_MAX + 1,	// DO NOT USE! INTERNAL USAGE ONLY!

		ZB_ENABLED,
		ZB_DISABLED,
		ZB_USEW,

		ZB_MAX,	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Render states for stencil
	enum StencilOp
	{
		STENCILOP = ZB_MAX + 1,	// DO NOT USE! INTERNAL USAGE ONLY!

		STENCILOP_KEEP,
		STENCILOP_ZERO,
		STENCILOP_REPLACE,
		STENCILOP_INCRSAT,
		STENCILOP_DECRSAT,
		STENCILOP_INVERT,
		STENCILOP_INCR,
		STENCILOP_DECR,

		STENCILOP_MAX	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Render states for geometry fill modes
	enum Fill
	{
		FILL = STENCILOP_MAX + 1,	// DO NOT USE! INTERNAL USAGE ONLY!

		FILL_POINT,
		FILL_WIREFRAME,
		FILL_SOLID,

		FILL_MAX	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	enum
	{
		RS_MAX = FILL_MAX + 1	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	///////////////////////////////////////////////////////////

	// SAMPLER STATES /////////////////////////////////////////

	enum
	{
		MAX_ANISOTROPY = 16		// Maximum anisotropy level
	};

	enum
	{
		MAX_NUM_VSAMPLERS = 4,  // Maximum number of texture samplers: vs3.0 has 4, vs2.0 has 0
		MAX_NUM_PSAMPLERS = 16  // Maximum number of texture samplers: ps2.0 and ps3.0 have 16
	};

	enum SamplerFilter
	{
		SF_NONE,	// DO NOT USE! INTERNAL USAGE ONLY!

		SF_MIN_MAG_POINT_MIP_NONE,
		SF_MIN_MAG_LINEAR_MIP_NONE,
		SF_MIN_MAG_POINT_MIP_POINT,
		SF_MIN_MAG_POINT_MIP_LINEAR,
		SF_MIN_MAG_LINEAR_MIP_POINT,
		SF_MIN_MAG_LINEAR_MIP_LINEAR,

		SF_MAX	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	enum SamplerAddressingMode
	{
		SAM_NONE,	// DO NOT USE! INTERNAL USAGE ONLY!

		SAM_CLAMP,
		SAM_WRAP,
		SAM_MIRROR,
		SAM_BORDER,

		SAM_MAX	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// A structure that describes a texture sampler
	struct SamplerStateDesc
	{
		float					fAnisotropy;
		float					fLodBias;
		SamplerFilter			eFilter;
		Vec4f					vBorderColor;
		SamplerAddressingMode	eAddressingMode[3];
		bool					bSRGBEnabled;
	};

	//////////////////////////////////////////////////////////////

	// SHADERS //////////////////////////////////////////////////

	// The types of shader programs
	enum ShaderProgramType
	{
		SPT_NONE,	// DO NOT USE! INTERNAL USAGE ONLY!

		SPT_VERTEX,
		SPT_PIXEL,

		SPT_MAX	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Shader input types
	enum InputType
	{
		IT_NONE,	// DO NOT USE! INTERNAL USAGE ONLY!

		IT_VOID,
		IT_BOOL,
		IT_INT,
		IT_FLOAT,
		IT_TEXTURE,
		IT_TEXTURE1D,
		IT_TEXTURE2D,
		IT_TEXTURE3D,
		IT_TEXTURECUBE,
		IT_SAMPLER,
		IT_SAMPLER1D,
		IT_SAMPLER2D,
		IT_SAMPLER3D,
		IT_SAMPLERCUBE,

		IT_MAX	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Shader input register types
	enum RegisterType
	{
		RT_NONE,	// DO NOT USE! INTERNAL USAGE ONLY!

		RT_BOOL,
		RT_INT4,
		RT_FLOAT4,
		RT_SAMPLER,

		RT_MAX	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// A structure describing a shader input
	struct ShaderInputDesc
	{
		std::string szName;
		InputType eInputType;
		RegisterType eRegisterType;
		unsigned int nRegisterIndex;
		unsigned int nRegisterCount;
		unsigned int nRows;
		unsigned int nColumns;
		unsigned int nArrayElements;
		unsigned int nBytes;
		unsigned int nOffsetInBytes;
	};

	/////////////////////////////////////////////////////////

	// TEXTURES /////////////////////////////////////////////

	// Specifies the format of the pixel
	enum PixelFormat
	{
		PF_NONE,	// DO NOT USE! INTERNAL USAGE ONLY!

		// Small-bit color formats
		PF_R5G6B5,
		PF_A1R5G5B5,
		PF_A4R4G4B4,

		// 8-bit integer formats
		PF_A8,
		PF_L8,
		PF_A8L8,
		PF_R8G8B8,
		PF_A8R8G8B8,
		PF_A8B8G8R8,

		// 16-bit integer formats
		PF_L16,
		PF_G16R16,
		PF_A16B16G16R16,

		// 16-bit floating-point formats ('half float' channels)
		PF_R16F,
		PF_G16R16F,
		PF_A16B16G16R16F,

		// 32-bit floating-point formats ('float' channels)
		PF_R32F,
		PF_G32R32F,
		PF_A32B32G32R32F,

		// DXT compressed formats
		PF_DXT1,
		PF_DXT3,
		PF_DXT5,

		// Depth-stencil format
		PF_D24S8,
		PF_INTZ,

		PF_MAX	// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Specifies the type of the texture
	enum TextureType
	{
		TT_1D,		// One-dimensional texture
		TT_2D,		// Two-dimensional texture
		TT_3D,		// Three-dimensional texture
		TT_CUBE,	// Cubic environment maps

		TT_MAX		// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// Specifies the face of a cubemap
	enum CubeFace
	{
		FACE_NONE = ~0u,

		FACE_XNEG = 0,
		FACE_YPOS,
		FACE_YNEG,
		FACE_ZPOS,
		FACE_XPOS,
		FACE_ZNEG,

		FACE_MAX
	};

	enum
	{
		// This is the maximum number of mipmap levels for a texture.
		// A 16384x16384 texture has a maximum of 15 levels.
		TEX_MAX_MIPMAP_LEVELS = 15
	};

	/////////////////////////////////////////////////////////////////////

	// VERTEX FORMATS ///////////////////////////////////////////////////

	enum
	{
		VF_MAX_ATTRIBUTES	= 16,	// The maximum number of attribues
		VF_MAX_TCOORD_UNITS	= 8,	// The maximum number of texcoord units
		VF_MAX_COLOR_UNITS	= 2		// The maximum number of vertex color units
	};

	//These flags describe the type of data which compose the vertex attributes
	enum VertexAttributeType
	{
		VAT_NONE,	// DO NOT USE! INTERNAL USAGE ONLY!

		VAT_FLOAT1,
		VAT_FLOAT2,
		VAT_FLOAT3,
		VAT_FLOAT4,
		VAT_HALF2,
		VAT_HALF4,
		VAT_UBYTE4,
		VAT_SHORT2,
		VAT_SHORT4,

		VAT_MAX		// DO NOT USE! INTERNAL USAGE ONLY!
	};

	//These flags describe an attribute's purpose
	enum VertexAttributeUsage
	{
		VAU_NONE,	// DO NOT USE! INTERNAL USAGE ONLY!

		VAU_POSITION,
		VAU_NORMAL,
		VAU_TANGENT,
		VAU_BINORMAL,
		VAU_TEXCOORD,
		VAU_COLOR,
		VAU_BLENDINDICES,
		VAU_BLENDWEIGHT,
		VAU_FOGCOORD,
		VAU_PSIZE,

		VAU_MAX		// DO NOT USE! INTERNAL USAGE ONLY!
	};

	// A structure describing a vertex element
	struct VertexElement
	{
		unsigned int			nOffset;	// The element's offset in the vertex format
		VertexAttributeType		eType;		// The data type of the element
		VertexAttributeUsage	eUsage;		// The usage/semantic of the element
		unsigned int			nUsageIdx;	// The usage/semantic index of the element

		friend std::ostream& operator<<(std::ostream& output_out, const VertexElement& ve_in);
		friend std::istream& operator>>(std::istream& s_in, VertexElement& ve_out);
	};

	//////////////////////////////////////////////////////////////////

	// MODELS ////////////////////////////////////////////////////////
	
	class VertexFormat;
	class VertexBuffer;
	class IndexBuffer;

	// A structure describind a model and its meshes
	struct Model
	{
		// Forward declarations
		struct Mesh;
		struct Material;
		struct TextureDesc;

		// A structure describing a mesh and its properties
		struct Mesh
		{
			std::string		szName;			// Name of mesh (not required)
			unsigned int	nVfIdx;			// Index for the corresponding vertex format resource in the resource manager
			unsigned int	nIbIdx;			// Index for the corresponding index buffer resource in the resource manager
			unsigned int	nVbIdx;			// Index for the corresponding vertex buffer resource in the resource manager
			VertexFormat*	pVertexFormat;	// The vertex format for the vertex buffer in which the mesh's vertices are stored
			IndexBuffer*	pIndexBuffer;	// The index buffer for the vertex buffer in which the mesh's vertices are stored
			VertexBuffer*	pVertexBuffer;	// The vertex buffer in which the mesh's vertices are stored
			unsigned int	nMaterialIdx;	// The index for the material used by this mesh

			friend std::ostream& operator<<(std::ostream& output_out, const Mesh& mesh_in);
			friend std::istream& operator>>(std::istream& s_in, Mesh& mesh_out);

			// Grant access to constructor
			friend std::istream& operator>>(std::istream& s_in, Model& model_out);

		private:
			Mesh() {}
			~Mesh();
			friend struct Model;
			friend class LibRendererTools::ModelCompiler;
		};

		// A structure describing a texture used by the mesh's material
		// The final color value from a texture stack (same TextureType, different indices) is calculated as such:
		// ConstantBaseColor (eTexOp0) fTexBlend0 * Texture0 (eTexOp1) fTexBlend1 * Texture1 ...
		struct TextureDesc
		{
			// Describes the contents of a texture
			enum TextureType
			{
				TT_NONE = 0,			// No actual texture, but can be used to configure a sampler for a dynamic texture, for example
				TT_DIFFUSE = 1,			// To be combined with the result of the diffuse lighting equation
				TT_SPECULAR = 2,		// To be combined with the result of the specular lighting equation
				TT_AMBIENT = 3,			// To be combined with the result of the ambient lighting equation
				TT_EMISSIVE = 4,		// To be added to the result of the lighting equation
				TT_HEIGHT = 5,			// Height map
				TT_NORMALS = 6,			// Normal map
				TT_SHININESS = 7,		// Glossiness of the material (i.e. the exponent of the specular lighting equation)
				TT_OPACITY = 8,			// Per-pixel opacity
				TT_DISPLACEMENT = 9,	// Displacement map (purpose is implementation dependent)
				TT_LIGHTMAP = 10,		// Scaling value for the result of the lighting equation (e.g. ambient occlusion)
				TT_REFLECTION = 11,		// Contains the color of the reflection
				TT_UNKNOWN = 12			// Custom texture (i.e. application specific usage)
			};

			enum TextureOp
			{
				TO_MULTIPLY = 0,	// T = T1 * T2
				TO_ADD = 1,			// T = T1 + T2
				TO_SUBTRACT = 2,	// T = T1 - T2
				TO_DIVIDE = 3,		// T = T1 / T2
				TO_SMOOTHADD = 4,	// T = (T1 + T2) - (T1 * T2)
				TO_SIGNEDADD = 5	// T = T1 + (T2 - 0.5)
			};

			enum TextureMapping
			{
				TM_UV = 0,			// Mapping coordinates are taken from UV channel
				TM_SPHERE = 1,		// Spherical mapping
				TM_CYLINDER = 2,	// Cylindrical mapping
				TM_BOX = 3,			// Cubic mapping
				TM_PLANE = 4,		// Planar mapping
				TM_OTHER = 5		// Undefined or unknown mapping
			};

			enum TextureMappingMode
			{
				TMM_WRAP = 0,
				TMM_CLAMP = 1,
				TMM_DECAL = 3,	// If the tex coords are outside [0, 1], the texture is not applied (i.e. SAM_BORDER addressing mode with transparent border color)
				TMM_MIRROR = 2
			};

			std::string			szFilePath;		// Path to the texture file
			TextureType			eTexType;		// Type of texture
			unsigned int		nTexIndex;		// Index in this texture type's stack
			unsigned int		nUVChannel;		// Used texcoord unit
			TextureOp			eTexOp;			// Blend operation
			TextureMapping		eTexMapping;	// Defined how the mapping coords are generated
			float				fTexBlend;		// Blend strength factor
			TextureMappingMode	eTexMapModeU;	// Wrapping mode on the X axis
			TextureMappingMode	eTexMapModeV;	// Wrapping mode on the Y axis

			friend std::ostream& operator<<(std::ostream& output_out, const TextureDesc& tex_in);
			friend std::istream& operator>>(std::istream& s_in, TextureDesc& tex_out);

			// Grant access to constructor
			friend std::istream& operator>>(std::istream& s_in, Material& mat_out);

		private:
			TextureDesc() {}
			~TextureDesc() {}
			friend struct Model;
			friend class LibRendererTools::ModelCompiler;
		};

		// A structure describing a material
		// NB: applications can choose to ignore all or part of this information
		// if they provide their own properties, shaders, render schemes, etc.
		struct Material
		{
			// The list of shading modes has been taken from Blender.
			// See Blender documentation for more information.
			enum ShadingModel
			{
				SM_FLAT = 1,			// Shading is done on per-face base, diffuse only
				SM_GOURAUD = 2,			// Simple Gouraud shading
				SM_PHONG = 3,			// Phong shading
				SM_BLINN = 4,			// Phong-Blinn shading
				SM_TOON = 5,			// Cel shading
				SM_ORENNAYAR = 6,		// Extension to standard Lambertian shading, taking the roughness of the material into account
				SM_MINNAERT = 7,		// Extension to standard Lambertian shading, taking the darkness of the material into account
				SM_COOKTORRANCE = 8,	// Special shader for metallic surfaces
				SM_NOSHADING = 9,		// Constant light influence of 1.0
				SM_FRESNEL = 10			// Fresnel shading
			};

			// These flags define how the final color value of a pixel is computed, based
			// on the previous color at that pixel and the new color value from the material.
			enum BlendMode
			{
				BM_DEFAULT = 0,	// SourceColor * SourceAlpha + DestColor * (1 - SourceAlpha)
				BM_ADDITIVE = 1	// SourceColor * 1 + DestColor * 1
			};
			
			std::string					szName;				// The name of the material (not required)
			bool						bTwoSided;			// Specifies whether this mesh must be rendered without culling
			ShadingModel				eShadingModel;		// The shading model to use for rendering the mesh
			bool						bEnableWireframe;	// Specifies whether this mesh should be rendered in wireframe mode
			BlendMode					eBlendMode;			// Defines how the final color value is computed
			float						fOpacity;			// The opacity of the material (i.e. if alpha testing is enabled)
			float						fBumpScaling;		// Scale value for normals
			float						fShininess;			// Shininess of a phong-shaded material (i.e. the exponent of the phong specular equation)
			float						fReflectivity;		// Reflectivity of the material
			float						fShininessStrength;	// Scale value for the specular color
			float						fRefractIndex;		// The material's refraction index (useful for raytracing)
			Vec3f						vColorDiffuse;		// Diffuse base color; Normalized RGB color components (i.e. between [0, 1])
			Vec3f						vColorAmbient;		// Ambient base color; Normalized RGB color components (i.e. between [0, 1])
			Vec3f						vColorSpecular;		// Specular base color; Normalized RGB color components (i.e. between [0, 1])
			Vec3f						vColorEmissive;		// The amount of light emitted by the object; Normalized RGB color components (i.e. between [0, 1])
			Vec3f						vColorTransparent;	// The transparent color, to be multiplied with the color of translucent light; Normalized RGB color components (i.e. between [0, 1])
			Vec3f						vColorReflective;	// The reflective color, to be multiplied with the color of the reflected light; Normalized RGB color components (i.e. between [0, 1])
			std::vector<TextureDesc*>	arrTexture;			// Textures associated with the material

			friend std::ostream& operator<<(std::ostream& output_out, const Material& mat_in);
			friend std::istream& operator>>(std::istream& s_in, Material& mat_out);

			// Grant access to constructor
			friend std::istream& operator>>(std::istream& s_in, Model& model_out);

		private:
			Material() {}
			~Material();
			friend struct Model;
			friend class LibRendererTools::ModelCompiler;
		};

		std::string				szName;			// Name of the model (not required)
		std::vector<Mesh*>		arrMesh;		// Meshes associated with the model
		std::vector<Material*>	arrMaterial;	// Materials associated with the mesh

		std::string				szSourceFile;	// File from which model was loaded

		LIBRENDERER_DLL friend std::ostream& operator<<(std::ostream& output_out, const Model& model_in);
		friend std::istream& operator>>(std::istream& s_in, Model& model_out);

		private:
			Model() {}
			LIBRENDERER_DLL ~Model();
			friend class ResourceManager;
			friend class LibRendererTools::ModelCompiler;
	};

	//////////////////////////////////////////////////////////////////

	// DEVICE CAPS ///////////////////////////////////////////////////

	// Structure describing device capabilities
	struct DeviceCaps
	{
		bool bCanAutoGenMipmaps;
		bool bDynamicTextures;
		bool bPresentIntervalImmediate;	// no vsync
		bool bPresentIntervalOne;		// 60Hz
		bool bPresentIntervalTwo;		// 30Hz
		bool bPresentIntervalThree;		// 20Hz
		bool bPresentIntervalFour;		// 15Hz
		bool bMrtIndependentBitDepths;
		bool bMrtPostPixelShaderBlending;
		bool bAnisotropicFiltering;
		bool bDepthBias;
		bool bSlopeScaledDepthBias;
		bool bMipmapLodBias;
		bool bWBuffer;
		bool bTexturePow2;
		unsigned int nMaxTextureWidth;
		unsigned int nMaxTextureHeight;
		unsigned int nMaxTextureDepth;
		unsigned int nVertexShaderVersionMajor;
		unsigned int nVertexShaderVersionMinor;
		unsigned int nPixelShaderVersionMajor;
		unsigned int nPixelShaderVersionMinor;
		unsigned int nNumSimultaneousRTs;

		struct SupportedScreenFormat
		{
			PixelFormat		ePixelFormat;
			unsigned int	nWidth;
			unsigned int	nHeight;
			unsigned int	nRefreshRate;
		};
		std::vector<const SupportedScreenFormat> arrSupportedScreenFormats;

		struct SupportedPixelFormat
		{
			TextureType	eTextureType;
			PixelFormat	ePixelFormat;
			BufferUsage	eResourceUsage;
		};
		std::vector<const SupportedPixelFormat> arrSupportedPixelFormats;
	};

	//////////////////////////////////////////////////////////////////
}

#endif // RESOURCEDATA_H