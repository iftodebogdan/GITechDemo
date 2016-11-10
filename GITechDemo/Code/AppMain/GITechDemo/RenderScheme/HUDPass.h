/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	HUDPass.h
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

#ifndef HUD_PASS_H_
#define HUD_PASS_H_

#include <string>
#include "RenderPass.h"

namespace Synesthesia3D
{
	class Texture;
}

namespace GITechDemoApp
{
	class HUDPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(HUDPass)

	private:
		struct GlyphCache
		{
			unsigned char* buffer;
			int left;
			int top;
			int width;
			int rows;
			int advance_x;
			int advance_y;

			GlyphCache() : buffer(nullptr) {}
			~GlyphCache() { if(buffer) delete[] buffer; }
		};

		GlyphCache					m_pGlyphCache[256];

		std::string					m_szTextBuf;

		Synesthesia3D::Texture*		m_pHUDTexture;
		unsigned int				m_nHUDTextureIdx;

	public:
		void Clear() { m_szTextBuf.clear(); }
		void PrintLn(const char* const text) { m_szTextBuf = m_szTextBuf + text + "\n"; }
		void Print(const char* const text) { m_szTextBuf += text; }

		void ReleaseHUDTexture();
	};
}

#endif //HUD_PASS_H_