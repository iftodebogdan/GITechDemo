#ifndef MOTION_BLUR_PASS_H_
#define MOTION_BLUR_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class MotionBlurPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(MotionBlurPass)

	private:
		void CalculateMotionBlur();
		void ApplyMotionBlur();
	};
}

#endif // MOTION_BLUR_PASS_H_
