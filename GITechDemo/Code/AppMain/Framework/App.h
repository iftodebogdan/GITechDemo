#ifndef APP_H_
#define APP_H_

#define CREATE_APP(CLASS) \
	App* AppMain = new CLASS();

#include <gmtl\gmtl.h>
using namespace gmtl;

class App
{
public:
	virtual ~App() {}

	virtual void Init(void* hWnd) = 0;
	virtual void Update(const float fDeltaTime) = 0;
	virtual void Draw() = 0;

protected:
	struct Camera
	{
		Camera() : fSpeedFactor(1.f) {}

		Vec3f vPos;
		Matrix44f mRot;
		Vec3f vMoveVec;
		float fSpeedFactor;
	} tCamera;

public:
	Camera& GetCamera() { return tCamera; }
};

extern App* AppMain;

#endif