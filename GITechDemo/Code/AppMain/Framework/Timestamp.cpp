#include "stdafx.h"

#include "Timestamp.h"

namespace AppFramework
{
	const char* const GetBuildDate()
	{
		return __DATE__;
	}

	const char* const GetBuildTime()
	{
		return __TIME__;
	}
}
