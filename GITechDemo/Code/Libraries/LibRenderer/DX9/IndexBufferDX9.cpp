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

#include "RendererDX9.h"
#include "MappingsDX9.h"

#include "IndexBufferDX9.h"
using namespace LibRendererDll;

IndexBufferDX9::IndexBufferDX9(const unsigned int indexCount, const IndexBufferFormat indexFormat, const BufferUsage usage)
	: IndexBuffer(indexCount, indexFormat, usage)
	, m_pIndexBuffer(nullptr)
	, m_pTempBuffer(nullptr)
{
	if (indexCount == 0)
		return;
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->CreateIndexBuffer((UINT)m_nSize, BufferUsageDX9[usage], IndexBufferFormatDX9[indexFormat], D3DPOOL_DEFAULT, &m_pIndexBuffer, 0);
	assert(SUCCEEDED(hr));
}

IndexBufferDX9::~IndexBufferDX9()
{
	Unbind();
}

void IndexBufferDX9::Enable()
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetIndices(m_pIndexBuffer);
	assert(SUCCEEDED(hr));
}

void IndexBufferDX9::Disable()
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

#ifdef _DEBUG
	IDirect3DIndexBuffer9* activeBuffer = 0;
	hr = device->GetIndices(&activeBuffer);
	assert(SUCCEEDED(hr));
	assert(activeBuffer == m_pIndexBuffer);
	ULONG refCount = 1;
	refCount = activeBuffer->Release();
	assert(refCount == 1);
#endif

	hr = device->SetIndices(0);
	assert(SUCCEEDED(hr));
}

void IndexBufferDX9::Lock(const BufferLocking lockMode)
{
	assert(m_pTempBuffer == nullptr);
	HRESULT hr = m_pIndexBuffer->Lock(0, 0, &m_pTempBuffer, BufferLockingDX9[lockMode]);
	assert(SUCCEEDED(hr));
}

void IndexBufferDX9::Unlock()
{
	assert(m_pTempBuffer != nullptr);
	HRESULT hr = m_pIndexBuffer->Unlock();
	assert(SUCCEEDED(hr));
	m_pTempBuffer = nullptr;
}

void IndexBufferDX9::Update()
{
	assert(m_pTempBuffer != nullptr);
	memcpy(m_pTempBuffer, GetData(), GetSize());
}

void IndexBufferDX9::Bind()
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->CreateIndexBuffer((UINT)m_nSize, BufferUsageDX9[m_eBufferUsage], IndexBufferFormatDX9[m_eIndexFormat], D3DPOOL_DEFAULT, &m_pIndexBuffer, 0);
	assert(SUCCEEDED(hr));

	Lock(BL_WRITE_ONLY);
	Update();
	Unlock();
}

void IndexBufferDX9::Unbind()
{
	ULONG refCount = 0;
	refCount = m_pIndexBuffer->Release();
	assert(refCount == 0);
	m_pIndexBuffer = nullptr;
}