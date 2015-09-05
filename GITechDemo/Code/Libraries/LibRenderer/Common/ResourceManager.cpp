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
{}

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
	m_arrShaderInput.push_back(new ShaderInput(shaderTemplate));
	return (unsigned int)m_arrShaderInput.size() - 1;
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

	m_arrShaderTemplate.push_back(new ShaderTemplate(shaderProgram));

	POP_PROFILE_MARKER();

	return (unsigned int)m_arrShaderTemplate.size() - 1;
}

const unsigned int ResourceManager::CreateTexture(const char* pathToFile)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	unsigned int texIdx = ~0u;
	std::ifstream texFile;
	texFile.open(pathToFile, std::ios::binary);
	if (texFile.is_open())
	{
		texIdx = CreateTexture(PF_NONE, TT_1D, 0, 0, 0, 0, BU_NONE);
		texFile >> *GetTexture(texIdx);
		texFile.close();
		GetTexture(texIdx)->m_szSourceFile = pathToFile;
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
		m_arrModel.push_back(new Model);
		modelFile >> *m_arrModel.back();
		modelFile.close();
		m_arrModel.back()->szSourceFile = pathToFile;
		modelIdx = (unsigned int)m_arrModel.size() - 1;
	}

	POP_PROFILE_MARKER();

	return modelIdx;
}

const unsigned int ResourceManager::FindTexture(const char * pathToFile, const bool strict)
{
	for (unsigned int i = 0; i < m_arrTexture.size(); i++)
		if (m_arrTexture[i] && m_arrTexture[i]->m_szSourceFile == pathToFile)
			return i;

	if (!strict)
		for (unsigned int i = 0; i < m_arrTexture.size(); i++)
			if (m_arrTexture[i] && m_arrTexture[i]->m_szSourceFile.find(pathToFile) != std::string::npos)
				return i;

	return ~0u;
}

const unsigned int ResourceManager::FindModel(const char * pathToFile, const bool strict)
{
	for (unsigned int i = 0; i < m_arrModel.size(); i++)
		if (m_arrModel[i] && m_arrModel[i]->szSourceFile == pathToFile)
			return i;

	if(!strict)
		for (unsigned int i = 0; i < m_arrModel.size(); i++)
			if (m_arrModel[i] && m_arrModel[i]->szSourceFile.find(pathToFile) != std::string::npos)
				return i;

	return ~0u;
}

VertexFormat* const ResourceManager::GetVertexFormat(const unsigned int idx) const
{
	assert(idx < m_arrVertexFormat.size());
	return m_arrVertexFormat[idx];
}

IndexBuffer* const ResourceManager::GetIndexBuffer(const unsigned int idx) const
{
	assert(idx < m_arrIndexBuffer.size());
	return m_arrIndexBuffer[idx];
}

VertexBuffer* const ResourceManager::GetVertexBuffer(const unsigned int idx) const
{
	assert(idx < m_arrVertexBuffer.size());
	return m_arrVertexBuffer[idx];
}

ShaderInput* const ResourceManager::GetShaderInput(const unsigned int idx) const
{
	assert(idx < m_arrShaderInput.size());
	return m_arrShaderInput[idx];
}

ShaderProgram* const ResourceManager::GetShaderProgram(const unsigned int idx) const
{
	assert(idx < m_arrShaderProgram.size());
	return m_arrShaderProgram[idx];
}

ShaderTemplate* const ResourceManager::GetShaderTemplate(const unsigned idx) const
{
	assert(idx < m_arrShaderTemplate.size());
	return m_arrShaderTemplate[idx];
}

Texture* const ResourceManager::GetTexture(const unsigned int idx) const
{
	assert(idx < m_arrTexture.size());
	return m_arrTexture[idx];
}

RenderTarget* const ResourceManager::GetRenderTarget(const unsigned int idx) const
{
	assert(idx < m_arrRenderTarget.size());
	return m_arrRenderTarget[idx];
}

Model* const ResourceManager::GetModel(const unsigned int idx) const
{
	assert(idx < m_arrModel.size());
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
	delete m_arrVertexFormat[idx];
	m_arrVertexFormat[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseIndexBuffer(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrIndexBuffer.size());
	delete m_arrIndexBuffer[idx];
	m_arrIndexBuffer[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseVertexBuffer(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrVertexBuffer.size());
	delete m_arrVertexBuffer[idx];
	m_arrVertexBuffer[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseShaderInput(const unsigned int idx)
{
	assert(idx < m_arrShaderInput.size());
	delete m_arrShaderInput[idx];
	m_arrShaderInput[idx] = nullptr;
}

void ResourceManager::ReleaseShaderProgram(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrShaderProgram.size());
	delete m_arrShaderProgram[idx];
	m_arrShaderProgram[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseShaderTemplate(const unsigned idx)
{
	assert(idx < m_arrShaderTemplate.size());
	delete m_arrShaderTemplate[idx];
	m_arrShaderTemplate[idx] = nullptr;
}

void ResourceManager::ReleaseTexture(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrTexture.size());
	delete m_arrTexture[idx];
	m_arrTexture[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseRenderTarget(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrRenderTarget.size());
	delete m_arrRenderTarget[idx];
	m_arrRenderTarget[idx] = nullptr;

	POP_PROFILE_MARKER();
}

void ResourceManager::ReleaseModel(const unsigned int idx)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(idx < m_arrRenderTarget.size());
	delete m_arrModel[idx];
	m_arrModel[idx] = nullptr;

	POP_PROFILE_MARKER();
}
