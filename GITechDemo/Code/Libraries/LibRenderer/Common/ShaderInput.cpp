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
#include "stdafx.h"

#include "Renderer.h"
#include "Buffer.h"
#include "ShaderInput.h"
#include "Texture.h"
using namespace LibRendererDll;


///////////////////////////////////////////////////////////////////////////////
// Compiler Warning(level 3) C4800											 //
// 																			 //
// 'type' : forcing value to bool 'true' or 'false' (performance warning)	 //
// This warning is generated when a value that is not bool is assigned or	 //
// coerced into type bool.Typically, this message is caused by assigning	 //
// int variables to bool variables where the int variable contains only		 //
// values true and false, and could be redeclared as type bool.If you cannot //
// rewrite the expression to use type bool, then you can add "!=0" to the	 //
// expression, which gives the expression type bool.Casting the expression	 //
// to type bool will not disable the warning, which is by design.			 //
///////////////////////////////////////////////////////////////////////////////


ShaderInput::ShaderInput(ShaderTemplate* shaderTemplate)
	: Buffer(shaderTemplate->GetTotalNumberOfUsedRegisters(), sizeof(float) * 4u, BU_NONE)
	, m_pShaderTemplate(shaderTemplate)
{
	assert(shaderTemplate);
}

ShaderInput::~ShaderInput()
{}

const bool ShaderInput::GetInputHandleByName(const char* const inputName, unsigned int& inputHandle) const
{
	for (unsigned int i = 0, n = (unsigned int)m_pShaderTemplate->m_arrInputDesc.size(); i < n; i++)
	{
		if (m_pShaderTemplate->m_arrInputDesc[i].szName.compare(inputName) == 0)
		{
			inputHandle = i;
			return true;
		}
	}

	inputHandle = ~0u;

	assert(false || Renderer::GetAPI() == API_NULL);
	return false;
}

void ShaderInput::SetBoolArray(const unsigned int handle, const bool* const data)
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	
	switch (desc.eRegisterType)
	{
	case RT_BOOL:
		memcpy(
			m_pData + desc.nOffsetInBytes,
			data,
			desc.nRegisterCount
			);
		break;
	case RT_FLOAT4:
		for (unsigned int i = 0; i < desc.nArrayElements; i++)
			for (unsigned int j = 0; j < desc.nColumns; j++)
			{
				float* dest = (float*)(m_pData + desc.nOffsetInBytes);
				unsigned int destRegisterOffset = i * 4u; // 4 floats per register
				
				*(dest + destRegisterOffset + j) = (float)*(data + i * desc.nColumns + j);
				//*(float*)(m_pData + desc.nOffsetInBytes + sizeof(float) * i * 4u + sizeof(float) * j) = (float)*(data + i * desc.nColumns + j);
			}
		break;
	case RT_INT4:
		assert(false);	// This should not happen
	}
}

void ShaderInput::SetFloatArray(const unsigned int handle, const float* const data)
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	for (unsigned int i = 0; i < desc.nArrayElements; i++)
	{
		memcpy(
			m_pData + desc.nOffsetInBytes + sizeof(float) * 4u * i,
			data + i * desc.nColumns,
			desc.nBytes / desc.nArrayElements
			);
	}
}

void ShaderInput::SetIntArray(const unsigned int handle, const int* const data)
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];

	switch (desc.eRegisterType)
	{
	case RT_INT4:
		for (unsigned int i = 0; i < desc.nArrayElements; i++)
		{
			memcpy(
				m_pData + desc.nOffsetInBytes + sizeof(int) * 4u * i,
				data + i * desc.nColumns,
				desc.nRegisterCount / desc.nArrayElements
				);
		}
		break;
	case RT_FLOAT4:
		for (unsigned int i = 0; i < desc.nArrayElements; i++)
			for (unsigned int j = 0; j < desc.nColumns; j++)
			{
				float* dest = (float*)(m_pData + desc.nOffsetInBytes);
				unsigned int destRegisterOffset = i * 4u; // 4 floats per register

				*(dest + destRegisterOffset + j) = (float)*(data + i * desc.nColumns + j);
			}
		break;
	case RT_BOOL:
		assert(false);	// This should not happen
	}
}

const bool ShaderInput::GetBool(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_BOOL);
	assert(desc.nColumns == 1);
	if (desc.eRegisterType == RT_FLOAT4)
		return (*((float*)(m_pData + desc.nOffsetInBytes) + idx * 4u)) != 0; // see headnote about Compiler Warning(level 3) C4800
	else
		return *((bool*)(m_pData + desc.nOffsetInBytes) + idx);
}

const Vec<bool, 2> ShaderInput::GetBool2(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_BOOL);
	assert(desc.nColumns == 2);
	Vec<bool, 2> retVec;
	if (desc.eRegisterType == RT_FLOAT4)
	{
		retVec[0] = (*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float))) != 0; // see headnote about Compiler Warning(level 3) C4800
		retVec[1] = (*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 1u * sizeof(float))) != 0; // see headnote about Compiler Warning(level 3) C4800
	}
	else
	{
		retVec[0] = *(bool*)(m_pData + desc.nOffsetInBytes + idx * sizeof(bool));
		retVec[1] = *(bool*)(m_pData + desc.nOffsetInBytes + idx * sizeof(bool) + 1u * sizeof(bool));
	}
	return retVec;
}

const Vec<bool, 3> ShaderInput::GetBool3(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_BOOL);
	assert(desc.nColumns == 3);
	Vec<bool, 3> retVec;
	if (desc.eRegisterType == RT_FLOAT4)
	{
		retVec[0] = (*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float))) != 0; // see headnote about Compiler Warning(level 3) C4800
		retVec[1] = (*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 1u * sizeof(float))) != 0; // see headnote about Compiler Warning(level 3) C4800
		retVec[2] = (*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 2u * sizeof(float))) != 0; // see headnote about Compiler Warning(level 3) C4800
	}
	else
	{
		retVec[0] = *(bool*)(m_pData + desc.nOffsetInBytes + idx * sizeof(bool));
		retVec[1] = *(bool*)(m_pData + desc.nOffsetInBytes + idx * sizeof(bool) + 1u * sizeof(bool));
		retVec[2] = *(bool*)(m_pData + desc.nOffsetInBytes + idx * sizeof(bool) + 2u * sizeof(bool));
	}
	return retVec;
}

const Vec<bool, 4> ShaderInput::GetBool4(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_BOOL);
	assert(desc.nColumns == 4);
	Vec<bool, 4> retVec;
	if (desc.eRegisterType == RT_FLOAT4)
	{
		retVec[0] = (*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float))) != 0; // see headnote about Compiler Warning(level 3) C4800
		retVec[1] = (*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 1u * sizeof(float))) != 0; // see headnote about Compiler Warning(level 3) C4800
		retVec[2] = (*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 2u * sizeof(float))) != 0; // see headnote about Compiler Warning(level 3) C4800
		retVec[3] = (*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 3u * sizeof(float))) != 0; // see headnote about Compiler Warning(level 3) C4800
	}
	else
	{
		retVec[0] = *(bool*)(m_pData + desc.nOffsetInBytes + idx * sizeof(bool));
		retVec[1] = *(bool*)(m_pData + desc.nOffsetInBytes + idx * sizeof(bool) + 1u * sizeof(bool));
		retVec[2] = *(bool*)(m_pData + desc.nOffsetInBytes + idx * sizeof(bool) + 2u * sizeof(bool));
		retVec[3] = *(bool*)(m_pData + desc.nOffsetInBytes + idx * sizeof(bool) + 3u * sizeof(bool));
	}
	return retVec;
}

const int ShaderInput::GetInt(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_INT);
	assert(desc.nColumns == 1);
	if (desc.eRegisterType == RT_FLOAT4)
		return (int)*((float*)(m_pData + desc.nOffsetInBytes) + idx * 4u);
	else
		return *((int*)(m_pData + desc.nOffsetInBytes) + idx * 4u);
}

const Vec2i ShaderInput::GetInt2(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_INT);
	assert(desc.nColumns == 2);
	Vec2i retVec;
	if (desc.eRegisterType == RT_FLOAT4)
	{
		retVec[0] = (int)*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float));
		retVec[1] = (int)*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 1u * sizeof(float));
	}
	else
	{
		retVec[0] = *(int*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(int));
		retVec[1] = *(int*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(int) + 1u * sizeof(int));
	}
	return retVec;
}

const Vec3i ShaderInput::GetInt3(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_INT);
	assert(desc.nColumns == 3);
	Vec3i retVec;
	if (desc.eRegisterType == RT_FLOAT4)
	{
		retVec[0] = (int)*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float));
		retVec[1] = (int)*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 1u * sizeof(float));
		retVec[2] = (int)*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 2u * sizeof(float));
	}
	else
	{
		retVec[0] = *(int*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(int));
		retVec[1] = *(int*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(int) + 1u * sizeof(int));
		retVec[2] = *(int*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(int) + 2u * sizeof(int));
	}
	return retVec;
}

const Vec4i ShaderInput::GetInt4(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_INT);
	assert(desc.nColumns == 4);
	Vec4i retVec;
	if (desc.eRegisterType == RT_FLOAT4)
	{
		retVec[0] = (int)*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float));
		retVec[1] = (int)*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 1u * sizeof(float));
		retVec[2] = (int)*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 2u * sizeof(float));
		retVec[3] = (int)*(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 3u * sizeof(float));
	}
	else
	{
		retVec[0] = *(int*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(int));
		retVec[1] = *(int*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(int) + 1u * sizeof(int));
		retVec[2] = *(int*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(int) + 2u * sizeof(int));
		retVec[3] = *(int*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(int) + 3u * sizeof(int));
	}
	return retVec;
}

const float ShaderInput::GetFloat(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_FLOAT);
	assert(desc.nColumns == 1);
	return *((float*)(m_pData + desc.nOffsetInBytes) + idx * 4u);
}

const Vec2f ShaderInput::GetFloat2(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_FLOAT);
	assert(desc.nColumns == 2);
	Vec2f retVec;
	retVec[0] = *(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float));
	retVec[1] = *(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 1u * sizeof(float));
	return retVec;
}

const Vec3f ShaderInput::GetFloat3(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_FLOAT);
	assert(desc.nColumns == 3);
	Vec3f retVec;
	retVec[0] = *(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float));
	retVec[1] = *(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 1u * sizeof(float));
	retVec[2] = *(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 2u * sizeof(float));
	return retVec;
}

const Vec4f ShaderInput::GetFloat4(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	ShaderInputDesc desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(idx < desc.nArrayElements);
	assert(desc.eInputType == IT_FLOAT);
	assert(desc.nColumns == 4);
	Vec4f retVec;
	retVec[0] = *(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float));
	retVec[1] = *(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 1u * sizeof(float));
	retVec[2] = *(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 2u * sizeof(float));
	retVec[3] = *(float*)(m_pData + desc.nOffsetInBytes + idx * 4u * sizeof(float) + 3u * sizeof(float));
	return retVec;
}

void ShaderInput::SetMatrix3x3(const unsigned int handle, const Matrix33f data)
{
	SetMatrixArray<float, 3, 3>(handle, &data);
}

void ShaderInput::SetMatrix4x4(const unsigned int handle, const Matrix44f data)
{
	SetMatrixArray<float, 4, 4>(handle, &data);
}

const Matrix33f ShaderInput::GetMatrix3x3(const unsigned int handle, const unsigned int idx) const
{
	return GetMatrix<float, 3, 3>(handle, idx);
}

const Matrix44f ShaderInput::GetMatrix4x4(const unsigned int handle, const unsigned int idx) const
{
	return GetMatrix<float, 4, 4>(handle, idx);
}

void ShaderInput::SetTexture(const unsigned int handle, const Texture* const tex)
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];
	assert(tex);
	assert(
		(desc.eInputType == IT_SAMPLER) ||
		(desc.eInputType == IT_SAMPLER1D && tex->GetTextureType() == TT_1D) ||
		(desc.eInputType == IT_SAMPLER2D && tex->GetTextureType() == TT_2D) ||
		(desc.eInputType == IT_SAMPLER3D && tex->GetTextureType() == TT_3D) ||
		(desc.eInputType == IT_SAMPLERCUBE && tex->GetTextureType() == TT_CUBE)
		);
	*(unsigned long long*)(m_pData + desc.nOffsetInBytes) = (unsigned long long)tex; // unsigned long long for 64bit pointer support
}
