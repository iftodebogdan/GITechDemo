/**
 *	@file		ShaderTemplate.h
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

#ifndef SHADERTEMPLATE_H
#define SHADERTEMPLATE_H

#include "ResourceData.h"

namespace Synesthesia3D
{
	class ShaderProgram;
	class ShaderInput;

	/**
	 * @brief	The bridge between a shader program and its inputs.
	 */
	class ShaderTemplate
	{

	public:

		/**
		 * @brief	Binds the shader inputs and sets the shader to an active state.
		 *
		 * @param[in]	shaderInput		Pointer to a @ref ShaderInput object matching this shader binary.
		 */
		SYNESTHESIA3D_DLL void Enable(ShaderInput* const shaderInput);

		/**
		 * @brief	Binds the shader inputs and sets the shader to an active state.
		 *
		 * @param[in]	shaderInput		Reference to a @ref ShaderInput object matching this shader binary.
		 */
		SYNESTHESIA3D_DLL void Enable(ShaderInput& shaderInput);

		/**
		 * @brief	Sets the shader to an inactive state.
		 */
		SYNESTHESIA3D_DLL void Disable();

		/**
		 * @brief	Retrieves the constant table.
		 */
		SYNESTHESIA3D_DLL	const std::vector<ShaderInputDesc> GetConstantTable();
		
	protected:
		
		/**
		 * @brief	Constructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::CreateShaderTemplate()
		 *
		 * @param[in]	shaderProgram		Pointer to the object containing the shader binary.
		 */
		ShaderTemplate(ShaderProgram* const shaderProgram);
		
		/**
		 * @brief	Destructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::ReleaseShaderTemplate()
		 */
		virtual ~ShaderTemplate();

		/**
		 * @brief	Populate the @ref ShaderInputDesc array.
		 */
		void DescribeShaderInputs();

		/**
		 * @brief	Retrieves the total number of registers used by the shader program.
		 */
		const unsigned int GetTotalNumberOfUsedRegisters() const;

		/**
		 * @brief	Retrieves the total size, in bytes, of shader inputs.
		 */
		const unsigned int GetTotalSizeOfInputConstants() const;

		ShaderProgram* m_pProgram;	/**< @brief Pointer to the matching shader program object. */
		std::vector<ShaderInputDesc> m_arrInputDesc;	/**< @brief An array containing metadata regarding each shader input. */
		ShaderInput* m_pShaderInput;	/**< @brief A pointer to the currently active shader input */

		friend class ShaderInput;
		friend class ResourceManager;
	};
}

#endif // SHADERTEMPLATE_H