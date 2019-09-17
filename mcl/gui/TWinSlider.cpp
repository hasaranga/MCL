
/*
	MCL - TWinSlider.cpp
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
#include "TWinSlider.h"

TWinSlider::TWinSlider()
{
	rangeMin = 0;
	rangeMax = 100;
	positionProperty = 0;
	onChange = nullptr;

	widthProperty = 100;
	heightProperty = 25;

	leftProperty = 0;
	topProperty = 0;

	styleProperty |= WS_TABSTOP | TBS_NOTICKS | TBS_HORZ;
	extendedStyleProperty = WS_EX_WINDOWEDGE;

	classNameProperty.assignStaticText(TXT_WITH_LEN("msctls_trackbar32"));
}

void TWinSlider::setEnableTicksImpl(bool enableTicks)
{
	if (enableTicks)
		this->setStyle((styleProperty & ~TBS_NOTICKS) | TBS_AUTOTICKS);
	else
		this->setStyle((styleProperty & ~TBS_AUTOTICKS) | TBS_NOTICKS);
}

void TWinSlider::setOrientationImpl(bool vertical)
{
	if (vertical)
		this->setStyle((styleProperty & ~TBS_HORZ) | TBS_VERT);
	else
		this->setStyle((styleProperty & ~TBS_VERT) | TBS_HORZ);
}

void TWinSlider::setRange(int min, int max)
{
	rangeMin = min;
	rangeMax = max;
	if(handleProperty)
		::SendMessageW(handleProperty, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(min, max));
}

void TWinSlider::setPositionImpl(int pos)
{
	this->positionProperty = pos;
	if(handleProperty)
		::SendMessageW(handleProperty, TBM_SETPOS, TRUE, (LPARAM)positionProperty);
}

int TWinSlider::getPositionImpl()
{
	return positionProperty;
}

bool TWinSlider::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if( (message.msg == WM_HSCROLL) || (message.msg == WM_VSCROLL) )
	{
		const int nScrollCode = (int)LOWORD(message.wParam);

		if( (TB_THUMBTRACK == nScrollCode) || (TB_LINEDOWN == nScrollCode) || (TB_LINEUP == nScrollCode) || 
			(TB_BOTTOM == nScrollCode) || (TB_TOP == nScrollCode) || (TB_PAGEUP == nScrollCode) || 
			(TB_PAGEDOWN == nScrollCode) || (TB_THUMBPOSITION == nScrollCode)) // its trackbar!
		{
			positionProperty = (int)::SendMessageW(handleProperty, TBM_GETPOS, 0, 0);

			if (onChange)
				onChange(this, positionProperty);
			
			result = 0;
			return true;
		}
	}

	return TControl::notifyProcHandler(message, result);
}

bool TWinSlider::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);

		::SendMessageW(handleProperty, TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(rangeMin, rangeMax));
		::SendMessageW(handleProperty, TBM_SETPOS, TRUE, (LPARAM)positionProperty);

		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

TWinSlider::~TWinSlider()
{
}

