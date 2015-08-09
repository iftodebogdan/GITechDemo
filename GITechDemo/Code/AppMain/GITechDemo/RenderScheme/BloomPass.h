#ifndef BLOOM_PASS_H_
#define BLOOM_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class BloomPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(BloomPass)

	private:
		void BloomBrightnessFilter();
		void BloomBlur();
		void BloomApply();
	};
}

#endif //BLOOM_PASS_H_
