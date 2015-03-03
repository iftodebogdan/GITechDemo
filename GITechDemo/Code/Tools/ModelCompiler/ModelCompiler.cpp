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
#include "stdafx.h"

#include <Renderer.h>
using namespace LibRendererDll;

class mstream
{
public:
	bool m_bVerbose;
	ofstream coss;
	mstream(const char* fileName, unsigned int mode, bool verbose = true)
	{ m_bVerbose = verbose; coss.open(fileName, mode); }
	~mstream(void)
	{ if(coss.is_open()) coss.close(); }
};

template <class T>
mstream& operator<< (mstream& st, T val)
{
	st.coss << val;
#ifdef _DEBUG
	st.coss.flush();
#endif
	if(st.m_bVerbose)
		cout << val;
	return st;
};

namespace LibRendererTools
{
	class ModelCompiler
	{
		const char* GetEnumString(Model::Material::ShadingModel val);
		const char* GetEnumString(Model::Material::BlendMode val);
		const char* GetEnumString(Model::TextureDesc::TextureType val);
		const char* GetEnumString(Model::TextureDesc::TextureOp val);
		const char* GetEnumString(Model::TextureDesc::TextureMapping val);
		const char* GetEnumString(Model::TextureDesc::TextureMappingMode val);

	public:
		void Run(int argc, char* argv[]);
	};

	void ModelCompiler::Run(int argc, char* argv[])
	{
		bool bValidCmdParams = false;
		bool bQuiet = false;

		for (unsigned int arg = 1; arg < (unsigned int)argc; arg++)
		{
			if (arg != argc - 1)
			{
				if (strcmp(argv[arg], "-q") == 0)
					bQuiet = true;
				else
					break;
			}
			else
			{
				if (argv[arg][0] == '-')
					break;
				else
					bValidCmdParams = true;
			}
		}

		if (!bValidCmdParams)
		{
			cout << "Usage: ModelCompiler [options] Path\\To\\model_file.ext" << endl << endl;
			cout << "Options:" << endl;
			cout << "-q\tQuiet. Does not produce output to the console window (only log file)" << endl;
			return;
		}

		char dirPath[1024];
		char fileName[256];
		char time[80];
		char logName[1024];

		_splitpath_s(argv[argc - 1], (char*)nullptr, 0, dirPath, 1024, fileName, 256, (char*)nullptr, 0);

		std::time_t rawtime;
		std::tm* timeinfo = new std::tm;
		std::time(&rawtime);
		localtime_s(timeinfo, &rawtime);
		std::strftime(time, 80, "%Y%m%d%H%M%S", timeinfo);
		delete timeinfo;

		if (!(CreateDirectoryA("Logs", NULL) || ERROR_ALREADY_EXISTS == GetLastError()))
		{
			cout << "ModelCompiler requires write permission into the current directory";
			return;
		}

		sprintf_s(logName, 1024, "Logs\\ModelCompiler_%s_%s.log", fileName, time);
		mstream Log(logName, ofstream::trunc, !bQuiet);

		Log << "Compiling: \"" << argv[argc - 1] << "\"\n";

		unsigned long long startTick = GetTickCount64();

		// Create an instance of the Importer class
		Assimp::Importer importer;
		const aiScene* scene = nullptr;

		// And have it read the given file with some example postprocessing
		// Usually - if speed is not the most important aspect for you - you'll
		// propably to request more postprocessing than we do in this example.
		unsigned int ppFlags = aiProcessPreset_TargetRealtime_MaxQuality & ~aiProcess_FindDegenerates; // & aiProcess_OptimizeGraph & aiProcess_PreTransformVertices & aiProcess_Debone
																									   //if (Renderer::GetAPI() == API_DX9)
																									   //	ppFlags |= aiProcess_ConvertToLeftHanded;
		scene = importer.ReadFile(argv[argc - 1], ppFlags);

		if (!scene)
		{
			Log << "[ERROR] " << importer.GetErrorString();
			return;
		}

		Renderer::CreateInstance(API_NULL);
		Renderer* renderer = Renderer::GetInstance();
		renderer->Initialize(nullptr);
		ResourceManager* const resMan = Renderer::GetInstance()->GetResourceManager();

		if (!renderer || !resMan)
		{
			Log << "[ERROR] Could not initialize renderer or resource manager!";
			return;
		}

		Model model;

		Log << "\nMesh count: " << scene->mNumMeshes << "\n";

		for (unsigned int meshIdx = 0; meshIdx < scene->mNumMeshes; meshIdx++)
		{
			Log << "[MESH]" << "\n";

			model.arrMesh.push_back(new Model::Mesh);

			model.arrMesh.back()->szName = scene->mMeshes[meshIdx]->mName.C_Str();
			Log << "\tName: " << model.arrMesh.back()->szName.c_str() << "\n";
			Log << "\tMesh index: " << meshIdx << "\n";

			std::vector<VertexAttributeUsage> arrVAU;
			unsigned int countUVComponents[AI_MAX_NUMBER_OF_TEXTURECOORDS] = { 0 };
			unsigned int maxUVChannels = 0;
			unsigned int maxColorChannels = 0;
			unsigned int totalIndexCount = scene->mMeshes[meshIdx]->mNumFaces * 3;
			unsigned int totalVertexCount = scene->mMeshes[meshIdx]->mNumVertices;

			Log << "\tVertex count: " << totalVertexCount << "\n";
			Log << "\tIndex count: " << totalIndexCount << "\n";

			Log << "\t[VERTEX FORMAT]" << "\n";

			if (scene->mMeshes[meshIdx]->HasPositions())
				if (std::find(arrVAU.begin(), arrVAU.end(), VAU_POSITION) == arrVAU.end())
				{
					arrVAU.push_back(VAU_POSITION);
					Log << "\t\tVAU_POSITION" << "\n";
				}

			if (scene->mMeshes[meshIdx]->HasNormals())
				if (std::find(arrVAU.begin(), arrVAU.end(), VAU_NORMAL) == arrVAU.end())
				{
					arrVAU.push_back(VAU_NORMAL);
					Log << "\t\tVAU_NORMAL" << "\n";
				}

			bool hasTexCoords = false;
			for (unsigned int tcIdx = 0; tcIdx < AI_MAX_NUMBER_OF_TEXTURECOORDS; tcIdx++)
				if (scene->mMeshes[meshIdx]->HasTextureCoords(tcIdx))
				{
					hasTexCoords = true;
					maxUVChannels = tcIdx + 1;
					countUVComponents[tcIdx] = scene->mMeshes[meshIdx]->mNumUVComponents[tcIdx];
					Log << "\t\tVAU_TEXCOORD" << "\n";
					Log << "\t\t\tChannel: " << tcIdx << "\n";
					Log << "\t\t\tFormat: VAT_FLOAT" << countUVComponents[tcIdx] << "\n";
				}
			if (hasTexCoords)
			{
				if (std::find(arrVAU.begin(), arrVAU.end(), VAU_TEXCOORD) == arrVAU.end())
					arrVAU.push_back(VAU_TEXCOORD);
			}

			bool hasVertexColors = false;
			for (unsigned int colorIdx = 0; colorIdx < AI_MAX_NUMBER_OF_COLOR_SETS; colorIdx++)
				if (scene->mMeshes[meshIdx]->HasVertexColors(colorIdx))
				{
					hasVertexColors = true;
					maxColorChannels = colorIdx + 1;
					Log << "\t\tVAU_COLOR" << "\n";
					Log << "\t\t\tChannel: " << colorIdx << "\n";
				}
			if (hasVertexColors)
				if (std::find(arrVAU.begin(), arrVAU.end(), VAU_COLOR) == arrVAU.end())
					arrVAU.push_back(VAU_COLOR);

			if (scene->mMeshes[meshIdx]->HasTangentsAndBitangents())
				if (std::find(arrVAU.begin(), arrVAU.end(), VAU_TANGENT) == arrVAU.end())
				{
					arrVAU.push_back(VAU_TANGENT);
					arrVAU.push_back(VAU_BINORMAL);
					Log << "\t\tVAU_TANGENT" << "\n";
					Log << "\t\tVAU_BINORMAL" << "\n";
				}

			Log << "\t[/VERTEX FORMAT]" << "\n";

			// Create the vertex format
			const unsigned int vfIdx = resMan->CreateVertexFormat((unsigned int)arrVAU.size()
				+ (maxUVChannels ? maxUVChannels - 1 : 0)
				+ (maxColorChannels ? maxColorChannels - 1 : 0));
			model.arrMesh.back()->pVertexFormat = resMan->GetVertexFormat(vfIdx);
			for (unsigned int vauIdx = 0, attribIdx = 0, offset = 0; vauIdx < arrVAU.size(); vauIdx++)
			{
				VertexAttributeType type = VAT_NONE;
				switch (arrVAU[vauIdx])
				{
				case VAU_POSITION:
				case VAU_NORMAL:
				case VAU_TANGENT:
				case VAU_BINORMAL:
					type = VAT_FLOAT3;
					break;
				case VAU_TEXCOORD:
					break;
				case VAU_COLOR:
					type = VAT_UBYTE4;
					break;
				default:
					assert(false);
				}

				if (arrVAU[vauIdx] == VAU_TEXCOORD)
					for (unsigned int uvCh = 0; uvCh < maxUVChannels; uvCh++)
					{
						switch (countUVComponents[uvCh])
						{
						case 1:
							type = VAT_FLOAT1;
							break;
						case 2:
							type = VAT_FLOAT2;
							break;
						case 3:
							type = VAT_FLOAT3;
							break;
						default:
							assert(false);
						}
						model.arrMesh.back()->pVertexFormat->SetAttribute(attribIdx++, offset, arrVAU[vauIdx], type, uvCh);
						offset += VertexFormat::GetAttributeTypeSize(type);
					}
				else
					if (arrVAU[vauIdx] == VAU_COLOR)
						for (unsigned int clrCh = 0; clrCh < maxColorChannels; clrCh++)
						{
							model.arrMesh.back()->pVertexFormat->SetAttribute(attribIdx++, offset, arrVAU[vauIdx], type, clrCh);
							offset += VertexFormat::GetAttributeTypeSize(type);
						}
					else
					{
						model.arrMesh.back()->pVertexFormat->SetAttribute(attribIdx++, offset, arrVAU[vauIdx], type, 0);
						offset += VertexFormat::GetAttributeTypeSize(type);
					}
			}
			model.arrMesh.back()->pVertexFormat->SetStride(model.arrMesh.back()->pVertexFormat->CalculateStride());
			model.arrMesh.back()->pVertexFormat->Update();

			const unsigned int ibIdx = resMan->CreateIndexBuffer(totalIndexCount, totalIndexCount > 65535 ? IBF_INDEX32 : IBF_INDEX16);
			model.arrMesh.back()->pIndexBuffer = resMan->GetIndexBuffer(ibIdx);
			const unsigned int vbIdx = resMan->CreateVertexBuffer(model.arrMesh.back()->pVertexFormat, totalVertexCount, model.arrMesh.back()->pIndexBuffer);
			model.arrMesh.back()->pVertexBuffer = resMan->GetVertexBuffer(vbIdx);

			model.arrMesh.back()->pIndexBuffer->Lock(BL_WRITE_ONLY);
			model.arrMesh.back()->pVertexBuffer->Lock(BL_WRITE_ONLY);

			unsigned int iterIndices = 0, iterVertices = 0;

			// Populate IB
			for (unsigned int faceIdx = 0; faceIdx < scene->mMeshes[meshIdx]->mNumFaces; faceIdx++)
			{
				for (unsigned int vertIdx = 0; vertIdx < 3; vertIdx++)
				{
					assert(scene->mMeshes[meshIdx]->mFaces[faceIdx].mNumIndices == 3);
					model.arrMesh.back()->pIndexBuffer->SetIndex(iterIndices++, scene->mMeshes[meshIdx]->mFaces[faceIdx].mIndices[vertIdx]);
				}
			}

			// Populate VB
			for (unsigned int vertIdx = 0; vertIdx < scene->mMeshes[meshIdx]->mNumVertices; vertIdx++)
			{
				if (model.arrMesh.back()->pVertexBuffer->HasPosition())
					model.arrMesh.back()->pVertexBuffer->Position<Vec3f>(iterVertices) = Vec3f(
					scene->mMeshes[meshIdx]->mVertices[vertIdx].x,
					scene->mMeshes[meshIdx]->mVertices[vertIdx].y,
					scene->mMeshes[meshIdx]->mVertices[vertIdx].z);

				if (model.arrMesh.back()->pVertexBuffer->HasNormal())
					model.arrMesh.back()->pVertexBuffer->Normal<Vec3f>(iterVertices) = Vec3f(
					scene->mMeshes[meshIdx]->mNormals[vertIdx].x,
					scene->mMeshes[meshIdx]->mNormals[vertIdx].y,
					scene->mMeshes[meshIdx]->mNormals[vertIdx].z);

				if (model.arrMesh.back()->pVertexBuffer->HasTangent())
					model.arrMesh.back()->pVertexBuffer->Tangent<Vec3f>(iterVertices) = Vec3f(
					scene->mMeshes[meshIdx]->mTangents[vertIdx].x,
					scene->mMeshes[meshIdx]->mTangents[vertIdx].y,
					scene->mMeshes[meshIdx]->mTangents[vertIdx].z);

				if (model.arrMesh.back()->pVertexBuffer->HasBinormal())
					model.arrMesh.back()->pVertexBuffer->Binormal<Vec3f>(iterVertices) = Vec3f(
					scene->mMeshes[meshIdx]->mBitangents[vertIdx].x,
					scene->mMeshes[meshIdx]->mBitangents[vertIdx].y,
					scene->mMeshes[meshIdx]->mBitangents[vertIdx].z);

				for (unsigned int tcIdx = 0; tcIdx < AI_MAX_NUMBER_OF_TEXTURECOORDS; tcIdx++)
					if (model.arrMesh.back()->pVertexBuffer->HasTexCoord(tcIdx) && scene->mMeshes[meshIdx]->HasTextureCoords(tcIdx))
						switch (scene->mMeshes[meshIdx]->mNumUVComponents[tcIdx])
						{
						case 1:
							model.arrMesh.back()->pVertexBuffer->TexCoord<float>(iterVertices, tcIdx) = scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].x;
							break;
						case 2:
							model.arrMesh.back()->pVertexBuffer->TexCoord<Vec2f>(iterVertices, tcIdx) = Vec2f(
								scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].x,
								scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].y);
							break;
						case 3:
							model.arrMesh.back()->pVertexBuffer->TexCoord<Vec3f>(iterVertices, tcIdx) = Vec3f(
								scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].x,
								scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].y,
								scene->mMeshes[meshIdx]->mTextureCoords[tcIdx][vertIdx].z);
							break;
						default:
							assert(false);
						}

				for (unsigned int colorIdx = 0; colorIdx < AI_MAX_NUMBER_OF_COLOR_SETS; colorIdx++)
					if (model.arrMesh.back()->pVertexBuffer->HasColor(colorIdx) && scene->mMeshes[meshIdx]->HasVertexColors(colorIdx))
						model.arrMesh.back()->pVertexBuffer->Color<DWORD>(iterVertices, colorIdx) =
						((((DWORD)(scene->mMeshes[meshIdx]->mColors[colorIdx][vertIdx].a * 255.f)) & 0xff) << 24) |
						((((DWORD)(scene->mMeshes[meshIdx]->mColors[colorIdx][vertIdx].r * 255.f)) & 0xff) << 16) |
						((((DWORD)(scene->mMeshes[meshIdx]->mColors[colorIdx][vertIdx].g * 255.f)) & 0xff) << 8) |
						((((DWORD)(scene->mMeshes[meshIdx]->mColors[colorIdx][vertIdx].b * 255.f)) & 0xff));

				iterVertices++;
			}
			assert(iterIndices == totalIndexCount && iterVertices == totalVertexCount);

			model.arrMesh.back()->pIndexBuffer->Update();
			model.arrMesh.back()->pIndexBuffer->Unlock();
			model.arrMesh.back()->pVertexBuffer->Update();
			model.arrMesh.back()->pVertexBuffer->Unlock();

			if (scene->HasMaterials())
				model.arrMesh.back()->nMaterialIdx = scene->mMeshes[meshIdx]->mMaterialIndex;

			Log << "\tMaterial index: " << model.arrMesh.back()->nMaterialIdx << "\n";

			Log << "[/MESH]" << "\n";
		}

		Log << "\nMaterial count: " << scene->mNumMaterials << "\n";

		for (unsigned int matIdx = 0; matIdx < scene->mNumMaterials; matIdx++)
		{
			Log << "[MATERIAL]" << "\n";

			model.arrMaterial.push_back(new Model::Material);

			aiMaterial* mat = scene->mMaterials[matIdx];

			aiString szMatName("");
			mat->Get(AI_MATKEY_NAME, szMatName);
			model.arrMaterial.back()->szName = szMatName.C_Str();
			Log << "\tName: " << szMatName.C_Str() << "\n";
			Log << "\tMaterial index: " << matIdx << "\n";

			int nTwoSided = 0;
			if (mat->Get(AI_MATKEY_TWOSIDED, nTwoSided) == AI_SUCCESS)
				Log << "\tTwo sided: " << nTwoSided << "\n";
			model.arrMaterial.back()->bTwoSided = nTwoSided != 0;

			aiShadingMode eShadingModel = aiShadingMode_Gouraud;
			if (mat->Get(AI_MATKEY_SHADING_MODEL, eShadingModel) == AI_SUCCESS)
				Log << "\tShading model: " << GetEnumString((Model::Material::ShadingModel)eShadingModel) << "\n";
			model.arrMaterial.back()->eShadingModel = (Model::Material::ShadingModel)eShadingModel;

			int bEnableWireframe = 0;
			if (mat->Get(AI_MATKEY_ENABLE_WIREFRAME, bEnableWireframe) == AI_SUCCESS)
				Log << "\tEnable wireframe: " << bEnableWireframe << "\n";
			model.arrMaterial.back()->bEnableWireframe = bEnableWireframe != 0;

			aiBlendMode eBlendMode = aiBlendMode_Default;
			if (mat->Get(AI_MATKEY_BLEND_FUNC, eBlendMode) == AI_SUCCESS)
				Log << "\tBlend mode: " << GetEnumString((Model::Material::BlendMode)eBlendMode) << "\n";
			model.arrMaterial.back()->eBlendMode = (Model::Material::BlendMode)eBlendMode;

			float fOpacity = 1.f;
			if (mat->Get(AI_MATKEY_OPACITY, fOpacity) == AI_SUCCESS)
				Log << "\tOpacity: " << fOpacity << "\n";
			model.arrMaterial.back()->fOpacity = fOpacity;

			float fBumpScaling = 1.f;
			if (mat->Get(AI_MATKEY_BUMPSCALING, fBumpScaling) == AI_SUCCESS)
				Log << "\tBump scaling: " << fBumpScaling << "\n";
			model.arrMaterial.back()->fBumpScaling = fBumpScaling;

			float fShininess = 0.f;
			if (mat->Get(AI_MATKEY_SHININESS, fShininess) == AI_SUCCESS)
				Log << "\tShininess: " << fShininess << "\n";
			model.arrMaterial.back()->fShininess = fShininess;

			float fReflectivity = 1.f;
			if (mat->Get(AI_MATKEY_REFLECTIVITY, fReflectivity) == AI_SUCCESS)
				Log << "\tRefectivity: " << fReflectivity << "\n";
			model.arrMaterial.back()->fReflectivity = fReflectivity;

			float fShininessStrength = 1.f;
			if (mat->Get(AI_MATKEY_SHININESS_STRENGTH, fShininessStrength) == AI_SUCCESS)
				Log << "\tShininess strength: " << fShininessStrength << "\n";
			model.arrMaterial.back()->fShininessStrength = fShininessStrength;

			float fRefractIndex = 1.f;
			if (mat->Get(AI_MATKEY_REFRACTI, fRefractIndex) == AI_SUCCESS)
				Log << "\tRefraction index: " << fRefractIndex << "\n";
			model.arrMaterial.back()->fRefractIndex = fRefractIndex;

			aiColor3D vColorDiffuse(0.f, 0.f, 0.f);
			if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, vColorDiffuse) == AI_SUCCESS)
				Log << "\tColor diffuse: " << "RGB(" << vColorDiffuse.r << ", " << vColorDiffuse.g << ", " << vColorDiffuse.b << ")" << "\n";
			model.arrMaterial.back()->vColorDiffuse = Vec3f(vColorDiffuse.r, vColorDiffuse.g, vColorDiffuse.b);

			aiColor3D vColorAmbient(0.f, 0.f, 0.f);
			if (mat->Get(AI_MATKEY_COLOR_AMBIENT, vColorAmbient) == AI_SUCCESS)
				Log << "\tColor ambient: " << "RGB(" << vColorAmbient.r << ", " << vColorAmbient.g << ", " << vColorAmbient.b << ")" << "\n";
			model.arrMaterial.back()->vColorAmbient = Vec3f(vColorAmbient.r, vColorAmbient.g, vColorAmbient.b);

			aiColor3D vColorSpecular(0.f, 0.f, 0.f);
			if (mat->Get(AI_MATKEY_COLOR_SPECULAR, vColorSpecular) == AI_SUCCESS)
				Log << "\tColor specular: " << "RGB(" << vColorSpecular.r << ", " << vColorSpecular.g << ", " << vColorSpecular.b << ")" << "\n";
			model.arrMaterial.back()->vColorSpecular = Vec3f(vColorSpecular.r, vColorSpecular.g, vColorSpecular.b);

			aiColor3D vColorEmissive(0.f, 0.f, 0.f);
			if (mat->Get(AI_MATKEY_COLOR_EMISSIVE, vColorEmissive) == AI_SUCCESS)
				Log << "\tColor emissive: " << "RGB(" << vColorEmissive.r << ", " << vColorEmissive.g << ", " << vColorEmissive.b << ")" << "\n";
			model.arrMaterial.back()->vColorEmissive = Vec3f(vColorEmissive.r, vColorEmissive.g, vColorEmissive.b);

			aiColor3D vColorTransparent(0.f, 0.f, 0.f);
			if (mat->Get(AI_MATKEY_COLOR_TRANSPARENT, vColorTransparent) == AI_SUCCESS)
				Log << "\tColor transparent: " << "RGB(" << vColorTransparent.r << ", " << vColorTransparent.g << ", " << vColorTransparent.b << ")" << "\n";
			model.arrMaterial.back()->vColorTransparent = Vec3f(vColorTransparent.r, vColorTransparent.g, vColorTransparent.b);

			aiColor3D vColorReflective(0.f, 0.f, 0.f);
			if (mat->Get(AI_MATKEY_COLOR_REFLECTIVE, vColorReflective) == AI_SUCCESS)
				Log << "\tColor reflective: " << "RGB(" << vColorReflective.r << ", " << vColorReflective.g << ", " << vColorReflective.b << ")" << "\n";
			model.arrMaterial.back()->vColorReflective = Vec3f(vColorReflective.r, vColorReflective.g, vColorReflective.b);

			for (unsigned int type = aiTextureType_NONE; type <= aiTextureType_UNKNOWN; type++)
			{
				aiString szTexturePath("");
				unsigned int nTexIdx = 0;

				while (mat->Get(AI_MATKEY_TEXTURE(type, nTexIdx), szTexturePath) == AI_SUCCESS)
				{
					Log << "\t[TEXTURE]" << "\n";

					model.arrMaterial.back()->arrTexture.push_back(new Model::TextureDesc());

					model.arrMaterial.back()->arrTexture.back()->szFilePath = szTexturePath.C_Str();
					Log << "\t\tFile: " << szTexturePath.C_Str() << "\n";

					model.arrMaterial.back()->arrTexture.back()->eTexType = (Model::TextureDesc::TextureType)type;
					model.arrMaterial.back()->arrTexture.back()->nTexIndex = nTexIdx;
					Log << "\t\tType: " << GetEnumString((Model::TextureDesc::TextureType)type) << "\n";

					int nUVChannel = -1;
					if (mat->Get(AI_MATKEY_UVWSRC(type, nTexIdx), nUVChannel) == AI_SUCCESS)
						Log << "\t\tUV channel: " << nUVChannel << "\n";
					model.arrMaterial.back()->arrTexture.back()->nUVChannel = nUVChannel;

					aiTextureOp eTexOp = aiTextureOp_Add;
					if (mat->Get(AI_MATKEY_TEXOP(type, nTexIdx), eTexOp) == AI_SUCCESS)
					{
						Log << "\t\tOp index: " << nTexIdx << "\n";
						Log << "\t\tOp: " << GetEnumString((Model::TextureDesc::TextureOp)eTexOp) << "\n";
					}
					model.arrMaterial.back()->arrTexture.back()->eTexOp = (Model::TextureDesc::TextureOp)eTexOp;

					float fTexBlend = 1.f;
					if (mat->Get(AI_MATKEY_TEXBLEND(type, nTexIdx), fTexBlend) == AI_SUCCESS)
						Log << "\t\tBlend: " << fTexBlend << "\n";
					model.arrMaterial.back()->arrTexture.back()->fTexBlend = fTexBlend;

					aiTextureMapping eTexMapping = aiTextureMapping_OTHER;
					if (mat->Get(AI_MATKEY_MAPPING(type, nTexIdx), eTexMapping) == AI_SUCCESS)
						Log << "\t\tMapping: " << GetEnumString((Model::TextureDesc::TextureMapping)eTexMapping) << "\n";
					model.arrMaterial.back()->arrTexture.back()->eTexMapping = (Model::TextureDesc::TextureMapping)eTexMapping;

					aiTextureMapMode eTexMapModeU = aiTextureMapMode_Wrap;
					aiTextureMapMode eTexMapModeV = aiTextureMapMode_Wrap;
					if (mat->Get(AI_MATKEY_MAPPINGMODE_U(type, nTexIdx), eTexMapModeU) == AI_SUCCESS)
						Log << "\t\tMapping mode U: " << GetEnumString((Model::TextureDesc::TextureMappingMode)eTexMapModeU) << "\n";
					if (mat->Get(AI_MATKEY_MAPPINGMODE_V(type, nTexIdx), eTexMapModeV) == AI_SUCCESS)
						Log << "\t\tMapping mode V: " << GetEnumString((Model::TextureDesc::TextureMappingMode)eTexMapModeV) << "\n";
					model.arrMaterial.back()->arrTexture.back()->eTexMapModeU = (Model::TextureDesc::TextureMappingMode)eTexMapModeU;
					model.arrMaterial.back()->arrTexture.back()->eTexMapModeV = (Model::TextureDesc::TextureMappingMode)eTexMapModeV;

					nTexIdx++;

					Log << "\t[/TEXTURE]" << "\n";
				}
			}

			Log << "[/MATERIAL]" << "\n";
		}

		Log << "\nCompilation of \"" << argv[argc - 1] << "\" finished in " << (float)(GetTickCount64() - startTick) / 1000.f << " seconds" << "\n";

		CreateDirectoryA("Out", NULL);
		std::string outFilePath = "Out\\Models\\";
		CreateDirectoryA(outFilePath.c_str(), NULL);
		outFilePath += fileName;
		CreateDirectoryA(outFilePath.c_str(), NULL);
		outFilePath += "\\";
		outFilePath += fileName;
		outFilePath += ".lrm";
		
		ofstream outModel;
		outModel.open(outFilePath.c_str(), ofstream::trunc | ofstream::binary);
#ifdef _DEBUG
		outModel.setf(ios_base::unitbuf);
#endif
		outModel << model;
		outModel.close();

#ifdef _DEBUG
		unsigned int modelIdx = resMan->CreateModel(outFilePath.c_str());
		Model* modelIn = resMan->GetModel(modelIdx);

		assert(model.szName == modelIn->szName);
		assert(model.arrMaterial.size() == modelIn->arrMaterial.size());
		assert(model.arrMesh.size() == modelIn->arrMesh.size());
#endif

		Renderer::DestroyInstance();
	}

	const char* ModelCompiler::GetEnumString(Model::Material::ShadingModel val)
	{
		switch (val)
		{
		case Model::Material::SM_FLAT:
			return "SM_FLAT";
		case Model::Material::SM_GOURAUD:
			return "SM_GOURAUD";
		case Model::Material::SM_PHONG:
			return "SM_PHONG";
		case Model::Material::SM_BLINN:
			return "SM_BLINN";
		case Model::Material::SM_TOON:
			return "SM_TOON";
		case Model::Material::SM_ORENNAYAR:
			return "SM_ORENNAYAR";
		case Model::Material::SM_MINNAERT:
			return "SM_MINNAERT";
		case Model::Material::SM_COOKTORRANCE:
			return "SM_COOKTORRANCE";
		case Model::Material::SM_NOSHADING:
			return "SM_NOSHADING";
		case Model::Material::SM_FRESNEL:
			return "SM_FRESNEL";
		default:
			return "";
		}
	}

	const char* ModelCompiler::GetEnumString(Model::Material::BlendMode val)
	{
		switch (val)
		{
		case Model::Material::BM_DEFAULT:
			return "BM_DEFAULT";
		case Model::Material::BM_ADDITIVE:
			return "BM_ADDITIVE";
		default:
			return "";
		}
	}

	const char* ModelCompiler::GetEnumString(Model::TextureDesc::TextureType val)
	{
		switch (val)
		{
		case Model::TextureDesc::TT_NONE:
			return "TT_NONE";
		case Model::TextureDesc::TT_DIFFUSE:
			return "TT_DIFFUSE";
		case Model::TextureDesc::TT_SPECULAR:
			return "TT_SPECULAR";
		case Model::TextureDesc::TT_AMBIENT:
			return "TT_AMBIENT";
		case Model::TextureDesc::TT_EMISSIVE:
			return "TT_EMISSIVE";
		case Model::TextureDesc::TT_HEIGHT:
			return "TT_HEIGHT";
		case Model::TextureDesc::TT_NORMALS:
			return "TT_NORMALS";
		case Model::TextureDesc::TT_SHININESS:
			return "TT_SHININESS";
		case Model::TextureDesc::TT_OPACITY:
			return "TT_OPACITY";
		case Model::TextureDesc::TT_DISPLACEMENT:
			return "TT_DISPLACEMENT";
		case Model::TextureDesc::TT_LIGHTMAP:
			return "TT_LIGHTMAP";
		case Model::TextureDesc::TT_REFLECTION:
			return "TT_REFLECTION";
		case Model::TextureDesc::TT_UNKNOWN:
			return "TT_UNKNOWN";
		default:
			return "";
		}
	}

	const char* ModelCompiler::GetEnumString(Model::TextureDesc::TextureOp val)
	{
		switch (val)
		{
		case Model::TextureDesc::TO_MULTIPLY:
			return "TO_MULTIPLY";
		case Model::TextureDesc::TO_ADD:
			return "TO_ADD";
		case Model::TextureDesc::TO_SUBTRACT:
			return "TO_SUBTRACT";
		case Model::TextureDesc::TO_DIVIDE:
			return "TO_DIVIDE";
		case Model::TextureDesc::TO_SMOOTHADD:
			return "TO_SMOOTHADD";
		case Model::TextureDesc::TO_SIGNEDADD:
			return "TO_SIGNEDADD";
		default:
			return "";
		}
	}

	const char* ModelCompiler::GetEnumString(Model::TextureDesc::TextureMapping val)
	{
		switch (val)
		{
		case Model::TextureDesc::TM_UV:
			return "TM_UV";
		case Model::TextureDesc::TM_SPHERE:
			return "TM_SPHERE";
		case Model::TextureDesc::TM_CYLINDER:
			return "TM_CYLINDER";
		case Model::TextureDesc::TM_BOX:
			return "TM_BOX";
		case Model::TextureDesc::TM_PLANE:
			return "TM_PLANE";
		case Model::TextureDesc::TM_OTHER:
			return "TM_OTHER";
		default:
			return "";
		}
	}

	const char* ModelCompiler::GetEnumString(Model::TextureDesc::TextureMappingMode val)
	{
		switch (val)
		{
		case Model::TextureDesc::TMM_WRAP:
			return "TMM_WRAP";
		case Model::TextureDesc::TMM_CLAMP:
			return "TMM_CLAMP";
		case Model::TextureDesc::TMM_DECAL:
			return "TMM_DECAL";
		case Model::TextureDesc::TMM_MIRROR:
			return "TMM_MIRROR";
		default:
			return "";
		}
	}
}

int main(int argc, char* argv[])
{
	LibRendererTools::ModelCompiler mc;
	mc.Run(argc, argv);

	return 0;
}
