#ifndef GITECHDEMO_H_
#define GITECHDEMO_H_

#include "App.h"

#include <gmtl\gmtl.h>
#include <RenderTarget.h>

namespace GITechDemoApp
{
	class GITechDemo : public App
	{
	public:
		IMPLEMENT_APP(GITechDemo)

		const float GetDeltaTime() { return m_fDeltaTime; }

	private:
		void AllocateRenderResources();
		void* m_pHWND;

		float m_fDeltaTime;
	};
}
#endif