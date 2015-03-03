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
#ifndef SHADERPROGRAMDX9_H
#define SHADERPROGRAMDX9_H

#include "d3dx9.h"
#include "ShaderProgram.h"
#include "ShaderTemplate.h"

namespace LibRendererDll
{
	class Texture;

	class ShaderProgramDX9 : public ShaderProgram
	{
	private:
		ShaderProgramDX9(const ShaderProgramType programType, const char* srcData = "", const char* entryPoint = "", const char* profile = "");
		~ShaderProgramDX9();

		void Enable();
		void Disable();
		const bool Compile(const char* srcData, char* const errors, const char* entryPoint = "", const char* profile = "");
		
		void Bind();
		void Unbind();

		const unsigned int GetConstantCount() const;
		const char* GetConstantName(const unsigned int handle) const;
		const InputType GetConstantType(const unsigned int handle) const;
		const RegisterType GetConstantRegisterType(const unsigned int handle) const;
		const unsigned int GetConstantRegisterIndex(const unsigned int handle) const;
		const unsigned int GetConstantRegisterCount(const unsigned int handle) const;
		const unsigned int GetConstantRowCount(const unsigned int handle) const;
		const unsigned int GetConstantColumnCount(const unsigned int handle) const;
		const unsigned int GetConstantArrayElementCount(const unsigned int handle) const;
		const unsigned int GetConstantStructMemberCount(const unsigned int handle) const;
		const unsigned int GetConstantByteCount(const unsigned int handle) const;

		void SetFloat(const unsigned int registerIndex, const float* const data, const unsigned int registerCount);
		void SetInt(const unsigned int registerIndex, const int* const data, const unsigned int registerCount);
		void SetBool(const unsigned int registerIndex, const bool* const data, const unsigned int registerCount);
		void SetTexture(const unsigned int registerIndex, const Texture* const tex);

		IDirect3DVertexShader9* m_pVertexShader;
		IDirect3DPixelShader9* m_pPixelShader;
		LPD3DXCONSTANTTABLE m_pConstantTable;

		friend class ResourceManagerDX9;
	};
}

#endif // SHADERPROGRAMDX9_H