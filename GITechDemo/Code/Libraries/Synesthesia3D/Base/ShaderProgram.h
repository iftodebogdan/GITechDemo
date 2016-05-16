/**
 *	@file		ShaderProgram.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2016 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "ResourceData.h"

namespace Synesthesia3D
{
	class ShaderProgram;
	class ShaderInput;
	class Texture;

	/**
	 * @brief	Holds the actual shader binary and allows for pairing with a @ref ShaderInput object for rendering.
	 */
	class ShaderProgram
	{

	public:

		/**
		 * @brief	Binds the shader inputs and sets the shader to an active state.
		 *
		 * @param[in]	shaderInput		Pointer to a @ref ShaderInput object matching this shader binary.
		 */
		virtual	SYNESTHESIA3D_DLL void Enable(ShaderInput* const shaderInput);

		/**
		 * @brief	Binds the shader inputs and sets the shader to an active state.
		 *
		 * @param[in]	shaderInput		Reference to a @ref ShaderInput object matching this shader binary.
		 */
		virtual	SYNESTHESIA3D_DLL void Enable(ShaderInput& shaderInput);

		/**
		 * @brief	Sets the shader to an inactive state.
		 */
		virtual	SYNESTHESIA3D_DLL void Disable();

		/**
		 * @brief	Retrieves the constant table.
		 */
				SYNESTHESIA3D_DLL const std::vector<ShaderInputDesc> GetConstantTable();
		
		/**
		 * @brief	Retrieves the shader type.
		 */
				SYNESTHESIA3D_DLL const ShaderProgramType	GetProgramType() const;
		
		/**
		 * @brief	Retrieves the source file from which the shader was compiled.
		 */
				SYNESTHESIA3D_DLL const char* const GetFilePath() const;
		
		/**
		 * @brief	Retrieves the compilation errors.
		 */
				SYNESTHESIA3D_DLL const char* const GetCompilationErrors() const;
		
		/**
		 * @brief	Retrieves the entry point function name.
		 */
				SYNESTHESIA3D_DLL const char* const GetEntryPoint() const;

	protected:
		
		/**
		 * @brief	Constructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::CreateShaderProgram()
		 *
		 * @param[in]	programType		Shader type.
		 */
		ShaderProgram(const ShaderProgramType programType);
		
		/**
		 * @brief	Destructor.
		 *
		 * @details	Meant to be used only by @ref ResourceManager.
		 * @see		ResourceManager::ReleaseShaderProgram()
		 */
		virtual ~ShaderProgram();

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
		
		/**
		 * @brief	Compiles the shader source.
		 */
		virtual const bool Compile(const char* filePath, const char* entryPoint = "");
		
		/**
		 * @brief	Creates the platform specific resource.
		 */
		virtual void Bind() PURE_VIRTUAL;
		
		/**
		 * @brief	Destroys the platform specific resource.
		 */
		virtual void Unbind() PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the number of shader constants.
		 *
		 * @return	Number of shader constants.
		 */
		virtual const unsigned int GetConstantCount() const PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the name of a shader constant.
		 *
		 * @param[in]	handle	Shader constant handle.
		 *
		 * @return	Name of specified shader constant.
		 */
		virtual const char* GetConstantName(const unsigned int handle) const PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the data type of a shader constant.
		 *
		 * @param[in]	handle	Shader constant handle.
		 *
		 * @return	Data type of specified shader constant.
		 */
		virtual const InputType GetConstantType(const unsigned int handle) const PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the destination register data type of a shader constant.
		 *
		 * @param[in]	handle	Shader constant handle.
		 *
		 * @return	Register type of specified shader constant.
		 */
		virtual const RegisterType GetConstantRegisterType(const unsigned int handle) const PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the destination register index of a shader constant.
		 *
		 * @param[in]	handle	Shader constant handle.
		 *
		 * @return	Register index of specified shader constant.
		 */
		virtual const unsigned int GetConstantRegisterIndex(const unsigned int handle) const PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the number of occupied registers for a shader constant.
		 *
		 * @param[in]	handle	Shader constant handle.
		 *
		 * @return	Number of registers used up by specified shader constant.
		 */
		virtual const unsigned int GetConstantRegisterCount(const unsigned int handle) const PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the number of rows for a matrix shader constant.
		 *
		 * @param[in]	handle	Shader constant handle.
		 *
		 * @return	Number of rows for specified matrix shader constant.
		 */
		virtual const unsigned int GetConstantRowCount(const unsigned int handle) const PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the number of columns for a matrix shader constant.
		 *
		 * @param[in]	handle	Shader constant handle.
		 *
		 * @return	Number of columns for specified matrix shader constant.
		 */
		virtual const unsigned int GetConstantColumnCount(const unsigned int handle) const PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the number of elements in an array shader constant.
		 *
		 * @param[in]	handle	Shader constant handle.
		 *
		 * @return	Number of elements in specified array shader constant.
		 */
		virtual const unsigned int GetConstantArrayElementCount(const unsigned int handle) const PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the number of structure members of a shader constant.
		 *
		 * @param[in]	handle	Shader constant handle.
		 *
		 * @return	Number of structure members of specified shader constant.
		 */
		virtual const unsigned int GetConstantStructMemberCount(const unsigned int handle) const PURE_VIRTUAL;
		
		/**
		 * @brief	Retrieves the size in bytes of a shader constant.
		 *
		 * @param[in]	handle	Shader constant handle.
		 *
		 * @return	Size, in bytes, of specified shader constant.
		 */
		virtual const unsigned int GetConstantSizeBytes(const unsigned int handle) const PURE_VIRTUAL;
		
		/**
		 * @brief	Sets an array of arbitrary data type values to the registers.
		 *
		 * @param[in]	registerType	Data type of the destination register.
		 * @param[in]	registerIndex	Index of the destination register.
		 * @param[in]	data			Pointer to data with which to fill the registers.
		 * @param[in]	registerCount	Number of registers to fill with data, starting with the destination register.
		 */
		void SetValue(const RegisterType registerType, const unsigned int registerIndex, const void* const data, const unsigned int registerCount);
		
		/**
		 * @brief	Sets an array of float values to the registers.
		 *
		 * @param[in]	registerIndex	Index of the destination register.
		 * @param[in]	data			Pointer to an array of floats with which to fill the registers.
		 * @param[in]	registerCount	Number of registers to fill with data, starting with the destination register.
		 */
		virtual void SetFloat(const unsigned int registerIndex, const float* const data, const unsigned int registerCount) PURE_VIRTUAL;
		
		/**
		 * @brief	Sets an array of integers values to the registers.
		 *
		 * @param[in]	registerIndex	Index of the destination register.
		 * @param[in]	data			Pointer to an array of integers with which to fill the registers.
		 * @param[in]	registerCount	Number of registers to fill with data, starting with the destination register.
		 */
		virtual void SetInt(const unsigned int registerIndex, const int* const data, const unsigned int registerCount) PURE_VIRTUAL;
		
		/**
		 * @brief	Sets an array of bool values to the registers.
		 *
		 * @param[in]	registerIndex	Index of the destination register.
		 * @param[in]	data			Pointer to an array of bools with which to fill the registers.
		 * @param[in]	registerCount	Number of registers to fill with data, starting with the destination register.
		 */
		virtual void SetBool(const unsigned int registerIndex, const bool* const data, const unsigned int registerCount) PURE_VIRTUAL;
		
		/**
		 * @brief	Sets a texture to a register.
		 *
		 * @param[in]	registerIndex	Index of the destination register.
		 * @param[in]	tex				Pointer to the texture.
		 */
		virtual void SetTexture(const unsigned int registerIndex, const Texture* const tex) PURE_VIRTUAL;

		ShaderProgramType m_eProgramType;				/**< @brief Shader type. */
		std::string m_szSrcFile;						/**< @brief File containing shader source code. */
		std::string m_szErrors;							/**< @brief Compilation errors. */
		std::string m_szEntryPoint;						/**< @brief Shader entry point function. */
		std::vector<ShaderInputDesc> m_arrInputDesc;	/**< @brief An array containing metadata regarding each shader input. */
		ShaderInput* m_pShaderInput;					/**< @brief A pointer to the currently active shader input */

		friend class ShaderInput;
		friend class ResourceManager;
	};
}

#endif // SHADERPROGRAM_H