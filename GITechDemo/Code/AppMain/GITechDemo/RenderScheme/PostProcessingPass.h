#ifndef POST_PROCESSING_PASS_H_
#define POST_PROCESSING_PASS_H_

#include "RenderPass.h"

namespace LibRendererDll
{
	class RenderTarget;
}

namespace GITechDemoApp
{
	class PostProcessingPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(PostProcessingPass)

	private:
		void CopyResultToBackBuffer(LibRendererDll::RenderTarget* const rt);

		LibRendererDll::RenderTarget*	m_pFinalImageBuffer;
	};
}

#endif //POST_PROCESSING_PASS_H_