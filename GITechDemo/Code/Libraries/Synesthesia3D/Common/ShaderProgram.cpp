/**
 *	@file		ShaderProgram.cpp
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#include "ShaderProgram.h"
using namespace Synesthesia3D;

ShaderProgram::ShaderProgram(const ShaderProgramType programType)
	: m_eProgramType(programType)
{}

ShaderProgram::~ShaderProgram()
{}

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

const char* const ShaderProgram::GetProfile() const
{
	return m_szProfile.c_str();
}
