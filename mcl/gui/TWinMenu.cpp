
/*
	MCL - TWinMenu.cpp
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

#include "TWinMenu.h"

TWinMenu::TWinMenu()
{
	hMenu = ::CreatePopupMenu();
}

void TWinMenu::add(TWinMenuItem& menuItem)
{
	menuItem.addToMenu(hMenu);
}

void TWinMenu::addSubMenu(const TString& text, const TWinMenu& menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)(HMENU)menu, text);
}

void TWinMenu::addSeperator()
{
	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_TYPE;
	mii.fType = MFT_SEPARATOR;

	::InsertMenuItemW(hMenu, 0xFFFFFFFF, FALSE, &mii);
}

HMENU TWinMenu::getMenuHandle()
{
	return hMenu;
}

TWinMenu::operator HMENU()const
{
	return hMenu;
}

void TWinMenu::popUpMenu(TWindow& window)
{
	if(window)
	{
		POINT p;
		::GetCursorPos(&p);
		::TrackPopupMenu(hMenu, TPM_LEFTBUTTON, p.x, p.y, 0, window, NULL);
	}
}

TWinMenu::~TWinMenu()
{
	::DestroyMenu(hMenu);
}