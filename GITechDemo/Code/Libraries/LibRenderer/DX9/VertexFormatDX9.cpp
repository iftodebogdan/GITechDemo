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
#include "VertexFormatDX9.h"
using namespace LibRendererDll;

VertexFormatDX9::VertexFormatDX9(const unsigned int attributeCount)
	: VertexFormat(attributeCount)
	, m_pVertexDeclaration(nullptr)
{}

VertexFormatDX9::~VertexFormatDX9()
{
	Unbind();
}

void VertexFormatDX9::Enable()
{
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	
	//Sync our vertex declaration before enabling it
	if (!m_pVertexDeclaration)
		Update();

	HRESULT hr = device->SetVertexDeclaration(m_pVertexDeclaration);
	assert(SUCCEEDED(hr));
}

void VertexFormatDX9::Disable()
{
	//Apparently this is not needed, resulting in a warning when using the debug DX9 libraries
	/*
	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();
	HRESULT hr;

#ifdef _DEBUG
	//Check to see if this vertex declaration is the one currently enabled
	IDirect3DVertexDeclaration9* activeDecl = nullptr;
	hr = device->GetVertexDeclaration(&activeDecl);
	assert(SUCCEEDED(hr));
	assert(activeDecl == m_pVertexDeclaration);
	ULONG refCount = 0;
	refCount = activeDecl->Release();
	assert(refCount == 1);
#endif //_DEBUG

	hr = device->SetVertexDeclaration(0);
	assert(SUCCEEDED(hr));
	*/
}

void VertexFormatDX9::Update()
{
	ULONG refCount = 0;
	if (m_pVertexDeclaration)
		refCount = m_pVertexDeclaration->Release();
	assert(refCount == 0);

	IDirect3DDevice9* device = RendererDX9::GetInstance()->GetDevice();

	for (unsigned int i = 0; i < m_nAttributeCount; i++)
	{
		m_pVertexElements[i].Method = D3DDECLMETHOD_DEFAULT;
		m_pVertexElements[i].Offset = (WORD)GetOffset(i);
		m_pVertexElements[i].Stream = (WORD)0;
		m_pVertexElements[i].Type = VertexAttributeTypeDX9[GetAttributeType(i)];
		m_pVertexElements[i].Usage = VertexAttributeUsageDX9[GetAttributeUsage(i)];
		m_pVertexElements[i].UsageIndex = (BYTE)GetUsageIndex(i);
	}

	m_pVertexElements[m_nAttributeCount].Method = 0;
	m_pVertexElements[m_nAttributeCount].Offset = 0;
	m_pVertexElements[m_nAttributeCount].Stream = 0xFF;
	m_pVertexElements[m_nAttributeCount].Type = D3DDECLTYPE_UNUSED;
	m_pVertexElements[m_nAttributeCount].Usage = 0;
	m_pVertexElements[m_nAttributeCount].UsageIndex = 0;

	HRESULT hr = device->CreateVertexDeclaration(m_pVertexElements, &m_pVertexDeclaration);
	assert(SUCCEEDED(hr));
}

void VertexFormatDX9::Bind()
{
	Update();
}

void VertexFormatDX9::Unbind()
{
	ULONG refCount = 0;
	refCount = m_pVertexDeclaration->Release();
	assert(refCount == 0);
	m_pVertexDeclaration = nullptr;
}