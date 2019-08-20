
/*
	MCL - TThread.cpp
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

#include "../mcl.h"
#include "TThread.h"


TThread::TThread()
{
	handle = 0; 
	onRun = nullptr;
	threadShouldStop = false;
}

void TThread::setHandle(HANDLE handle)
{
	this->handle = handle;
}

HANDLE TThread::getHandle()
{
	return handle;
}

TThread::operator HANDLE()const
{
	return handle;
}

bool TThread::shouldRun()
{
	return !threadShouldStop;
}

bool TThread::isThreadRunning()
{
	if (handle)
	{
		const DWORD result = ::WaitForSingleObject(handle, 0);
		return (result != WAIT_OBJECT_0);
	}

	return false;
}

void TThread::shouldStop()
{
	threadShouldStop = true;
}

void TThread::waitUntilThreadFinish()
{
	::WaitForSingleObject(handle, INFINITE);
}

bool TThread::startThread()
{
	threadShouldStop = false;

	if (handle) // close old handle
	{
		::CloseHandle(handle);
		handle = 0;
	}

	return ::CreateMCLThread(this);
}

void TThread::uSleep(int waitTime)
{
	__int64 time1 = 0, time2 = 0, freq = 0;

	QueryPerformanceCounter((LARGE_INTEGER *)&time1);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

	do {
		QueryPerformanceCounter((LARGE_INTEGER *)&time2);
	} while ((time2 - time1) < ((waitTime * freq) / 1000000));
}

TThread::~TThread()
{
	if (handle)
		::CloseHandle(handle);
}
