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

#include "VertexFormat.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ShaderInput.h"
#include "ShaderProgram.h"
#include "ShaderTemplate.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "Renderer.h"
#include "ResourceManager.h"
using namespace LibRendererDll;

#include <fstream>

ResourceManager::ResourceManager()
{
	MUTEX_INIT(mVFMutex);
	MUTEX_INIT(mIBMutex);
	MUTEX_INIT(mVBMutex);
	MUTEX_INIT(mShdInMutex);
	MUTEX_INIT(mShdProgMutex);
	MUTEX_INIT(mShdTmplMutex);
	MUTEX_INIT(mTexMutex);
	MUTEX_INIT(mRTMutex);
	MUTEX_INIT(mModelMutex);
}

ResourceManager::~ResourceManager()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	// Resources should already be released by the time
	// we get here, but just as a precaution, check if
	// there is anything that should be released
	if (GetVertexFormatCount() ||
		GetIndexBufferCount() ||
		GetVertexBufferCount() ||
		GetShaderInputCount() ||
		GetShaderProgramCount() ||
		GetShaderTemplateCount() ||
		GetTextureCount() ||
		GetRenderTargetCount() ||
		GetModelCount())
		ReleaseAll();

	MUTEX_DESTROY(mVFMutex);
	MUTEX_DESTROY(mIBMutex);
	MUTEX_DESTROY(mVBMutex);
	MUTEX_DESTROY(mShdInMutex);
	MUTEX_DESTROY(mShdProgMutex);
	MUTEX_DESTROY(mShdTmplMutex);
	MUTEX_DESTROY(mTexMutex);
	MUTEX_DESTROY(mRTMutex);
	MUTEX_DESTROY(mModelMutex);

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseAll()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	for (unsigned int i = 0; i < m_arrModel.size(); i++)
		delete m_arrModel[i];
	for (unsigned int i = 0; i < m_arrVertexFormat.size(); i++)
		delete m_arrVertexFormat[i];
	for (unsigned int i = 0; i < m_arrIndexBuffer.size(); i++)
		delete m_arrIndexBuffer[i];
	for (unsigned int i = 0; i < m_arrVertexBuffer.size(); i++)
		delete m_arrVertexBuffer[i];
	for (unsigned int i = 0; i < m_arrShaderInput.size(); i++)
		delete m_arrShaderInput[i];
	for (unsigned int i = 0; i < m_arrShaderProgram.size(); i++)
		delete m_arrShaderProgram[i];
	for (unsigned int i = 0; i < m_arrShaderTemplate.size(); i++)
		delete m_arrShaderTemplate[i];
	for (unsigned int i = 0; i < m_arrRenderTarget.size(); i++)
		delete m_arrRenderTarget[i];
	for (unsigned int i = 0; i < m_arrTexture.size(); i++)
		delete m_arrTexture[i];

	m_arrModel.clear();
	m_arrVertexFormat.clear();
	m_arrIndexBuffer.clear();
	m_arrVertexBuffer.clear();
	m_arrShaderInput.clear();
	m_arrShaderProgram.clear();
	m_arrShaderTemplate.clear();
	m_arrTexture.clear();
	m_arrRenderTarget.clear();

	POP_PROFILE_MARKER();
}

void ResourceManager::BindAll()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	for (unsigned int i = 0; i < m_arrVertexFormat.size(); i++)
		if (m_arrVertexFormat[i])
			m_arrVertexFormat[i]->Bind();
	for (unsigned int i = 0; i < m_arrIndexBuffer.size(); i++)
		if (m_arrIndexBuffer[i])
			m_arrIndexBuffer[i]->Bind();
	for (unsigned int i = 0; i < m_arrVertexBuffer.size(); i++)
		if (m_arrVertexBuffer[i])
			m_arrVertexBuffer[i]->Bind();
	//for (unsigned int i = 0; i < m_arrShaderProgram.size(); i++)
	//	if (m_arrShaderProgram[i])
	//		m_arrShaderProgram[i]->Bind();
	for (unsigned int i = 0; i < m_arrTexture.size(); i++)
		if (m_arrTexture[i])
			m_arrTexture[i]->Bind();
	for (unsigned int i = 0; i < m_arrRenderTarget.size(); i++)
		if (m_arrRenderTarget[i])
			m_arrRenderTarget[i]->Bind();

	POP_PROFILE_MARKER();
}

void ResourceManager::UnbindAll()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	for (unsigned int i = 0; i < m_arrVertexFormat.size(); i++)
		if (m_arrVertexFormat[i])
			m_arrVertexFormat[i]->Unbind();
	for (unsigned int i = 0; i < m_arrIndexBuffer.size(); i++)
		if (m_arrIndexBuffer[i])
			m_arrIndexBuffer[i]->Unbind();
	for (unsigned int i = 0; i < m_arrVertexBuffer.size(); i++)
		if (m_arrVertexBuffer[i])
			m_arrVertexBuffer[i]->Unbind();
	//for (unsigned int i = 0; i < m_arrShaderProgram.size(); i++)
	//	if (m_arrShaderProgram[i])
	//		m_arrShaderProgram[i]->Unbind();
	for (unsigned int i = 0; i < m_arrTexture.size(); i++)
		if (m_arrTexture[i])
			m_arrTexture[i]->Unbind();
	for (unsigned int i = 0; i < m_arrRenderTarget.size(); i++)
		if (m_arrRenderTarget[i])
			m_arrRenderTarget[i]->Unbind();

	POP_PROFILE_MARKER();
}

const unsigned int ResourceManager::CreateShaderInput(ShaderTemplate* const shaderTemplate)
{
	ShaderInput* const shdIn = new ShaderInput(shaderTemplate);
	MUTEX_LOCK(mShdInMutex);
	m_arrShaderInput.push_back(shdIn);
	const unsigned int ret = (unsigned int)m_arrShaderInput.size() - 1;
	MUTEX_UNLOCK(mShdInMutex);
	return ret;
}

const unsigned int ResourceManager::CreateShaderProgram(const char* filePath, const ShaderProgramType programType, char* const errors, const char* entryPoint, const char* profile)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	const unsigned int spIdx = CreateShaderProgram(programType);
	ShaderProgram* const sp = GetShaderProgram(spIdx);
	sp->Compile(filePath, errors, entryPoint, profile);

	POP_PROFILE_MARKER();

	return spIdx;
}

const unsigned int ResourceManager::CreateShaderTemplate(ShaderProgram* const shaderProgram)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	ShaderTemplate* const shdTmpl = new ShaderTemplate(shaderProgram);
	MUTEX_LOCK(mShdTmplMutex);
	m_arrShaderTemplate.push_back(shdTmpl);
	const unsigned int ret = (unsigned int)m_arrShaderTemplate.size() - 1;
	MUTEX_UNLOCK(mShdTmplMutex);

	POP_PROFILE_MARKER();

	return ret;
}

const unsigned int ResourceManager::CreateTexture(const char* pathToFile)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	unsigned int texIdx = ~0u;
	std::ifstream texFile;
	texFile.open(pathToFile, std::ios::binary);
	if (texFile.is_open())
	{
		MUTEX_LOCK(mTexMutex);
		texIdx = CreateTexture(PF_NONE, TT_1D, 0, 0, 0, 0, BU_NONE);
		GetTexture(texIdx)->m_szSourceFile = pathToFile;
		MUTEX_UNLOCK(mTexMutex);
		texFile >> *GetTexture(texIdx);
		texFile.close();
	}

	POP_PROFILE_MARKER();

	return texIdx;
}

const unsigned int ResourceManager::CreateModel(const char* pathToFile)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	std::ifstream modelFile;
	modelFile.open(pathToFile, std::ios::binary);
	unsigned int modelIdx = ~0u;
	if (modelFile.is_open())
	{
		Model* const mdl = new Model;
		mdl->szSourceFile = pathToFile;
		MUTEX_LOCK(mModelMutex);
		m_arrModel.push_back(mdl);
		modelIdx = (unsigned int)m_arrModel.size() - 1;
		MUTEX_UNLOCK(mModelMutex);
		modelFile >> *mdl;
		modelFile.close();
	}

	POP_PROFILE_MARKER();

	return modelIdx;
}

const unsigned int ResourceManager::FindTexture(const char * pathToFile, const bool strict)
{
	MUTEX_LOCK(mTexMutex);
	for (unsigned int i = 0; i < m_arrTexture.size(); i++)
		if (m_arrTexture[i] && m_arrTexture[i]->m_szSourceFile == pathToFile)
		{
			MUTEX_UNLOCK(mTexMutex);
			return i;
		}

	if (!strict)
		for (unsigned int i = 0; i < m_arrTexture.size(); i++)
			if (m_arrTexture[i] && m_arrTexture[i]->m_szSourceFile.find(pathToFile) != std::string::npos)
			{
				MUTEX_UNLOCK(mTexMutex);
				return i;
			}

	MUTEX_UNLOCK(mTexMutex);
	return ~0u;
}

const unsigned int ResourceManager::FindModel(const char * pathToFile, const bool strict)
{
	MUTEX_LOCK(mModelMutex);
	for (unsigned int i = 0; i < m_arrModel.size(); i++)
		if (m_arrModel[i] && m_arrModel[i]->szSourceFile == pathToFile)
		{
			MUTEX_UNLOCK(mModelMutex);
			return i;
		}

	if(!strict)
		for (unsigned int i = 0; i < m_arrModel.size(); i++)
			if (m_arrModel[i] && m_arrModel[i]->szSourceFile.find(pathToFile) != std::string::npos)
			{
				MUTEX_UNLOCK(mModelMutex);
				return i;
			}

	MUTEX_UNLOCK(mModelMutex);
	return ~0u;
}

VertexFormat* const ResourceManager::GetVertexFormat(const unsigned int idx) const
{
	assert(idx < m_arrVertexFormat.size());
	if (idx >= m_arrVertexFormat.size())
		return nullptr;
	return m_arrVertexFormat[idx];
}

IndexBuffer* const ResourceManager::GetIndexBuffer(const unsigned int idx) const
{
	assert(idx < m_arrIndexBuffer.size());
	if (idx >= m_arrIndexBuffer.size())
		return nullptr;
	return m_arrIndexBuffer[idx];
}

VertexBuffer* const ResourceManager::GetVertexBuffer(const unsigned int idx) const
{
	assert(idx < m_arrVertexBuffer.size());
	if (idx >= m_arrVertexBuffer.size())
		return nullptr;
	return m_arrVertexBuffer[idx];
}

ShaderInput* const ResourceManager::GetShaderInput(const unsigned int idx) const
{
	assert(idx < m_arrShaderInput.size());
	if (idx >= m_arrShaderInput.size())
		return nullptr;
	return m_arrShaderInput[idx];
}

ShaderProgram* const ResourceManager::GetShaderProgram(const unsigned int idx) const
{
	assert(idx < m_arrShaderProgram.size());
	if (idx >= m_arrShaderProgram.size())
		return nullptr;
	return m_arrShaderProgram[idx];
}

ShaderTemplate* const ResourceManager::GetShaderTemplate(const unsigned idx) const
{
	assert(idx < m_arrShaderTemplate.size());
	if (idx >= m_arrShaderTemplate.size())
		return nullptr;
	return m_arrShaderTemplate[idx];
}

Texture* const ResourceManager::GetTexture(const unsigned int idx) const
{
	assert(idx < m_arrTexture.size());
	if (idx >= m_arrTexture.size())
		return nullptr;
	return m_arrTexture[idx];
}

RenderTarget* const ResourceManager::GetRenderTarget(const unsigned int idx) const
{
	assert(idx < m_arrRenderTarget.size());
	if (idx >= m_arrRenderTarget.size())
		return nullptr;
	return m_arrRenderTarget[idx];
}

Model* const ResourceManager::GetModel(const unsigned int idx) const
{
	assert(idx < m_arrModel.size());
	if (idx >= m_arrModel.size())
		return nullptr;
	return m_arrModel[idx];
}

const unsigned int ResourceManager::GetVertexFormatCount() const
{
	return (unsigned int)m_arrVertexFormat.size();
}

const unsigned int ResourceManager::GetIndexBufferCount() const
{
	return (unsigned int)m_arrIndexBuffer.size();
}

const unsigned int ResourceManager::GetVertexBufferCount() const
{
	return (unsigned int)m_arrVertexBuffer.size();
}

const unsigned int ResourceManager::GetShaderInputCount() const
{
	return (unsigned int)m_arrShaderInput.size();
}

const unsigned int ResourceManager::GetShaderProgramCount() const
{
	return (unsigned int)m_arrShaderProgram.size();
}

const unsigned int ResourceManager::GetShaderTemplateCount() const
{
	return (unsigned int)m_arrShaderTemplate.size();
}

const unsigned int ResourceManager::GetTextureCount() const
{
	return (unsigned int)m_arrTexture.size();
}

const unsigned int ResourceManager::GetRenderTargetCount() const
{
	return (unsigned int)m_arrRenderTarget.size();
}

const unsigned int ResourceManager::GetModelCount() const
{
	return (unsigned int)m_arrModel.size();
}

void ResourceManager::ReleaseVertexFormat(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrVertexFormat.size());
	if (idx >= m_arrVertexFormat.size())
		return;

	delete m_arrVertexFormat[idx];
	m_arrVertexFormat[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseIndexBuffer(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrIndexBuffer.size());
	if (idx >= m_arrIndexBuffer.size())
		return;

	delete m_arrIndexBuffer[idx];
	m_arrIndexBuffer[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseVertexBuffer(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrVertexBuffer.size());
	if (idx >= m_arrVertexBuffer.size())
		return;

	delete m_arrVertexBuffer[idx];
	m_arrVertexBuffer[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseShaderInput(const unsigned int idx)
{
	assert(idx < m_arrShaderInput.size());
	if (idx >= m_arrShaderInput.size())
		return;

	delete m_arrShaderInput[idx];
	m_arrShaderInput[idx] = nullptr;
}

void ResourceManager::ReleaseShaderProgram(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrShaderProgram.size());
	if (idx >= m_arrShaderProgram.size())
		return;

	delete m_arrShaderProgram[idx];
	m_arrShaderProgram[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseShaderTemplate(const unsigned idx)
{
	assert(idx < m_arrShaderTemplate.size());
	if (idx >= m_arrShaderTemplate.size())
		return;

	delete m_arrShaderTemplate[idx];
	m_arrShaderTemplate[idx] = nullptr;
}

void ResourceManager::ReleaseTexture(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrTexture.size());
	if (idx >= m_arrTexture.size())
		return;

	delete m_arrTexture[idx];
	m_arrTexture[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseRenderTarget(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrRenderTarget.size());
	if (idx >= m_arrRenderTarget.size())
		return;

	delete m_arrRenderTarget[idx];
	m_arrRenderTarget[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseModel(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrRenderTarget.size());
	if (idx > m_arrRenderTarget.size())
		return;

	delete m_arrModel[idx];
	m_arrModel[idx] = nullptr;

	POP_PROFILE_MARKER();
}
