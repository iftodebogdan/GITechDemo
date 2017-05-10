/**
 * @file        ResourceManager.cpp
 *
 * @note        This file is part of the "Synesthesia3D" graphics engine
 *
 * @copyright   Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * @copyright
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * @copyright
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"

#include "VertexFormat.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ShaderInput.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "Profiler.h"
using namespace Synesthesia3D;

#include <fstream>

#include <Utility/Mutex.h>

#include <lz4/lz4hc.h>

// Mutexes for each resource pool
MUTEX   VFMutex;
MUTEX   IBMutex;
MUTEX   VBMutex;
MUTEX   ShdInMutex;
MUTEX   ShdProgMutex;
MUTEX   TexMutex;
MUTEX   RTMutex;
MUTEX   ModelMutex;

bool    bMutexesInitialized = false;

struct membuf : std::streambuf {
    membuf(char const* base, size_t size) {
        char* p(const_cast<char*>(base));
        this->setg(p, p, p + size);
    }
};

struct imemstream : virtual membuf, std::istream {
    imemstream(char const* base, size_t size)
        : membuf(base, size)
        , std::istream(static_cast<std::streambuf*>(this)) {
    }
};

ResourceManager::ResourceManager()
{
    if (!bMutexesInitialized)
    {
        MUTEX_INIT(VFMutex);
        MUTEX_INIT(IBMutex);
        MUTEX_INIT(VBMutex);
        MUTEX_INIT(ShdInMutex);
        MUTEX_INIT(ShdProgMutex);
        MUTEX_INIT(TexMutex);
        MUTEX_INIT(RTMutex);
        MUTEX_INIT(ModelMutex);

        bMutexesInitialized = true;
    }
}

ResourceManager::~ResourceManager()
{
    // Resources should already be released by the time
    // we get here, but just as a precaution, check if
    // there is anything that should be released
    if (GetVertexFormatCount() ||
        GetIndexBufferCount() ||
        GetVertexBufferCount() ||
        GetShaderInputCount() ||
        GetShaderProgramCount() ||
        GetTextureCount() ||
        GetRenderTargetCount() ||
        GetModelCount())
        ReleaseAll();

    if (bMutexesInitialized)
    {
        MUTEX_DESTROY(VFMutex);
        MUTEX_DESTROY(IBMutex);
        MUTEX_DESTROY(VBMutex);
        MUTEX_DESTROY(ShdInMutex);
        MUTEX_DESTROY(ShdProgMutex);
        MUTEX_DESTROY(TexMutex);
        MUTEX_DESTROY(RTMutex);
        MUTEX_DESTROY(ModelMutex);

        bMutexesInitialized = false;
    }
}

void ResourceManager::ReleaseAll()
{
    UnbindAll();

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
    m_arrTexture.clear();
    m_arrRenderTarget.clear();
}

void ResourceManager::BindAll()
{
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
    //  if (m_arrShaderProgram[i])
    //      m_arrShaderProgram[i]->Bind();
    for (unsigned int i = 0; i < m_arrTexture.size(); i++)
        if (m_arrTexture[i])
            m_arrTexture[i]->Bind();
    for (unsigned int i = 0; i < m_arrRenderTarget.size(); i++)
        if (m_arrRenderTarget[i])
            m_arrRenderTarget[i]->Bind();
}

void ResourceManager::UnbindAll()
{
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
    //  if (m_arrShaderProgram[i])
    //      m_arrShaderProgram[i]->Unbind();
    for (unsigned int i = 0; i < m_arrTexture.size(); i++)
        if (m_arrTexture[i])
            m_arrTexture[i]->Unbind();
    for (unsigned int i = 0; i < m_arrRenderTarget.size(); i++)
        if (m_arrRenderTarget[i])
            m_arrRenderTarget[i]->Unbind();

    if(Renderer::GetInstance()->GetProfiler())
        Renderer::GetInstance()->GetProfiler()->ReleaseGPUProfileMarkerResults();
}

const unsigned int ResourceManager::CreateShaderInput(ShaderProgram* const shaderProgram)
{
    ShaderInput* const shdIn = new ShaderInput(shaderProgram);
    MUTEX_LOCK(ShdInMutex);
    m_arrShaderInput.push_back(shdIn);
    const unsigned int ret = (unsigned int)m_arrShaderInput.size() - 1;
    MUTEX_UNLOCK(ShdInMutex);
    return ret;
}

const unsigned int ResourceManager::CreateTexture(const char* pathToFile)
{
    unsigned int texIdx = ~0u;
    std::ifstream texFile;
    texFile.open(pathToFile, std::ios::binary);

    if (texFile.is_open())
    {
        char fileSignature[S3D_TEXTURE_FILE_HEADER_SIZE];
        texFile.read(fileSignature, S3D_TEXTURE_FILE_HEADER_SIZE);

        if (memcmp(S3D_TEXTURE_FILE_HEADER, fileSignature, S3D_TEXTURE_FILE_HEADER_SIZE) == 0)
        {
            unsigned int fileVersion = 0;
            texFile.read((char*)&fileVersion, sizeof(unsigned int));

            if (fileVersion == S3D_TEXTURE_FILE_VERSION)
            {
                unsigned int compressedBufferSize = 0, decompressedBufferSize = 0;
                texFile.read((char*)&compressedBufferSize, sizeof(unsigned int));
                texFile.read((char*)&decompressedBufferSize, sizeof(unsigned int));

                if (compressedBufferSize > 0 && compressedBufferSize <= LZ4_COMPRESSBOUND(LZ4_MAX_INPUT_SIZE) &&
                    decompressedBufferSize > 0 && decompressedBufferSize <= LZ4_MAX_INPUT_SIZE)
                {
                    char* const compressedBuffer = new char[compressedBufferSize];
                    char* const decompressedBuffer = new char[decompressedBufferSize];
                    texFile.read(compressedBuffer, compressedBufferSize);
                    const int readBytes = LZ4_decompress_fast(compressedBuffer, decompressedBuffer, decompressedBufferSize);

                    if (readBytes == compressedBufferSize)
                    {
                        imemstream  texBuffer(decompressedBuffer, decompressedBufferSize);
                        //MUTEX_LOCK(TexMutex);
                        texIdx = CreateTexture(PF_NONE, TT_1D, 0, 0, 0, 0, BU_NONE);
                        GetTexture(texIdx)->m_szSourceFile = pathToFile;
                        //MUTEX_UNLOCK(TexMutex);
                        texBuffer >> *GetTexture(texIdx);
                    }
                    else
                    {
                        S3D_DBGPRINT("Error: Texture %s could not be decompressed", pathToFile);
                        assert(0);
                    }

                    delete[] compressedBuffer;
                    delete[] decompressedBuffer;
                }
                else
                {
                    S3D_DBGPRINT("Error: Texture %s has invalid data size", pathToFile);
                    assert(0);
                }
            }
            else
            {
                S3D_DBGPRINT("Error: Texture %s is version %u but version %u was expected", pathToFile, fileVersion, S3D_TEXTURE_FILE_VERSION);
                assert(0);
            }
        }
        else
        {
            S3D_DBGPRINT("Error: File %s is not a Synesthesia3D texture file", pathToFile);
            assert(0);
        }

        texFile.close();
    }

    return texIdx;
}

const unsigned int ResourceManager::CreateModel(const char* pathToFile)
{
    unsigned int modelIdx = ~0u;
    std::ifstream modelFile;
    modelFile.open(pathToFile, std::ios::binary);

    if (modelFile.is_open())
    {
        char fileSignature[S3D_MODEL_FILE_HEADER_SIZE];
        modelFile.read(fileSignature, S3D_MODEL_FILE_HEADER_SIZE);

        if (memcmp(S3D_MODEL_FILE_HEADER, fileSignature, S3D_MODEL_FILE_HEADER_SIZE) == 0)
        {
            unsigned int fileVersion = 0;
            modelFile.read((char*)&fileVersion, sizeof(unsigned int));

            if (fileVersion == S3D_MODEL_FILE_VERSION)
            {
                unsigned int compressedBufferSize = 0, decompressedBufferSize = 0;
                modelFile.read((char*)&compressedBufferSize, sizeof(unsigned int));
                modelFile.read((char*)&decompressedBufferSize, sizeof(unsigned int));

                if (compressedBufferSize > 0 && compressedBufferSize <= LZ4_COMPRESSBOUND(LZ4_MAX_INPUT_SIZE) &&
                    decompressedBufferSize > 0 && decompressedBufferSize <= LZ4_MAX_INPUT_SIZE)
                {
                    char* const compressedBuffer = new char[compressedBufferSize];
                    char* const decompressedBuffer = new char[decompressedBufferSize];
                    modelFile.read(compressedBuffer, compressedBufferSize);
                    const int readBytes = LZ4_decompress_fast(compressedBuffer, decompressedBuffer, decompressedBufferSize);

                    if (readBytes == compressedBufferSize)
                    {
                        imemstream  modelBuffer(decompressedBuffer, decompressedBufferSize);
                        Model* const mdl = new Model;
                        mdl->szSourceFile = pathToFile;
                        MUTEX_LOCK(ModelMutex);
                        m_arrModel.push_back(mdl);
                        modelIdx = (unsigned int)m_arrModel.size() - 1;
                        MUTEX_UNLOCK(ModelMutex);
                        modelBuffer >> *mdl;
                    }
                    else
                    {
                        S3D_DBGPRINT("Error: Model %s could not be decompressed", pathToFile);
                        assert(0);
                    }

                    delete[] compressedBuffer;
                    delete[] decompressedBuffer;
                }
                else
                {
                    S3D_DBGPRINT("Error: Model %s has invalid data size", pathToFile);
                    assert(0);
                }
            }
            else
            {
                S3D_DBGPRINT("Error: Model %s is version %u but version %u was expected", pathToFile, fileVersion, S3D_MODEL_FILE_VERSION);
                assert(0);
            }
        }
        else
        {
            S3D_DBGPRINT("Error: File %s is not a Synesthesia3D model file", pathToFile);
            assert(0);
        }

        modelFile.close();
    }

    return modelIdx;
}

const unsigned int ResourceManager::FindTexture(const char * pathToFile, const bool strict)
{
    //MUTEX_LOCK(TexMutex);
    for (unsigned int i = 0; i < m_arrTexture.size(); i++)
        if (m_arrTexture[i] && m_arrTexture[i]->m_szSourceFile == pathToFile)
        {
            //MUTEX_UNLOCK(TexMutex);
            return i;
        }

    if (!strict)
        for (unsigned int i = 0; i < m_arrTexture.size(); i++)
            if (m_arrTexture[i] && m_arrTexture[i]->m_szSourceFile.find(pathToFile) != std::string::npos)
            {
                //MUTEX_UNLOCK(TexMutex);
                return i;
            }

    //MUTEX_UNLOCK(TexMutex);
    return ~0u;
}

const unsigned int ResourceManager::FindModel(const char * pathToFile, const bool strict)
{
    MUTEX_LOCK(ModelMutex);
    for (unsigned int i = 0; i < m_arrModel.size(); i++)
        if (m_arrModel[i] && m_arrModel[i]->szSourceFile == pathToFile)
        {
            MUTEX_UNLOCK(ModelMutex);
            return i;
        }

    if(!strict)
        for (unsigned int i = 0; i < m_arrModel.size(); i++)
            if (m_arrModel[i] && m_arrModel[i]->szSourceFile.find(pathToFile) != std::string::npos)
            {
                MUTEX_UNLOCK(ModelMutex);
                return i;
            }

    MUTEX_UNLOCK(ModelMutex);
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
    assert(idx < m_arrVertexFormat.size());
    if (idx >= m_arrVertexFormat.size())
        return;

    delete m_arrVertexFormat[idx];
    m_arrVertexFormat[idx] = nullptr;
}

void ResourceManager::ReleaseIndexBuffer(const unsigned int idx)
{
    assert(idx < m_arrIndexBuffer.size());
    if (idx >= m_arrIndexBuffer.size())
        return;

    delete m_arrIndexBuffer[idx];
    m_arrIndexBuffer[idx] = nullptr;
}

void ResourceManager::ReleaseVertexBuffer(const unsigned int idx)
{
    assert(idx < m_arrVertexBuffer.size());
    if (idx >= m_arrVertexBuffer.size())
        return;

    delete m_arrVertexBuffer[idx];
    m_arrVertexBuffer[idx] = nullptr;
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
    assert(idx < m_arrShaderProgram.size());
    if (idx >= m_arrShaderProgram.size())
        return;

    delete m_arrShaderProgram[idx];
    m_arrShaderProgram[idx] = nullptr;
}

void ResourceManager::ReleaseTexture(const unsigned int idx)
{
    assert(idx < m_arrTexture.size());
    if (idx >= m_arrTexture.size())
        return;

    delete m_arrTexture[idx];
    m_arrTexture[idx] = nullptr;
}

void ResourceManager::ReleaseRenderTarget(const unsigned int idx)
{
    assert(idx < m_arrRenderTarget.size());
    if (idx >= m_arrRenderTarget.size())
        return;

    delete m_arrRenderTarget[idx];
    m_arrRenderTarget[idx] = nullptr;
}

void ResourceManager::ReleaseModel(const unsigned int idx)
{
    assert(idx < m_arrRenderTarget.size());
    if (idx > m_arrRenderTarget.size())
        return;

    delete m_arrModel[idx];
    m_arrModel[idx] = nullptr;
}
