#ifndef LIGHTING_PASS_H_
#define LIGHTING_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class LightingPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(LightingPass)

	private:
		void CopyDepthBuffer();
	};
}

#endif //LIGHTING_PASS_H_