#ifndef GITECHDEMO_H_
#define GITECHDEMO_H_

#include <gmtl\gmtl.h>

#include "App.h"
using namespace AppFramework;

namespace gainput
{
	class InputMap;
}

namespace GITechDemoApp
{
	class GITechDemo : public App
	{
	public:
		IMPLEMENT_APP(GITechDemo)

		struct Camera
		{
			Camera() : fSpeedFactor(1.f) {}

			Vec3f vPos;
			Matrix44f mRot;
			Vec3f vMoveVec;
			float fSpeedFactor;
		};

		Camera& GetCamera() { return m_tCamera; }
		const float GetDeltaTime() { return m_fDeltaTime; }

	private:
		Camera m_tCamera;
		void* m_pHWND;
		float m_fDeltaTime;
		gainput::InputMap* m_pInputMap;

		enum Command
		{
			APP_CMD_NONE		=			0,					// No input
			APP_CMD_CTRL_YAW	=			1,					// Enable yaw control 
			APP_CMD_CTRL_PITCH	= APP_CMD_CTRL_YAW		<< 1,	// Enable pitch control
			APP_CMD_CTRL_ROLL	= APP_CMD_CTRL_PITCH	<< 1,	// Enable roll control
			APP_CMD_YAW_AXIS	= APP_CMD_CTRL_ROLL		<< 1,	// Analog yaw input
			APP_CMD_PITCH_AXIS	= APP_CMD_YAW_AXIS		<< 1,	// Analog pitch input
			APP_CMD_ROLL_AXIS	= APP_CMD_PITCH_AXIS	<< 1,	// Analog roll input
			APP_CMD_FORWARD		= APP_CMD_ROLL_AXIS		<< 1,	// Camera move forward
			APP_CMD_BACKWARD	= APP_CMD_FORWARD		<< 1,	// Camera move backward
			APP_CMD_LEFT		= APP_CMD_BACKWARD		<< 1,	// Camera move left
			APP_CMD_RIGHT		= APP_CMD_LEFT			<< 1,	// Camera move right
			APP_CMD_SPEED_UP	= APP_CMD_RIGHT			<< 1,	// Enable faster moving camera
			APP_CMD_SLOW_DOWN	= APP_CMD_SPEED_UP		<< 1	// Enable slower moving camera
		};
	};
}
#endif