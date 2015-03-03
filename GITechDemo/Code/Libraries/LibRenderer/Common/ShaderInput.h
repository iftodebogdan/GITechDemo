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
#ifndef SHADERINPUT_H
#define SHADERINPUT_H

#ifndef LIBRENDERER_DLL
#ifdef LIBRENDERER_EXPORTS
#define LIBRENDERER_DLL __declspec(dllexport) 
#else
#define LIBRENDERER_DLL __declspec(dllimport) 
#endif
#endif // LIBRENDERER_DLL

#include <gmtl\gmtl.h>
using namespace gmtl;

#include "Buffer.h"
#include "ShaderTemplate.h"
#include "ResourceData.h"

namespace LibRendererDll
{
	class Texture;

	// This class constains data used by the shader during rendering
	class ShaderInput : public Buffer
	{
	public:
				// Set a texture
				LIBRENDERER_DLL void	SetTexture(const unsigned int handle, const Texture* const tex);

				// Set bools
				LIBRENDERER_DLL void	SetBoolArray(const unsigned int handle, const bool* const data);
				LIBRENDERER_DLL void	SetBool(const unsigned int handle, const bool data) { SetBoolArray(handle, &data); }
				LIBRENDERER_DLL void	SetBool2(const unsigned int handle, const Vec<bool, 2> data) { SetBoolArray(handle, data.getData()); }
				LIBRENDERER_DLL void	SetBool3(const unsigned int handle, const Vec<bool, 3> data) { SetBoolArray(handle, data.getData()); }
				LIBRENDERER_DLL void	SetBool4(const unsigned int handle, const Vec<bool, 4> data) { SetBoolArray(handle, data.getData()); }
				///////////////////////////////////////////////////

				// Set floats
				LIBRENDERER_DLL void	SetFloatArray(const unsigned int handle, const float* const data);
				LIBRENDERER_DLL void	SetFloat(const unsigned int handle, const float data) { SetFloatArray(handle, &data); }
				LIBRENDERER_DLL void	SetFloat2(const unsigned int handle, const Vec2f data) { SetFloatArray(handle, data.getData()); }
				LIBRENDERER_DLL void	SetFloat3(const unsigned int handle, const Vec3f data) { SetFloatArray(handle, data.getData()); }
				LIBRENDERER_DLL void	SetFloat4(const unsigned int handle, const Vec4f data) { SetFloatArray(handle, data.getData()); }
				////////////////////////////////////////////////////

				// Set integers
				LIBRENDERER_DLL void	SetIntArray(const unsigned int handle, const int* const data);
				LIBRENDERER_DLL void	SetInt(const unsigned int handle, const int data) { SetIntArray(handle, &data); }
				LIBRENDERER_DLL void	SetInt2(const unsigned int handle, const Vec2i data) { SetIntArray(handle, data.getData()); }
				LIBRENDERER_DLL void	SetInt3(const unsigned int handle, const Vec3i data) { SetIntArray(handle, data.getData()); }
				LIBRENDERER_DLL void	SetInt4(const unsigned int handle, const Vec4i data) { SetIntArray(handle, data.getData()); }
				/////////////////////////////////////////////////////

		// Set matrices
		template < typename T, const unsigned int ROWS, const unsigned int COLS >
		inline	LIBRENDERER_DLL void	SetMatrix(const unsigned int handle, const Matrix<T, ROWS, COLS> data);
		template < typename T, const unsigned int ROWS, const unsigned int COLS >
		inline	LIBRENDERER_DLL void	SetMatrixArray(const unsigned int handle, const Matrix<T, ROWS, COLS>* const data);
		//////////////////////////////////////////////////////

		// Set float matrices
		template < const unsigned int ROWS, const unsigned int COLS >
		inline	LIBRENDERER_DLL void	SetMatrix(const unsigned int handle, const Matrix<float, ROWS, COLS> data);
		template < const unsigned int ROWS, const unsigned int COLS >
		inline	LIBRENDERER_DLL void	SetMatrixArray(const unsigned int handle, const Matrix<float, ROWS, COLS>* const data);
		
				LIBRENDERER_DLL void	SetMatrix3x3(const unsigned int handle, const Matrix33f data);
				LIBRENDERER_DLL void	SetMatrix4x4(const unsigned int handle, const Matrix44f data);
		//////////////////////////////////////////////////////////////////
		
		// Getters for various types of shader inputs
				LIBRENDERER_DLL Texture* const	GetTexture(const unsigned int handle) const;

				LIBRENDERER_DLL const bool			GetBool(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Vec<bool, 2>	GetBool2(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Vec<bool, 3>	GetBool3(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Vec<bool, 4>	GetBool4(const unsigned int handle, const unsigned int idx = 0) const;

				LIBRENDERER_DLL const float	GetFloat(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Vec2f	GetFloat2(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Vec3f	GetFloat3(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Vec4f	GetFloat4(const unsigned int handle, const unsigned int idx = 0) const;

				LIBRENDERER_DLL const int	GetInt(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Vec2i	GetInt2(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Vec3i	GetInt3(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Vec4i	GetInt4(const unsigned int handle, const unsigned int idx = 0) const;

		template < typename T, const unsigned int ROWS, const unsigned int COLS >
		inline	LIBRENDERER_DLL const Matrix<T, ROWS, COLS>		GetMatrix(const unsigned int handle, const unsigned int idx = 0) const;
		template < const unsigned int ROWS, const unsigned int COLS >
		inline	LIBRENDERER_DLL const Matrix<float, ROWS, COLS>	GetMatrix(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Matrix33f					GetMatrix3x3(const unsigned int handle, const unsigned int idx = 0) const;
				LIBRENDERER_DLL const Matrix44f					GetMatrix4x4(const unsigned int handle, const unsigned int idx = 0) const;

				LIBRENDERER_DLL const unsigned int		GetInputCount() const { return (unsigned int)m_pShaderTemplate->m_arrInputDesc.size(); }
				LIBRENDERER_DLL const bool				GetInputHandleByName(const char* const inputName, unsigned int& inputHandle) const;
				LIBRENDERER_DLL const ShaderInputDesc	GetInputDesc(const unsigned int handle) const { return m_pShaderTemplate->m_arrInputDesc[handle]; }
		//////////////////////////////////////////////////////////////////

	protected:
		ShaderInput(ShaderTemplate* const shaderTemplate);
		virtual ~ShaderInput();

		ShaderTemplate* m_pShaderTemplate;

		friend class ResourceManager;
	};

#include "ShaderInput.inl"
}

#endif // SHADERINPUT_H