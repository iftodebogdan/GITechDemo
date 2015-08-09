#ifndef HDR_DOWNSAMPLE_PASS_H_
#define HDR_DOWNSAMPLE_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class RenderTarget;

	class HDRDownsamplePass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(HDRDownsamplePass)

	private:
		void DownsamplePass(GITechDemoApp::RenderTarget* const pSource, GITechDemoApp::RenderTarget* const pDest);
	};
}

#endif //HDR_DOWNSAMPLE_PASS_H_