
/*
	MCL - TWinButton.cpp
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

#include "TWinButton.h"
#include "../mcl.h"

TWinButton::TWinButton()
{
	onPress = nullptr;

	classNameProperty.assignStaticText(TXT_WITH_LEN("BUTTON"));
	textProperty.assignStaticText(TXT_WITH_LEN("Button"));

	widthProperty = 100;
	heightProperty = 30;

	styleProperty |= BS_NOTIFY | WS_TABSTOP;
	extendedStyleProperty = WS_EX_WINDOWEDGE;
}

bool TWinButton::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if ((message.msg == WM_COMMAND) && (HIWORD(message.wParam) == BN_CLICKED))
	{
		if (onPress)
			onPress(this);

		result = 0;
		return true;
	}

	return TControl::notifyProcHandler(message, result);
}

bool TWinButton::create()
{
	if(!parentProperty) // user must specify parent handle!
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

TWinButton::~TWinButton()
{
}