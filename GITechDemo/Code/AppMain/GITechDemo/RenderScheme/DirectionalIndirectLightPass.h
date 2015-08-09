#ifndef DIRECTIONAL_INDIRECT_LIGHT_PASS_H_
#define DIRECTIONAL_INDIRECT_LIGHT_PASS_H_

#include "gmtl\gmtl.h"
using namespace gmtl;

#include "RenderPass.h"

namespace GITechDemoApp
{
	class DirectionalIndirectLightPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(DirectionalIndirectLightPass)
	};
}

#endif //DIRECTIONAL_INDIRECT_LIGHT_PASS_H_