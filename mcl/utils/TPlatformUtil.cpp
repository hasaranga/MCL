
/*
	MCL - TPlatformUtil.cpp
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

#include "TPlatformUtil.h"
#include "../mcl.h"

static const int mcl_InitialMenuItemCount	= 400;
static const int mcl_InitialTimerCount		= 40;
static const int mcl_InitialControlID		= 100;
static const int mcl_InitialMenuItemID		= 30000;
static const int mcl_InitialTimerID			= 1000;

TPlatformUtil* TPlatformUtil::_instance = 0;

TPlatformUtil::TPlatformUtil()
{
	MCL_INIT_VERIFIER;

	timerCount = 0;
	menuItemCount = 0;
	classCount = 0;
	controlCount = 0;
	menuItemList = 0;
	timerList = 0;

	::InitializeCriticalSection(&criticalSectionForCount);
}

TPlatformUtil* TPlatformUtil::getInstance()
{
	if(_instance)
		return _instance;
	_instance = new TPlatformUtil();
	return _instance;
}

UINT TPlatformUtil::generateControlID()
{
	::EnterCriticalSection(&criticalSectionForCount);

	++controlCount;
	UINT retVal = controlCount + mcl_InitialControlID;

	::LeaveCriticalSection(&criticalSectionForCount);

	return retVal;
}

UINT TPlatformUtil::generateMenuItemID(TMenuItem *menuItem)
{
	::EnterCriticalSection(&criticalSectionForCount);

	if (menuItemList == 0) // generate on first call
		menuItemList = new TPointerList<TMenuItem*>(mcl_InitialMenuItemCount);

	++menuItemCount;
	menuItemList->addPointer(menuItem);
	UINT retVal = menuItemCount + mcl_InitialMenuItemID;

	::LeaveCriticalSection(&criticalSectionForCount);

	return retVal;
}

TMenuItem* TPlatformUtil::getMenuItemByID(UINT id)
{
	if (menuItemList)
		return menuItemList->getPointer(id - (mcl_InitialMenuItemID + 1));
	return 0;
}

TString TPlatformUtil::generateClassName()
{ 
	::EnterCriticalSection(&criticalSectionForCount);

	wchar_t *className = (wchar_t*)::malloc(32 * sizeof(wchar_t));

	className[0] = L'M';
	className[1] = L'C';
	className[2] = L'L';
	className[3] = L'_';

	::_itow_s((int)(INT_PTR)TApplication::hInstance, &className[4], 28, 10);

	int lastPos = (int)::wcslen(className);
	className[lastPos] = L'_';

	::_itow_s(classCount, &className[lastPos + 1], 32 - (lastPos + 1 ), 10);

	/*
	#ifdef _MSC_VER
		::swprintf(className, 32, L"RFC_%d_%d", (int)hInstance, classCount);
	#else
		::swprintf(className,L"RFC_%d_%d", (int)hInstance, classCount);
	#endif */

	++classCount;
	::LeaveCriticalSection(&criticalSectionForCount);

	return TString(className, TString::FREE_TEXT_WHEN_DONE);
}

UINT TPlatformUtil::generateTimerID(TTimer *timer)
{
	::EnterCriticalSection(&criticalSectionForCount);

	if (timerList == 0) // generate on first call
		timerList = new TPointerList<TTimer*>(mcl_InitialTimerCount);

	++timerCount;
	timerList->addPointer(timer);

	UINT retVal = timerCount + mcl_InitialTimerID;
	::LeaveCriticalSection(&criticalSectionForCount);

	return retVal;
}

TTimer* TPlatformUtil::getTimerByID(UINT id)
{
	if (timerList)
		return timerList->getPointer(id - (mcl_InitialTimerID + 1));
	return 0;
}

TPlatformUtil::~TPlatformUtil()
{
	if (menuItemList)
		delete menuItemList;

	if (timerList)
		delete timerList;

	::DeleteCriticalSection(&criticalSectionForCount);
}