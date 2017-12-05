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
    MUTEX_INIT(VFMutex);
    MUTEX_INIT(IBMutex);
    MUTEX_INIT(VBMutex);
    MUTEX_INIT(ShdInMutex);
    MUTEX_INIT(ShdProgMutex);
    MUTEX_INIT(TexMutex);
    MUTEX_INIT(RTMutex);
    MUTEX_INIT(ModelMutex);
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

    MUTEX_DESTROY(VFMutex);
    MUTEX_DESTROY(IBMutex);
    MUTEX_DESTROY(VBMutex);
    MUTEX_DESTROY(ShdInMutex);
    MUTEX_DESTROY(ShdProgMutex);
    MUTEX_DESTROY(TexMutex);
    MUTEX_DESTROY(RTMutex);
    MUTEX_DESTROY(ModelMutex);
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
    return AddShaderInput(shdIn);
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
                        modelIdx = AddModel(mdl);
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
    //MUTEX_LOCK(ModelMutex);
    for (unsigned int i = 0; i < m_arrModel.size(); i++)
        if (m_arrModel[i] && m_arrModel[i]->szSourceFile == pathToFile)
        {
            //MUTEX_UNLOCK(ModelMutex);
            return i;
        }

    if(!strict)
        for (unsigned int i = 0; i < m_arrModel.size(); i++)
            if (m_arrModel[i] && m_arrModel[i]->szSourceFile.find(pathToFile) != std::string::npos)
            {
                //MUTEX_UNLOCK(ModelMutex);
                return i;
            }

    //MUTEX_UNLOCK(ModelMutex);
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

    MUTEX_LOCK(VFMutex);
    m_arrVertexFormatFreeSlots.push_back(idx);
    MUTEX_UNLOCK(VFMutex);
}

void ResourceManager::ReleaseIndexBuffer(const unsigned int idx)
{
    assert(idx < m_arrIndexBuffer.size());
    if (idx >= m_arrIndexBuffer.size())
        return;

    delete m_arrIndexBuffer[idx];
    m_arrIndexBuffer[idx] = nullptr;

    MUTEX_LOCK(IBMutex);
    m_arrIndexBufferFreeSlots.push_back(idx);
    MUTEX_UNLOCK(IBMutex);
}

void ResourceManager::ReleaseVertexBuffer(const unsigned int idx)
{
    assert(idx < m_arrVertexBuffer.size());
    if (idx >= m_arrVertexBuffer.size())
        return;

    delete m_arrVertexBuffer[idx];
    m_arrVertexBuffer[idx] = nullptr;

    MUTEX_LOCK(VBMutex);
    m_arrVertexBufferFreeSlots.push_back(idx);
    MUTEX_UNLOCK(VBMutex);
}

void ResourceManager::ReleaseShaderInput(const unsigned int idx)
{
    assert(idx < m_arrShaderInput.size());
    if (idx >= m_arrShaderInput.size())
        return;

    delete m_arrShaderInput[idx];
    m_arrShaderInput[idx] = nullptr;

    MUTEX_LOCK(ShdInMutex);
    m_arrShaderInputFreeSlots.push_back(idx);
    MUTEX_UNLOCK(ShdInMutex);
}

void ResourceManager::ReleaseShaderProgram(const unsigned int idx)
{
    assert(idx < m_arrShaderProgram.size());
    if (idx >= m_arrShaderProgram.size())
        return;

    delete m_arrShaderProgram[idx];
    m_arrShaderProgram[idx] = nullptr;

    MUTEX_LOCK(ShdProgMutex);
    m_arrShaderProgramFreeSlots.push_back(idx);
    MUTEX_UNLOCK(ShdProgMutex);
}

void ResourceManager::ReleaseTexture(const unsigned int idx)
{
    assert(idx < m_arrTexture.size());
    if (idx >= m_arrTexture.size())
        return;

    delete m_arrTexture[idx];
    m_arrTexture[idx] = nullptr;

    MUTEX_LOCK(TexMutex);
    m_arrTextureFreeSlots.push_back(idx);
    MUTEX_UNLOCK(TexMutex);
}

void ResourceManager::ReleaseRenderTarget(const unsigned int idx)
{
    assert(idx < m_arrRenderTarget.size());
    if (idx >= m_arrRenderTarget.size())
        return;

    delete m_arrRenderTarget[idx];
    m_arrRenderTarget[idx] = nullptr;

    MUTEX_LOCK(RTMutex);
    m_arrRenderTargetFreeSlots.push_back(idx);
    MUTEX_UNLOCK(RTMutex);
}

void ResourceManager::ReleaseModel(const unsigned int idx)
{
    assert(idx < m_arrRenderTarget.size());
    if (idx > m_arrRenderTarget.size())
        return;

    delete m_arrModel[idx];
    m_arrModel[idx] = nullptr;

    MUTEX_LOCK(ModelMutex);
    m_arrModelFreeSlots.push_back(idx);
    MUTEX_UNLOCK(ModelMutex);
}

const unsigned int ResourceManager::AddVertexFormat(VertexFormat* vf)
{
    unsigned int idx = ~0u;

    MUTEX_LOCK(VFMutex);
    if (!m_arrVertexFormatFreeSlots.empty())
    {
        idx = m_arrVertexFormatFreeSlots.front();
        m_arrVertexFormat[idx] = vf;
        m_arrVertexFormatFreeSlots.erase(m_arrVertexFormatFreeSlots.begin());
    }
    else
    {
        m_arrVertexFormat.push_back(vf);
        idx = (unsigned int)m_arrVertexFormat.size() - 1;
    }
    MUTEX_UNLOCK(VFMutex);

    return idx;
}

const unsigned int ResourceManager::AddIndexBuffer(IndexBuffer* ib)
{
    unsigned int idx = ~0u;

    MUTEX_LOCK(IBMutex);
    if (!m_arrIndexBufferFreeSlots.empty())
    {
        idx = m_arrIndexBufferFreeSlots.front();
        m_arrIndexBuffer[idx] = ib;
        m_arrIndexBufferFreeSlots.erase(m_arrIndexBufferFreeSlots.begin());
    }
    else
    {
        m_arrIndexBuffer.push_back(ib);
        idx = (unsigned int)m_arrIndexBuffer.size() - 1;
    }
    MUTEX_UNLOCK(IBMutex);

    return idx;
}

const unsigned int ResourceManager::AddVertexBuffer(VertexBuffer* vb)
{
    unsigned int idx = ~0u;

    MUTEX_LOCK(VBMutex);
    if (!m_arrVertexBufferFreeSlots.empty())
    {
        idx = m_arrVertexBufferFreeSlots.front();
        m_arrVertexBuffer[idx] = vb;
        m_arrVertexBufferFreeSlots.erase(m_arrVertexBufferFreeSlots.begin());
    }
    else
    {
        m_arrVertexBuffer.push_back(vb);
        idx = (unsigned int)m_arrVertexBuffer.size() - 1;
    }
    MUTEX_UNLOCK(VBMutex);

    return idx;
}

const unsigned int ResourceManager::AddShaderProgram(ShaderProgram* shdProg)
{
    unsigned int idx = ~0u;

    MUTEX_LOCK(ShdProgMutex);
    if (!m_arrShaderProgramFreeSlots.empty())
    {
        idx = m_arrShaderProgramFreeSlots.front();
        m_arrShaderProgram[idx] = shdProg;
        m_arrShaderProgramFreeSlots.erase(m_arrShaderProgramFreeSlots.begin());
    }
    else
    {
        m_arrShaderProgram.push_back(shdProg);
        idx = (unsigned int)m_arrShaderProgram.size() - 1;
    }
    MUTEX_UNLOCK(ShdProgMutex);

    return idx;
}

const unsigned int ResourceManager::AddTexture(Texture* tex)
{
    unsigned int idx = ~0u;

    MUTEX_LOCK(TexMutex);
    if (!m_arrTextureFreeSlots.empty())
    {
        idx = m_arrTextureFreeSlots.front();
        m_arrTexture[idx] = tex;
        m_arrTextureFreeSlots.erase(m_arrTextureFreeSlots.begin());
    }
    else
    {
        m_arrTexture.push_back(tex);
        idx = (unsigned int)m_arrTexture.size() - 1;
    }
    MUTEX_UNLOCK(TexMutex);

    return idx;
}

const unsigned int ResourceManager::AddRenderTarget(RenderTarget* rt)
{
    unsigned int idx = ~0u;

    MUTEX_LOCK(RTMutex);
    if (!m_arrRenderTargetFreeSlots.empty())
    {
        idx = m_arrRenderTargetFreeSlots.front();
        m_arrRenderTarget[idx] = rt;
        m_arrRenderTargetFreeSlots.erase(m_arrRenderTargetFreeSlots.begin());
    }
    else
    {
        m_arrRenderTarget.push_back(rt);
        idx = (unsigned int)m_arrRenderTarget.size() - 1;
    }
    MUTEX_UNLOCK(RTMutex);

    return idx;
}

const unsigned int ResourceManager::AddShaderInput(ShaderInput* shdIn)
{
    unsigned int idx = ~0u;

    MUTEX_LOCK(ShdInMutex);
    if (!m_arrShaderInputFreeSlots.empty())
    {
        idx = m_arrShaderInputFreeSlots.front();
        m_arrShaderInput[idx] = shdIn;
        m_arrShaderInputFreeSlots.erase(m_arrShaderInputFreeSlots.begin());
    }
    else
    {
        m_arrShaderInput.push_back(shdIn);
        idx = (unsigned int)m_arrShaderInput.size() - 1;
    }
    MUTEX_UNLOCK(ShdInMutex);

    return idx;
}

const unsigned int ResourceManager::AddModel(Model* mdl)
{
    unsigned int idx = ~0u;

    MUTEX_LOCK(ModelMutex);
    if (!m_arrModelFreeSlots.empty())
    {
        idx = m_arrModelFreeSlots.front();
        m_arrModel[idx] = mdl;
        m_arrModelFreeSlots.erase(m_arrModelFreeSlots.begin());
    }
    else
    {
        m_arrModel.push_back(mdl);
        idx = (unsigned int)m_arrModel.size() - 1;
    }
    MUTEX_UNLOCK(ModelMutex);

    return idx;
}
