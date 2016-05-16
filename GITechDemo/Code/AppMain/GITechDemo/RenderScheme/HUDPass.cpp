/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	HUDPass.cpp
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

#include <ft2build.h>
#include FT_FREETYPE_H
#include <ftcache.h>

#include <Renderer.h>
#include <ResourceManager.h>
#include <Texture.h>
#include <RenderState.h>
using namespace Synesthesia3D;

#include "HUDPass.h"
using namespace GITechDemoApp;

#include "RenderResourcesDef.h"

namespace GITechDemoApp
{
	const float HUD_TEX_WIDTH_RATIO = 0.5f;
	const float HUD_TEX_HEIGHT_RATIO = 1.f;

	const int HUD_TEXT_LEFT_MARGIN = 20;
	const int HUD_TEXT_TOP_MARGIN = 20;
}

HUDPass::HUDPass(const char* const passName, RenderPass* const parentPass)
	: RenderPass(passName, parentPass)
	, m_pHUDTexture(nullptr)
	, m_nHUDTextureIdx(-1)
{
	FT_Library		fontLibrary = nullptr;
	FT_Face			face = nullptr;
	FT_GlyphSlot	slot = nullptr;

	FT_Error error = FT_Err_Ok;
	error = FT_Init_FreeType(&fontLibrary);
	assert(error == FT_Err_Ok);

	if (fontLibrary)
	{
		error = FT_New_Face(fontLibrary,
			"fonts/arial.ttf",
			0,
			&face);
		assert(error == FT_Err_Ok);

		if (face)
		{
			error = FT_Set_Char_Size(
				face,		/* handle to face object			*/
				0,			/* char_width in 1/64th of points	*/
				14 * 64,	/* char_height in 1/64th of points	*/
				92,			/* horizontal device resolution		*/
				92);		/* vertical device resolution		*/
			assert(error == FT_Err_Ok);

			slot = face->glyph;	/* a small shortcut */

			for (int i = 0; i < 256; i++)
			{
				error = FT_Load_Char(face, i, FT_LOAD_RENDER);
				assert(error == FT_Err_Ok);

				m_pGlyphCache[i].buffer = new unsigned char[slot->bitmap.width * slot->bitmap.rows];
				memcpy(m_pGlyphCache[i].buffer, slot->bitmap.buffer, slot->bitmap.width * slot->bitmap.rows);
				m_pGlyphCache[i].left = slot->bitmap_left;
				m_pGlyphCache[i].top = slot->bitmap_top;
				m_pGlyphCache[i].width = slot->bitmap.width;
				m_pGlyphCache[i].rows = slot->bitmap.rows;
				m_pGlyphCache[i].advance_x = slot->advance.x;
				m_pGlyphCache[i].advance_y = face->height;
			}

			error = FT_Done_Face(face);
			assert(error == FT_Err_Ok);
		}

		error = FT_Done_FreeType(fontLibrary);
		assert(error == FT_Err_Ok);
	}
}

HUDPass::~HUDPass()
{}

void HUDPass::Update(const float fDeltaTime)
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
	if (!ResourceMgr)
		return;

	if (!m_pHUDTexture ||
		RenderContext->GetDisplayResolution()[0] * HUD_TEX_WIDTH_RATIO != m_pHUDTexture->GetWidth() ||
		RenderContext->GetDisplayResolution()[1] * HUD_TEX_HEIGHT_RATIO != m_pHUDTexture->GetHeight())
	{
		if (m_pHUDTexture)
		{
			ResourceMgr->ReleaseTexture(m_nHUDTextureIdx);
			m_pHUDTexture = nullptr;
		}

		m_nHUDTextureIdx = ResourceMgr->CreateTexture(PF_L8, TT_2D,
			(unsigned int)(RenderContext->GetDisplayResolution()[0] * HUD_TEX_WIDTH_RATIO),
			(unsigned int)(RenderContext->GetDisplayResolution()[1] * HUD_TEX_HEIGHT_RATIO),
			1, 1, BU_DYNAMIC);
		m_pHUDTexture = ResourceMgr->GetTexture(m_nHUDTextureIdx);
	}

	f2HalfTexelOffset = Vec2f(0.5f / m_pHUDTexture->GetWidth(), 0.5f / m_pHUDTexture->GetHeight());
	texSource = m_nHUDTextureIdx;
}

void HUDPass::Draw()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (!RenderContext)
		return;

	RenderState* RSMgr = RenderContext->GetRenderStateManager();
	if (!RSMgr)
		return;

	if (m_szTextBuf.length() == 0)
		return;

	if (m_pHUDTexture->Lock(0, BL_WRITE_ONLY))
	{
		memset(m_pHUDTexture->GetMipData(0), 0, m_pHUDTexture->GetMipSizeBytes(0));

		int pen_x = HUD_TEXT_LEFT_MARGIN;
		int pen_y = HUD_TEXT_TOP_MARGIN;

		for (unsigned int n = 0; n < m_szTextBuf.length(); n++)
		{
			char ch = m_szTextBuf[n];
			if (ch == '\n' || (unsigned int)(pen_x + m_pGlyphCache[ch].left + m_pGlyphCache[ch].width) > m_pHUDTexture->GetWidth())
			{
				pen_x = HUD_TEXT_LEFT_MARGIN;
				pen_y += m_pGlyphCache[ch].advance_y >> 6;
					
				if (ch == '\n')
					continue;
			}

			/* now, draw to our target surface */
			int x = pen_x + m_pGlyphCache[ch].left;
			int y = pen_y - m_pGlyphCache[ch].top;
			int x_max = x + m_pGlyphCache[ch].width;
			int y_max = y + m_pGlyphCache[ch].rows;

			for (int j = y, q = 0; j < y_max; j++, q++)
			{
				if (x < 0 || j < 0 ||
					x_max >= (FT_Int)m_pHUDTexture->GetWidth() ||
					j >= (FT_Int)m_pHUDTexture->GetHeight())
					continue;

				memcpy(
					m_pHUDTexture->GetMipData(0) + j * m_pHUDTexture->GetWidth() + x,
					m_pGlyphCache[ch].buffer + q * m_pGlyphCache[ch].width,
					m_pGlyphCache[ch].width
					);
			}

			/* increment pen position */
			pen_x += m_pGlyphCache[ch].advance_x >> 6;
		}

		m_pHUDTexture->Update();
		m_pHUDTexture->Unlock();
	}

	const bool sRGBEnabled = RSMgr->GetSRGBWriteEnabled();
	const bool blendEnabled = RSMgr->GetColorBlendEnabled();
	const bool zWriteEnable = RSMgr->GetZWriteEnabled();
	const Cmp zFunc = RSMgr->GetZFunc();
	const bool scissorEnabled = RSMgr->GetScissorEnabled();

	RSMgr->SetSRGBWriteEnabled(true);
	RSMgr->SetColorBlendEnabled(false);
	RSMgr->SetZWriteEnabled(false);
	RSMgr->SetZFunc(CMP_ALWAYS);
	RSMgr->SetScissorEnabled(true);

	RenderContext->SetViewport(Vec2i(m_pHUDTexture->GetWidth(), m_pHUDTexture->GetHeight()));
	RSMgr->SetScissor(Vec2i(m_pHUDTexture->GetWidth(), m_pHUDTexture->GetHeight()));

	HUDTextShader.Enable();
	RenderContext->DrawVertexBuffer(FullScreenTri);
	HUDTextShader.Disable();

	RenderContext->SetViewport(RenderContext->GetDisplayResolution());

	RSMgr->SetSRGBWriteEnabled(sRGBEnabled);
	RSMgr->SetColorBlendEnabled(blendEnabled);
	RSMgr->SetZWriteEnabled(zWriteEnable);
	RSMgr->SetZFunc(zFunc);
	RSMgr->SetScissorEnabled(scissorEnabled);

	Clear();
}

void HUDPass::ReleaseHUDTexture()
{
	Renderer* RenderContext = Renderer::GetInstance();
	if (RenderContext)
	{
		ResourceManager* ResourceMgr = RenderContext->GetResourceManager();
		if (ResourceMgr && m_nHUDTextureIdx != ~0u)
			ResourceMgr->ReleaseTexture(m_nHUDTextureIdx);
	}

	m_nHUDTextureIdx = ~0u;
	m_pHUDTexture = nullptr;
}