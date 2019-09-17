
/*
	MCL - TWinTextArea.cpp
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

#include "TWinTextArea.h"

TWinTextArea::TWinTextArea()
{
	widthProperty = 200;
	heightProperty = 100;

	styleProperty |= ES_MULTILINE | ES_WANTRETURN | WS_HSCROLL | WS_VSCROLL;
	styleProperty = styleProperty & ~ES_AUTOHSCROLL; // remove ES_AUTOHSCROLL which comes from TWinEditBox
}

void TWinTextArea::setEnableVScrollBarImpl(bool enableVScrollBar)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	if (enableVScrollBar)
		this->setStyle((styleProperty  & ~ES_AUTOVSCROLL) | WS_VSCROLL);
	else
		this->setStyle((styleProperty & ~WS_VSCROLL) | ES_AUTOVSCROLL);

	if(destroyed)
		this->create();
}

void TWinTextArea::setEnableHScrollBarImpl(bool enableHScrollBar)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	if (enableHScrollBar)
		this->setStyle((styleProperty & ~ES_AUTOHSCROLL) | WS_HSCROLL);
	else
		this->setStyle((styleProperty & ~WS_HSCROLL) | ES_AUTOHSCROLL);

	if (destroyed)
		this->create();
}

void TWinTextArea::setEnableWordWrapImpl(bool enableWordWrap)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	if (enableWordWrap)
		this->setStyle((styleProperty & ~ES_AUTOHSCROLL) & ~WS_HSCROLL);
	else
		this->setStyle((styleProperty | WS_HSCROLL) & ~ES_AUTOHSCROLL);

	if (destroyed)
		this->create();
}

LRESULT TWinTextArea::windowProcHandler(TMessage& message)
{
	if(message.msg == WM_GETDLGCODE)
		return DLGC_WANTALLKEYS; // to catch TAB key

	return TWinEditBox::windowProcHandler(message);
}

TWinTextArea::~TWinTextArea()
{
}