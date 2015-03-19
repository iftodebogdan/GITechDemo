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
#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "ResourceData.h"

namespace LibRendererDll
{
	class Texture;

	// This class holds the actual shader program
	// NB: Currently, all member functions are protected. Use shader templates!
	class ShaderProgram
	{
	protected:
		ShaderProgram(const ShaderProgramType programType);
		virtual ~ShaderProgram();

		virtual void Enable() = 0;
		virtual void Disable() = 0;
		virtual const bool Compile(const char* srcData, char* const errors = nullptr, const char* entryPoint = "", const char* profile = "") = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		const char* const GetSourceCode() const { return m_szSrcData.c_str(); }
		const char* const GetCompilationErrors() const { return m_szErrors.c_str(); }
		const char* const GetEntryPoint() const { return m_szEntryPoint.c_str(); }
		const char* const GetProfile() const { return m_szProfile.c_str(); }

		virtual const unsigned int GetConstantCount() const = 0;
		virtual const char* GetConstantName(const unsigned int handle) const = 0;
		virtual const InputType GetConstantType(const unsigned int handle) const = 0;
		virtual const RegisterType GetConstantRegisterType(const unsigned int handle) const = 0;
		virtual const unsigned int GetConstantRegisterIndex(const unsigned int handle) const = 0;
		virtual const unsigned int GetConstantRegisterCount(const unsigned int handle) const = 0;
		virtual const unsigned int GetConstantRowCount(const unsigned int handle) const = 0;
		virtual const unsigned int GetConstantColumnCount(const unsigned int handle) const = 0;
		virtual const unsigned int GetConstantArrayElementCount(const unsigned int handle) const = 0;
		virtual const unsigned int GetConstantStructMemberCount(const unsigned int handle) const = 0;
		virtual const unsigned int GetConstantSizeBytes(const unsigned int handle) const = 0;

		void SetValue(const RegisterType registerType, const unsigned int registerIndex, const void* const data, const unsigned int registerCount);
		virtual void SetFloat(const unsigned int registerIndex, const float* const data, const unsigned int registerCount) = 0;
		virtual void SetInt(const unsigned int registerIndex, const int* const data, const unsigned int registerCount) = 0;
		virtual void SetBool(const unsigned int registerIndex, const bool* const data, const unsigned int registerCount) = 0;
		virtual void SetTexture(const unsigned int registerIndex, const Texture* const tex) = 0;

		ShaderProgramType m_eProgramType;
		std::string m_szSrcData;
		std::string m_szErrors;
		std::string m_szEntryPoint;
		std::string m_szProfile;

		friend class ShaderTemplate;
		friend class ResourceManager;
	};
}

#endif // SHADERPROGRAM_H