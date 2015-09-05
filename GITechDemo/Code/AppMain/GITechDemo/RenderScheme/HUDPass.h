#ifndef HUD_PASS_H_
#define HUD_PASS_H_

#include <string>
#include "RenderPass.h"

namespace LibRendererDll
{
	class Texture;
}

namespace GITechDemoApp
{
	class HUDPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(HUDPass);

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

		LibRendererDll::Texture*	m_pHUDTexture;
		unsigned int				m_nHUDTextureIdx;

	public:
		void Clear() { m_szTextBuf.clear(); }
		void PrintLn(const char* const text) { m_szTextBuf = m_szTextBuf + text + "\n"; }
		void Print(const char* const text) { m_szTextBuf += text; }

	};
}

#endif //HUD_PASS_H_