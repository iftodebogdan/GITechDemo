/*=============================================================================
 *	This file is part of the "Synesthesia3D" graphics engine
 *	Copyright (C) 2014-2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	RenderTarget.cpp
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#include "stdafx.h"

#include "RenderTarget.h"
#include "Renderer.h"
#include "Texture.h"
#include "ResourceManager.h"
using namespace Synesthesia3D;

RenderTarget* RenderTarget::ms_pActiveRenderTarget = nullptr;

RenderTarget::RenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: m_nTargetCount(targetCount)
	, m_nWidth(width)
	, m_nHeight(height)
	, m_fWidthRatio(0)
	, m_fHeightRatio(0)
	, m_bHasMipmaps(hasMipmaps)
	, m_bHasDepthStencil(hasDepthStencil)
	, m_nColorBufferTexIdx(nullptr)
	, m_nDepthBufferTexIdx(UINT_MAX)
	, m_pColorBuffer(nullptr)
	, m_pDepthBuffer(nullptr)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(targetCount > 0);
	assert(targetCount <= Renderer::GetInstance()->GetDeviceCaps().nNumSimultaneousRTs
		|| Renderer::GetAPI() == API_NULL);
	//assert(pixelFormat != PF_NONE);

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

	POP_PROFILE_MARKER();
}

RenderTarget::RenderTarget(const unsigned int targetCount, PixelFormat pixelFormat,
	const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: m_nTargetCount(targetCount)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_fWidthRatio(widthRatio)
	, m_fHeightRatio(heightRatio)
	, m_bHasMipmaps(hasMipmaps)
	, m_bHasDepthStencil(hasDepthStencil)
	, m_nColorBufferTexIdx(nullptr)
	, m_nDepthBufferTexIdx(UINT_MAX)
	, m_pColorBuffer(nullptr)
	, m_pDepthBuffer(nullptr)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(targetCount > 0);
	assert(targetCount <= Renderer::GetInstance()->GetDeviceCaps().nNumSimultaneousRTs
		|| Renderer::GetAPI() == API_NULL);
	//assert(pixelFormat != PF_NONE);

	m_nColorBufferTexIdx = new unsigned int[m_nTargetCount];
	m_pColorBuffer = new Texture*[m_nTargetCount];

	for (unsigned int i = 0; i < m_nTargetCount; i++)
	{
		m_nColorBufferTexIdx[i] = Renderer::GetInstance()->GetResourceManager()->CreateTexture(
			pixelFormat, TT_2D, 0, 0, 1, hasMipmaps ? 0 : 1, BU_RENDERTAGET);
		m_pColorBuffer[i] = Renderer::GetInstance()->GetResourceManager()->GetTexture(m_nColorBufferTexIdx[i]);
		m_pColorBuffer[i]->SetDynamicSizeRatios(widthRatio, heightRatio);
	}

	if (hasDepthStencil)
	{
		m_nDepthBufferTexIdx = Renderer::GetInstance()->GetResourceManager()->CreateTexture(
			depthStencilFormat, TT_2D, 0, 0, 1, 1, BU_DEPTHSTENCIL);
		m_pDepthBuffer = Renderer::GetInstance()->GetResourceManager()->GetTexture(m_nDepthBufferTexIdx);
		m_pDepthBuffer->SetDynamicSizeRatios(widthRatio, heightRatio);
	}

	POP_PROFILE_MARKER();
}

RenderTarget::RenderTarget(const unsigned int targetCount,
	PixelFormat PixelFormatRT0, PixelFormat PixelFormatRT1, PixelFormat PixelFormatRT2, PixelFormat PixelFormatRT3,
	const unsigned int width, const unsigned int height, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: m_nTargetCount(targetCount)
	, m_nWidth(width)
	, m_nHeight(height)
	, m_fWidthRatio(0)
	, m_fHeightRatio(0)
	, m_bHasMipmaps(hasMipmaps)
	, m_bHasDepthStencil(hasDepthStencil)
	, m_nColorBufferTexIdx(nullptr)
	, m_nDepthBufferTexIdx(UINT_MAX)
	, m_pColorBuffer(nullptr)
	, m_pDepthBuffer(nullptr)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(targetCount > 0);
	assert(targetCount <= 4);
	assert(targetCount <= Renderer::GetInstance()->GetDeviceCaps().nNumSimultaneousRTs
		|| Renderer::GetAPI() == API_NULL);

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

		//assert(pf != PF_NONE);

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

	POP_PROFILE_MARKER();
}

RenderTarget::RenderTarget(const unsigned int targetCount,
	PixelFormat PixelFormatRT0, PixelFormat PixelFormatRT1, PixelFormat PixelFormatRT2, PixelFormat PixelFormatRT3,
	const float widthRatio, const float heightRatio, bool hasMipmaps, bool hasDepthStencil, PixelFormat depthStencilFormat)
	: m_nTargetCount(targetCount)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_fWidthRatio(widthRatio)
	, m_fHeightRatio(heightRatio)
	, m_bHasMipmaps(hasMipmaps)
	, m_bHasDepthStencil(hasDepthStencil)
	, m_nColorBufferTexIdx(nullptr)
	, m_nDepthBufferTexIdx(UINT_MAX)
	, m_pColorBuffer(nullptr)
	, m_pDepthBuffer(nullptr)
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	assert(targetCount > 0);
	assert(targetCount <= 4);
	assert(targetCount <= Renderer::GetInstance()->GetDeviceCaps().nNumSimultaneousRTs
		|| Renderer::GetAPI() == API_NULL);

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
		m_pColorBuffer[i]->SetDynamicSizeRatios(widthRatio, heightRatio);
	}

	if (hasDepthStencil)
	{
		m_nDepthBufferTexIdx = Renderer::GetInstance()->GetResourceManager()->CreateTexture(
			depthStencilFormat, TT_2D, 0, 0, 1, 1, BU_DEPTHSTENCIL);
		m_pDepthBuffer = Renderer::GetInstance()->GetResourceManager()->GetTexture(m_nDepthBufferTexIdx);
		m_pDepthBuffer->SetDynamicSizeRatios(widthRatio, heightRatio);
	}

	POP_PROFILE_MARKER();
}

RenderTarget::~RenderTarget()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	Unbind();

	if (m_pColorBuffer)
		delete[] m_pColorBuffer;

	if (m_nColorBufferTexIdx)
		delete[] m_nColorBufferTexIdx;

	POP_PROFILE_MARKER();
}

void RenderTarget::Bind()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	for (unsigned int i = 0; i < m_nTargetCount; i++)
		m_pColorBuffer[i]->Bind();

	if (m_bHasDepthStencil)
		m_pDepthBuffer->Bind();

	POP_PROFILE_MARKER();
}

void RenderTarget::Unbind()
{
	PUSH_PROFILE_MARKER(__FUNCSIG__);

	for (unsigned int i = 0; i < m_nTargetCount; i++)
		if (m_pColorBuffer && m_pColorBuffer[i])
			m_pColorBuffer[i]->Unbind();

	if (m_pDepthBuffer)
		m_pDepthBuffer->Unbind();

	POP_PROFILE_MARKER();
}

const unsigned int RenderTarget::GetTargetCount() const
{
	return m_nTargetCount;
}

const PixelFormat RenderTarget::GetPixelFormat(const unsigned int colorBufferIdx) const
{
	assert(colorBufferIdx < m_nTargetCount);
	return m_pColorBuffer[colorBufferIdx]->GetPixelFormat();
}

const unsigned int RenderTarget::GetWidth() const
{
	return m_pColorBuffer[0]->GetWidth();
}

const unsigned int RenderTarget::GetHeight() const
{
	return m_pColorBuffer[0]->GetHeight();
}

const unsigned int RenderTarget::GetColorBuffer(const unsigned int colorBufferIdx) const
{
	assert(colorBufferIdx < m_nTargetCount);
	return m_nColorBufferTexIdx[colorBufferIdx];
}

const unsigned int RenderTarget::GetDepthBuffer() const
{
	return m_nDepthBufferTexIdx;
}

const bool RenderTarget::HasMipmaps() const
{
	return m_bHasMipmaps;
}

const bool RenderTarget::HasDepthBuffer() const
{
	return m_pDepthBuffer != 0;
}

RenderTarget* RenderTarget::GetActiveRenderTarget()
{
	return ms_pActiveRenderTarget;
}

void RenderTarget::SetActiveRenderTarget(RenderTarget* const activeRenderTarget)
{
	ms_pActiveRenderTarget = activeRenderTarget;
}

void RenderTarget::Enable()
{
	SetActiveRenderTarget(this);
}

void RenderTarget::Disable()
{
	SetActiveRenderTarget(nullptr);
}