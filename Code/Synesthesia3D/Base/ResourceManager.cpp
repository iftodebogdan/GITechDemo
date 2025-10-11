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
}

template <typename T>
static void ResourceManager::DeleteResourcesAndClearList(ThreadSafeList<T>& resourceList)
{
    ThreadSafeList<T>::Writer listWriter = resourceList.GetWriter();
    std::vector<T>& list = listWriter.GetList();

    for (unsigned int i = 0; i < list.size(); i++)
        delete list[i];

    list.clear();
}

void ResourceManager::ReleaseAll()
{
    UnbindAll();

    DeleteResourcesAndClearList(m_arrModel);
    DeleteResourcesAndClearList(m_arrVertexFormat);
    DeleteResourcesAndClearList(m_arrIndexBuffer);
    DeleteResourcesAndClearList(m_arrVertexBuffer);
    DeleteResourcesAndClearList(m_arrShaderInput);
    DeleteResourcesAndClearList(m_arrShaderProgram);
    DeleteResourcesAndClearList(m_arrRenderTarget);
    DeleteResourcesAndClearList(m_arrTexture);
}

template <typename T>
static void ResourceManager::BindResourcesInList(ThreadSafeList<T>& resourceList)
{
    ThreadSafeList<T>::Reader listReader = resourceList.GetReader();
    const std::vector<T>& list = listReader.GetList();

    for (unsigned int i = 0; i < list.size(); i++)
        if (list[i])
            list[i]->Bind();
}

void ResourceManager::BindAll()
{
    BindResourcesInList(m_arrVertexFormat);
    BindResourcesInList(m_arrIndexBuffer);
    BindResourcesInList(m_arrVertexBuffer);
    //BindResourcesInList(m_arrShaderProgram);
    BindResourcesInList(m_arrTexture);
    BindResourcesInList(m_arrRenderTarget);
}

template <typename T>
static void ResourceManager::UnbindResourcesInList(ThreadSafeList<T>& resourceList)
{
    ThreadSafeList<T>::Reader listReader = resourceList.GetReader();
    const std::vector<T>& list = listReader.GetList();

    for (unsigned int i = 0; i < list.size(); i++)
        if (list[i])
            list[i]->Unbind();
}

void ResourceManager::UnbindAll()
{
    UnbindResourcesInList(m_arrVertexFormat);
    UnbindResourcesInList(m_arrIndexBuffer);
    UnbindResourcesInList(m_arrVertexBuffer);
    //UnbindResourcesInList(m_arrShaderProgram);
    UnbindResourcesInList(m_arrTexture);
    UnbindResourcesInList(m_arrRenderTarget);

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

                S3D_DBGPRINT("Info: Loading texture %s (compressed size: %.3f KB, decompressed size: %.3f KB, %.2f%% compression ratio)\n",
                    pathToFile, (float)compressedBufferSize / 1024.f, (float)decompressedBufferSize / 1024.f,
                    (float)compressedBufferSize / (float)decompressedBufferSize * 100.f);

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
                        texIdx = CreateTexture(PF_NONE, TT_1D, 0, 0, 0, 0, BU_NONE);
                        GetTexture(texIdx)->m_szSourceFile = pathToFile;
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

                S3D_DBGPRINT("Info: Loading model %s (compressed size: %.3f KB, decompressed size: %.3f KB, %.2f%% compression ratio)\n",
                    pathToFile, (float)compressedBufferSize / 1024.f, (float)decompressedBufferSize / 1024.f,
                    (float)compressedBufferSize / (float)decompressedBufferSize * 100.f);

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

template <typename T>
static const unsigned int ResourceManager::FindByName(const char* pathToFile, const bool strict, ThreadSafeList<T>& resourceList)
{
    ThreadSafeList<T>::Reader listReader = resourceList.GetReader();
    const std::vector<T>& list = listReader.GetList();

    for (unsigned int i = 0; i < list.size(); i++)
        if (list[i] && list[i]->GetSourceFile() == pathToFile)
            return i;

    if (!strict)
        for (unsigned int i = 0; i < list.size(); i++)
            if (list[i] && list[i]->GetSourceFile().find(pathToFile) != std::string::npos)
                return i;

    return ~0u;
}

const unsigned int ResourceManager::FindTexture(const char * pathToFile, const bool strict)
{
    return FindByName(pathToFile, strict, m_arrTexture);
}

const unsigned int ResourceManager::FindModel(const char * pathToFile, const bool strict)
{
    return FindByName(pathToFile, strict, m_arrModel);
}

template <typename T>
static T ResourceManager::GetResourceFromIndex(const unsigned int idx, const ThreadSafeList<T>& resourceList)
{
    ThreadSafeList<T>::Reader listReader = resourceList.GetReader();
    const std::vector<T>& list = listReader.GetList();

    assert(idx < list.size());
    if (idx >= list.size())
        return nullptr;
    return list[idx];
}

VertexFormat* const ResourceManager::GetVertexFormat(const unsigned int idx) const
{
    return GetResourceFromIndex(idx, m_arrVertexFormat);
}

IndexBuffer* const ResourceManager::GetIndexBuffer(const unsigned int idx) const
{
    return GetResourceFromIndex(idx, m_arrIndexBuffer);
}

VertexBuffer* const ResourceManager::GetVertexBuffer(const unsigned int idx) const
{
    return GetResourceFromIndex(idx, m_arrVertexBuffer);
}

ShaderInput* const ResourceManager::GetShaderInput(const unsigned int idx) const
{
    return GetResourceFromIndex(idx, m_arrShaderInput);
}

ShaderProgram* const ResourceManager::GetShaderProgram(const unsigned int idx) const
{
    return GetResourceFromIndex(idx, m_arrShaderProgram);
}

Texture* const ResourceManager::GetTexture(const unsigned int idx) const
{
    return GetResourceFromIndex(idx, m_arrTexture);
}

RenderTarget* const ResourceManager::GetRenderTarget(const unsigned int idx) const
{
    return GetResourceFromIndex(idx, m_arrRenderTarget);
}

Model* const ResourceManager::GetModel(const unsigned int idx) const
{
    return GetResourceFromIndex(idx, m_arrModel);
}

const unsigned int ResourceManager::GetVertexFormatCount() const
{
    return (unsigned int)m_arrVertexFormat.GetReader().GetList().size();
}

const unsigned int ResourceManager::GetIndexBufferCount() const
{
    return (unsigned int)m_arrIndexBuffer.GetReader().GetList().size();
}

const unsigned int ResourceManager::GetVertexBufferCount() const
{
    return (unsigned int)m_arrVertexBuffer.GetReader().GetList().size();
}

const unsigned int ResourceManager::GetShaderInputCount() const
{
    return (unsigned int)m_arrShaderInput.GetReader().GetList().size();
}

const unsigned int ResourceManager::GetShaderProgramCount() const
{
    return (unsigned int)m_arrShaderProgram.GetReader().GetList().size();
}

const unsigned int ResourceManager::GetTextureCount() const
{
    return (unsigned int)m_arrTexture.GetReader().GetList().size();
}

const unsigned int ResourceManager::GetRenderTargetCount() const
{
    return (unsigned int)m_arrRenderTarget.GetReader().GetList().size();
}

const unsigned int ResourceManager::GetModelCount() const
{
    return (unsigned int)m_arrModel.GetReader().GetList().size();
}

template <typename T>
static void ResourceManager::ReleaseResourceByIndex(const unsigned int idx, ThreadSafeList<T>& resourceList, ThreadSafeList<unsigned int>& freeSlotsList)
{
    ThreadSafeList<T>::Writer listWriter = resourceList.GetWriter();
    std::vector<T>& list = listWriter.GetList();

    assert(idx < list.size());
    if (idx >= list.size())
        return;

    delete list[idx];
    list[idx] = nullptr;

    freeSlotsList.GetWriter().GetList().push_back(idx);
}

void ResourceManager::ReleaseVertexFormat(const unsigned int idx)
{
    ReleaseResourceByIndex(idx, m_arrVertexFormat, m_arrVertexFormatFreeSlots);
}

void ResourceManager::ReleaseIndexBuffer(const unsigned int idx)
{
    ReleaseResourceByIndex(idx, m_arrIndexBuffer, m_arrIndexBufferFreeSlots);
}

void ResourceManager::ReleaseVertexBuffer(const unsigned int idx)
{
    ReleaseResourceByIndex(idx, m_arrVertexBuffer, m_arrVertexBufferFreeSlots);
}

void ResourceManager::ReleaseShaderInput(const unsigned int idx)
{
    ReleaseResourceByIndex(idx, m_arrShaderInput, m_arrShaderInputFreeSlots);
}

void ResourceManager::ReleaseShaderProgram(const unsigned int idx)
{
    ReleaseResourceByIndex(idx, m_arrShaderProgram, m_arrShaderProgramFreeSlots);
}

void ResourceManager::ReleaseTexture(const unsigned int idx)
{
    ReleaseResourceByIndex(idx, m_arrTexture, m_arrTextureFreeSlots);
}

void ResourceManager::ReleaseRenderTarget(const unsigned int idx)
{
    ReleaseResourceByIndex(idx, m_arrRenderTarget, m_arrRenderTargetFreeSlots);
}

void ResourceManager::ReleaseModel(const unsigned int idx)
{
    ReleaseResourceByIndex(idx, m_arrModel, m_arrModelFreeSlots);
}

template <typename T>
static const unsigned int ResourceManager::AddResourceAndGetIndex(T resource, ThreadSafeList<T>& resourceList, ThreadSafeList<unsigned int>& freeSlotsList)
{
    unsigned int idx = ~0u;

    ThreadSafeList<T>::Writer resourceListWriter = resourceList.GetWriter();
    std::vector<T>& resources = resourceListWriter.GetList();

    {
        ThreadSafeList<unsigned int>::Writer freeSlotsListWriter = freeSlotsList.GetWriter();
        std::vector<unsigned int>& freeSlots = freeSlotsListWriter.GetList();

        if (!freeSlots.empty())
        {
            idx = freeSlots.front();
            resources[idx] = resource;
            freeSlots.erase(freeSlots.begin());

            return idx;
        }
    }

    resources.push_back(resource);
    idx = (unsigned int)resources.size() - 1;

    return idx;
}

const unsigned int ResourceManager::AddVertexFormat(VertexFormat* vf)
{
    return AddResourceAndGetIndex(vf, m_arrVertexFormat, m_arrVertexFormatFreeSlots);
}

const unsigned int ResourceManager::AddIndexBuffer(IndexBuffer* ib)
{
    return AddResourceAndGetIndex(ib, m_arrIndexBuffer, m_arrIndexBufferFreeSlots);
}

const unsigned int ResourceManager::AddVertexBuffer(VertexBuffer* vb)
{
    return AddResourceAndGetIndex(vb, m_arrVertexBuffer, m_arrVertexBufferFreeSlots);
}

const unsigned int ResourceManager::AddShaderProgram(ShaderProgram* shdProg)
{
    return AddResourceAndGetIndex(shdProg, m_arrShaderProgram, m_arrShaderProgramFreeSlots);
}

const unsigned int ResourceManager::AddTexture(Texture* tex)
{
    return AddResourceAndGetIndex(tex, m_arrTexture, m_arrTextureFreeSlots);
}

const unsigned int ResourceManager::AddRenderTarget(RenderTarget* rt)
{
    return AddResourceAndGetIndex(rt, m_arrRenderTarget, m_arrRenderTargetFreeSlots);
}

const unsigned int ResourceManager::AddShaderInput(ShaderInput* shdIn)
{
    return AddResourceAndGetIndex(shdIn, m_arrShaderInput, m_arrShaderInputFreeSlots);
}

const unsigned int ResourceManager::AddModel(Model* mdl)
{
    return AddResourceAndGetIndex(mdl, m_arrModel, m_arrModelFreeSlots);
}
