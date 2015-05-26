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
#ifndef SHADERTEMPLATE_H
#define SHADERTEMPLATE_H

#ifndef LIBRENDERER_DLL
#ifdef LIBRENDERER_EXPORTS
#define LIBRENDERER_DLL __declspec(dllexport) 
#else
#define LIBRENDERER_DLL __declspec(dllimport) 
#endif
#endif // LIBRENDERER_DLL

#include "ResourceData.h"

namespace LibRendererDll
{
	class ShaderProgram;
	class ShaderInput;

	// This class is the bridge between a shader program and its inputs
	class ShaderTemplate
	{
	public:
		// Set the shader inputs and set the shader to an active state
		LIBRENDERER_DLL void Enable(ShaderInput* const shaderInput);
		// Set the shader inputs and set the shader to an active state
		LIBRENDERER_DLL void Enable(ShaderInput& shaderInput);
		// Disable the shader
		LIBRENDERER_DLL void Disable();

		// Get the constant table
		LIBRENDERER_DLL	const std::vector<ShaderInputDesc> GetConstantTable();
		
	protected:
		ShaderTemplate(ShaderProgram* const shaderProgram);
		virtual ~ShaderTemplate();

		void DescribeShaderInputs();
		const unsigned int GetTotalNumberOfUsedRegisters() const;
		const unsigned int GetTotalSizeOfInputConstants() const;

		ShaderProgram* m_pProgram;
		std::vector<ShaderInputDesc> m_arrInputDesc;
		ShaderInput* m_pShaderInput;

		friend class ShaderInput;
		friend class ResourceManager;
	};
}

#endif // SHADERTEMPLATE_H