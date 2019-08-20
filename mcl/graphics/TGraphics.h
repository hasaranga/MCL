
/*
	MCL - TGraphics.h
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
#include "../containers/TLeakDetector.h"
#include <windows.h>

class TGraphics
{
public:
	TGraphics();

	virtual ~TGraphics();

	static void draw3dVLine(HDC hdc, int startX, int startY, int height);

	static void draw3dHLine(HDC hdc, int startX, int startY, int width);

	static void draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void fillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color);

	static void fillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color);

	static RECT calculateTextSize(const wchar_t *text, HFONT hFont);

private:
	MCL_LEAK_DETECTOR(TGraphics)
};
