
/*
	MCL - TMenuButton.h
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

#include "TButton.h"
#include "TMenu.h"
#include "../graphics/TFont.h"

class TMenuButton : public TButton
{
protected:
	TFont* arrowFont;
	HFONT glyphFont;
	TMenu* buttonMenu;
	const wchar_t *glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:

	PROPERTY_DEF_WRITEONLY(TMenu&, menu, setMenu)

	TMenuButton();

	virtual ~TMenuButton();

	virtual void setMenu(TMenu& buttonMenu_);

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor is not specified.
	*/
	virtual void setGlyph(const wchar_t *glyphChar, HFONT glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;
};
