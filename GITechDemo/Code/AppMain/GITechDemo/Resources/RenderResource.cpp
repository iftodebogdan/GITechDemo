#include "stdafx.h"

#include "Framework.h"
using namespace AppFramework;

#include "RenderResource.h"
using namespace GITechDemoApp;

#include <Renderer.h>
#include <ShaderTemplate.h>
#include <ShaderInput.h>
#include <RenderTarget.h>
#include <ResourceManager.h>
#include <Texture.h>

vector<RenderResource*> RenderResource::arrResources;
LibRendererDll::ResourceManager* RenderResource::ResMgr = nullptr;

namespace GITechDemoApp
{
	extern const char* const ResourceTypeMap[RenderResource::RES_MAX] = {
		"",
		"Model",
		"Shader",
		"Shader constant",
		"Texture",
		"Render target",
	};
}

RenderResource::RenderResource(const char* filePath, ResourceType resType)
	: nId((unsigned int)arrResources.size())
	, szDesc(filePath)
	, eResType(resType)
	, bInitialized(false)
{
	MUTEX_INIT(mResMutex);
	MUTEX_INIT(mInitMutex);

	arrResources.push_back(this);
}

RenderResource::~RenderResource()
{
	MUTEX_DESTROY(mResMutex);
	MUTEX_DESTROY(mInitMutex);
}

const bool RenderResource::Init()
{
	if (bInitialized)
		return false;

	if (MUTEX_TRYLOCK(mInitMutex))
	{
		if (!bInitialized)
		{
			//cout << ResourceTypeMap[eResType] << ": \"" << szDesc.c_str() << "\"" << endl;
			bInitialized = true;
			MUTEX_UNLOCK(mInitMutex);
			return true;
		}
		else
		{
			MUTEX_UNLOCK(mInitMutex);
			return false;
		}
	}
	else
		return false;
}

void RenderResource::Free()
{
	MUTEX_LOCK(mInitMutex);
	bInitialized = false;
	MUTEX_UNLOCK(mInitMutex);
}

void RenderResource::SetResourceManager(LibRendererDll::ResourceManager* const resMgr)
{
	ResMgr = resMgr;
}

void RenderResource::InitAllResources()
{
	cout << "Start initializing resources" << endl;
	cout << endl;
	InitAllModels();
	cout << endl;
	InitAllTextures();
	cout << endl;
	InitAllShaders();
	cout << endl;
	InitAllRenderTargets();
	cout << endl;
	cout << "Done initializing resources" << endl;
}

void RenderResource::InitAllModels()
{
	for (unsigned int i = 0; i < arrResources.size(); i++)
		if (arrResources[i]->eResType == RES_MODEL)
			arrResources[i]->Init();
}

void RenderResource::InitAllTextures()
{
	for (unsigned int i = 0; i < arrResources.size(); i++)
		if (arrResources[i]->eResType == RES_TEXTURE)
			arrResources[i]->Init();
}

void RenderResource::InitAllShaders()
{
	for (unsigned int i = 0; i < arrResources.size(); i++)
		if (arrResources[i]->eResType == RES_SHADER)
			arrResources[i]->Init();
}

void RenderResource::InitAllRenderTargets()
{
	for (unsigned int i = 0; i < arrResources.size(); i++)
		if (arrResources[i]->eResType == RES_RENDERTARGET)
			arrResources[i]->Init();
}

void RenderResource::FreeAll()
{
	for (unsigned int i = 0; i < arrResources.size(); i++)
		if (arrResources[i]->eResType != RES_SHADER_CONSTANT)
			arrResources[i]->Free();
}

Shader::Shader(const char* filePath)
	: RenderResource(filePath, RES_SHADER)
	, pVertexShaderProg(nullptr)
	, pPixelShaderProg(nullptr)
	, pVertexShaderTemplate(nullptr)
	, pPixelShaderTemplate(nullptr)
	, pVertexShaderInput(nullptr)
	, pPixelShaderInput(nullptr)
	, nVertexShaderProgIdx(~0u)
	, nPixelShaderProgIdx(~0u)
	, nVertexShaderTemplateIdx(~0u)
	, nPixelShaderTemplateIdx(~0u)
	, nVertexShaderInputIdx(~0u)
	, nPixelShaderInputIdx(~0u)
{}

const bool Shader::Init()
{
	if (!ResMgr || bInitialized)
		return false;

	if (RenderResource::Init())
	{
		nVertexShaderProgIdx = ResMgr->CreateShaderProgram(szDesc.c_str(), LibRendererDll::SPT_VERTEX);
		pVertexShaderProg = ResMgr->GetShaderProgram(nVertexShaderProgIdx);
		nPixelShaderProgIdx = ResMgr->CreateShaderProgram(szDesc.c_str(), LibRendererDll::SPT_PIXEL);
		pPixelShaderProg = ResMgr->GetShaderProgram(nPixelShaderProgIdx);

		nVertexShaderTemplateIdx = ResMgr->CreateShaderTemplate(pVertexShaderProg);
		pVertexShaderTemplate = ResMgr->GetShaderTemplate(nVertexShaderTemplateIdx);
		nPixelShaderTemplateIdx = ResMgr->CreateShaderTemplate(pPixelShaderProg);
		pPixelShaderTemplate = ResMgr->GetShaderTemplate(nPixelShaderTemplateIdx);

		nVertexShaderInputIdx = ResMgr->CreateShaderInput(pVertexShaderTemplate);
		pVertexShaderInput = ResMgr->GetShaderInput(nVertexShaderInputIdx);
		nPixelShaderInputIdx = ResMgr->CreateShaderInput(pPixelShaderTemplate);
		pPixelShaderInput = ResMgr->GetShaderInput(nPixelShaderInputIdx);

		for (int spt = LibRendererDll::SPT_NONE; spt < LibRendererDll::SPT_MAX; spt++)
		{
			const LibRendererDll::ShaderInput* shdInput = nullptr;

			switch (spt)
			{
			case LibRendererDll::SPT_VERTEX:
				shdInput = pVertexShaderInput;
				break;
			case LibRendererDll::SPT_PIXEL:
				shdInput = pPixelShaderInput;
				break;
			default:
				continue;
			}

			for (unsigned int i = 0; i < shdInput->GetInputCount(); i++)
			{
				const LibRendererDll::ShaderInputDesc& desc = shdInput->GetInputDesc(i);

				for (unsigned int j = 0; j < arrResources.size(); j++)
				{
					if (arrResources[j]->GetResourceType() == RES_SHADER_CONSTANT &&
						desc.szName == ((ShaderConstantTemplate<void*>*)arrResources[j])->GetName())
					{
						ShaderConstantInstance constInst;
						constInst.pShaderConstantTemplate = arrResources[j];
						constInst.nShaderConstantHandle = i;
						constInst.eShaderType = (LibRendererDll::ShaderProgramType)spt;
						constInst.eConstantType = desc.eInputType;
						constInst.nNumRows = desc.nRows;
						constInst.nNumColumns = desc.nColumns;
						constInst.nNumArrayElem = desc.nArrayElements;

						arrConstantList.push_back(constInst);
					}
				}
			}
		}
		/*
		for (unsigned int i = 0; i < arrResources.size(); i++)
		{
			if (arrResources[i]->GetResourceType() == RES_SHADER_CONSTANT)
			{
				for (int spt = LibRendererDll::SPT_VERTEX; spt < LibRendererDll::SPT_MAX; spt++)
				{
					LibRendererDll::ShaderInput* shdInput = nullptr;

					switch (spt)
					{
					case LibRendererDll::SPT_VERTEX:
						shdInput = pVertexShaderInput;
						break;
					case LibRendererDll::SPT_PIXEL:
						shdInput = pPixelShaderInput;
						break;
					default:
						assert(0);
					}

					unsigned int handle;
					shdInput->GetInputHandleByName(((ShaderConstantTemplate<void*>*)arrResources[i])->GetName(), handle);

					if (handle != ~0u)
					{
						const LibRendererDll::ShaderInputDesc& desc = shdInput->GetInputDesc(handle);
						ShaderConstantInstance constInst;

						constInst.pShaderConstantTemplate = arrResources[i];
						constInst.nShaderConstantHandle = handle;
						constInst.eShaderType = (LibRendererDll::ShaderProgramType)spt;
						constInst.eConstantType = desc.eInputType;
						constInst.nNumRows = desc.nRows;
						constInst.nNumColumns = desc.nColumns;
						constInst.nNumArrayElem = desc.nArrayElements;

						arrConstantList.push_back(constInst);
					}
				}
			}
		}*/

		return true;
	}
	else
		return false;
}

void Shader::Free()
{
	RenderResource::Free();

	if (ResMgr)
	{
		ResMgr->ReleaseShaderInput(nPixelShaderInputIdx);
		ResMgr->ReleaseShaderInput(nVertexShaderInputIdx);
		ResMgr->ReleaseShaderTemplate(nPixelShaderTemplateIdx);
		ResMgr->ReleaseShaderTemplate(nVertexShaderTemplateIdx);
		ResMgr->ReleaseShaderProgram(nPixelShaderProgIdx);
		ResMgr->ReleaseShaderProgram(nVertexShaderProgIdx);
	}

	nPixelShaderInputIdx = ~0u;
	pPixelShaderInput = nullptr;

	nVertexShaderInputIdx = ~0u;
	pVertexShaderInput = nullptr;

	nPixelShaderTemplateIdx = ~0u;
	pPixelShaderTemplate = nullptr;

	nVertexShaderTemplateIdx = ~0u;
	pVertexShaderTemplate = nullptr;

	nPixelShaderProgIdx = ~0u;
	pPixelShaderProg = nullptr;

	nVertexShaderProgIdx = ~0u;
	pVertexShaderProg = nullptr;

	arrConstantList.clear();
}

template<typename T, unsigned int ROWS>
void SetMatrixHelper(
	LibRendererDll::ShaderInput* const shdInput,
	const unsigned int handle,
	const unsigned int columns, 
	const T* const data)
{
	switch (columns)
	{
	case 1:
		shdInput->SetMatrixArray<T, ROWS, 1>(handle, (Matrix<T, ROWS, 1>*)data);
		break;
	case 2:
		shdInput->SetMatrixArray<T, ROWS, 2>(handle, (Matrix<T, ROWS, 2>*)data);
		break;
	case 3:
		shdInput->SetMatrixArray<T, ROWS, 3>(handle, (Matrix<T, ROWS, 3>*)data);
		break;
	case 4:
		shdInput->SetMatrixArray<T, ROWS, 4>(handle, (Matrix<T, ROWS, 4>*)data);
		break;
	default:
		assert(false);
	}
}

void Shader::Enable()
{
	PUSH_PROFILE_MARKER(szDesc.c_str());

	for (unsigned int i = 0; i < arrConstantList.size(); i++)
	{
		LibRendererDll::ShaderInput* shdInput = nullptr;

		switch (arrConstantList[i].eShaderType)
		{
		case LibRendererDll::SPT_VERTEX:
			shdInput = pVertexShaderInput;
			break;
		case LibRendererDll::SPT_PIXEL:
			shdInput = pPixelShaderInput;
			break;
		default:
			assert(false);
		}

		switch (arrConstantList[i].eConstantType)
		{
		case LibRendererDll::IT_BOOL:
			if (arrConstantList[i].nNumArrayElem > 1)
				switch (arrConstantList[i].nNumRows)
				{
				case 1:
					shdInput->SetBoolArray(
						arrConstantList[i].nShaderConstantHandle,
						((ShaderConstantTemplate<bool*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 2:
					SetMatrixHelper<bool, 2>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						((ShaderConstantTemplate<bool*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 3:
					SetMatrixHelper<bool, 3>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						((ShaderConstantTemplate<bool*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 4:
					SetMatrixHelper<bool, 4>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						((ShaderConstantTemplate<bool*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				default:
					assert(false);
				}
			else
				switch (arrConstantList[i].nNumRows)
				{
				case 1:
					shdInput->SetBoolArray(
						arrConstantList[i].nShaderConstantHandle,
						&((ShaderConstantTemplate<bool>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 2:
					SetMatrixHelper<bool, 2>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						&((ShaderConstantTemplate<bool>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 3:
					SetMatrixHelper<bool, 3>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						&((ShaderConstantTemplate<bool>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 4:
					SetMatrixHelper<bool, 4>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						&((ShaderConstantTemplate<bool>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				default:
					assert(false);
				}
			break;
		case LibRendererDll::IT_FLOAT:
			if (arrConstantList[i].nNumArrayElem > 1)
				switch (arrConstantList[i].nNumRows)
				{
				case 1:
					shdInput->SetFloatArray(
						arrConstantList[i].nShaderConstantHandle,
						((ShaderConstantTemplate<float*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 2:
					SetMatrixHelper<float, 2>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						((ShaderConstantTemplate<float*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 3:
					SetMatrixHelper<float, 3>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						((ShaderConstantTemplate<float*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 4:
					SetMatrixHelper<float, 4>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						((ShaderConstantTemplate<float*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				default:
					assert(false);
				}
			else
				switch (arrConstantList[i].nNumRows)
				{
				case 1:
					shdInput->SetFloatArray(
						arrConstantList[i].nShaderConstantHandle,
						&((ShaderConstantTemplate<float>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 2:
					SetMatrixHelper<float, 2>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						&((ShaderConstantTemplate<float>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 3:
					SetMatrixHelper<float, 3>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						&((ShaderConstantTemplate<float>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 4:
					SetMatrixHelper<float, 4>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						&((ShaderConstantTemplate<float>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				default:
					assert(false);
				}
			break;
		case LibRendererDll::IT_INT:
			if (arrConstantList[i].nNumArrayElem > 1)
				switch (arrConstantList[i].nNumRows)
				{
				case 1:
					shdInput->SetIntArray(
						arrConstantList[i].nShaderConstantHandle,
						((ShaderConstantTemplate<int*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 2:
					SetMatrixHelper<int, 2>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						((ShaderConstantTemplate<int*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 3:
					SetMatrixHelper<int, 3>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						((ShaderConstantTemplate<int*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 4:
					SetMatrixHelper<int, 4>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						((ShaderConstantTemplate<int*>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				default:
					assert(false);
				}
			else
				switch (arrConstantList[i].nNumRows)
				{
				case 1:
					shdInput->SetIntArray(
						arrConstantList[i].nShaderConstantHandle,
						&((ShaderConstantTemplate<int>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 2:
					SetMatrixHelper<int, 2>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						&((ShaderConstantTemplate<int>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 3:
					SetMatrixHelper<int, 3>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						&((ShaderConstantTemplate<int>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				case 4:
					SetMatrixHelper<int, 4>(
						shdInput,
						arrConstantList[i].nShaderConstantHandle,
						arrConstantList[i].nNumColumns,
						&((ShaderConstantTemplate<int>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
					break;
				default:
					assert(false);
				}
			break;
		case LibRendererDll::IT_SAMPLER:
		case LibRendererDll::IT_SAMPLER1D:
		case LibRendererDll::IT_SAMPLER2D:
		case LibRendererDll::IT_SAMPLER3D:
		case LibRendererDll::IT_SAMPLERCUBE:
			shdInput->SetTexture(
				arrConstantList[i].nShaderConstantHandle,
				((ShaderConstantTemplate<unsigned int>*)arrConstantList[i].pShaderConstantTemplate)->GetCurrentValue());
			break;
		default:
			assert(false);
		}
	}

	pVertexShaderTemplate->Enable(pVertexShaderInput);
	pPixelShaderTemplate->Enable(pPixelShaderInput);
}

void Shader::Disable()
{
	pVertexShaderTemplate->Disable();
	pPixelShaderTemplate->Disable();

	POP_PROFILE_MARKER();
}

Model::Model(const char* filePath)
	: RenderResource(filePath, RES_MODEL)
	, pModel(nullptr)
	, nModelIdx(~0u)
{}

Model::~Model()
{
	for (unsigned int i = 0; i < TextureList.size(); i++)
	{
		arrResources[TextureList[i]->nId] = nullptr;
		delete TextureList[i];
	}

	TextureList.clear();
}

const bool Model::Init()
{
	if (!ResMgr || bInitialized)
		return false;

	if (RenderResource::Init())
	{
		nModelIdx = ResMgr->CreateModel(szDesc.c_str());
		pModel = ResMgr->GetModel(nModelIdx);

		for (unsigned int tt = LibRendererDll::Model::TextureDesc::TT_NONE; tt < LibRendererDll::Model::TextureDesc::TT_UNKNOWN; tt++)
			TextureLUT[tt].resize(pModel->arrMaterial.size(), -1);

		// Populate the TextureList
		for (unsigned int i = 0; i < pModel->arrMaterial.size(); i++)
		{
			for (unsigned int j = 0; j < pModel->arrMaterial[i]->arrTexture.size(); j++)
			{
				size_t found = szDesc.find_last_of("/\\");
				string filePath = (found ? szDesc.substr(0, found + 1) : "") + pModel->arrMaterial[i]->arrTexture[j]->szFilePath;

				const unsigned int offset = (unsigned int)filePath.rfind('.');
				if (offset != string::npos)
					filePath.replace(offset, UINT_MAX, ".lrt");

				unsigned int texIdx = -1;
				texIdx = ResMgr->FindTexture(filePath.c_str());
				if (texIdx == -1)
				{
					bool bDuplicateFound = false;
					for (unsigned int k = 0; k < TextureList.size(); k++)
					{
						if (TextureList[k]->GetFilePath() == filePath)
						{
							bDuplicateFound = true;
							break;
						}
					}

					if (!bDuplicateFound)
						TextureList.push_back(new Texture(filePath.c_str()));
				}
			}
		}

		// Initialize the textures later, so as to allow other threads
		// to maybe pick some of them up while populating TextureList.
		for (unsigned int i = 0; i < TextureList.size(); i++)
		{
			if(TextureList[i]->TryLockRes())
			{
				TextureList[i]->Init();
				TextureList[i]->UnlockRes();
			}
		}

		for (unsigned int i = 0; i < pModel->arrMaterial.size(); i++)
		{
			for (unsigned int j = 0; j < pModel->arrMaterial[i]->arrTexture.size(); j++)
			{
				size_t found = szDesc.find_last_of("/\\");
				string filePath = (found ? szDesc.substr(0, found + 1) : "") + pModel->arrMaterial[i]->arrTexture[j]->szFilePath;

				const unsigned int offset = (unsigned int)filePath.rfind('.');
				if (offset != string::npos)
					filePath.replace(offset, UINT_MAX, ".lrt");

				unsigned int texIdx = -1;
				//texIdx = ResMgr->FindTexture(filePath.c_str());
				for (unsigned int k = 0; k < TextureList.size(); k++)
				{
					if (TextureList[k]->GetFilePath() == filePath)
					{
						bool bGotLockOnTex = false;
						do
						{
							if (TextureList[k]->TryLockRes())
							{
								texIdx = TextureList[k]->GetTextureIndex();
								bGotLockOnTex = true;
								break;
							}
							else
							{
								texIdx = TextureList[k]->GetTextureIndex();
								Framework::GetInstance()->Sleep(1);
							}
						} while (texIdx == ~0u);

						// This should never happen, but for extra safety, in case this texture
						// was locked by another thread when we tried to Init() it above,
						// and somehow it still isn't initialized after being unlocked:
						if (texIdx == ~0u && bGotLockOnTex)
						{
							TextureList[k]->Init();
							TextureList[k]->UnlockRes();
							bGotLockOnTex = false;
						}

						break;
					}
				}
				assert(texIdx != -1);

				if (texIdx != -1)
				{
					LibRendererDll::Texture* tex = ResMgr->GetTexture(texIdx);

					// Diffuse albedo texture
					if (pModel->arrMaterial[i]->arrTexture[j]->eTexType == LibRendererDll::Model::TextureDesc::TT_DIFFUSE)
					{
						tex->SetAnisotropy(/*MAX_ANISOTROPY*/ 1.f);
						tex->SetFilter(LibRendererDll::SF_MIN_MAG_LINEAR_MIP_LINEAR);
						tex->SetSRGBEnabled(true);
					}

					// Specular power texture
					if (pModel->arrMaterial[i]->arrTexture[j]->eTexType == LibRendererDll::Model::TextureDesc::TT_SPECULAR)
					{
						tex->SetAnisotropy(1.f);
						tex->SetFilter(LibRendererDll::SF_MIN_MAG_LINEAR_MIP_LINEAR);
					}

					// Normal map
					if (pModel->arrMaterial[i]->arrTexture[j]->eTexType == LibRendererDll::Model::TextureDesc::TT_HEIGHT)
					{
						tex->SetAnisotropy(1.f);
						tex->SetFilter(LibRendererDll::SF_MIN_MAG_LINEAR_MIP_LINEAR);
					}

					// Material type map (dielectric/metallic)
					if (pModel->arrMaterial[i]->arrTexture[j]->eTexType == LibRendererDll::Model::TextureDesc::TT_AMBIENT)
					{
						tex->SetAnisotropy(1.f);
						tex->SetFilter(LibRendererDll::SF_MIN_MAG_LINEAR_MIP_LINEAR);
					}

					// Roughness map
					if (pModel->arrMaterial[i]->arrTexture[j]->eTexType == LibRendererDll::Model::TextureDesc::TT_SHININESS)
					{
						tex->SetAnisotropy(1.f);
						tex->SetFilter(LibRendererDll::SF_MIN_MAG_LINEAR_MIP_LINEAR);
					}
				}

				assert(TextureLUT[pModel->arrMaterial[i]->arrTexture[j]->eTexType][i] == -1 ||
					TextureLUT[pModel->arrMaterial[i]->arrTexture[j]->eTexType][i] == texIdx);

				if (TextureLUT[pModel->arrMaterial[i]->arrTexture[j]->eTexType][i] == -1)
					TextureLUT[pModel->arrMaterial[i]->arrTexture[j]->eTexType][i] = texIdx;
			}
		}

		return true;
	}
	else
		return false;
}

void Model::Free()
{
	RenderResource::Free();

	if(ResMgr)
		ResMgr->ReleaseModel(nModelIdx);

	nModelIdx = ~0u;
	pModel = nullptr;

	for (unsigned int i = 0; i < TextureList.size(); i++)
		for (unsigned int j = 0; j < arrResources.size(); j++)
			if (TextureList[i] == arrResources[j])
			{
				arrResources.erase(arrResources.begin() + j);
				j--;
			}

	TextureList.clear();
	for (unsigned int tt = LibRendererDll::Model::TextureDesc::TT_NONE; tt < LibRendererDll::Model::TextureDesc::TT_UNKNOWN; tt++)
		TextureLUT[tt].clear();
}

Texture::Texture(const char* filePath)
	: RenderResource(filePath, RES_TEXTURE)
	, pTexture(nullptr)
	, nTexIdx(~0u)
{}

const bool Texture::Init()
{
	if (!ResMgr || bInitialized)
		return false;

	if (RenderResource::Init())
	{
		nTexIdx = ResMgr->CreateTexture(szDesc.c_str());
		if (nTexIdx != ~0u)
			pTexture = ResMgr->GetTexture(nTexIdx);

		return true;
	}
	else
		return false;
}

void Texture::Free()
{
	RenderResource::Free();

	if (ResMgr)
		ResMgr->ReleaseTexture(nTexIdx);

	nTexIdx = ~0u;
	pTexture = nullptr;
}

RenderTarget::RenderTarget(const char* name, const unsigned int targetCount,
	LibRendererDll::PixelFormat pixelFormatRT0, LibRendererDll::PixelFormat pixelFormatRT1,
	LibRendererDll::PixelFormat pixelFormatRT2, LibRendererDll::PixelFormat pixelFormatRT3,
	const float widthRatio, const float heightRatio,
	LibRendererDll::PixelFormat depthStencilFormat)
	: RenderResource(name, RES_RENDERTARGET)
	, pRenderTarget(nullptr)
	, nRenderTargetIdx(~0u)
	, nTargetCount(targetCount)
	, ePixelFormatRT0(pixelFormatRT0)
	, ePixelFormatRT1(pixelFormatRT1)
	, ePixelFormatRT2(pixelFormatRT2)
	, ePixelFormatRT3(pixelFormatRT3)
	, nWidth(0)
	, nHeight(0)
	, widthRatio(widthRatio)
	, heightRatio(heightRatio)
	, eDepthStencilFormat(depthStencilFormat)
	, bIsDynamic(true)
{}

RenderTarget::RenderTarget(const char* name, const unsigned int targetCount,
	LibRendererDll::PixelFormat pixelFormatRT0, LibRendererDll::PixelFormat pixelFormatRT1,
	LibRendererDll::PixelFormat pixelFormatRT2, LibRendererDll::PixelFormat pixelFormatRT3,
	const unsigned int width, const unsigned int height,
	LibRendererDll::PixelFormat depthStencilFormat)
	: RenderResource(name, RES_RENDERTARGET)
	, pRenderTarget(nullptr)
	, nRenderTargetIdx(~0u)
	, nTargetCount(targetCount)
	, ePixelFormatRT0(pixelFormatRT0)
	, ePixelFormatRT1(pixelFormatRT1)
	, ePixelFormatRT2(pixelFormatRT2)
	, ePixelFormatRT3(pixelFormatRT3)
	, nWidth(width)
	, nHeight(width)
	, widthRatio(0)
	, heightRatio(0)
	, eDepthStencilFormat(depthStencilFormat)
	, bIsDynamic(false)
{}

const bool RenderTarget::Init()
{
	if (!ResMgr || bInitialized)
		return false;

	if (RenderResource::Init())
	{
		if (bIsDynamic)
		{
			nRenderTargetIdx = ResMgr->CreateRenderTarget(nTargetCount,
				ePixelFormatRT0, ePixelFormatRT1, ePixelFormatRT2, ePixelFormatRT3,
				widthRatio, heightRatio, false, eDepthStencilFormat != LibRendererDll::PF_NONE, eDepthStencilFormat);
			pRenderTarget = ResMgr->GetRenderTarget(nRenderTargetIdx);
		}
		else
		{
			nRenderTargetIdx = ResMgr->CreateRenderTarget(nTargetCount,
				ePixelFormatRT0, ePixelFormatRT1, ePixelFormatRT2, ePixelFormatRT3,
				nWidth, nHeight, false, eDepthStencilFormat != LibRendererDll::PF_NONE, eDepthStencilFormat);
			pRenderTarget = ResMgr->GetRenderTarget(nRenderTargetIdx);
		}

		return true;
	}
	else
		return false;
}

void RenderTarget::Free()
{
	RenderResource::Free();

	if (ResMgr)
		ResMgr->ReleaseRenderTarget(nRenderTargetIdx);

	pRenderTarget = nullptr;
	nRenderTargetIdx = ~0u;
}

void RenderTarget::Enable()
{
	PUSH_PROFILE_MARKER(("Render Target: " + szDesc).c_str());
	pRenderTarget->Enable();
}

void RenderTarget::Disable()
{
	pRenderTarget->Disable();
	POP_PROFILE_MARKER();
}
