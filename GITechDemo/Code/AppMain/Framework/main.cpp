#include "stdafx.h"

#include "FrameworkWin.h"
using namespace AppFramework;

namespace AppFramework
{
	Framework* Framework::m_pInstance = nullptr;
}

int APIENTRY _tWinMain(
	_In_		HINSTANCE	hInstance,
	_In_opt_	HINSTANCE	hPrevInstance,
	_In_		LPTSTR		lpCmdLine,
	_In_		int			nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	FrameworkWin fw;
	fw.Init(hInstance, nCmdShow);
	return fw.Run();
}
