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

		// Low level, platform specific functionality required by the application
		virtual void ShowCursor(const bool bShow) = 0;
		virtual bool IsCursorHidden() = 0;
		virtual void SetCursorAtPos(const int x, const int y) = 0;
		virtual void GetClientArea(int& left, int& top, int& right, int& bottom) = 0;
		virtual void GetWindowArea(int& left, int& top, int& right, int& bottom) = 0;

	protected:
		virtual unsigned int	GetTicks() = 0;
		virtual float			CalculateDeltaTime() = 0; // in miliseconds

		static Framework*	m_pInstance;
	};
}

#endif // FRAMEWORK_H_
