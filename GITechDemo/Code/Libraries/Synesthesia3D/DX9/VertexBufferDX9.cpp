/**
 *	@file		VertexBufferDX9.cpp
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

#include "stdafx.h"

#include "RendererDX9.h"
#include "MappingsDX9.h"

#include "VertexFormatDX9.h"
#include "VertexBufferDX9.h"
#include "IndexBufferDX9.h"
using namespace Synesthesia3D;

VertexBufferDX9::VertexBufferDX9(VertexFormatDX9* const vertexFormat, const unsigned int vertexCount, IndexBufferDX9* const indexBuffer, const BufferUsage usage)
	: VertexBuffer(vertexFormat, vertexCount, indexBuffer, usage)
	, m_pVertexBuffer(nullptr)
	, m_pTempBuffer(nullptr)
{
	if (vertexCount != 0)
	{
		PUSH_PROFILE_MARKER(__FUNCSIG__);
		Bind();
		POP_PROFILE_MARKER();
	}
}

VertexBufferDX9::~VertexBufferDX9()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	Unbind();

	POP_PROFILE_MARKER();
}

void VertexBufferDX9::Enable(const unsigned int offset)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(offset < GetElementCount());

	//Enable the proper vertex format for our vertex buffer
	assert(m_pVertexFormat != nullptr);
	m_pVertexFormat->Enable();

	if (m_pIndexBuffer)
		m_pIndexBuffer->Enable();

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->SetStreamSource(0, m_pVertexBuffer, offset * m_nElementSize, m_pVertexFormat->GetStride());
	S3D_VALIDATE_HRESULT(hr);

	POP_PROFILE_MARKER();
}

void VertexBufferDX9::Disable()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

#ifdef _DEBUG
	//Check to see if this vertex buffer is the one currently enabled
	IDirect3DVertexBuffer9* dbgBuffer = 0;
	unsigned int dbgOffset = 0;
	unsigned int dbgStride = 0;
	hr = device->GetStreamSource(0, &dbgBuffer, &dbgOffset, &dbgStride);
	assert(hr == D3D_OK);
	assert(dbgBuffer == m_pVertexBuffer);
	ULONG refCount = 1;
	refCount = dbgBuffer->Release();
	assert(refCount == 1);
#endif
	
	hr = device->SetStreamSource(0, 0, 0, 0);
	S3D_VALIDATE_HRESULT(hr);

	if (m_pIndexBuffer)
		m_pIndexBuffer->Disable();

	//Disable our vertex format
	assert(m_pVertexFormat != nullptr);
	m_pVertexFormat->Disable();

	POP_PROFILE_MARKER();
}

void VertexBufferDX9::Lock(const BufferLocking lockMode)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	//The pointer to the locked data is saved for future use
	assert(m_pTempBuffer == nullptr);
	HRESULT hr = m_pVertexBuffer->Lock(0, 0, &m_pTempBuffer, BufferLockingDX9[lockMode]);
	S3D_VALIDATE_HRESULT(hr);

	POP_PROFILE_MARKER();
}

void VertexBufferDX9::Unlock()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	//Unlock the vertex data
	assert(m_pTempBuffer != nullptr);
	HRESULT hr = m_pVertexBuffer->Unlock();
	S3D_VALIDATE_HRESULT(hr);
	m_pTempBuffer = nullptr;

	POP_PROFILE_MARKER();
}

void VertexBufferDX9::Update()
{
	//Copy the local changes to our vertex buffer to where the locked data is
	assert(m_pTempBuffer != nullptr);
	memcpy(m_pTempBuffer, GetData(), GetSize());
}

void VertexBufferDX9::Bind()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr = device->CreateVertexBuffer((UINT)m_nSize, BufferUsageDX9[m_eBufferUsage], 0, D3DPOOL_DEFAULT, &m_pVertexBuffer, 0);
	S3D_VALIDATE_HRESULT(hr);

	Lock(BL_WRITE_ONLY);
	Update();
	Unlock();

	POP_PROFILE_MARKER();
}

void VertexBufferDX9::Unbind()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	ULONG refCount = 0;
	if(m_pVertexBuffer)
		refCount = m_pVertexBuffer->Release();
	assert(refCount == 0);
	m_pVertexBuffer = nullptr;

	POP_PROFILE_MARKER();
}