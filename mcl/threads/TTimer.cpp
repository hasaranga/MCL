
/*
	MCL - TTimer.cpp
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

#include "TTimer.h"
#include "../mcl.h"

TTimer::TTimer()
{
	MCL_INIT_VERIFIER;

	onTimer = nullptr;
	resolution = 1000;
	started = false;
	ownerWindow = nullptr;

	timerID = TPlatformUtil::getInstance()->generateTimerID(this);
}

void TTimer::setIntervalImpl(int resolution)
{
	this->resolution = resolution;
}

int TTimer::getIntervalImpl()
{
	return resolution;
}

void TTimer::setTimerWindowImpl(TWindow &window)
{
	ownerWindow = &window;
}

void TTimer::setTimerID(UINT timerID)
{
	this->timerID = timerID;
}

UINT TTimer::getTimerID()
{
	return timerID;
}

void TTimer::setEnabledImpl(bool enable)
{
	if (enable)
	{
		if (started)
			return;

		if (ownerWindow)
		{
			HWND hwnd = ownerWindow->getHandle();
			if (hwnd)
			{
				::SetTimer(hwnd, timerID, resolution, 0);
				started = true;
			}
		}
	}
	else
	{
		if (ownerWindow)
		{
			HWND hwnd = ownerWindow->getHandle();
			if (hwnd)
			{
				if (started)
					::KillTimer(hwnd, timerID);

				started = false;
			}
		}
	}
}

bool TTimer::isTimerRunningImpl()
{
	return started;
}

TTimer::~TTimer()
{
	if(started)
		this->setEnabled(false);
}