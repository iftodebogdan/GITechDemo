#ifndef SHADOW_MAP_DIRECTIONAL_LIGHT_PASS_H_
#define SHADOW_MAP_DIRECTIONAL_LIGHT_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class ShadowMapDirectionalLightPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(ShadowMapDirectionalLightPass)

	private:
		void UpdateSceneAABB();
	};
}

#endif //SHADOW_MAP_DIRECTIONAL_LIGHT_PASS_H_
