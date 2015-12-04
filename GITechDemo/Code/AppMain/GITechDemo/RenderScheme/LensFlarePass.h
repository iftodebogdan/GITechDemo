#ifndef LENS_FLARE_PASS_H_
#define LENS_FLARE_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class RenderTarget;

	class LensFlarePass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(LensFlarePass)

	private:
		void	ApplyBrightnessFilter();
		void	GenerateFeatures();
		void	Blur();
		void	UpscaleAndBlend();

		RenderTarget** CurrentLensFlareBuffer;
	};
}

#endif // LENS_FLARE_PASS_H_
