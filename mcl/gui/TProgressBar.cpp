
/*
	MCL - TProgressBar.cpp
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

#include "TProgressBar.h"
#include "../mcl.h"

TProgressBar::TProgressBar()
{
	progress = 0;

	classNameProperty.assignStaticText(TXT_WITH_LEN("msctls_progress32"));

	widthProperty = 100;
	heightProperty = 20;

	leftProperty = 0;
	topProperty = 0;

	extendedStyleProperty = WS_EX_WINDOWEDGE;
	styleProperty |= PBS_SMOOTH;
}

void TProgressBar::setEnableSmooth(bool smooth_)
{
	if (smooth_)
		this->setStyle(styleProperty | PBS_SMOOTH);
	else
		this->setStyle(styleProperty & ~PBS_SMOOTH);
}

void TProgressBar::setOrientation(bool vertical_)
{
	if (vertical_)
		this->setStyle(styleProperty | PBS_VERTICAL);
	else
		this->setStyle(styleProperty & ~PBS_VERTICAL);
}

int TProgressBar::getValue()
{
	return progress;
}

void TProgressBar::setValue(int val)
{
	this->progress = val;

	if(handleProperty)
		::SendMessageW(handleProperty, PBM_SETPOS, progress, 0);
}

bool TProgressBar::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // set range between 0-100
		::SendMessageW(handleProperty, PBM_SETPOS, value, 0); // set current value!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

TProgressBar::~TProgressBar()
{
}
