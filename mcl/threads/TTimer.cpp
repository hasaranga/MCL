
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

	timerID = TPlatformUtil::getInstance()->generateTimerID(this);
}

void TTimer::setInterval(int resolution)
{
	this->resolution = resolution;
}

int TTimer::getInterval()
{
	return resolution;
}

void TTimer::setTimerWindow(TWindow *window)
{
	this->window = window;
}

void TTimer::setTimerID(UINT timerID)
{
	this->timerID = timerID;
}

UINT TTimer::getTimerID()
{
	return timerID;
}

void TTimer::startTimer()
{
	if(started)
		return;

	if(window)
	{
		HWND hwnd = window->getHandle();
		if(hwnd)
		{
			::SetTimer(hwnd, timerID, resolution, 0);
			started = true;
		}
	}
}

void TTimer::stopTimer()
{
	if(window)
	{
		HWND hwnd = window->getHandle();
		if(hwnd)
		{
			if(started)
				::KillTimer(hwnd, timerID);

			started = false;
		}
	}
}

bool TTimer::isTimerRunning()
{
	return started;
}

TTimer::~TTimer()
{
	if(started)
		this->stopTimer();
}