
/*
	MCL - TStringHolder.cpp
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

#include "TStringHolder.h"


TStringHolder::TStringHolder(wchar_t *w_text, int count)
{
	refCount = 1;
	a_text = 0;
	this->w_text = w_text;
	this->count = count;

	::InitializeCriticalSection(&cs_a_text);
}

TStringHolder::~TStringHolder()
{
	::DeleteCriticalSection(&cs_a_text);
}

void TStringHolder::addReference()
{
	::InterlockedIncrement(&refCount);
}

void TStringHolder::releaseReference()
{
	const LONG res = ::InterlockedDecrement(&refCount);
	if(res == 0)
	{
		if(a_text)
			::free(a_text);

		if(w_text)
			::free(w_text);

		delete this;
	}
}

const char* TStringHolder::getAnsiVersion(UINT codePage)
{
	::EnterCriticalSection(&cs_a_text);

	if(a_text)
	{
		::LeaveCriticalSection(&cs_a_text);
		return a_text;
	}else
	{
		const int length = ::WideCharToMultiByte(codePage, 0, w_text, -1, 0, 0, 0, 0);
		if (length)
		{
			a_text = (char*)::malloc(length);
			if (::WideCharToMultiByte(codePage, 0, w_text, -1, a_text, length, 0, 0))
			{
				::LeaveCriticalSection(&cs_a_text);
				return a_text;
			}
			::free(a_text);
			a_text = 0;
		}

		::LeaveCriticalSection(&cs_a_text);
		return 0; // conversion error
	}
}