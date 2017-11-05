/**
 * @file        ShaderProgram.cpp
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

#include "Renderer.h"
#include "SamplerState.h"
#include "ShaderProgram.h"
#include "ShaderInput.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "Profiler.h"
using namespace Synesthesia3D;

#include "Utility/Hash.h"

ShaderProgram::ShaderProgram(const ShaderProgramType programType)
    : m_eProgramType(programType)
    , m_pShaderInput(nullptr)
{
    assert(programType > SPT_NONE && programType < SPT_MAX);
}

ShaderProgram::~ShaderProgram()
{}

void ShaderProgram::DescribeShaderInputs()
{
    unsigned int paramCount = GetConstantCount();   
    unsigned int offset = 0;
    for (unsigned int i = 0; i < paramCount; i++)
    {
        ShaderInputDesc inputDesc;
        inputDesc.szName = GetConstantName(i);
        inputDesc.nNameHash = S3DHASH(inputDesc.szName.c_str());
        inputDesc.eInputType = GetConstantType(i);
        inputDesc.eRegisterType = GetConstantRegisterType(i);
        inputDesc.nRegisterIndex = GetConstantRegisterIndex(i);
        inputDesc.nRegisterCount = GetConstantRegisterCount(i);
        inputDesc.nRows = GetConstantRowCount(i);
        inputDesc.nColumns = GetConstantColumnCount(i);
        inputDesc.nArrayElements = GetConstantArrayElementCount(i);
        inputDesc.nBytes = GetConstantSizeBytes(i);
        inputDesc.nOffsetInBytes = offset;
        offset += inputDesc.nBytes; //inputDesc.nRegisterCount * sizeof(float) * 4u;

        m_arrInputDesc.push_back(inputDesc);
    }
}

const unsigned int ShaderProgram::GetTotalNumberOfUsedRegisters() const
{
    unsigned int ret = 0;
    for (unsigned int i = 0, n = (unsigned int)m_arrInputDesc.size(); i < n; i++)
    {
        ret += m_arrInputDesc[i].nRegisterCount;
    }

    return ret;
}

const unsigned int ShaderProgram::GetTotalSizeOfInputConstants() const
{
    unsigned int ret = 0;
    for (unsigned int i = 0, n = (unsigned int)m_arrInputDesc.size(); i < n; i++)
    {
        ret += m_arrInputDesc[i].nBytes;
    }

    return ret;
}

void ShaderProgram::Enable(ShaderInput* const shaderInput)
{
    assert(m_pShaderInput == nullptr);
    //assert((shaderInput && m_arrInputDesc.size()) || (!shaderInput && !m_arrInputDesc.size()) || (Renderer::GetAPI() == API_NULL));

    m_pShaderInput = shaderInput;

    for (unsigned int i = 0, n = (unsigned int)m_arrInputDesc.size(); i < n; i++)
    {
        PUSH_PROFILE_MARKER(m_arrInputDesc[i].szName.c_str());

        if (m_arrInputDesc[i].eInputType >= IT_BOOL && m_arrInputDesc[i].eInputType <= IT_FLOAT)
        {
            SetValue(
                m_arrInputDesc[i].eRegisterType,
                m_arrInputDesc[i].nRegisterIndex,
                shaderInput->GetData() + m_arrInputDesc[i].nOffsetInBytes,
                m_arrInputDesc[i].nRegisterCount
            );
        }
        else
        {
            if (m_arrInputDesc[i].eInputType >= IT_SAMPLER && m_arrInputDesc[i].eInputType <= IT_SAMPLERCUBE)
            {
                const unsigned int texIdx = *(unsigned int*)(shaderInput->GetData() + m_arrInputDesc[i].nOffsetInBytes);
                const Texture* const tex = texIdx != ~0u ? Renderer::GetInstance()->GetResourceManager()->GetTexture(texIdx) : nullptr;

                if (tex)
                {
                    if(strlen(tex->GetSourceFileName()))
                        PUSH_PROFILE_MARKER(tex->GetSourceFileName());

                    tex->Enable(m_arrInputDesc[i].nRegisterIndex);
                    SamplerState* ssm = Renderer::GetInstance()->GetSamplerStateManager();
                    ssm->SetAnisotropy(m_arrInputDesc[i].nRegisterIndex, tex->GetAnisotropy());
                    ssm->SetMipLodBias(m_arrInputDesc[i].nRegisterIndex, tex->GetMipLodBias());
                    ssm->SetFilter(m_arrInputDesc[i].nRegisterIndex, tex->GetFilter());
                    ssm->SetBorderColor(m_arrInputDesc[i].nRegisterIndex, tex->GetBorderColor());
                    ssm->SetAddressingModeU(m_arrInputDesc[i].nRegisterIndex, tex->GetAddressingModeU());
                    ssm->SetAddressingModeV(m_arrInputDesc[i].nRegisterIndex, tex->GetAddressingModeV());
                    ssm->SetAddressingModeW(m_arrInputDesc[i].nRegisterIndex, tex->GetAddressingModeW());
                    ssm->SetSRGBEnabled(m_arrInputDesc[i].nRegisterIndex, tex->GetSRGBEnabled());

                    if (strlen(tex->GetSourceFileName()))
                        POP_PROFILE_MARKER();
                }
            }
            else
                assert(false); // shouldn't happen
        }

        POP_PROFILE_MARKER();
    }
}

void ShaderProgram::Disable()
{
    //assert((m_pShaderInput && m_arrInputDesc.size()) || (!m_pShaderInput && !m_arrInputDesc.size()) || (Renderer::GetAPI() == API_NULL));

    for (unsigned int i = 0, n = (unsigned int)m_arrInputDesc.size(); i < n; i++)
    {
        if (m_arrInputDesc[i].eInputType >= IT_SAMPLER && m_arrInputDesc[i].eInputType <= IT_SAMPLERCUBE)
        {
            const unsigned int texIdx = *(unsigned int*)(m_pShaderInput->GetData() + m_arrInputDesc[i].nOffsetInBytes);
            const Texture* const tex = texIdx != ~0u ? Renderer::GetInstance()->GetResourceManager()->GetTexture(texIdx) : nullptr;
            if (tex)
                tex->Disable(m_arrInputDesc[i].nRegisterIndex);
        }
    }

    m_pShaderInput = nullptr;
}

void ShaderProgram::Enable(ShaderInput& shaderInput)
{
    Enable(&shaderInput);
}

const std::vector<ShaderInputDesc> ShaderProgram::GetConstantTable()
{
    return m_arrInputDesc;
}

void ShaderProgram::SetValue(const RegisterType registerType, const unsigned int registerIndex, const void* const data, const unsigned int registerCount)
{
    switch (registerType)
    {
    case RT_BOOL:
        SetBool(registerIndex, (const bool* const)data, registerCount);
        break;
    case RT_INT4:
        SetInt(registerIndex, (const int* const)data, registerCount);
        break;
    case RT_FLOAT4:
        SetFloat(registerIndex, (const float* const)data, registerCount);
        break;
    default:
        assert(false);
    }
}

const ShaderProgramType ShaderProgram::GetProgramType() const
{
    return m_eProgramType;
}

const char* const ShaderProgram::GetFilePath() const
{
    return m_szSrcFile.c_str();
}

const char* const ShaderProgram::GetCompilationErrors() const
{
    return m_szErrors.c_str();
}

const char* const ShaderProgram::GetEntryPoint() const
{
    return m_szEntryPoint.c_str();
}

const bool ShaderProgram::Compile(const char* filePath, const char* entryPoint)
{
    m_szSrcFile = filePath;
    m_szEntryPoint = entryPoint;

    DescribeShaderInputs();

    return true;
}
