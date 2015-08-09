#ifndef RENDER_SCHEME_H_
#define RENDER_SCHEME_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class RenderScheme
	{
	public:
		static RenderPass&	GetRootPass() { return RootPass; }
		static void			Draw() { RootPass.Draw(); }

	private:
		static RenderPass	RootPass;
	};
}

#endif //RENDER_SCHEME_H_