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
#ifndef SHADERPROGRAMNULL_H
#define SHADERPROGRAMNULL_H

#include "ShaderProgram.h"
#include "ShaderTemplate.h"

namespace LibRendererDll
{
	class Texture;

	class ShaderProgramNULL : public ShaderProgram
	{
	private:
		ShaderProgramNULL(const ShaderProgramType programType, const char* srcData = "", const char* entryPoint = "", const char* profile = "")
			: ShaderProgram(programType) {}
		~ShaderProgramNULL() {}

		void Enable() {}
		void Disable() {}
		const bool Compile(const char* srcData, char* const errors, const char* entryPoint = "", const char* profile = "") { return true; }
		
		void Bind() {}
		void Unbind() {}

		const unsigned int GetConstantCount() const { return 0; }
		const char* GetConstantName(const unsigned int handle) const { return ""; }
		const InputType GetConstantType(const unsigned int handle) const { return IT_NONE; }
		const RegisterType GetConstantRegisterType(const unsigned int handle) const { return RT_NONE; }
		const unsigned int GetConstantRegisterIndex(const unsigned int handle) const { return 0; }
		const unsigned int GetConstantRegisterCount(const unsigned int handle) const { return 0; }
		const unsigned int GetConstantRowCount(const unsigned int handle) const { return 0; }
		const unsigned int GetConstantColumnCount(const unsigned int handle) const { return 0; }
		const unsigned int GetConstantArrayElementCount(const unsigned int handle) const { return 0; }
		const unsigned int GetConstantStructMemberCount(const unsigned int handle) const { return 0; }
		const unsigned int GetConstantSizeBytes(const unsigned int handle) const { return 0; }

		void SetFloat(const unsigned int registerIndex, const float* const data, const unsigned int registerCount) {}
		void SetInt(const unsigned int registerIndex, const int* const data, const unsigned int registerCount) {}
		void SetBool(const unsigned int registerIndex, const bool* const data, const unsigned int registerCount) {}
		void SetTexture(const unsigned int registerIndex, const Texture* const tex) {}

		friend class ResourceManagerNULL;
	};
}

#endif // SHADERPROGRAMNULL_H