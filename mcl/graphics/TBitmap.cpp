
/*
	MCL - TBitmap.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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

#include "../graphics/TBitmap.h"
#include "../mcl.h"

TBitmap::TBitmap()
{
	MCL_INIT_VERIFIER;
	hBitmap = 0;
}

TBitmap::operator HBITMAP()const
{
	return hBitmap;
}

bool TBitmap::loadFromResource(WORD resourceID)
{
	hBitmap = (HBITMAP)::LoadImageW(TApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

bool TBitmap::loadFromFile(const TString& filePath)
{
	hBitmap = (HBITMAP)::LoadImageW(TApplication::hInstance, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

void TBitmap::drawOnHDC(HDC hdc, int x, int y, int width, int height)
{
	HDC memHDC = ::CreateCompatibleDC(hdc);

	::SelectObject(memHDC, hBitmap);
	::BitBlt(hdc, x, y, width, height, memHDC, 0, 0, SRCCOPY);

	::DeleteDC(memHDC);
}

HBITMAP TBitmap::getHandle()
{
	return hBitmap;
}

TBitmap::~TBitmap()
{
	if(hBitmap)
		::DeleteObject(hBitmap);
}