
/*
	MCL - TWinMenuButton.cpp
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

#include "TWinMenuButton.h"
#include "../mcl.h"

TWinMenuButton::TWinMenuButton()
{
	buttonMenu = 0;
	glyphFont = 0;
	glyphChar = 0;
	glyphLeft = 6;
	arrowFont = new TFont(CONST_TXT("Webdings"), 18);
}	

TWinMenuButton::~TWinMenuButton()
{
	delete arrowFont;
}

void TWinMenuButton::setMenuImpl(TWinMenu& buttonMenu_)
{
	buttonMenu = &buttonMenu_;
}

void TWinMenuButton::setGlyph(const wchar_t *glyphChar, HFONT glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->repaint();
}

bool TWinMenuButton::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if (message.msg == WM_NOTIFY)
	{		
		if (((LPNMHDR)message.lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
		{
			LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW)message.lParam;

			result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

			if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
			{
				result = CDRF_NOTIFYPOSTPAINT;
			}
			else if ( CDDS_POSTPAINT== lpNMCD->dwDrawStage ) //  postpaint stage
			{
				const RECT rc = lpNMCD->rc;
				TGraphics::draw3dVLine(lpNMCD->hdc, rc.right - 22, rc.top + 6, rc.bottom - 12); // draw line

				const bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED|CDIS_GRAYED)) != 0;

				HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, arrowFont->getFontHandle());
				const COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, ::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
				const int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

				RECT rcIcon = { rc.right - 18, rc.top, rc.right, rc.bottom };
				::DrawTextW(lpNMCD->hdc, L"\x36", 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw arrow

				if (glyphFont) // draw glyph
				{
					::SelectObject(lpNMCD->hdc, glyphFont);
					::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);

					rcIcon = { rc.left + glyphLeft, rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
				}

				::SetBkMode(lpNMCD->hdc, oldBkMode);
				::SetTextColor(lpNMCD->hdc, oldTextColor);
				::SelectObject(lpNMCD->hdc, oldFont);

				result = CDRF_DODEFAULT;
			}

			return true; // indicate that we processed this msg & result is valid.
		}
	}
	else if ((message.msg == WM_COMMAND) && (HIWORD(message.wParam) == BN_CLICKED))
	{
		if (buttonMenu)
		{
			POINT point = { leftProperty, topProperty };
			::ClientToScreen(parentProperty, &point); // get screen cordinates

			::TrackPopupMenu(buttonMenu->getMenuHandle(), TPM_LEFTBUTTON, point.x, point.y + heightProperty, 0, parentProperty, NULL);
		}

		if (onPress)
			onPress(this);

		result = 0;
		return true;
	}

	return TWinButton::notifyProcHandler(message, result); // pass unprocessed messages to parent
}