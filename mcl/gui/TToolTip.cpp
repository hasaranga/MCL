
/*
	MCL - TToolTip.cpp
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

#include "TToolTip.h"
#include "../mcl.h"

TToolTip::TToolTip()
{
	attachedControl = 0;
	classNameProperty.assignStaticText(TXT_WITH_LEN("tooltips_class32"));

	styleProperty = WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX;
}

TToolTip::~TToolTip()
{
}

void TToolTip::attachToControl(TWindow *parentWindow, TControl *attachedControl)
{
	parentProperty = parentWindow->getHandle();
	this->attachedControl = attachedControl->getHandle();

	handleProperty = ::CreateWindowExW(0, classNameProperty, NULL, styleProperty, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parentProperty,
		NULL, TApplication::hInstance, 0);

	if (handleProperty)
	{
		::SetWindowPos(handleProperty, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		::AttachMCLPropertiesToHWND(handleProperty, (TComponent*)this);

		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = parentProperty;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)this->attachedControl;
		toolInfo.lpszText = textProperty;

		SendMessageW(handleProperty, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	}
}

bool TToolTip::create()
{
	return false;
}

void TToolTip::setText(const TString& caption)
{
	textProperty = caption;

	if (handleProperty)
	{
		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = parentProperty;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)attachedControl;
		toolInfo.lpszText = textProperty;
		toolInfo.hinst = TApplication::hInstance;

		SendMessageW(handleProperty, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
	}
}

