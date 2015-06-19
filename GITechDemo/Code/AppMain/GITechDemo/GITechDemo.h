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
		GITechDemo();
		~GITechDemo();

		void Init(void* hWnd);
		void Update(const float fDeltaTime);
		void Draw();

	private:
		void AllocateRenderResources();
		void UpdateMatrices();
		void GenerateDirectionalShadowMap();
		void GenerateRSM();
		void GenerateGBuffer();
		void CopyDepthBuffer();
		void DrawSky();
		void CalculateAmbientOcclusion();
		void BlurAmbientOcclusion();
		void ApplyAmbientOcclusion();
		void AccumulateAmbientOcclusion();
		void AccumulateAmbientLight();
		void AccumulateDirectionalLight();
		void AccumulateIndirectLight();
		void AccumulateLight();
		void HDRDownsamplePass();
		void LuminanceMeasurementPass();
		void HDRToneMappingPass();
		void FxaaPass();
		void BloomDownsample();
		void BloomBlur();
		void BloomApply();
		void BloomPass();
		void CalculateDoF();
		void ApplyDoF();
		void DepthOfFieldPass();
		void ApplyPostProcessing();
		void CopyResultToBackBuffer(LibRendererDll::RenderTarget* const rt);
		void RenderScene();

		void* pHWND;
	};
}
#endif