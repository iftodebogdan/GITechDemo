//////////////////////////////////////////////////////////////////////////
// This file is part of the "LibRenderer" 3D graphics library           //
//                                                                      //
// Copyright (C) 2014 - Iftode Bogdan-Marius <iftode.bogdan@gmail.com>  //
//                                                                      //
// This program is free software: you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation: either version 3 of the License: or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful:      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program. If not: see <http://www.gnu.org/licenses/>. //
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "ShaderProgramNULL.h"
using namespace LibRendererDll;

ShaderProgramNULL::ShaderProgramNULL(const ShaderProgramType programType,
	const char* srcData, const char* entryPoint, const char* profile)
	: ShaderProgram(programType)
{}

ShaderProgramNULL::~ShaderProgramNULL()
{}

void ShaderProgramNULL::Enable()
{}

void ShaderProgramNULL::Disable()
{}

const bool ShaderProgramNULL::Compile(const char* srcData, char* const errors, const char* entryPoint, const char* profile)
{
	return true;
}

const unsigned int ShaderProgramNULL::GetConstantCount() const
{
	return 0;
}

const char* ShaderProgramNULL::GetConstantName(const unsigned int handle) const
{
	return "";
}

const InputType ShaderProgramNULL::GetConstantType(const unsigned int handle) const
{
	return IT_NONE;
}

const RegisterType ShaderProgramNULL::GetConstantRegisterType(const unsigned int handle) const
{
	return RT_NONE;
}

const unsigned int ShaderProgramNULL::GetConstantRegisterIndex(const unsigned int handle) const
{
	return 0;
}

const unsigned int ShaderProgramNULL::GetConstantRegisterCount(const unsigned int handle) const
{
	return 0;
}

const unsigned int ShaderProgramNULL::GetConstantRowCount(const unsigned int handle) const
{
	return 0;
}
const unsigned int ShaderProgramNULL::GetConstantColumnCount(const unsigned int handle) const
{
	return 0;
}

const unsigned int ShaderProgramNULL::GetConstantArrayElementCount(const unsigned int handle) const
{
	return 0;
}

const unsigned int ShaderProgramNULL::GetConstantStructMemberCount(const unsigned int handle) const
{
	return 0;
}

const unsigned int ShaderProgramNULL::GetConstantSizeBytes(const unsigned int handle) const
{
	return 0;
}

void ShaderProgramNULL::SetFloat(const unsigned int registerIndex, const float* const data, const unsigned int registerCount)
{}


void ShaderProgramNULL::SetInt(const unsigned int registerIndex, const int* const data, const unsigned int registerCount)
{}

void ShaderProgramNULL::SetBool(const unsigned int registerIndex, const bool* const data, const unsigned int registerCount)
{}

void ShaderProgramNULL::SetTexture(const unsigned int registerIndex, const Texture* const tex)
{}

void ShaderProgramNULL::Bind()
{}

void ShaderProgramNULL::Unbind()
{}