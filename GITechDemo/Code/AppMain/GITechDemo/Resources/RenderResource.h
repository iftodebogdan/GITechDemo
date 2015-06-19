#ifndef RENDER_RESOURCE_H_
#define RENDER_RESOURCE_H_

#include <string>
using namespace std;

#include "ShaderTemplate.h"
#include "ShaderInput.h"
#include "RenderTarget.h"

#include <gmtl\gmtl.h>
using namespace gmtl;

namespace GITechDemoApp
{
	class RenderResource
	{
	public:
		enum ResourceType
		{
			RES_NONE = 0,
			RES_MODEL,
			RES_SHADER,
			RES_SHADER_CONSTANT,
			RES_TEXTURE,
			RES_RENDERTARGET,
			RES_MAX
		};

		static void InitAllResources(LibRendererDll::ResourceManager* const resMgr);
		static void InitAllModels(LibRendererDll::ResourceManager* const resMgr);
		static void InitAllTextures(LibRendererDll::ResourceManager* const resMgr);
		static void InitAllShaders(LibRendererDll::ResourceManager* const resMgr);
		static void InitAllRenderTargets(LibRendererDll::ResourceManager* const resMgr);

		const ResourceType GetResourceType() { return eResType; }

	protected:
		RenderResource(const char* filePath, ResourceType resType);
		virtual ~RenderResource() {}

		virtual void Init() = 0;

		unsigned int	nId;
		string			szFile;
		ResourceType	eResType;
		bool			bInitialized;

		static vector<RenderResource*>	arrResources;
		static LibRendererDll::ResourceManager*		ResMgr;
	};

	class Texture;

	class Model : public RenderResource
	{
	public:
		Model(const char* filePath);
		~Model();

		LibRendererDll::Model* const		GetModel() { return pModel; }
		const unsigned int	GetTexture(const LibRendererDll::Model::TextureDesc::TextureType texType, const unsigned int nMatIdx) { return TextureLUT[texType][nMatIdx]; }

	protected:
		void	Init();

		LibRendererDll::Model*	pModel;

		// A lookup table for textures (faster than searching everytime by its file name when setting materials)
		// Usage: TextureLUT[TextureType][MaterialIndex] = TextureIndex
		vector<unsigned int>	TextureLUT[LibRendererDll::Model::TextureDesc::TT_UNKNOWN];
		vector<Texture*>		TextureList;
	};

	class Texture : public RenderResource
	{
	public:
		Texture(const char* filePath);

		LibRendererDll::Texture* const		GetTexture() { return pTexture; }
		const unsigned int	GetTextureIndex() { return nTexIdx; }

	protected:
		void	Init();

		LibRendererDll::Texture* pTexture;
		unsigned int nTexIdx;

		friend class Model;
	};

	template<class T>
	class ShaderConstantTemplate : public RenderResource
	{
	public:
		ShaderConstantTemplate(const char* name, T defaultVal)
			: RenderResource("", RES_SHADER_CONSTANT)
		{
			szName = name;
			currentValue = defaultVal;
			bInitialized = true;
		}

		ShaderConstantTemplate(const char* name)
			: RenderResource("", RES_SHADER_CONSTANT)
		{
			szName = name;
			bInitialized = true;
		}

		const char* GetName() { return szName.c_str(); }

		T& GetCurrentValue() { return currentValue; }

		void operator=(const T& value) { currentValue = value; }
	protected:
		void Init() { bInitialized = true; }

		string	szName;
		T		currentValue;
	};

	class Shader : public RenderResource
	{
	public:
		Shader(const char* filePath);

		void Enable();
		void Disable();

	protected:
		struct ShaderConstantInstance
		{
			RenderResource*		pShaderConstantTemplate;
			unsigned int		nShaderConstantHandle;
			LibRendererDll::ShaderProgramType	eShaderType;
			LibRendererDll::InputType			eConstantType;
			unsigned int		nNumRows;
			unsigned int		nNumColumns;
			unsigned int		nNumArrayElem;
		};

		void Init();

		LibRendererDll::ShaderProgram*	pVertexShaderProg;
		LibRendererDll::ShaderProgram*	pPixelShaderProg;

		LibRendererDll::ShaderTemplate* pVertexShaderTemplate;
		LibRendererDll::ShaderTemplate* pPixelShaderTemplate;

		LibRendererDll::ShaderInput*	pVertexShaderInput;
		LibRendererDll::ShaderInput*	pPixelShaderInput;

		vector<ShaderConstantInstance>	arrConstantList;
	};

	class RenderTarget : public RenderResource
	{
	public:
		RenderTarget(const unsigned int targetCount,
			LibRendererDll::PixelFormat pixelFormatRT0, LibRendererDll::PixelFormat pixelFormatRT1,
			LibRendererDll::PixelFormat pixelFormatRT2, LibRendererDll::PixelFormat pixelFormatRT3,
			const float widthRatio, const float heightRatio,
			LibRendererDll::PixelFormat depthStencilFormat);
		RenderTarget(const unsigned int targetCount,
			LibRendererDll::PixelFormat pixelFormatRT0, LibRendererDll::PixelFormat pixelFormatRT1,
			LibRendererDll::PixelFormat pixelFormatRT2, LibRendererDll::PixelFormat pixelFormatRT3,
			const unsigned int width, const unsigned int height,
			LibRendererDll::PixelFormat depthStencilFormat);

		void Enable() { pRenderTarget->Enable(); }
		void Disable() { pRenderTarget->Disable(); }

		LibRendererDll::RenderTarget* const GetRenderTarget() { return pRenderTarget; }

	protected:
		void Init();

		LibRendererDll::RenderTarget*	pRenderTarget;
		unsigned int nTargetCount;
		LibRendererDll::PixelFormat ePixelFormatRT0;
		LibRendererDll::PixelFormat ePixelFormatRT1;
		LibRendererDll::PixelFormat ePixelFormatRT2;
		LibRendererDll::PixelFormat ePixelFormatRT3;
		unsigned int nWidth;
		unsigned int nHeight;
		float widthRatio;
		float heightRatio;
		LibRendererDll::PixelFormat eDepthStencilFormat;
		bool bIsDynamic;
	};
}
#endif