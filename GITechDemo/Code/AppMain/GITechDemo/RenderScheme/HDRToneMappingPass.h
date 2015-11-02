#ifndef HDR_TONE_MAPPING_PASS_H_
#define HDR_TONE_MAPPING_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class HDRToneMappingPass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(HDRToneMappingPass)

	private:
		void LuminanceMeasurementPass();
		void LuminanceAdaptationPass();
		void ToneMappingPass();
	};
}

#endif //HDR_TONE_MAPPING_PASS_H_