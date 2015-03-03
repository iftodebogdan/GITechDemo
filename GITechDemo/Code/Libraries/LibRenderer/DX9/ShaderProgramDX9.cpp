//////////////////////////////////////////////////////////////////////////
// This file is part of the "LibRenderer" 3D graphics library           //
//                                                                      //
// Copyright (C) 2014 - Iftode Bogdan-Marius <iftode.bogdan@gmail.com>  //
//                                                                      //
// This program is free software: you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation: either version 3 of the License: or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful:      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program. If not: see <http://www.gnu.org/licenses/>. //
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "ShaderProgramDX9.h"
#include "RendererDX9.h"
#include "TextureDX9.h"
using namespace LibRendererDll;
using namespace std;

#define CONST_MAX_ARRAY_SIZE 16;

ShaderProgramDX9::ShaderProgramDX9(const ShaderProgramType programType,
	const char* srcData, const char* entryPoint, const char* profile)
	: ShaderProgram(programType)
	, m_pVertexShader(nullptr)
	, m_pPixelShader(nullptr)
	, m_pConstantTable(nullptr)
{
	if (strlen(srcData))
		Compile(srcData, nullptr, entryPoint, profile);
}

ShaderProgramDX9::~ShaderProgramDX9()
{
	Unbind();
}

void ShaderProgramDX9::Enable()
{
	assert(m_pVertexShader || m_pPixelShader);
	
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;
	switch (m_eProgramType)
	{
	case SPT_VERTEX:
		hr = device->SetVertexShader(m_pVertexShader);
		break;
	case SPT_PIXEL:
		hr = device->SetPixelShader(m_pPixelShader);
	}
	assert(SUCCEEDED(hr));
}

void ShaderProgramDX9::Disable()
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

#ifdef _DEBUG
	IDirect3DVertexShader9* activeVShader = nullptr;
	IDirect3DPixelShader9* activePShader = nullptr;
	unsigned int refCount = 1;
	switch (m_eProgramType)
	{
	case SPT_VERTEX:
		hr = device->GetVertexShader(&activeVShader);
		assert(SUCCEEDED(hr));
		assert(activeVShader == m_pVertexShader);
		refCount = activeVShader->Release();
		assert(refCount == 1);
		break;
	case SPT_PIXEL:
		hr = device->GetPixelShader(&activePShader);
		assert(SUCCEEDED(hr));
		assert(activePShader == m_pPixelShader);
		refCount = activePShader->Release();
		assert(refCount == 1);
	}
#endif

	switch (m_eProgramType)
	{
	case SPT_VERTEX:
		hr = device->SetVertexShader(0);
		break;
	case SPT_PIXEL:
		hr = device->SetPixelShader(0);
	}
	assert(SUCCEEDED(hr));
}

const bool ShaderProgramDX9::Compile(const char* srcData, char* const errors, const char* entryPoint, const char* profile)
{
	m_szSrcData = srcData;

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();

	switch (m_eProgramType)
	{
	case SPT_VERTEX:
		if (strlen(profile) == 0)
			profile = "vs_3_0";
		if (strlen(entryPoint) == 0)
			entryPoint = "vsmain";
		break;
	case SPT_PIXEL:
		if (strlen(profile) == 0)
			profile = "ps_3_0";
		if (strlen(entryPoint) == 0)
			entryPoint = "psmain";
	}
	LPD3DXBUFFER compiledData = nullptr;
	LPD3DXBUFFER errorMsg = nullptr;
	m_szEntryPoint = entryPoint;
	m_szProfile = profile;
	HRESULT hr = D3DXCompileShader(srcData, (UINT)strlen(srcData), NULL, NULL, entryPoint, profile,
		NULL, &compiledData, &errorMsg, &m_pConstantTable);

#ifdef _DEBUG
	if (errorMsg)
	{
		DWORD size = errorMsg->GetBufferSize();
		const char* data = (const char*)errorMsg->GetBufferPointer();
		cout << endl << data << endl;
		assert(compiledData);
	}
#endif

	if (errors && errorMsg)
		strcpy_s(errors, (int)errorMsg->GetBufferSize(), (const char*)errorMsg->GetBufferPointer());

	if (errorMsg)
		m_szErrors = (const char*)errorMsg->GetBufferPointer();
	else
		m_szErrors.clear();

	if (FAILED(hr))
		return false;

	unsigned int refCount = 0;
	switch (m_eProgramType)
	{
	case SPT_VERTEX:
		if (m_pVertexShader)
			refCount = m_pVertexShader->Release();
		assert(refCount == 0);
		hr = device->CreateVertexShader((DWORD*)(compiledData->GetBufferPointer()), &m_pVertexShader);
		break;
	case SPT_PIXEL:
		if (m_pPixelShader)
			refCount = m_pPixelShader->Release();
		assert(refCount == 0);
		hr = device->CreatePixelShader((DWORD*)(compiledData->GetBufferPointer()), &m_pPixelShader);
	}
	assert(SUCCEEDED(hr));

	if (compiledData)
		refCount = compiledData->Release();
	assert(refCount == 0);

	if (errorMsg)
		refCount = errorMsg->Release();
	assert(refCount == 0);

	return true;
}

const unsigned int ShaderProgramDX9::GetConstantCount() const
{
	D3DXCONSTANTTABLE_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetDesc(&constDesc);
	assert(SUCCEEDED(hr));
	
	return constDesc.Constants;
}

const char* ShaderProgramDX9::GetConstantName(const unsigned int handle) const
{
	unsigned int count = 1u;
	D3DXCONSTANT_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
	assert(SUCCEEDED(hr));

	return constDesc.Name;
}

const InputType ShaderProgramDX9::GetConstantType(const unsigned int handle) const
{
	unsigned int count = 1u;
	D3DXCONSTANT_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
	assert(SUCCEEDED(hr));
	assert(count == 1);

	switch (constDesc.Type)
	{
	case D3DXPT_VOID:
		assert(false); // no structs or whatever
		return IT_NONE;
	case D3DXPT_BOOL:
		return IT_BOOL;
	case D3DXPT_INT:
		return IT_INT;
	case D3DXPT_FLOAT:
		return IT_FLOAT;
	case D3DXPT_STRING:
		assert(false); // no strings
		return IT_NONE;
	case D3DXPT_TEXTURE:
		assert(false); // DX9 documentation isn't very clear, but apparently this is only for the Effects framework
		return IT_TEXTURE;
	case D3DXPT_TEXTURE1D:
		assert(false); // DX9 documentation isn't very clear, but apparently this is only for the Effects framework
		return IT_TEXTURE1D;
	case D3DXPT_TEXTURE2D:
		assert(false); // DX9 documentation isn't very clear, but apparently this is only for the Effects framework
		return IT_TEXTURE2D;
	case D3DXPT_TEXTURE3D:
		assert(false); // DX9 documentation isn't very clear, but apparently this is only for the Effects framework
		return IT_TEXTURE3D;
	case D3DXPT_TEXTURECUBE:
		assert(false); // DX9 documentation isn't very clear, but apparently this is only for the Effects framework
		return IT_TEXTURECUBE;
	case D3DXPT_SAMPLER:
		return IT_SAMPLER;
	case D3DXPT_SAMPLER1D:
		return IT_SAMPLER1D;
	case D3DXPT_SAMPLER2D:
		return IT_SAMPLER2D;
	case D3DXPT_SAMPLER3D:
		return IT_SAMPLER3D;
	case D3DXPT_SAMPLERCUBE:
		return IT_SAMPLERCUBE;
	default:
		assert(false);
	}

	return IT_NONE;
}

const RegisterType ShaderProgramDX9::GetConstantRegisterType(const unsigned int handle) const
{
	unsigned int count = 1u;
	D3DXCONSTANT_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
	assert(SUCCEEDED(hr));
	assert(count == 1);

	switch (constDesc.RegisterSet)
	{
	case D3DXRS_BOOL:
		return RT_BOOL;
	case D3DXRS_INT4:
		return RT_INT4;
	case D3DXRS_FLOAT4:
		return RT_FLOAT4;
	case D3DXRS_SAMPLER:
		return RT_SAMPLER;
	default:
		return RT_NONE;
	}
}

const unsigned int ShaderProgramDX9::GetConstantRegisterIndex(const unsigned int handle) const
{
	unsigned int count = 1u;
	D3DXCONSTANT_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
	assert(SUCCEEDED(hr));

	return constDesc.RegisterIndex;
}

const unsigned int ShaderProgramDX9::GetConstantRegisterCount(const unsigned int handle) const
{
	unsigned int count = 1u;
	D3DXCONSTANT_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
	assert(SUCCEEDED(hr));

	return constDesc.RegisterCount;
}

const unsigned int ShaderProgramDX9::GetConstantRowCount(const unsigned int handle) const
{
	unsigned int count = 1u;
	D3DXCONSTANT_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
	assert(SUCCEEDED(hr));

	return constDesc.Rows;
}
const unsigned int ShaderProgramDX9::GetConstantColumnCount(const unsigned int handle) const
{
	unsigned int count = 1u;
	D3DXCONSTANT_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
	assert(SUCCEEDED(hr));

	return constDesc.Columns;
}

const unsigned int ShaderProgramDX9::GetConstantArrayElementCount(const unsigned int handle) const
{
	unsigned int count = 1u;
	D3DXCONSTANT_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
	assert(SUCCEEDED(hr));

	return constDesc.Elements;
}

const unsigned int ShaderProgramDX9::GetConstantStructMemberCount(const unsigned int handle) const
{
	unsigned int count = 1u;
	D3DXCONSTANT_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
	assert(SUCCEEDED(hr));

	return constDesc.StructMembers;
}

const unsigned int ShaderProgramDX9::GetConstantByteCount(const unsigned int handle) const
{
	unsigned int count = 1u;
	D3DXCONSTANT_DESC constDesc;
	HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
	assert(SUCCEEDED(hr));

	return constDesc.Bytes;
}

void ShaderProgramDX9::SetFloat(const unsigned int registerIndex, const float* const data, const unsigned int registerCount)
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;
	switch (m_eProgramType)
	{
	case SPT_VERTEX:
		hr = device->SetVertexShaderConstantF(registerIndex, data, registerCount);
		break;
	case SPT_PIXEL:
		hr = device->SetPixelShaderConstantF(registerIndex, data, registerCount);
	}
	assert(SUCCEEDED(hr));
}


void ShaderProgramDX9::SetInt(const unsigned int registerIndex, const int* const data, const unsigned int registerCount)
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;
	switch (m_eProgramType)
	{
	case SPT_VERTEX:
		hr = device->SetVertexShaderConstantI(registerIndex, data, registerCount);
		break;
	case SPT_PIXEL:
		hr = device->SetPixelShaderConstantI(registerIndex, data, registerCount);
	}
	assert(SUCCEEDED(hr));
}

void ShaderProgramDX9::SetBool(const unsigned int registerIndex, const bool* const data, const unsigned int registerCount)
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;
	
	// Convert the bool (2 bytes) array to a Microsoft BOOL (4 bytes) array
	BOOL* tempBuffer = new BOOL[registerCount];
	for (unsigned int i = 0; i < registerCount; i++)
		tempBuffer[i] = data[i];

	switch (m_eProgramType)
	{
	case SPT_VERTEX:
		hr = device->SetVertexShaderConstantB(registerIndex, tempBuffer, registerCount);
		break;
	case SPT_PIXEL:
		hr = device->SetPixelShaderConstantB(registerIndex, tempBuffer, registerCount);
	}
	delete[] tempBuffer;
	assert(SUCCEEDED(hr));
}

void ShaderProgramDX9::SetTexture(const unsigned int registerIndex, const Texture* const tex)
{
	assert(tex);
	tex->Enable(registerIndex);
}

void ShaderProgramDX9::Bind()
{
	if (m_szSrcData.length())
		Compile(m_szSrcData.c_str(), nullptr, m_szEntryPoint.c_str(), m_szProfile.c_str());
}

void ShaderProgramDX9::Unbind()
{
	unsigned int refCount = 0;

	if (m_pVertexShader)
	{
		refCount = m_pVertexShader->Release();
		m_pVertexShader = nullptr;
	}
	assert(refCount == 0);

	if (m_pPixelShader)
	{
		refCount = m_pPixelShader->Release();
		m_pPixelShader = nullptr;
	}
	assert(refCount == 0);

	if (m_pConstantTable)
	{
		refCount = m_pConstantTable->Release();
		m_pConstantTable = nullptr;
	}
	assert(refCount == 0);
}