#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

namespace AppFramework
{
	class Framework
	{
	public:
		Framework() { m_pInstance = this; };
		virtual ~Framework() { m_pInstance = nullptr; };

		virtual int Run() = 0;
		static Framework* const GetInstance() { return m_pInstance; }

		const bool	IsRenderingPaused() { return m_bPauseRendering; }

		// Low level, platform specific functionality required by the application
		virtual void ShowCursor(const bool bShow) = 0;
		virtual bool IsCursorHidden() = 0;
		virtual void SetCursorAtPos(const int x, const int y) = 0;
		virtual void GetClientArea(int& left, int& top, int& right, int& bottom) = 0;
		virtual void GetWindowArea(int& left, int& top, int& right, int& bottom) = 0;

		virtual unsigned int	GetTicks() = 0; // in microseconds
		virtual void			Sleep(const unsigned int miliseconds) = 0;

	protected:
		// Rendering pause
		void	PauseRendering(const bool pauseEnable) { m_bPauseRendering = pauseEnable; }
		virtual float	CalculateDeltaTime() = 0; // in miliseconds
		// Pause rendering when not in focus
		bool	m_bPauseRendering;
		static Framework*	m_pInstance;
	};
}

#endif // FRAMEWORK_H_
