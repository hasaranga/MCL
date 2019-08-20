
/*
	MCL - TGraphics.cpp
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

#include "TGraphics.h"

TGraphics::TGraphics(){}

TGraphics::~TGraphics(){}

void TGraphics::draw3dVLine(HDC hdc, int startX, int startY, int height)
{
	TGraphics::draw3dRect(hdc, startX, startY, 2, height, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void TGraphics::draw3dHLine(HDC hdc, int startX, int startY, int width)
{
	TGraphics::draw3dRect(hdc, startX, startY, width, 2, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void TGraphics::draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	TGraphics::draw3dRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}

void TGraphics::draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	TGraphics::fillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	TGraphics::fillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	TGraphics::fillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	TGraphics::fillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void TGraphics::fillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color)
{
	RECT rect = { x, y, x + cx, y + cy };
	TGraphics::fillSolidRect(hdc, &rect, color);
}

void TGraphics::fillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color)
{
	const COLORREF clrOld = ::SetBkColor(hdc, color);

	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	::SetBkColor(hdc, clrOld);
}

RECT TGraphics::calculateTextSize(const wchar_t *text, HFONT hFont)
{
	HDC hDC = ::CreateICW(L"DISPLAY", NULL, NULL, NULL);
	HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
	RECT sz = {0, 0, 0, 0};

	::DrawText(hDC, text, ::lstrlenW(text), &sz, DT_CALCRECT | DT_NOPREFIX);
	::SelectObject(hDC, hOldFont);

	::DeleteDC(hDC);
	return sz;
}
