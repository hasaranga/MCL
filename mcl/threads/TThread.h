
/*
	MCL - TThread.h
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

#include "../config.h"
#include <windows.h>
#include "../containers/TLeakDetector.h"

class TThread
{
protected:
	HANDLE handle;
	volatile bool threadShouldStop;

public:

	// void onRun(TThread* thread)
	EVENT_DEF(void, onRun, TThread*)

	TThread();

	/**
		For internal use only!
	*/
	virtual void setHandle(HANDLE handle);

	/**
		Returns handle of the thread
	*/
	virtual HANDLE getHandle();

	operator HANDLE()const;

	/**
		Starts thread
	*/
	virtual bool startThread();

	/**
		Another thread can signal this thread should stop. 
	*/
	virtual void shouldStop();

	/**
		@returns true if thread should run
	*/
	virtual bool shouldRun();

	/**
		@returns true if thread is still running
	*/
	virtual bool isThreadRunning();

	/**
		Calling thread is not return until this thread finish.
	*/
	virtual void waitUntilThreadFinish();

	/**
		Sleeps calling thread to given micro seconds.
	*/
	static void uSleep(int waitTime);

	virtual ~TThread();

private:
	MCL_LEAK_DETECTOR(TThread)
};
