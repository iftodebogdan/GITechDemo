/**
 * @file        ShaderInput.h
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

#ifndef SHADERINPUT_H
#define SHADERINPUT_H

#include "Buffer.h"

namespace Synesthesia3D
{
    class Texture;
    class ShaderProgram;

    /**
     * @brief   Handles data used by the shader during rendering.
     */
    class ShaderInput : public Buffer
    {

    public:

        /**
         * @brief   Sets a texture using a texture handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref ShaderInput::GetInputHandleByName().
         * @param[in]   texIdx  Texture handle obtained from @ref ResourceManager::CreateTexture().
         */
        SYNESTHESIA3D_DLL void  SetTexture(const unsigned int handle, const unsigned int texIdx);
        
        /**
         * @brief   Sets a texture using a pointer to the texture object.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   tex     Texture object obtained from @ref ResourceManager::GetTexture().
         */
        SYNESTHESIA3D_DLL void  SetTexture(const unsigned int handle, const Texture* const tex);
        
        /**
         * @brief   Sets a bool array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of bool values.
         */
        SYNESTHESIA3D_DLL void  SetBoolArray(const unsigned int handle, const bool* const data);
        
        /**
         * @brief   Sets a bool2 array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of bool2 values.
         */
        SYNESTHESIA3D_DLL void  SetBoolArray(const unsigned int handle, const Vec<bool, 2>* const data);
        
        /**
         * @brief   Sets a bool3 array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of bool3 values.
         */
        SYNESTHESIA3D_DLL void  SetBoolArray(const unsigned int handle, const Vec<bool, 3>* const data);
        
        /**
         * @brief   Sets a bool4 array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of bool4 values.
         */
        SYNESTHESIA3D_DLL void  SetBoolArray(const unsigned int handle, const Vec<bool, 4>* const data);
        
        /**
         * @brief   Sets a bool value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Bool value.
         */
        SYNESTHESIA3D_DLL void  SetBool(const unsigned int handle, const bool data);
        
        /**
         * @brief   Sets a bool2 value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Bool2 value.
         */
        SYNESTHESIA3D_DLL void  SetBool2(const unsigned int handle, const Vec<bool, 2> data);
        
        /**
         * @brief   Sets a bool3 value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Bool3 value.
         */
        SYNESTHESIA3D_DLL void  SetBool3(const unsigned int handle, const Vec<bool, 3> data);
        
        /**
         * @brief   Sets a bool4 value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Bool4 value.
         */
        SYNESTHESIA3D_DLL void  SetBool4(const unsigned int handle, const Vec<bool, 4> data);
        ///////////////////////////////////////////////////
        
        /**
         * @brief   Sets a float array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of float values.
         */
        SYNESTHESIA3D_DLL void  SetFloatArray(const unsigned int handle, const float* const data);

        /**
         * @brief   Sets a float2 array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of float2 values.
         */
        SYNESTHESIA3D_DLL void  SetFloatArray(const unsigned int handle, const Vec2f* const data);
        
        /**
         * @brief   Sets a float3 array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of float3 values.
         */
        SYNESTHESIA3D_DLL void  SetFloatArray(const unsigned int handle, const Vec3f* const data);
        
        /**
         * @brief   Sets a float4 array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of float4 values.
         */
        SYNESTHESIA3D_DLL void  SetFloatArray(const unsigned int handle, const Vec4f* const data);
        
        /**
         * @brief   Sets a float value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Float value.
         */
        SYNESTHESIA3D_DLL void  SetFloat(const unsigned int handle, const float data);
        
        /**
         * @brief   Sets a float2 value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Float2 value.
         */
        SYNESTHESIA3D_DLL void  SetFloat2(const unsigned int handle, const Vec2f data);
        
        /**
         * @brief   Sets a float3 value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Float3 value.
         */
        SYNESTHESIA3D_DLL void  SetFloat3(const unsigned int handle, const Vec3f data);
        
        /**
         * @brief   Sets a float4 value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Float4 value.
         */
        SYNESTHESIA3D_DLL void  SetFloat4(const unsigned int handle, const Vec4f data);
        ////////////////////////////////////////////////////
        
        /**
         * @brief   Sets an integer array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of integer values.
         */
        SYNESTHESIA3D_DLL void  SetIntArray(const unsigned int handle, const int* const data);
        
        /**
         * @brief   Sets an integer2 array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of integer2 values.
         */
        SYNESTHESIA3D_DLL void  SetIntArray(const unsigned int handle, const Vec2i* const data);
        
        /**
         * @brief   Sets an integer3 array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of integer3 values.
         */
        SYNESTHESIA3D_DLL void  SetIntArray(const unsigned int handle, const Vec3i* const data);
        
        /**
         * @brief   Sets an integer4 array using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of integer4 values.
         */
        SYNESTHESIA3D_DLL void  SetIntArray(const unsigned int handle, const Vec4i* const data);
        
        /**
         * @brief   Sets an integer value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Integer value.
         */
        SYNESTHESIA3D_DLL void  SetInt(const unsigned int handle, const int data);
        
        /**
         * @brief   Sets an integer2 value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Integer2 value.
         */
        SYNESTHESIA3D_DLL void  SetInt2(const unsigned int handle, const Vec2i data);
        
        /**
         * @brief   Sets an integer3 value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Integer3 value.
         */
        SYNESTHESIA3D_DLL void  SetInt3(const unsigned int handle, const Vec3i data);
        
        /**
         * @brief   Sets an integer4 value using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Integer4 value.
         */
        SYNESTHESIA3D_DLL void  SetInt4(const unsigned int handle, const Vec4i data);
        /////////////////////////////////////////////////////
        
        /**
         * @brief   Sets a matrix with ROWS rows and COLS columns using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Matrix with ROWS rows and COLS columns.
         */
        template < typename T, const unsigned int ROWS, const unsigned int COLS >
        inline  void    SetMatrix(const unsigned int handle, const Matrix<T, ROWS, COLS> data);
        
        /**
         * @brief   Sets an array of matrices with ROWS rows and COLS columns using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of matrices with ROWS rows and COLS columns.
         */
        template < typename T, const unsigned int ROWS, const unsigned int COLS >
        inline  void    SetMatrixArray(const unsigned int handle, const Matrix<T, ROWS, COLS>* const data);
        //////////////////////////////////////////////////////
        
        /**
         * @brief   Sets a matrix of floats with ROWS rows and COLS columns using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Matrix of floats with ROWS rows and COLS columns.
         */
        template < const unsigned int ROWS, const unsigned int COLS >
        inline  void    SetMatrix(const unsigned int handle, const Matrix<float, ROWS, COLS> data);
        
        /**
         * @brief   Sets an array of matrices of floats with ROWS rows and COLS columns using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    Pointer to an array of matrices of floats with ROWS rows and COLS columns.
         */
        template < const unsigned int ROWS, const unsigned int COLS >
        inline  void    SetMatrixArray(const unsigned int handle, const Matrix<float, ROWS, COLS>* const data);
        
        /**
         * @brief   Sets a 3x3 matrix of floats using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    3x3 matrix of floats.
         */
        SYNESTHESIA3D_DLL void  SetMatrix3x3(const unsigned int handle, const Matrix33f data);
        
        /**
         * @brief   Sets a 4x4 matrix of floats using a shader constant handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   data    4x4 matrix of floats.
         */
        SYNESTHESIA3D_DLL void  SetMatrix4x4(const unsigned int handle, const Matrix44f data);
        //////////////////////////////////////////////////////////////////
        
        /**
         * @brief   Retrieves the texture object that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         *
         * @return  Texture bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL Texture* const    GetTexture(const unsigned int handle) const;
        
        /**
         * @brief   Retrieves the bool value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Bool value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const bool            GetBool(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the bool2 value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Bool2 value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Vec<bool, 2>    GetBool2(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the bool3 value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Bool3 value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Vec<bool, 3>    GetBool3(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the bool4 value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Bool4 value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Vec<bool, 4>    GetBool4(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the float value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Float value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const float   GetFloat(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the float2 value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Float2 value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Vec2f   GetFloat2(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the float3 value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Float3 value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Vec3f   GetFloat3(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the float4 value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Float4 value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Vec4f   GetFloat4(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the integer value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Int value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const int     GetInt(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the integer2 value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Int2 value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Vec2i   GetInt2(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the integer3 value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Int3 value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Vec3i   GetInt3(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the integer4 value that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Int4 value bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Vec4i   GetInt4(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the matrix with ROWS rows and COLS columns that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Matrix bound to specified shader input handle.
         */
        template < typename T, const unsigned int ROWS, const unsigned int COLS >
        inline  const Matrix<T, ROWS, COLS>     GetMatrix(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the matrix of floats with ROWS rows and COLS columns that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  Matrix of float values bound to specified shader input handle.
         */
        template < const unsigned int ROWS, const unsigned int COLS >
        inline  const Matrix<float, ROWS, COLS> GetMatrix(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the 3x3 matrix of floats with ROWS rows and COLS columns that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  3x3 matrix of float values bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Matrix33f           GetMatrix3x3(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the 4x4 matrix of floats with ROWS rows and COLS columns that is set in the shader constant corresponding to the supplied handle.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         * @param[in]   idx     Index in the array (where applicable).
         *
         * @return  4x4 matrix of float values bound to specified shader input handle.
         */
        SYNESTHESIA3D_DLL const Matrix44f           GetMatrix4x4(const unsigned int handle, const unsigned int idx = 0) const;
        
        /**
         * @brief   Retrieves the number of shader constants managed by this shader input object.
         *
         * @return  Number of shader inputs.
         */
        SYNESTHESIA3D_DLL const unsigned int        GetInputCount() const;
        
        /**
         * @brief   Retrieves the handle of the shader constant which matches the supplied name.
         *
         * @param[in]   inputName       Name of the shader constant (as appears in the shader).
         * @param[out]  inputHandle     Handle for the shader constant.
         *
         * @return  Success of operation.
         */
        SYNESTHESIA3D_DLL const bool                GetInputHandleByName(const char* const inputName, unsigned int& inputHandle) const;
        
        /**
         * @brief   Retrieves the handle of the shader constant which matches the supplied name hash.
         * @note    Use @ref S3DHASH() from Utility/Hash.h to generate the hash.
         *
         * @param[in]   inputNameHash   Hash of the name of the shader constant (as appears in the shader).
         * @param[out]  inputHandle     Handle for the shader constant.
         *
         * @return  Success of operation.
         */
        SYNESTHESIA3D_DLL const bool                GetInputHandleByNameHash(const unsigned int inputNameHash, unsigned int& inputHandle) const;

        /**
         * @brief   Retrieves a detailed description of a shader constant.
         *
         * @param[in]   handle  Handle for the shader constant obtained from @ref GetInputHandleByName().
         *
         * @return  Description of shader inputs.
         */
        SYNESTHESIA3D_DLL const ShaderInputDesc&    GetInputDesc(const unsigned int handle) const;
        //////////////////////////////////////////////////////////////////

    protected:
        
        /**
         * @brief   Constructor.
         *
         * @details Meant to be used only by @ref ResourceManager.
         * @see     ResourceManager::CreateShaderInput()
         *
         * @param[in]   shaderProgram       Shader program object corresponding to the compiled shader binary.
         */
        ShaderInput(ShaderProgram* const shaderProgram);
        
        /**
         * @brief   Destructor.
         *
         * @details Meant to be used only by @ref ResourceManager.
         * @see     ResourceManager::ReleaseShaderInput()
         */
        virtual ~ShaderInput();

        ShaderProgram* m_pShaderProgram;    /**< @brief Pointer to the corresponding shader program. */

        friend class ResourceManager;
    };
}

#include "ShaderInput.inl"

#endif // SHADERINPUT_H