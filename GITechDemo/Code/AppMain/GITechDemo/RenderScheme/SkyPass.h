#ifndef SKY_PASS_H_
#define SKY_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class SkyPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(SkyPass);

	private:
		void CreateSkyBoxVB();
	};
}

#endif //SKY_PASS_H_