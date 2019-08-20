
/*
	MCL - TGuid.h
	Copyright (C) 2019 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.

*/

#pragma once

#include "../containers/TLeakDetector.h"
#include "../text/TString.h"
#include <windows.h>

class TGuid
{
public:

	TGuid(){}

	static bool generateGUID(GUID *pGUID)
	{
		return (::CoCreateGuid(pGUID) == S_OK);
	}

	static TString generateGUID()
	{
		GUID guid;

		if (TGuid::generateGUID(&guid))
			return TGuid::guidToString(&guid);

		return TString();
	}

	static TString guidToString(GUID *pGUID)
	{
		wchar_t* strGuid = NULL;
		::UuidToStringW(pGUID, (RPC_WSTR*)&strGuid);

		TString result(strGuid, TString::USE_COPY_OF_TEXT);
		RpcStringFreeW((RPC_WSTR*)&strGuid);

		return result;
	}

	virtual ~TGuid(){}

private:
	MCL_LEAK_DETECTOR(TGuid)
};
