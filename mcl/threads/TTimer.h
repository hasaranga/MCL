
/*
	MCL - TTimer.h
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

#include "../gui/TWindow.h"
#include <windows.h>
#include "../containers/TLeakDetector.h"

/**
	Encapsulates a timer.

	The timer can be started with the startTimer() method
	and controlled with various other methods. Before you start timer, you must set 
	timer window by calling setTimerWindow method.
*/
class TTimer
{
protected:
	UINT timerID;
	int resolution;
	bool started;
	TWindow *window;

public:

	// void onTimer(TTimer* timer)
	EVENT_DEF(void, onTimer, TTimer*)

	TTimer();

	/**
		@param resolution timer interval
	*/
	virtual void setInterval(int resolution);

	virtual int getInterval();

	/**
		Call this method before you start the timer
	*/
	virtual void setTimerWindow(TWindow *window);

	virtual void setTimerID(UINT timerID);

	/**
		@returns unique id of this timer
	*/
	virtual UINT getTimerID();

	/**
		Starts timer
	*/
	virtual void startTimer();

	/**
		Stops the timer. You can restart it by calling startTimer() method.
	*/
	virtual void stopTimer();

	virtual bool isTimerRunning();

	virtual ~TTimer();

private:
	MCL_LEAK_DETECTOR(TTimer)
};
