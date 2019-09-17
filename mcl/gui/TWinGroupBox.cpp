
/*
	MCL - TWinGroupBox.cpp
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

#include "TWinGroupBox.h"
#include "../mcl.h"

TWinGroupBox::TWinGroupBox()
{
	classNameProperty.assignStaticText(TXT_WITH_LEN("BUTTON"));
	textProperty.assignStaticText(TXT_WITH_LEN("GroupBox"));

	widthProperty = 100;
	heightProperty = 100;

	leftProperty = 0;
	topProperty = 0;

	styleProperty |= BS_GROUPBOX;
	extendedStyleProperty = WS_EX_WINDOWEDGE;
}

bool TWinGroupBox::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

TWinGroupBox::~TWinGroupBox()
{
}