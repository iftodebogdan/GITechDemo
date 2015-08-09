#ifndef AMBIENT_OCCLUSION_PASS_H_
#define AMBIENT_OCCLUSION_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class RenderTarget;

	class AmbientOcclusionPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(AmbientOcclusionPass)

	private:
		void CalculateAmbientOcclusion();
		void BlurAmbientOcclusion();
		void ApplyAmbientOcclusion();

		RenderTarget**	AmbientOcclusionBuffer;
		unsigned int	BlurKernelCount;
	};
}

#endif //AMBIENT_OCCLUSION_PASS_H_