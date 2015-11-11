#ifndef SSAO_PASS_H_
#define SSAO_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class RenderTarget;

	class SSAOPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(SSAOPass)

	private:
		void CalculateSSAO();
		void BlurSSAO();
		void ApplySSAO();

		RenderTarget**	SSAOBuffer;
		unsigned int	BlurKernelCount;
	};
}

#endif //SSAO_PASS_H_