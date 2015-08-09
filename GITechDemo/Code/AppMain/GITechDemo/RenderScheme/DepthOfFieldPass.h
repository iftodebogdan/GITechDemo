#ifndef DEPTH_OF_FIELD_PASS_H_
#define DEPTH_OF_FIELD_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class DepthOfFieldPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(DepthOfFieldPass)

	private:
		void AutofocusPass();
		void CalculateDoF();
		void ApplyDoF();
	};
}

#endif //DEPTH_OF_FIELD_PASS_H_
