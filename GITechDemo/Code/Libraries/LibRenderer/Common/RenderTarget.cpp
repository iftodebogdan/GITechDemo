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

#include "RenderTarget.h"
#include "RendererDX9.h"
using namespace LibRendererDll;

RenderTarget::RenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: m_nTargetCount(targetCount)
	, m_nWidth(width)
	, m_nHeight(height)
	, m_bHasMipmaps(hasMipmaps)
	, m_bHasDepthStencil(hasDepthStencil)
	, m_nColorBufferTexIdx(nullptr)
	, m_nDepthBufferTexIdx(UINT_MAX)
	, m_pColorBuffer(nullptr)
	, m_pDepthBuffer(nullptr)
{
	assert(targetCount > 0);
	assert(targetCount <= Renderer::GetInstance()->GetDeviceCaps().nNumSimultaneousRTs);
	assert(pixelFormat != PF_NONE);

	m_nColorBufferTexIdx = new unsigned int[m_nTargetCount];
	m_pColorBuffer = new Texture*[m_nTargetCount];
	
	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		m_nColorBufferTexIdx[i] = Renderer::GetInstance()->GetResourceManager()->CreateTexture(
			pixelFormat, TT_2D, width, height, 1, hasMipmaps ? 0 : 1, BU_RENDERTAGET);
		m_pColorBuffer[i] = Renderer::GetInstance()->GetResourceManager()->GetTexture(m_nColorBufferTexIdx[i]);
	}

	if (hasDepthStencil)
	{
		m_nDepthBufferTexIdx = Renderer::GetInstance()->GetResourceManager()->CreateTexture(
			depthStencilFormat, TT_2D, width, height, 1, 1, BU_DEPTHSTENCIL);
		m_pDepthBuffer = Renderer::GetInstance()->GetResourceManager()->GetTexture(m_nDepthBufferTexIdx);
	}
}

RenderTarget::RenderTarget(const unsigned int targetCount,
	PixelFormat PixelFormatRT0, PixelFormat PixelFormatRT1, PixelFormat PixelFormatRT2, PixelFormat PixelFormatRT3,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: m_nTargetCount(targetCount)
	, m_nWidth(width)
	, m_nHeight(height)
	, m_bHasMipmaps(hasMipmaps)
	, m_bHasDepthStencil(hasDepthStencil)
	, m_nColorBufferTexIdx(nullptr)
	, m_nDepthBufferTexIdx(UINT_MAX)
	, m_pColorBuffer(nullptr)
	, m_pDepthBuffer(nullptr)
{
	assert(targetCount > 0);
	assert(targetCount <= 4);
	assert(targetCount <= Renderer::GetInstance()->GetDeviceCaps().nNumSimultaneousRTs);

	m_nColorBufferTexIdx = new unsigned int[m_nTargetCount];
	m_pColorBuffer = new Texture*[m_nTargetCount];

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		PixelFormat pf = PF_NONE;

		switch (i)
		{
		case 0:
			pf = PixelFormatRT0;
			break;
		case 1:
			pf = PixelFormatRT1;
			break;
		case 2:
			pf = PixelFormatRT2;
			break;
		case 3:
			pf = PixelFormatRT3;
		}

		assert(pf != PF_NONE);

		m_nColorBufferTexIdx[i] = Renderer::GetInstance()->GetResourceManager()->CreateTexture(
			pf, TT_2D, width, height, 1, hasMipmaps ? 0 : 1, BU_RENDERTAGET);
		m_pColorBuffer[i] = Renderer::GetInstance()->GetResourceManager()->GetTexture(m_nColorBufferTexIdx[i]);
	}

	if (hasDepthStencil)
	{
		m_nDepthBufferTexIdx = Renderer::GetInstance()->GetResourceManager()->CreateTexture(
			depthStencilFormat, TT_2D, width, height, 1, 1, BU_DEPTHSTENCIL);
		m_pDepthBuffer = Renderer::GetInstance()->GetResourceManager()->GetTexture(m_nDepthBufferTexIdx);
	}
}

RenderTarget::RenderTarget(const unsigned int targetCount,
	PixelFormat PixelFormatRT0, PixelFormat PixelFormatRT1, PixelFormat PixelFormatRT2, PixelFormat PixelFormatRT3,
	bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: m_nTargetCount(targetCount)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_bHasMipmaps(hasMipmaps)
	, m_bHasDepthStencil(hasDepthStencil)
	, m_nColorBufferTexIdx(nullptr)
	, m_nDepthBufferTexIdx(UINT_MAX)
	, m_pColorBuffer(nullptr)
	, m_pDepthBuffer(nullptr)
{
	assert(targetCount > 0);
	assert(targetCount <= 4);
	assert(targetCount <= Renderer::GetInstance()->GetDeviceCaps().nNumSimultaneousRTs);

	m_nColorBufferTexIdx = new unsigned int[m_nTargetCount];
	m_pColorBuffer = new Texture*[m_nTargetCount];

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		PixelFormat pf = PF_NONE;

		switch (i)
		{
		case 0:
			pf = PixelFormatRT0;
			break;
		case 1:
			pf = PixelFormatRT1;
			break;
		case 2:
			pf = PixelFormatRT2;
			break;
		case 3:
			pf = PixelFormatRT3;
		}

		assert(pf != PF_NONE);

		m_nColorBufferTexIdx[i] = Renderer::GetInstance()->GetResourceManager()->CreateTexture(
			pf, TT_2D, 0, 0, 1, hasMipmaps ? 0 : 1, BU_RENDERTAGET);
		m_pColorBuffer[i] = Renderer::GetInstance()->GetResourceManager()->GetTexture(m_nColorBufferTexIdx[i]);
	}

	if (hasDepthStencil)
	{
		m_nDepthBufferTexIdx = Renderer::GetInstance()->GetResourceManager()->CreateTexture(
			depthStencilFormat, TT_2D, 0, 0, 1, 1, BU_DEPTHSTENCIL);
		m_pDepthBuffer = Renderer::GetInstance()->GetResourceManager()->GetTexture(m_nDepthBufferTexIdx);
	}
}

RenderTarget::~RenderTarget()
{
	Unbind();

	if (m_pColorBuffer)
		delete[] m_pColorBuffer;

	if (m_nColorBufferTexIdx)
		delete[] m_nColorBufferTexIdx;
}

void RenderTarget::Bind()
{
	for (unsigned int i = 0; i < m_nTargetCount; i++)
		m_pColorBuffer[i]->Bind();

	if (m_bHasDepthStencil)
		m_pDepthBuffer->Bind();
}

void RenderTarget::Unbind()
{
	for (unsigned int i = 0; i < m_nTargetCount; i++)
		if (m_pColorBuffer && m_pColorBuffer[i])
			m_pColorBuffer[i]->Unbind();

	if (m_pDepthBuffer)
		m_pDepthBuffer->Unbind();
}