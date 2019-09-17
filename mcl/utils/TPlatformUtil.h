
/*
	MCL - TPlatformUtil.h
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

#include "../text/TString.h"
#include <windows.h>
#include <stdio.h>
#include "../containers/TPointerList.h"
#include "../gui/TWinMenuItem.h"
#include "../threads/TTimer.h"

/**
	Singleton class which can be use to generate class names, timer ids etc...
	(this class is for internal use)
*/
class TPlatformUtil
{
private:
	MCL_LEAK_DETECTOR(TPlatformUtil)

	static TPlatformUtil *_instance;
	TPlatformUtil();

protected:
	volatile int classCount;
	volatile int timerCount;
	volatile int controlCount;
	volatile UINT menuItemCount;

	TPointerList<TWinMenuItem*> *menuItemList;
	TPointerList<TTimer*> *timerList;

	CRITICAL_SECTION criticalSectionForCount;

public:

	static TPlatformUtil* getInstance();

	UINT generateControlID();

	// TApplication:hInstance must be valid before calling this method
	TString generateClassName();

	UINT generateMenuItemID(TWinMenuItem *menuItem);
	TWinMenuItem* getMenuItemByID(UINT id);

	UINT generateTimerID(TTimer *timer);
	TTimer* getTimerByID(UINT id);

	~TPlatformUtil();

};
