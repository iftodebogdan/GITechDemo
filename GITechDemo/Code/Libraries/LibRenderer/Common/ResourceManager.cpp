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

#include "ResourceManager.h"
using namespace LibRendererDll;

#include <fstream>

ResourceManager::ResourceManager()
{}

ResourceManager::~ResourceManager()
{
	ReleaseAll();
}

void ResourceManager::ReleaseAll()
{
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
}

void ResourceManager::BindAll()
{
	for (unsigned int i = 0; i < m_arrVertexFormat.size(); i++)
		m_arrVertexFormat[i]->Bind();
	for (unsigned int i = 0; i < m_arrIndexBuffer.size(); i++)
		m_arrIndexBuffer[i]->Bind();
	for (unsigned int i = 0; i < m_arrVertexBuffer.size(); i++)
		m_arrVertexBuffer[i]->Bind();
	for (unsigned int i = 0; i < m_arrShaderProgram.size(); i++)
		m_arrShaderProgram[i]->Bind();
	for (unsigned int i = 0; i < m_arrTexture.size(); i++)
		m_arrTexture[i]->Bind();
	for (unsigned int i = 0; i < m_arrRenderTarget.size(); i++)
		m_arrRenderTarget[i]->Bind();
}

void ResourceManager::UnbindAll()
{
	for (unsigned int i = 0; i < m_arrVertexFormat.size(); i++)
		m_arrVertexFormat[i]->Unbind();
	for (unsigned int i = 0; i < m_arrIndexBuffer.size(); i++)
		m_arrIndexBuffer[i]->Unbind();
	for (unsigned int i = 0; i < m_arrVertexBuffer.size(); i++)
		m_arrVertexBuffer[i]->Unbind();
	for (unsigned int i = 0; i < m_arrShaderProgram.size(); i++)
		m_arrShaderProgram[i]->Unbind();
	for (unsigned int i = 0; i < m_arrTexture.size(); i++)
		m_arrTexture[i]->Unbind();
	for (unsigned int i = 0; i < m_arrRenderTarget.size(); i++)
		m_arrRenderTarget[i]->Unbind();
}

const unsigned int ResourceManager::CreateShaderInput(ShaderTemplate* const shaderTemplate)
{
	m_arrShaderInput.push_back(new ShaderInput(shaderTemplate));
	return (unsigned int)m_arrShaderInput.size() - 1;
}

const unsigned int ResourceManager::CreateShaderProgram(const ShaderProgramType programType, const char* srcData, char* const errors, const char* entryPoint, const char* profile)
{
	// This is just a safety measure to convert CR and CRLF
	// line ending to LF
	std::string srcDataLF;
	const char* i;
	unsigned int n;
	for (i = srcData, n = (unsigned int)strlen(srcData); i + n > srcData; srcData++)
	{
		switch (*srcData) {
		case '\r':
			assert(false);
			srcDataLF += '\n';	// write LF instead of CR
			if (*(srcData + 1) == '\n')	// if it's CRLF line ending
				srcData++;				// skip another character
		case '\n':
		default:
			srcDataLF += (char)*srcData;
		}
	}

	const unsigned int spIdx = CreateShaderProgram(programType);
	ShaderProgram* const sp = GetShaderProgram(spIdx);
	sp->Compile(srcDataLF.c_str(), errors, entryPoint, profile);
	return spIdx;
}

const unsigned int ResourceManager::CreateShaderTemplate(ShaderProgram* const shaderProgram)
{
	m_arrShaderTemplate.push_back(new ShaderTemplate(shaderProgram));
	return (unsigned int)m_arrShaderTemplate.size() - 1;
}

const unsigned int ResourceManager::CreateTexture(const char* pathToFile)
{
	const unsigned int texIdx = CreateTexture(PF_NONE, TT_1D, 0, 0, 0, 0, BU_NONE);
	std::ifstream texFile;
	texFile.open(pathToFile, std::ios::binary);
	if (texFile.is_open())
	{
		texFile >> *GetTexture(texIdx);
		texFile.close();
		return texIdx;
	}
	else
	{
		delete GetTexture(texIdx);
		m_arrTexture.pop_back();
		return -1;
	}
}

const unsigned int ResourceManager::CreateModel(const char* pathToFile)
{
	m_arrModel.push_back(new Model);
	std::ifstream modelFile;
	modelFile.open(pathToFile, std::ios::binary);
	if (modelFile.is_open())
	{
		modelFile >> *m_arrModel.back();
		modelFile.close();
		return (unsigned int)m_arrModel.size() - 1;
	}
	else
	{
		delete m_arrModel.back();
		m_arrModel.pop_back();
		return -1;
	}
}