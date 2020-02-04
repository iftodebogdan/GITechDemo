/**
 * @file        ShaderInput.inl
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

namespace Synesthesia3D
{
    template < typename T, const unsigned int ROWS, const unsigned int COLS >
    inline void ShaderInput::SetMatrix(const unsigned int handle, const Matrix<T, ROWS, COLS> data)
    {
        SetMatrixArray<T, ROWS, COLS>(handle, &data);
    }

    template < typename T, const unsigned int ROWS, const unsigned int COLS >
    inline void ShaderInput::SetMatrixArray(const unsigned int handle, const Matrix<T, ROWS, COLS>* const data)
    {
        assert(handle < m_pShaderProgram->m_arrInputDesc.size());
        const ShaderInputDesc& desc = m_pShaderProgram->m_arrInputDesc[handle];
        assert(desc.nRows == ROWS && desc.nColumns == COLS);
        assert((desc.eRegisterType == RT_BOOL) == (typeid(T) == typeid(bool)));
        assert((desc.eRegisterType == RT_INT4) == (typeid(T) == typeid(int)));
        assert((desc.eRegisterType == RT_FLOAT4) == (typeid(T) == typeid(float)));
        for (unsigned int i = 0; i < desc.nArrayElements; i++)
        {
            for (unsigned int j = 0; j < desc.nColumns; j++)
            {
                memcpy(
                    m_pData + desc.nOffsetInBytes + desc.nBytes / desc.nArrayElements * i + j * sizeof(float) * 4u,
                    data[i].getData() + j * desc.nRows,
                    desc.nBytes / desc.nArrayElements / desc.nColumns
                    );
            }
        }
    }

    template < const unsigned int ROWS, const unsigned int COLS >
    inline void ShaderInput::SetMatrix(const unsigned int handle, const Matrix<float, ROWS, COLS> data)
    {
        SetMatrixArray<float, ROWS, COLS>(handle, &data);
    }

    template < const unsigned int ROWS, const unsigned int COLS >
    inline void ShaderInput::SetMatrixArray(const unsigned int handle, const Matrix<float, ROWS, COLS>* const data)
    {
        SetMatrixArray<float, ROWS, COLS>(handle, data);
    }

    template < typename T, const unsigned int ROWS, const unsigned int COLS >
    inline const Matrix<T, ROWS, COLS> ShaderInput::GetMatrix(const unsigned int handle, const unsigned int idx) const
    {
        Matrix<T, ROWS, COLS> retMat;
        assert(handle < m_pShaderProgram->m_arrInputDesc.size());
        const ShaderInputDesc& desc = m_pShaderProgram->m_arrInputDesc[handle];
        assert(desc.nRows == ROWS && desc.nColumns == COLS);
        assert((desc.eRegisterType == RT_BOOL) == (typeid(T) == typeid(bool)));
        assert((desc.eRegisterType == RT_INT4) == (typeid(T) == typeid(int)));
        assert((desc.eRegisterType == RT_FLOAT4) == (typeid(T) == typeid(float)));
        for (unsigned int i = 0; i < desc.nColumns; i++)
        {
            memcpy(
                retMat.mData + i * desc.nRows,
                m_pData + desc.nOffsetInBytes + desc.nBytes / desc.nArrayElements * idx + i * sizeof(float) * 4u,
                desc.nBytes / desc.nArrayElements / desc.nColumns
                );
        }
        return retMat;
    }

    template < const unsigned int ROWS, const unsigned int COLS >
    inline const Matrix<float, ROWS, COLS> ShaderInput::GetMatrix(const unsigned int handle, const unsigned int idx) const
    {
        return GetMatrix<float, ROWS, COLS>(handle, idx);
    }

    template < typename T >
    inline void ShaderInput::SetStruct(const unsigned int handle, const T& data)
    {
        static_assert(!std::is_pointer<T>::value, "[S3D] ShaderInput::SetStruct() - You cannot pass a pointer to structs");
        static_assert(!std::is_array<T>::value, "[S3D] ShaderInput::SetStruct() - You cannot pass an array of structs");
        SetStructArray<T>(handle, &data);
    }

    template < typename T >
    inline void ShaderInput::SetStructArray(const unsigned int handle, const T* const data)
    {
        static_assert(!std::is_pointer<T>::value && !std::is_array<T>::value, "[S3D] ShaderInput::SetStructArray() - You must pass a pointer or an array of structs");
        assert(handle < m_pShaderProgram->m_arrInputDesc.size());
        // unused struct members at the bottom may be optimized away, so <= instead of == is theoretically valid here
        assert(m_pShaderProgram->m_arrInputDesc[handle].nBytes <= sizeof(T) * m_pShaderProgram->m_arrInputDesc[handle].nArrayElements);
        SetStructArray(handle, (void*)data);
    }

    template < typename T >
    inline const T& ShaderInput::GetStruct(const unsigned int handle, const unsigned int idx) const
    {
        assert(handle < m_pShaderProgram->m_arrInputDesc.size());
        const ShaderInputDesc& desc = m_pShaderProgram->m_arrInputDesc[handle];
        assert(desc.eInputType == IT_STRUCT);
        assert(desc.nBytes / desc.nArrayElements == sizeof(T));
        assert(idx < desc.nArrayElements);
        return *(T*)(m_pData + desc.nOffsetInBytes + sizeof(T) * idx);
    }
}
