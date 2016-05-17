/*=============================================================================
 *	This file is part of the "GITechDemo" application
 *	Copyright (C) 2015 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *		File:	Framework.h
 *		Author:	Bogdan Iftode
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
=============================================================================*/

#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

namespace AppFramework
{
	class Framework
	{
	public:
		Framework()
			: m_bPauseRendering(false)
			, m_bIsFullscreen(false)
		{ m_pInstance = this; };
		virtual ~Framework() { m_pInstance = nullptr; };

		virtual int Run() = 0;
		static Framework* const GetInstance() { return m_pInstance; }

		const bool	IsRenderingPaused() { return m_bPauseRendering; }
		const bool	IsFullscreen() { return m_bIsFullscreen; }

		// Low level, platform specific functionality required by the application
		virtual void ShowCursor(const bool bShow) = 0;
		virtual bool IsCursorHidden() = 0;
		virtual void SetCursorAtPos(const int x, const int y) = 0;
		virtual void GetClientArea(int& left, int& top, int& right, int& bottom) = 0;
		virtual void GetWindowArea(int& left, int& top, int& right, int& bottom) = 0;

		virtual unsigned int	GetTicks() = 0; // in microseconds
		virtual void			Sleep(const unsigned int miliseconds) = 0;

		virtual void OnSwitchToFullscreenMode() { m_bIsFullscreen = true; }
		virtual void OnSwitchToWindowedMode() { m_bIsFullscreen = false; }

	protected:
		// Rendering pause
		void	PauseRendering(const bool pauseEnable) { m_bPauseRendering = pauseEnable; }
		virtual float	CalculateDeltaTime() = 0; // in miliseconds
		// Pause rendering when not in focus
		bool	m_bPauseRendering;
		bool	m_bIsFullscreen;
		static Framework*	m_pInstance;
	};
}

#endif // FRAMEWORK_H_
