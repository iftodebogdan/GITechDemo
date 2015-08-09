#ifndef APP_H_
#define APP_H_

#define CREATE_APP(CLASS) App* AppMain = new CLASS();

#define IMPLEMENT_APP(CLASS) \
	CLASS (); \
	~ CLASS (); \
	void Init(void* hWnd); \
	void Update(const float fDeltaTime); \
	void Draw();

#include <gmtl\gmtl.h>
using namespace gmtl;

class App
{
public:
	virtual ~App() {}

	virtual void Init(void* hWnd) = 0;
	virtual void Update(const float fDeltaTime) = 0;
	virtual void Draw() = 0;

	struct Camera
	{
		Camera() : fSpeedFactor(1.f) {}

		Vec3f vPos;
		Matrix44f mRot;
		Vec3f vMoveVec;
		float fSpeedFactor;
	};

	Camera& GetCamera() { return tCamera; }

protected:
	Camera tCamera;
};

extern App* AppMain;

#endif