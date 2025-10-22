/**
 * @file        ShaderProgramNVRHI.cpp
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

#include "ShaderProgramNVRHI.h"
using namespace Synesthesia3D;

ShaderProgramNVRHI::ShaderProgramNVRHI(const ShaderProgramType programType,
    const char* srcData, const char* entryPoint, const char* profile)
    : ShaderProgram(programType)
{
    if (strlen(srcData))
        Compile(srcData, entryPoint);
}

ShaderProgramNVRHI::~ShaderProgramNVRHI()
{
    Unbind();
}

void ShaderProgramNVRHI::Enable(ShaderInput* const shaderInput)
{
    ShaderProgram::Enable(shaderInput);
}

void ShaderProgramNVRHI::Disable()
{
    ShaderProgram::Disable();
}

const bool ShaderProgramNVRHI::Compile(const char* filePath, const char* entryPoint)
{
    return ShaderProgram::Compile(filePath, entryPoint);
}

const unsigned int ShaderProgramNVRHI::GetConstantCount() const
{
    return 0u;
}

const char* ShaderProgramNVRHI::GetConstantName(const unsigned int handle) const
{
    return "";
}

const InputType ShaderProgramNVRHI::GetConstantType(const unsigned int handle) const
{
    return IT_NONE;
}

const RegisterType ShaderProgramNVRHI::GetConstantRegisterType(const unsigned int handle) const
{
    return RT_NONE;
}

const unsigned int ShaderProgramNVRHI::GetConstantRegisterIndex(const unsigned int handle) const
{
    return 0u;
}

const unsigned int ShaderProgramNVRHI::GetConstantRegisterCount(const unsigned int handle) const
{
    return 0u;
}

const unsigned int ShaderProgramNVRHI::GetConstantRowCount(const unsigned int handle) const
{
    return 0u;
}
const unsigned int ShaderProgramNVRHI::GetConstantColumnCount(const unsigned int handle) const
{
    return 0u;
}

const unsigned int ShaderProgramNVRHI::GetConstantArrayElementCount(const unsigned int handle) const
{
    return 0u;
}

const unsigned int ShaderProgramNVRHI::GetConstantStructMemberCount(const unsigned int handle) const
{
    return 0u;
}

const unsigned int ShaderProgramNVRHI::GetConstantSizeBytes(const unsigned int handle) const
{
    return 0u;
}

void ShaderProgramNVRHI::SetFloat(const unsigned int registerIndex, const float* const data, const unsigned int registerCount)
{

}


void ShaderProgramNVRHI::SetInt(const unsigned int registerIndex, const int* const data, const unsigned int registerCount)
{

}

void ShaderProgramNVRHI::SetBool(const unsigned int registerIndex, const bool* const data, const unsigned int registerCount)
{

}

void ShaderProgramNVRHI::SetTexture(const unsigned int registerIndex, const Texture* const tex)
{

}

void ShaderProgramNVRHI::Bind()
{
    if (m_szSrcFile.length())
        Compile(m_szSrcFile.c_str(), m_szEntryPoint.c_str());
}

void ShaderProgramNVRHI::Unbind()
{

}
