
/*
	MCL - TLogger.h
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

#include <windows.h>
#include "../text/TString.h"
#include "../utils/TPerformanceCounter.h"
#include "../containers/TLeakDetector.h"

/**
	Super fast logging class for logging within a (audio)loop. (not thread safe)
	Writes logging data into ram & dumps data into file when needed.
	You can use this class instead of OutputDebugString API.(OutputDebugString is too slow & ETW is too complex?)
	Use Log Viewer tool to view generated log file.

	Log File Format:
		file header:				'T' 'L' 'O' 'G'
		event count:				int32					; (event count)
		event start packet format:	byte|short16|short16	; (event type|secs|mills)
		event param number format:	byte|data				; (param type|data)
		event param string format:	byte|byte|data			; (param type|data size[max 255]|data)
		event end packet format:	byte					; (EVT_END)
*/
class TLogger
{
protected:
	DWORD bufferSize;
	DWORD bufferIndex;
	unsigned int totalMills;
	unsigned int totalEvents;
	char *buffer;
	bool bufferFull, isFirstCall;
	TPerformanceCounter pCounter;

public:

	enum ByteSizes
	{
		SZ_MEGABYTE = 1024 * 1024,
		SZ_KILOBYTE = 1024,
	};

	enum EventTypes
	{
		EVT_END = 0,
		EVT_INFORMATION = 1,
		EVT_WARNING = 2,
		EVT_ERROR = 3,
	};

	enum ParamTypes
	{
		// skipped value zero. because parser will fail to recognize EVT_END.
		PARAM_STRING = 1,
		PARAM_INT32 = 2,
		PARAM_SHORT16 = 3,
		PARAM_FLOAT = 4,
		PARAM_DOUBLE = 5,
	};

	TLogger(DWORD bufferSize = (SZ_MEGABYTE * 10));

	virtual bool writeNewEvent(unsigned char eventType = EVT_INFORMATION);

	virtual bool endEvent();

	/**
		textLength is number of chars. max value is 255.
	*/
	virtual bool addTextParam(const char *text, unsigned char textLength);

	virtual bool addIntParam(int value);

	virtual bool addShortParam(unsigned short value);

	virtual bool addFloatParam(float value);
	
	virtual bool addDoubleParam(double value);

	virtual bool isBufferFull();

	virtual bool writeToFile(const TString& filePath);

	virtual ~TLogger();

private:
	MCL_LEAK_DETECTOR(TLogger)
};
