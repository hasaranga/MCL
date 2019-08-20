
/*
	MCL - TIcon.cpp
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

#include "../graphics/TIcon.h"
#include "../mcl.h"

TIcon::TIcon()
{
	MCL_INIT_VERIFIER;
	hIcon = 0;
}

bool TIcon::loadFromResource(WORD resourceID)
{
	hIcon = (HICON)::LoadImageW(TApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hIcon)
		return true;	
	return false;
}

bool TIcon::loadFromFile(const TString& filePath)
{
	hIcon = (HICON)::LoadImageW(TApplication::hInstance, filePath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hIcon)
		return true;	
	return false;
}

HICON TIcon::getHandle()
{
	return hIcon;
}

TIcon::operator HICON()const
{
	return hIcon;
}

TIcon::~TIcon()
{
	if(hIcon)
		::DestroyIcon(hIcon);
}