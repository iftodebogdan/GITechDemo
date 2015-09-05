#ifndef RENDER_RESOURCE_H_
#define RENDER_RESOURCE_H_

#include <string>
using namespace std;

#include <gmtl\gmtl.h>
using namespace gmtl;

#include <ResourceData.h>
namespace LibRendererDll
{
	class ShaderProgram;
	class ShaderTemplate;
	class ShaderInput;
	class Texture;
	class RenderTarget;
}

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

		static void SetResourceManager(LibRendererDll::ResourceManager* const resMgr);

		static void InitAllResources();
		static void InitAllModels();
		static void InitAllTextures();
		static void InitAllShaders();
		static void InitAllRenderTargets();

		const ResourceType GetResourceType() { return eResType; }
		const bool IsInitialized() { return bInitialized; }

	protected:
		RenderResource(const char* filePath, ResourceType resType);
		virtual ~RenderResource() {}

		virtual void Init();

		unsigned int	nId;
		string			szDesc;
		ResourceType	eResType;
		bool			bInitialized;

		static vector<RenderResource*> arrResources;
		static LibRendererDll::ResourceManager* ResMgr;
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
		// Usage: TextureIndex = TextureLUT[TextureType][MaterialIndex]
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
			: RenderResource(name, RES_SHADER_CONSTANT)
		{
			currentValue = defaultVal;
		}

		ShaderConstantTemplate(const char* name)
			: RenderResource(name, RES_SHADER_CONSTANT)
		{

		}

		const char* GetName() { return szDesc.c_str(); }

		T& GetCurrentValue() { return currentValue; }

		void operator=(const T& value) { currentValue = value; }

		operator T&() { return currentValue; }

		template<class T>
		T& operator [] (const int idx) { return currentValue[idx]; }

		template<class DATA_TYPE, unsigned SIZE>
		Vec<DATA_TYPE, SIZE> operator * (const Vec<DATA_TYPE, SIZE>& rhs) { return currentValue * rhs; }

	protected:
		T		currentValue;

		template<class T>
		friend T operator * (const T lhs, const ShaderConstantTemplate<T> rhs);

		template<class T>
		friend T operator * (const ShaderConstantTemplate<T> lhs, const T rhs);

		template<class T, class U>
		friend T operator * (const ShaderConstantTemplate<T> lhs, const ShaderConstantTemplate<U> rhs);

		template<class DATA_TYPE, unsigned SIZE>
		friend Vec<DATA_TYPE, SIZE> operator * (const Vec<DATA_TYPE, SIZE> lhs, ShaderConstantTemplate<T> rhs);
	};

	template<class T>
	T operator * (const T lhs, const ShaderConstantTemplate<T> rhs) { return lhs * rhs.currentValue; }

	template<class T>
	T operator * (const ShaderConstantTemplate<T> lhs, const T rhs) { return lhs.currentValue * rhs; }

	template<class T, class U>
	T operator * (const ShaderConstantTemplate<T> lhs, const ShaderConstantTemplate<U> rhs) { return lhs.currentValue * rhs.currentValue; }

	template<class DATA_TYPE, unsigned SIZE, class T>
	Vec<DATA_TYPE, SIZE> operator * (const Vec<DATA_TYPE, SIZE> lhs, ShaderConstantTemplate<T> rhs) { return lhs * rhs.currentValue; }

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
		RenderTarget(const char* name, const unsigned int targetCount,
			LibRendererDll::PixelFormat pixelFormatRT0, LibRendererDll::PixelFormat pixelFormatRT1,
			LibRendererDll::PixelFormat pixelFormatRT2, LibRendererDll::PixelFormat pixelFormatRT3,
			const float widthRatio, const float heightRatio,
			LibRendererDll::PixelFormat depthStencilFormat);
		RenderTarget(const char* name, const unsigned int targetCount,
			LibRendererDll::PixelFormat pixelFormatRT0, LibRendererDll::PixelFormat pixelFormatRT1,
			LibRendererDll::PixelFormat pixelFormatRT2, LibRendererDll::PixelFormat pixelFormatRT3,
			const unsigned int width, const unsigned int height,
			LibRendererDll::PixelFormat depthStencilFormat);

		void Enable();
		void Disable();

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