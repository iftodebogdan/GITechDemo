/**
 *	@file		ResourceSerialization.cpp
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

#include "stdafx.h"

#include "ResourceData.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "VertexFormat.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

namespace Synesthesia3D
{
	std::ostream& operator<<(std::ostream& output_out, const Model& model_in)
	{
		// model name size
		unsigned int modelNameSize = (unsigned int)model_in.szName.size();
		output_out.write((const char*)&modelNameSize, sizeof(unsigned int));

		// model name
		output_out.write(model_in.szName.c_str(), modelNameSize);

		// mesh count
		unsigned int meshCount = (unsigned int)model_in.arrMesh.size();
		output_out.write((const char*)&meshCount, sizeof(unsigned int));

		// meshes
		for (unsigned int mesh = 0; mesh < meshCount; mesh++)
			output_out << *model_in.arrMesh[mesh];

		// material count
		unsigned int matCount = (unsigned int)model_in.arrMaterial.size();
		output_out.write((const char*)&matCount, sizeof(unsigned int));

		// materials
		for (unsigned int mat = 0; mat < matCount; mat++)
			output_out << *model_in.arrMaterial[mat];

		return output_out;
	}

	std::istream& operator>>(std::istream& s_in, Model& model_out)
	{
		// model name size
		unsigned int modelNameSize = 0;
		s_in.read((char*)&modelNameSize, sizeof(unsigned int));

		// model name
		char* modelName = nullptr;
		modelName = new char[modelNameSize + 1];
		s_in.read(modelName, modelNameSize);
		modelName[modelNameSize] = '\0';
		model_out.szName = modelName;
		delete[] modelName;

		// mesh count
		unsigned int meshCount = 0;
		s_in.read((char*)&meshCount, sizeof(unsigned int));
		model_out.arrMesh.reserve(meshCount);

		// meshes
		for (unsigned int mesh = 0; mesh < meshCount; mesh++)
		{
			model_out.arrMesh.push_back(new Model::Mesh);
			s_in >> *model_out.arrMesh.back();
		}

		// material count
		unsigned int matCount = 0;
		s_in.read((char*)&matCount, sizeof(unsigned int));
		model_out.arrMaterial.reserve(matCount);

		// materials
		for (unsigned int mat = 0; mat < matCount; mat++)
		{
			model_out.arrMaterial.push_back(new Model::Material);
			s_in >> *model_out.arrMaterial.back();
		}

		return s_in;
	}

	std::ostream & operator<<(std::ostream & output_out, const VertexElement & ve_in)
	{
		output_out.write((const char*)&ve_in.nOffset, sizeof(unsigned int));
		output_out.write((const char*)&ve_in.eType, sizeof(VertexAttributeType));
		output_out.write((const char*)&ve_in.eSemantic, sizeof(VertexAttributeSemantic));
		output_out.write((const char*)&ve_in.nSemanticIdx, sizeof(unsigned int));

		return output_out;
	}

	std::istream & operator>>(std::istream & s_in, VertexElement & ve_out)
	{
		s_in.read((char*)&ve_out.nOffset, sizeof(unsigned int));
		s_in.read((char*)&ve_out.eType, sizeof(VertexAttributeType));
		s_in.read((char*)&ve_out.eSemantic, sizeof(VertexAttributeSemantic));
		s_in.read((char*)&ve_out.nSemanticIdx, sizeof(unsigned int));

		return s_in;
	}

	std::ostream &operator<<(std::ostream &output_out, VertexFormat &vf_in)
	{
		output_out.write((const char*)&vf_in.m_nAttributeCount, sizeof(unsigned int));
		for (unsigned int i = 0; i < vf_in.m_nAttributeCount; i++)
			output_out << vf_in.m_pElements[i];
		output_out.write((const char*)&vf_in.m_nStride, sizeof(unsigned int));

		return output_out;
	}

	std::istream &operator>>(std::istream &s_in, VertexFormat &vf_out)
	{
		s_in.read((char*)&vf_out.m_nAttributeCount, sizeof(unsigned int));
		delete[] vf_out.m_pElements;
		vf_out.m_pElements = new VertexElement[vf_out.m_nAttributeCount];
		for (unsigned int i = 0; i < vf_out.m_nAttributeCount; i++)
			s_in >> vf_out.m_pElements[i];
		s_in.read((char*)&vf_out.m_nStride, sizeof(unsigned int));

		return s_in;
	}

	std::ostream& operator<<(std::ostream& output_out, VertexBuffer& vb_in)
	{
		output_out << *(Buffer*)&vb_in;

		return output_out;
	}

	std::istream& operator>>(std::istream& s_in, VertexBuffer& vb_out)
	{
		s_in >> *(Buffer*)&vb_out;

		return s_in;
	}

	std::ostream& operator<<(std::ostream& output_out, IndexBuffer& ib_in)
	{
		output_out << *(Buffer*)&ib_in;
		output_out.write((const char*)&ib_in.m_eIndexFormat, sizeof(IndexBufferFormat));

		return output_out;
	}

	std::istream& operator>>(std::istream& s_in, IndexBuffer& ib_out)
	{
		s_in >> *(Buffer*)&ib_out;
		s_in.read((char*)&ib_out.m_eIndexFormat, sizeof(IndexBufferFormat));

		return s_in;
	}

	std::ostream& operator<<(std::ostream& output_out, Buffer& buf_in)
	{
		output_out.write((const char*)&buf_in.m_nElementCount, sizeof(unsigned int));
		output_out.write((const char*)&buf_in.m_nElementSize, sizeof(unsigned int));
		output_out.write((const char*)&buf_in.m_eBufferUsage, sizeof(BufferUsage));
		output_out.write((const char*)&buf_in.m_nSize, sizeof(unsigned int));
		output_out.write((const char*)buf_in.m_pData, buf_in.m_nSize);

		return output_out;
	}

	std::istream& operator>>(std::istream& s_in, Buffer& buf_out)
	{
		s_in.read((char*)&buf_out.m_nElementCount, sizeof(unsigned int));
		s_in.read((char*)&buf_out.m_nElementSize, sizeof(unsigned int));
		s_in.read((char*)&buf_out.m_eBufferUsage, sizeof(BufferUsage));
		s_in.read((char*)&buf_out.m_nSize, sizeof(unsigned int));
		delete[] buf_out.m_pData;
		buf_out.m_pData = new byte[buf_out.m_nSize];
		s_in.read((char*)buf_out.m_pData, buf_out.m_nSize);

		return s_in;
	}

	std::ostream& operator<<(std::ostream& output_out, const Model::Mesh& mesh_in)
	{
		// mesh name size
		unsigned int meshNameSize = (unsigned int)mesh_in.szName.size();
		output_out.write((const char*)&meshNameSize, sizeof(unsigned int));

		// mesh name
		output_out.write(mesh_in.szName.c_str(), meshNameSize);

		// vertex format data
		output_out << *(mesh_in.pVertexFormat);

		// index buffer data
		output_out << *(mesh_in.pIndexBuffer);

		// vertex buffer data
		output_out << *(mesh_in.pVertexBuffer);

		// material index
		output_out.write((const char*)&mesh_in.nMaterialIdx, sizeof(unsigned int));

		return output_out;
	}

	std::istream& operator>>(std::istream& s_in, Model::Mesh& mesh_out)
	{
		ResourceManager* resMan = Renderer::GetInstance()->GetResourceManager();
		if (!resMan)
			return s_in;

		// mesh name size
		unsigned int meshNameSize = 0;
		s_in.read((char*)&meshNameSize, sizeof(unsigned int));

		// mesh name
		char* meshName = nullptr;
		meshName = new char[meshNameSize + 1];
		s_in.read(meshName, meshNameSize);
		meshName[meshNameSize] = '\0';
		mesh_out.szName = meshName;
		delete[] meshName;

		// vertex format data
		mesh_out.nVfIdx = resMan->CreateVertexFormat(0);
		mesh_out.pVertexFormat = resMan->GetVertexFormat(mesh_out.nVfIdx);
		s_in >> *(mesh_out.pVertexFormat);
		mesh_out.pVertexFormat->Bind();

		// index buffer data
		mesh_out.nIbIdx = resMan->CreateIndexBuffer(0);
		mesh_out.pIndexBuffer = resMan->GetIndexBuffer(mesh_out.nIbIdx);
		s_in >> *(mesh_out.pIndexBuffer);
		mesh_out.pIndexBuffer->Bind();

		// vertex buffer data
		mesh_out.nVbIdx = resMan->CreateVertexBuffer(
			mesh_out.pVertexFormat,
			0,
			mesh_out.pIndexBuffer);
		mesh_out.pVertexBuffer = resMan->GetVertexBuffer(mesh_out.nVbIdx);
		s_in >> *(mesh_out.pVertexBuffer);
		mesh_out.pVertexBuffer->Bind();

		// material index
		s_in.read((char*)&mesh_out.nMaterialIdx, sizeof(unsigned int));

		return s_in;
	}

	std::ostream& operator<<(std::ostream& output_out, const Model::Material& mat_in)
	{
		// material name size
		unsigned int matNameSize = (unsigned int)mat_in.szName.size();
		output_out.write((const char*)&matNameSize, sizeof(unsigned int));

		// material name
		output_out.write(mat_in.szName.c_str(), matNameSize);

		// culling
		output_out.write((const char*)&mat_in.bTwoSided, sizeof(bool));

		// shading model
		output_out.write((const char*)&mat_in.eShadingModel, sizeof(Model::Material::ShadingModel));

		// wireframe
		output_out.write((const char*)&mat_in.bEnableWireframe, sizeof(bool));

		// blend mode
		output_out.write((const char*)&mat_in.eBlendMode, sizeof(Model::Material::BlendMode));

		// opacity
		output_out.write((const char*)&mat_in.fOpacity, sizeof(float));

		// bump scaling
		output_out.write((const char*)&mat_in.fBumpScaling, sizeof(float));

		// shininess
		output_out.write((const char*)&mat_in.fShininess, sizeof(float));

		// reflectivity
		output_out.write((const char*)&mat_in.fReflectivity, sizeof(float));

		// shininess strength
		output_out.write((const char*)&mat_in.fShininessStrength, sizeof(float));

		// refraction index
		output_out.write((const char*)&mat_in.fRefractIndex, sizeof(float));

		// diffuse base color
		output_out.write((const char*)mat_in.vColorDiffuse.getData(), sizeof(Vec3f));

		// ambient base color
		output_out.write((const char*)mat_in.vColorAmbient.getData(), sizeof(Vec3f));

		// specular base color
		output_out.write((const char*)mat_in.vColorSpecular.getData(), sizeof(Vec3f));

		// emissive base color
		output_out.write((const char*)mat_in.vColorEmissive.getData(), sizeof(Vec3f));

		// transparency base color
		output_out.write((const char*)mat_in.vColorTransparent.getData(), sizeof(Vec3f));

		// reflection base color
		output_out.write((const char*)mat_in.vColorReflective.getData(), sizeof(Vec3f));

		// number of associated textures
		unsigned int texCount = (unsigned int)mat_in.arrTexture.size();
		output_out.write((const char*)&texCount, sizeof(unsigned int));

		// texture descriptors
		for (unsigned int tex = 0; tex < texCount; tex++)
			output_out << *mat_in.arrTexture[tex];

		return output_out;
	}

	std::istream& operator>>(std::istream& s_in, Model::Material& mat_out)
	{
		// material name size
		unsigned int matNameSize = 0;
		s_in.read((char*)&matNameSize, sizeof(unsigned int));

		// material name
		char* matName = nullptr;
		matName = new char[matNameSize + 1];
		s_in.read(matName, matNameSize);
		matName[matNameSize] = '\0';
		mat_out.szName = matName;
		delete[] matName;

		// culling
		s_in.read((char*)&mat_out.bTwoSided, sizeof(bool));

		// shading model
		s_in.read((char*)&mat_out.eShadingModel, sizeof(Model::Material::ShadingModel));

		// wireframe
		s_in.read((char*)&mat_out.bEnableWireframe, sizeof(bool));

		// blend mode
		s_in.read((char*)&mat_out.eBlendMode, sizeof(Model::Material::BlendMode));

		// opacity
		s_in.read((char*)&mat_out.fOpacity, sizeof(float));

		// bump scaling
		s_in.read((char*)&mat_out.fBumpScaling, sizeof(float));

		// shininess
		s_in.read((char*)&mat_out.fShininess, sizeof(float));

		// reflectivity
		s_in.read((char*)&mat_out.fReflectivity, sizeof(float));

		// shininess strength
		s_in.read((char*)&mat_out.fShininessStrength, sizeof(float));

		// refraction index
		s_in.read((char*)&mat_out.fRefractIndex, sizeof(float));

		// diffuse base color
		s_in.read((char*)mat_out.vColorDiffuse.getData(), sizeof(Vec3f));

		// ambient base color
		s_in.read((char*)mat_out.vColorAmbient.getData(), sizeof(Vec3f));

		// specular base color
		s_in.read((char*)mat_out.vColorSpecular.getData(), sizeof(Vec3f));

		// emissive base color
		s_in.read((char*)mat_out.vColorEmissive.getData(), sizeof(Vec3f));

		// transparency base color
		s_in.read((char*)mat_out.vColorTransparent.getData(), sizeof(Vec3f));

		// reflection base color
		s_in.read((char*)mat_out.vColorReflective.getData(), sizeof(Vec3f));

		// number of associated textures
		unsigned int texCount = (unsigned int)mat_out.arrTexture.size();
		s_in.read((char*)&texCount, sizeof(unsigned int));

		// texture descriptors
		for (unsigned int tex = 0; tex < texCount; tex++)
		{
			mat_out.arrTexture.push_back(new Model::TextureDesc);
			s_in >> *mat_out.arrTexture.back();
		}

		return s_in;
	}

	std::ostream& operator<<(std::ostream& output_out, const Model::TextureDesc& tex_in)
	{
		// file path size
		unsigned int filePathSize = (unsigned int)tex_in.szFilePath.size();
		output_out.write((const char*)&filePathSize, sizeof(unsigned int));

		// file path
		output_out.write(tex_in.szFilePath.c_str(), filePathSize);

		// texture type
		output_out.write((const char*)&tex_in.eTexType, sizeof(Model::TextureDesc::TextureType));

		// texture index
		output_out.write((const char*)&tex_in.nTexIndex, sizeof(unsigned int));

		// UV channel
		output_out.write((const char*)&tex_in.nUVChannel, sizeof(unsigned int));

		// texture blend operation
		output_out.write((const char*)&tex_in.eTexOp, sizeof(Model::TextureDesc::TextureOp));

		// texture mapping
		output_out.write((const char*)&tex_in.eTexMapping, sizeof(Model::TextureDesc::TextureMapping));

		// texture blend factor
		output_out.write((const char*)&tex_in.fTexBlend, sizeof(float));

		// texture mapping mode on U
		output_out.write((const char*)&tex_in.eTexMapModeU, sizeof(Model::TextureDesc::TextureMappingMode));

		// texture mapping mode on V
		output_out.write((const char*)&tex_in.eTexMapModeV, sizeof(Model::TextureDesc::TextureMappingMode));

		return output_out;
	}

	std::istream& operator>>(std::istream& s_in, Model::TextureDesc& tex_out)
	{
		// file path size
		unsigned int filePathSize = 0;
		s_in.read((char*)&filePathSize, sizeof(unsigned int));

		// file path
		char* filePath = nullptr;
		filePath = new char[filePathSize + 1];
		s_in.read(filePath, filePathSize);
		filePath[filePathSize] = '\0';
		tex_out.szFilePath = filePath;
		delete[] filePath;

		// texture type
		s_in.read((char*)&tex_out.eTexType, sizeof(Model::TextureDesc::TextureType));

		// texture index
		s_in.read((char*)&tex_out.nTexIndex, sizeof(unsigned int));

		// UV channel
		s_in.read((char*)&tex_out.nUVChannel, sizeof(unsigned int));

		// texture blend operation
		s_in.read((char*)&tex_out.eTexOp, sizeof(Model::TextureDesc::TextureOp));

		// texture mapping
		s_in.read((char*)&tex_out.eTexMapping, sizeof(Model::TextureDesc::TextureMapping));

		// texture blend factor
		s_in.read((char*)&tex_out.fTexBlend, sizeof(float));

		// texture mapping mode on U
		s_in.read((char*)&tex_out.eTexMapModeU, sizeof(Model::TextureDesc::TextureMappingMode));

		// texture mapping mode on V
		s_in.read((char*)&tex_out.eTexMapModeV, sizeof(Model::TextureDesc::TextureMappingMode));

		return s_in;
	}

	Model::Mesh::~Mesh()
	{
		if (Renderer::GetInstance())
		{
			ResourceManager* resMan = Renderer::GetInstance()->GetResourceManager();
			if (resMan)
			{
				resMan->ReleaseVertexFormat(nVfIdx);
				resMan->ReleaseIndexBuffer(nIbIdx);
				resMan->ReleaseVertexBuffer(nVbIdx);
				nVfIdx = nIbIdx = nVbIdx = ~0u;
				pVertexFormat = nullptr;
				pIndexBuffer = nullptr;
				pVertexBuffer = nullptr;
			}
		}
	}

	Model::Material::~Material()
	{
		for (unsigned int tex = 0; tex < arrTexture.size(); tex++)
		{
			if (arrTexture[tex] != nullptr)
			{
				delete arrTexture[tex];
				arrTexture[tex] = nullptr;
			}
		}
	}

	Model::~Model()
	{
		for (unsigned int mesh = 0; mesh < arrMesh.size(); mesh++)
		{
			if (arrMesh[mesh] != nullptr)
			{
				delete arrMesh[mesh];
				arrMesh[mesh] = nullptr;
			}
		}

		for (unsigned int mat = 0; mat < arrMaterial.size(); mat++)
		{
			if (arrMaterial[mat] != nullptr)
			{
				delete arrMaterial[mat];
				arrMaterial[mat] = nullptr;
			}
		}
	}

	std::ostream& operator<<(std::ostream& output_out, Texture& tex_in)
	{
		output_out << *(Buffer*)&tex_in;

		output_out.write((char*)&tex_in.m_ePixelFormat, sizeof(PixelFormat));
		output_out.write((char*)&tex_in.m_eTexType, sizeof(TextureType));
		output_out.write((char*)&tex_in.m_nMipCount, sizeof(unsigned int));

		output_out.write((char*)&tex_in.m_nDimensionCount, sizeof(unsigned int));
		for (unsigned int i = 0; i < tex_in.m_nMipCount; i++)
			output_out.write((char*)&tex_in.m_nDimension[i], sizeof(Vec<unsigned int, 3U>));
		for (unsigned int i = 0; i < tex_in.m_nMipCount; i++)
			output_out.write((char*)&tex_in.m_nMipSizeBytes[i], sizeof(unsigned int));
		for (unsigned int i = 0; i < tex_in.m_nMipCount; i++)
			output_out.write((char*)&tex_in.m_nMipOffset[i], sizeof(unsigned int));

		return output_out;
	}

	std::istream& operator>>(std::istream& s_in, Texture& tex_out)
	{
		s_in >> *(Buffer*)&tex_out;

		s_in.read((char*)&tex_out.m_ePixelFormat, sizeof(PixelFormat));
		s_in.read((char*)&tex_out.m_eTexType, sizeof(TextureType));
		s_in.read((char*)&tex_out.m_nMipCount, sizeof(unsigned int));

		s_in.read((char*)&tex_out.m_nDimensionCount, sizeof(unsigned int));
		for (unsigned int i = 0; i < tex_out.m_nMipCount; i++)
			s_in.read((char*)&tex_out.m_nDimension[i], sizeof(Vec<unsigned int, 3U>));
		for (unsigned int i = 0; i < tex_out.m_nMipCount; i++)
			s_in.read((char*)&tex_out.m_nMipSizeBytes[i], sizeof(unsigned int));
		for (unsigned int i = 0; i < tex_out.m_nMipCount; i++)
			s_in.read((char*)&tex_out.m_nMipOffset[i], sizeof(unsigned int));

		tex_out.Bind();

		return s_in;
	}
}
