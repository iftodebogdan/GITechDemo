#ifndef DIRECTIONAL_LIGHT_VOLUME_PASS_H_
#define DIRECTIONAL_LIGHT_VOLUME_PASS_H_

#include "RenderPass.h"

namespace GITechDemoApp
{
	class RenderTarget;

	class DirectionalLightVolumePass : public RenderPass
	{
		IMPLEMENT_RENDER_PASS(DirectionalLightVolumePass)

	protected:
		void	CalculateLightVolume();
		void	GatherSamples();
		void	ApplyLightVolume();

	private:
		RenderTarget** VolumetricLightAccumulationBuffer;
	};
}

#endif // DIRECTIONAL_LIGHT_VOLUME_PASS_H_