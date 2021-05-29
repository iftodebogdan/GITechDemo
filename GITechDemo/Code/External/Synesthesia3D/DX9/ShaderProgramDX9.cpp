/**
 * @file        ShaderProgramDX9.cpp
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

#include "stdafx.h"

#include "ShaderProgramDX9.h"
#include "RendererDX9.h"
#include "TextureDX9.h"
#include "ProfilerDX9.h"
using namespace Synesthesia3D;

#define CONST_MAX_ARRAY_SIZE 16;

ShaderProgramDX9::ShaderProgramDX9(const ShaderProgramType programType,
    const char* srcData, const char* entryPoint, const char* profile)
    : ShaderProgram(programType)
    , m_pVertexShader(nullptr)
    , m_pPixelShader(nullptr)
    , m_pConstantTable(nullptr)
{
    if (strlen(srcData))
        Compile(srcData, entryPoint);
}

ShaderProgramDX9::~ShaderProgramDX9()
{
    Unbind();
}

void ShaderProgramDX9::Enable(ShaderInput* const shaderInput)
{
    ShaderProgram::Enable(shaderInput);

    assert(m_pVertexShader || m_pPixelShader);
    
    IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
    HRESULT hr = E_FAIL;
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
    HRESULT hr = E_FAIL;

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

    ShaderProgram::Disable();
}

const bool ShaderProgramDX9::Compile(const char* filePath, const char* entryPoint)
{
    IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();

    // We need a null terminated array of D3DXMACROs, so adapt std:basic_string to our needs (because strings are null terminated)
    std::basic_string<D3DXMACRO, std::char_traits<D3DXMACRO>, std::allocator<D3DXMACRO>> macroList;

    const char* profile = "";
    switch (m_eProgramType)
    {
    case SPT_VERTEX:
        macroList.push_back({ "VERTEX", "" });
        profile = "vs_3_0";
        if (strlen(entryPoint) == 0)
            entryPoint = "vsmain";
        break;
    case SPT_PIXEL:
        macroList.push_back({ "PIXEL", "" });
        profile = "ps_3_0";
        if (strlen(entryPoint) == 0)
            entryPoint = "psmain";
    }

    LPD3DXBUFFER compiledData = nullptr;
    LPD3DXBUFFER errorMsg = nullptr;
    DWORD flags = NULL;

#if defined(_DEBUG) || defined(_PROFILE)
    flags |= D3DXSHADER_DEBUG;
    macroList.push_back({ "DEBUG", "" });
#endif

#ifndef _DEBUG
    flags |= D3DXSHADER_OPTIMIZATION_LEVEL3;
    //flags |= D3DXSHADER_AVOID_FLOW_CONTROL;
#endif

    macroList.push_back({ "HLSL", "" });

    macroList.push_back({ "DX9", "" });

    HRESULT hr = D3DXCompileShaderFromFile(filePath, macroList.c_str(), NULL, entryPoint, profile,
        flags, &compiledData, &errorMsg, &m_pConstantTable);

#ifdef _DEBUG
    if (errorMsg)
    {
        //DWORD size = errorMsg->GetBufferSize();
        const char* data = (const char*)errorMsg->GetBufferPointer();
        std::cout << std::endl << data << std::endl;
        S3D_DBGPRINT("Error:\n%s", data);
        assert(compiledData);
    }
#endif

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

    return ShaderProgram::Compile(filePath, entryPoint);
}

const unsigned int ShaderProgramDX9::GetConstantCount() const
{
    D3DXCONSTANTTABLE_DESC constDesc;
    memset(&constDesc, 0, sizeof(constDesc));
    HRESULT hr = m_pConstantTable ? m_pConstantTable->GetDesc(&constDesc) : E_FAIL;
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
        // Not sure what else could come through here, but support only non-empty structs strictly assigned to c# float registers (more details below)
        if (constDesc.Class == D3DXPC_STRUCT && constDesc.StructMembers > 0 && constDesc.RegisterSet == D3DXRS_FLOAT4)
        {
            return IT_STRUCT;
        }
        else
        {
            // The way FXC for Shader Model 3.0 handles structs with mixed member types (basically floats in combination with bools/ints) is... stupid.
            //
            // They seem to be using different packing rules when counting float register offsets vs bool register offsets.
            // More exactly, a float vector maps to one float register regardless of whether you pack tightly (e.g. float2 + float2,
            // float2 + float + float, etc). However, when counting bool registers, float vectors "map" to the same number of bool
            // registers as its dimensions (e.g. 2 bool registers for a float2). This makes it impossible to map a HLSL struct to a
            // CPP struct w.r.t. memory layout because a float2 will occupy one float register (16 bytes), but will only generate a
            // 2 bool register offset (8 bytes).
            //
            // Even if we order them in such a way as to make it work, you don't have any guarantee bools in your struct will end up in bool registers.
            // It all depends on whether the compiler decides to use an actual flow-control instruction (i.e. somehting like 'if b0 ... else ... endif'), in which case the bool uses up one
            // 4-byte bool register, or it flattens the conditional (i.e. something like 'cmp ##, -c0.x, ##, ##'), in which case it uses a 16-byte float4 register. This means that there's
            // no realiable way of knowing how to setup the memory layout of CPP structs without parsing the output assembly from FXC.
            //
            // As such we have to force the use of only float registers for structs in HLSL, since int registers have the same problem.
            //
            // Concrete example taken from FXC output, annotated for clarity:
            //
            // Parameters:
            //
            //   struct
            //   {
            //       float2 HalfTexelOffset;        -> (c0) and (b0-b1)
            //       bool SingleChannelCopy;        -> (c1) and (b2)
            //       bool ApplyTonemap;             -> (c2) and (b3)
            //       float4 CustomColorModulator;   -> (c3) and [optimized out of b# registers]
            //
            //   } ColorCopyParams;
            //
            // Registers:
            //
            //   Name                   Reg   Size
            //   ---------------------- ----- ----
            //   ColorCopyParams        b0       4 // i.e. 4 x 4-byte bool registers (b#)
            //   ColorCopyParams        c0       4 // i.e. 4 x 16-byte float4 registers (c#)
            //
            // The reason why CustomColorModulator isn't found in the b# registers is because it wasn't used as an actual bool in the shader code,
            // so FXC decided to remove it from the b# registers so as not to waste them. Only SingleChannelCopy and ApplyTonemap are used in flow
            // control instructions in the tested shader code, so they need to be mapped to b# registers. But since HalfTexelOffset appears before
            // them in the struct declaration, it is also assigned to b# registers (in addition to c# registers), even though they are never used!
            //
            // The corresponding memory layout for the two will look like this:
            //
            //   ColorCopyParams in c# registers:
            //                  c0                      c1                  c2                      c3
            //      | HalfTexelOffset.xy00 | SingleChannelCopy.x000 | ApplyTonemap.x000 | CusomColorModulator.xyzw | => each member is padded to a float4 and stored in a separate float register
            //
            //   ColorCopyParams in b# registers:
            //              b0                  b1                  b2                  b3
            //      | HalfTexelOffset.x | HalfTexelOffset.y | SingleChannelCopy | ApplyTonemap | => each member is split into separate bool registers
            //
            // Each c# float register is 16 bytes, whereas each b# bool register is 4 bytes. So it makes it impossible to map the same constant buffer to
            // both register sets, because it would need different packing rules. Ideally, FXC would have had better packing for c# registers, such that 
            // HalfTexelOffset.xy, SingleChannelCopy and ApplyTonemap would have occupied a single c# float register. That would have matched the four b#
            // registers they also occupy and allow for the same constant buffer to be set directly with one IDirect3DDevice9::SetPixelShaderConstantX()
            // call for each register set the struct gets assigned to (c# float and b# bool registers in this particular example).

            assert(false);
            return IT_NONE;
        }
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
    case D3DXPT_TEXTURE1D:
    case D3DXPT_TEXTURE2D:
    case D3DXPT_TEXTURE3D:
    case D3DXPT_TEXTURECUBE:
        assert(false); // Only for the Effects framework
        return IT_NONE;
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

const unsigned int ShaderProgramDX9::GetConstantSizeBytes(const unsigned int handle) const
{
    unsigned int count = 1u;
    D3DXCONSTANT_DESC constDesc;
    HRESULT hr = m_pConstantTable->GetConstantDesc(m_pConstantTable->GetConstant(NULL, handle), &constDesc, &count);
    assert(SUCCEEDED(hr));

    unsigned int registerSize = 0;
    switch (constDesc.RegisterSet)
    {
    case D3DXRS_FLOAT4:
        registerSize = sizeof(float) * 4u;
        break;
    case D3DXRS_INT4:
        registerSize = sizeof(int) * 4u;
        break;
    case D3DXRS_BOOL:
        registerSize = sizeof(BOOL);
    }

    //assert(size == constDesc.Bytes);
    unsigned int constantSize = 0;
    constantSize = max(constDesc.RegisterCount * registerSize, constDesc.Bytes); // compiler optimizations and struct packings make this calculation tricky

    return constantSize;
}

void ShaderProgramDX9::SetFloat(const unsigned int registerIndex, const float* const data, const unsigned int registerCount)
{
    IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
    HRESULT hr = E_FAIL;
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
    HRESULT hr = E_FAIL;
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
    HRESULT hr = E_FAIL;
    
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
    if (m_szSrcFile.length())
        Compile(m_szSrcFile.c_str(), m_szEntryPoint.c_str());
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