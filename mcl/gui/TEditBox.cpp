
/*
	MCL - TEditBox.cpp
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
#include "TEditBox.h"

TEditBox::TEditBox()
{
	classNameProperty.assignStaticText(TXT_WITH_LEN("EDIT"));

	widthProperty = 100;
	heightProperty = 20;

	leftProperty = 0;
	topProperty = 0;

	readOnlyProperty = false;
	onChange = nullptr;

	styleProperty |= WS_TABSTOP | ES_AUTOHSCROLL;
	extendedStyleProperty = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
}

bool TEditBox::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if ((message.msg == WM_COMMAND) && (HIWORD(message.wParam) == EN_CHANGE))
	{
		if (onChange)
			onChange(this);
	}

	return TControl::notifyProcHandler(message, result);
}

bool TEditBox::isReadOnly()
{
	return readOnlyProperty;
}

void TEditBox::setReadOnly(bool readOnly_)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	readOnlyProperty = readOnly_;

	if (readOnlyProperty)
		this->setStyle(styleProperty | ES_READONLY);
	else
		this->setStyle(styleProperty & ~ES_READONLY);

	if (destroyed)
		this->create();
}

void TEditBox::setLowercaseOnly(bool lowercaseOnly_)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	if (styleProperty & ES_UPPERCASE) // remove upper case style if already set
		styleProperty &= ~ES_UPPERCASE;

	if (lowercaseOnly_)
		this->setStyle(styleProperty | ES_LOWERCASE);
	else
		this->setStyle(styleProperty & ~ES_LOWERCASE);

	if (destroyed)
		this->create();
}

void TEditBox::setUppercaseOnly(bool uppercaseOnly_)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	if (styleProperty & ES_LOWERCASE) // remove lower case style if already set
		styleProperty &= ~ES_LOWERCASE;

	if (uppercaseOnly_)
		this->setStyle(styleProperty | ES_UPPERCASE);
	else
		this->setStyle(styleProperty & ~ES_UPPERCASE);

	if (destroyed)
		this->create();
}

TString TEditBox::getText()
{
	if(handleProperty)
	{
		const int length = ::GetWindowTextLengthW(handleProperty);
		if(length)
		{
			const int size = (length + 1) * sizeof(wchar_t);
			wchar_t* text = (wchar_t*)::malloc(size);
			text[0] = 0;
			::GetWindowTextW(handleProperty, text, size);
			textProperty = TString(text, TString::FREE_TEXT_WHEN_DONE);
		}else
		{
			textProperty = TString();
		}
	}
	return textProperty;
}

bool TEditBox::create()
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

TEditBox::~TEditBox()
{
}