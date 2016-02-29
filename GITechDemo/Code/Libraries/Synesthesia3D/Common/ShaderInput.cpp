/**
 *	@file		ShaderInput.cpp
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

#include "stdafx.h"

#include "Renderer.h"
#include "ShaderInput.h"
#include "ShaderTemplate.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "ShaderProgram.h"
using namespace Synesthesia3D;

#include "Utility/Hash.h"

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


ShaderInput::ShaderInput(ShaderTemplate* const shaderTemplate)
	: Buffer(shaderTemplate->GetTotalSizeOfInputConstants(), 1u, BU_NONE)
	, m_pShaderTemplate(shaderTemplate)
{
	assert(shaderTemplate);
}

ShaderInput::~ShaderInput()
{}

const bool ShaderInput::GetInputHandleByName(const char* const inputName, unsigned int& inputHandle) const
{
	const unsigned int nNameHash = S3DHASH(inputName);

	for (unsigned int i = 0, n = (unsigned int)m_pShaderTemplate->m_arrInputDesc.size(); i < n; i++)
	{
		//if (m_pShaderTemplate->m_arrInputDesc[i].szName.compare(inputName) == 0)
		if (m_pShaderTemplate->m_arrInputDesc[i].nNameHash == nNameHash)
		{
			inputHandle = i;
			return true;
		}
	}

	inputHandle = ~0u;

#ifdef _DEBUG
	if(Renderer::GetInstance()->GetAPI() != API_NULL)
		std::cout << "Could not find input \"" << inputName << "\" in " <<
			(m_pShaderTemplate->m_pProgram->GetProgramType() == SPT_VERTEX ? "vertex" : "pixel") <<
			" shader: \"" << m_pShaderTemplate->m_pProgram->GetFilePath() << "\"\n";
#endif

	return false;
}

const bool ShaderInput::GetInputHandleByNameHash(const unsigned int inputNameHash, unsigned int& inputHandle) const
{
	for (unsigned int i = 0, n = (unsigned int)m_pShaderTemplate->m_arrInputDesc.size(); i < n; i++)
	{
		if (m_pShaderTemplate->m_arrInputDesc[i].nNameHash == inputNameHash)
		{
			inputHandle = i;
			return true;
		}
	}

	inputHandle = ~0u;

#ifdef _DEBUG
	if (Renderer::GetInstance()->GetAPI() != API_NULL)
		std::cout << "Could not find input with hash value \"" << std::hex << inputNameHash << std::dec << "\" in " <<
		(m_pShaderTemplate->m_pProgram->GetProgramType() == SPT_VERTEX ? "vertex" : "pixel") <<
		" shader: \"" << m_pShaderTemplate->m_pProgram->GetFilePath() << "\"\n";
#endif

	return false;
}

void ShaderInput::SetBoolArray(const unsigned int handle, const bool* const data)
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];
	
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

void ShaderInput::SetBoolArray(const unsigned int handle, const Vec<bool, 2>* const data)
{
	SetBoolArray(handle, (const bool* const)data);
}

void ShaderInput::SetBoolArray(const unsigned int handle, const Vec<bool, 3>* const data)
{
	SetBoolArray(handle, (const bool* const)data);
}

void ShaderInput::SetBoolArray(const unsigned int handle, const Vec<bool, 4>* const data)
{
	SetBoolArray(handle, (const bool* const)data);
}

void ShaderInput::SetBool(const unsigned int handle, const bool data)
{
	SetBoolArray(handle, &data);
}

void ShaderInput::SetBool2(const unsigned int handle, const Vec<bool, 2> data)
{
	SetBoolArray(handle, data.getData());
}

void ShaderInput::SetBool3(const unsigned int handle, const Vec<bool, 3> data)
{
	SetBoolArray(handle, data.getData());
}

void ShaderInput::SetBool4(const unsigned int handle, const Vec<bool, 4> data)
{
	SetBoolArray(handle, data.getData());
}

void ShaderInput::SetFloatArray(const unsigned int handle, const float* const data)
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];
	for (unsigned int i = 0; i < desc.nArrayElements; i++)
	{
		memcpy(
			m_pData + desc.nOffsetInBytes + sizeof(float) * 4u * i,
			data + i * desc.nColumns,
			desc.nBytes / desc.nArrayElements
			);
	}
}

void ShaderInput::SetFloatArray(const unsigned int handle, const Vec2f* const data)
{
	SetFloatArray(handle, (const float* const)data);
}

void ShaderInput::SetFloatArray(const unsigned int handle, const Vec3f* const data)
{
	SetFloatArray(handle, (const float* const)data);
}

void ShaderInput::SetFloatArray(const unsigned int handle, const Vec4f* const data)
{
	SetFloatArray(handle, (const float* const)data);
}

void ShaderInput::SetFloat(const unsigned int handle, const float data)
{
	SetFloatArray(handle, &data);
}

void ShaderInput::SetFloat2(const unsigned int handle, const Vec2f data)
{
	SetFloatArray(handle, data.getData());
}

void ShaderInput::SetFloat3(const unsigned int handle, const Vec3f data) 
{
	SetFloatArray(handle, data.getData());
}

void ShaderInput::SetFloat4(const unsigned int handle, const Vec4f data)
{
	SetFloatArray(handle, data.getData());
}

void ShaderInput::SetIntArray(const unsigned int handle, const int* const data)
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];

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

void ShaderInput::SetIntArray(const unsigned int handle, const Vec2i* const data)
{
	SetIntArray(handle, (const int* const)data);
}

void ShaderInput::SetIntArray(const unsigned int handle, const Vec3i* const data)
{
	SetIntArray(handle, (const int* const)data);
}

void ShaderInput::SetIntArray(const unsigned int handle, const Vec4i* const data)
{
	SetIntArray(handle, (const int* const)data);
}

void ShaderInput::SetInt(const unsigned int handle, const int data)
{
	SetIntArray(handle, &data);
}

void ShaderInput::SetInt2(const unsigned int handle, const Vec2i data)
{
	SetIntArray(handle, data.getData());
}

void ShaderInput::SetInt3(const unsigned int handle, const Vec3i data)
{
	SetIntArray(handle, data.getData());
}

void ShaderInput::SetInt4(const unsigned int handle, const Vec4i data)
{
	SetIntArray(handle, data.getData());
}

const bool ShaderInput::GetBool(const unsigned int handle, const unsigned int idx) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];
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
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];
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
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];
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
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];
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
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];
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
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];
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

Texture* const ShaderInput::GetTexture(const unsigned int handle) const
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];
	const unsigned int texIdx = *(unsigned int*)(m_pData + desc.nOffsetInBytes);
	return Renderer::GetInstance()->GetResourceManager()->GetTexture(texIdx);
}

void ShaderInput::SetTexture(const unsigned int handle, const unsigned int texIdx)
{
	assert(handle < m_pShaderTemplate->m_arrInputDesc.size());
	const ShaderInputDesc& desc = m_pShaderTemplate->m_arrInputDesc[handle];

	assert(
		texIdx == -1 ||
		(desc.eInputType == IT_SAMPLER) ||
		(desc.eInputType == IT_SAMPLER1D && Renderer::GetInstance()->GetResourceManager()->GetTexture(texIdx)->GetTextureType() == TT_1D) ||
		(desc.eInputType == IT_SAMPLER2D && Renderer::GetInstance()->GetResourceManager()->GetTexture(texIdx)->GetTextureType() == TT_2D) ||
		(desc.eInputType == IT_SAMPLER3D && Renderer::GetInstance()->GetResourceManager()->GetTexture(texIdx)->GetTextureType() == TT_3D) ||
		(desc.eInputType == IT_SAMPLERCUBE && Renderer::GetInstance()->GetResourceManager()->GetTexture(texIdx)->GetTextureType() == TT_CUBE)
		);
	*(unsigned int*)(m_pData + desc.nOffsetInBytes) = (unsigned int)texIdx;
}

void ShaderInput::SetTexture(const unsigned int handle, const Texture* const tex)
{
	const ResourceManager* const resMan = Renderer::GetInstance()->GetResourceManager();
	const unsigned int texCount = resMan->GetTextureCount();
	for (unsigned int i = 0; i < texCount; i++)
		if (resMan->GetTexture(i) == tex)
		{
			SetTexture(handle, i);
			break;
		}
}

const unsigned int ShaderInput::GetInputCount() const
{
	return (unsigned int)m_pShaderTemplate->m_arrInputDesc.size();
}

const ShaderInputDesc& ShaderInput::GetInputDesc(const unsigned int handle) const
{
	assert(handle < GetInputCount());
	return m_pShaderTemplate->m_arrInputDesc[handle];
}
