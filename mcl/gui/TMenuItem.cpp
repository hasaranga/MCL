
/*
	MCL - TMenuItem.cpp
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

#include"TMenuItem.h"
#include "../mcl.h"

TMenuItem::TMenuItem()
{
	MCL_INIT_VERIFIER;
	hMenu = 0;
	onPress = nullptr;
	enabled = true;
	checked = false;
	itemID = TPlatformUtil::getInstance()->generateMenuItemID(this);
}

void TMenuItem::addToMenu(HMENU hMenu)
{
	this->hMenu = hMenu;

	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.dwTypeData = (LPWSTR)(const wchar_t*)textProperty;
	mii.cch = lstrlenW((LPWSTR)(const wchar_t*)textProperty);
	mii.fState = (enabled ? MFS_ENABLED : MFS_DISABLED) | (checked ? MFS_CHECKED : MFS_UNCHECKED);
	mii.wID = itemID;
	mii.dwItemData = (ULONG_PTR)this; // for future!

	::InsertMenuItemW(hMenu, itemID, FALSE, &mii);
}

bool TMenuItem::isChecked()
{
	return checked;
}

void TMenuItem::setCheckedState(bool state)
{
	checked = state;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = checked ? MFS_CHECKED : MFS_UNCHECKED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

bool TMenuItem::isEnabled()
{
	return enabled; 
}

void TMenuItem::setEnabled(bool state)
{
	enabled = state;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = enabled ? MFS_ENABLED : MFS_DISABLED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

void TMenuItem::setText(const TString& text)
{
	textProperty = text;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.dwTypeData = (LPWSTR)(const wchar_t*)textProperty;
		mii.cch = lstrlenW((LPWSTR)(const wchar_t*)textProperty);

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

TString TMenuItem::getText()
{
	return textProperty;
}

UINT TMenuItem::getItemID()
{
	return itemID;
}

HMENU TMenuItem::getMenuHandle()
{
	return hMenu;
}

TMenuItem::~TMenuItem()
{
}

