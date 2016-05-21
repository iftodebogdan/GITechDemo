/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	RenderResource.h
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#ifndef RENDER_RESOURCE_H_
#define RENDER_RESOURCE_H_

#include <string>
using namespace std;

#include <gmtl\gmtl.h>
using namespace gmtl;

//#include <ResourceData.h>
namespace Synesthesia3D
{
	class ShaderProgram;
	class ShaderInput;
	class Texture;
	class RenderTarget;
}

#include <Utility/Mutex.h>

namespace GITechDemoApp
{
	using namespace Synesthesia3D;

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

		static void SetResourceManager(ResourceManager* const resMgr);

		static const vector<RenderResource*>& GetResourceList() { return arrResources; }

		static void InitAllResources();
		static void InitAllModels();
		static void InitAllTextures();
		static void InitAllShaders();
		static void InitAllRenderTargets();

		static void FreeAll();

		const char* GetDesc() { return szDesc.c_str(); }
		const ResourceType GetResourceType() { return eResType; }

		const bool IsInitialized() { return bInitialized; }
		virtual const bool Init();
		virtual void Free();

		const bool	TryLockRes() { return MUTEX_TRYLOCK(mResMutex); }
		void		LockRes() { MUTEX_LOCK(mResMutex); }
		void		UnlockRes() { MUTEX_UNLOCK(mResMutex); }

	protected:
		RenderResource(const char* filePath, ResourceType resType);
		virtual ~RenderResource();

		virtual void operator= (const RenderResource& lhs) { assert(0); }

		unsigned int	nId;
		string			szDesc;
		ResourceType	eResType;
		bool			bInitialized;

		MUTEX			mResMutex;
		MUTEX			mInitMutex;

		static vector<RenderResource*> arrResources;
		static ResourceManager* ResMgr;
	};

	class Texture;

	class Model : public RenderResource
	{
	public:
		Model(const char* filePath);
		~Model();

		Synesthesia3D::Model* const		GetModel() { return pModel; }
		const unsigned int	GetTexture(const Synesthesia3D::Model::TextureDesc::TextureType texType, const unsigned int nMatIdx) { return TextureLUT[texType][nMatIdx]; }

	protected:
		const bool Init();
		void Free();

		void operator= (const Model& lhs) { assert(0); }

		Synesthesia3D::Model*	pModel;
		unsigned int			nModelIdx;

		// A lookup table for textures (faster than searching everytime by its file name when setting materials)
		// Usage: TextureIndex = TextureLUT[TextureType][MaterialIndex]
		vector<unsigned int>	TextureLUT[Synesthesia3D::Model::TextureDesc::TT_UNKNOWN];
		vector<Texture*>		TextureList;
	};

	class Texture : public RenderResource
	{
	public:
		Texture(const char* filePath);

		Synesthesia3D::Texture* const		GetTexture() { return pTexture; }
		const unsigned int	GetTextureIndex() { return nTexIdx; }

		const char* GetFilePath() { return szDesc.c_str(); }

	protected:
		const bool Init();
		void Free();

		void operator= (const Texture& lhs) { assert(0); }

		Synesthesia3D::Texture* pTexture;
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
			bInitialized = true;
		}

		ShaderConstantTemplate(const char* name)
			: RenderResource(name, RES_SHADER_CONSTANT)
		{
			bInitialized = true;
		}

		const char* GetName() { return szDesc.c_str(); }

		T& GetCurrentValue() { return currentValue; }

		void operator=(const T& value) { currentValue = value; }
		void operator= (const ShaderConstantTemplate& lhs) { currentValue = lhs.currentValue; }

		operator T&() { return currentValue; }

		template<class T>
		T& operator [] (const int idx) { return currentValue[idx]; }

		template<class DATA_TYPE, unsigned SIZE>
		Vec<DATA_TYPE, SIZE> operator * (const Vec<DATA_TYPE, SIZE>& rhs) { return currentValue * rhs; }

	protected:
		T		currentValue;

		template<class T>
		friend T operator * (const T& lhs, const ShaderConstantTemplate<T>& rhs);

		template<class T>
		friend T operator * (const ShaderConstantTemplate<T>& lhs, const T& rhs);

		template<class T, class U>
		friend T operator * (const ShaderConstantTemplate<T>& lhs, const ShaderConstantTemplate<U>& rhs);

		template<class DATA_TYPE, unsigned SIZE>
		friend Vec<DATA_TYPE, SIZE> operator * (const Vec<DATA_TYPE, SIZE>& lhs, ShaderConstantTemplate<T>& rhs);
	};

	template<class T>
	T operator * (const T& lhs, const ShaderConstantTemplate<T>& rhs) { return lhs * rhs.currentValue; }

	template<class T>
	T operator * (const ShaderConstantTemplate<T>& lhs, const T& rhs) { return lhs.currentValue * rhs; }

	template<class T, class U>
	T operator * (const ShaderConstantTemplate<T>& lhs, const ShaderConstantTemplate<U>& rhs) { return lhs.currentValue * rhs.currentValue; }

	template<class DATA_TYPE, unsigned SIZE, class T>
	Vec<DATA_TYPE, SIZE> operator * (const Vec<DATA_TYPE, SIZE>& lhs, ShaderConstantTemplate<T>& rhs) { return lhs * rhs.currentValue; }

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
			ShaderProgramType	eShaderType;
			InputType			eConstantType;
			unsigned int		nNumRows;
			unsigned int		nNumColumns;
			unsigned int		nNumArrayElem;
		};

		const bool Init();
		void Free();

		void operator= (const Shader& lhs) { assert(0); }

		ShaderProgram*	pVertexShaderProg;
		unsigned int	nVertexShaderProgIdx;
		ShaderProgram*	pPixelShaderProg;
		unsigned int	nPixelShaderProgIdx;

		ShaderInput*	pVertexShaderInput;
		unsigned int	nVertexShaderInputIdx;
		ShaderInput*	pPixelShaderInput;
		unsigned int	nPixelShaderInputIdx;

		vector<ShaderConstantInstance>	arrConstantList;
	};

	class RenderTarget : public RenderResource
	{
	public:
		RenderTarget(const char* name, const unsigned int targetCount,
			PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1,
			PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
			const float widthRatio, const float heightRatio,
			PixelFormat depthStencilFormat);
		RenderTarget(const char* name, const unsigned int targetCount,
			PixelFormat pixelFormatRT0, PixelFormat pixelFormatRT1,
			PixelFormat pixelFormatRT2, PixelFormat pixelFormatRT3,
			const unsigned int width, const unsigned int height,
			PixelFormat depthStencilFormat);

		void Enable();
		void Disable();

		Synesthesia3D::RenderTarget* const GetRenderTarget() { return pRenderTarget; }

	protected:
		const bool Init();
		void Free();
		
		void operator= (const RenderTarget& lhs) { assert(0); }

		Synesthesia3D::RenderTarget*	pRenderTarget;
		unsigned int nRenderTargetIdx;
		unsigned int nTargetCount;
		PixelFormat ePixelFormatRT0;
		PixelFormat ePixelFormatRT1;
		PixelFormat ePixelFormatRT2;
		PixelFormat ePixelFormatRT3;
		unsigned int nWidth;
		unsigned int nHeight;
		float widthRatio;
		float heightRatio;
		PixelFormat eDepthStencilFormat;
		bool bIsDynamic;
	};
}
#endif