#ifndef SKY_PASS_H_
#define SKY_PASS_H_

#include "RenderPass.h"

namespace LibRendererDll
{
	class VertexBuffer;
}

namespace GITechDemoApp
{
	class SkyPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(SkyPass);

	private:
		void CreateSkyBoxVB();

		// A cube used to draw the sky
		LibRendererDll::VertexBuffer*	m_pSkyBoxCube;
	};
}

#endif //SKY_PASS_H_