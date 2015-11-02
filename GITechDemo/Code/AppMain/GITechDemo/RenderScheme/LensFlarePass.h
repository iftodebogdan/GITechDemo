#ifndef LENS_FLARE_PASS_H_
#define LENS_FLARE_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class LensFlarePass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(LensFlarePass)

	private:
		void	ApplyBrightnessFilter();
		void	GenerateFeatures();
		void	Blur();
		void	UpscaleAndBlend();
	};
}

#endif // LENS_FLARE_PASS_H_
