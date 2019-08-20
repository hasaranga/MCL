
/*
	MCL - TFont.cpp
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

#include "TFont.h"
#include "../mcl.h"

TFont::TFont()
{
	MCL_INIT_VERIFIER;
	hFont = 0;
}

TFont::TFont(const TString& face, int size, bool bold, bool italic, bool antiAliased)
{
	MCL_INIT_VERIFIER;
	hFont = ::CreateFontW(size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE, 
		0, 0, DEFAULT_CHARSET, 0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, 
		VARIABLE_PITCH | FF_DONTCARE, face);
}

bool TFont::loadFont(const TString& face, int size, bool bold, bool italic, bool antiAliased)
{
	if(hFont)
		::DeleteObject(hFont);

	hFont = ::CreateFontW(size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE,
		0, 0, DEFAULT_CHARSET, 0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, 
		VARIABLE_PITCH | FF_DONTCARE, face);

	return (hFont == NULL);
}

HFONT TFont::getDefaultFont()
{
	return (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
}

bool TFont::loadFont(const TString& path)
{
	return (::AddFontResourceExW(path, FR_PRIVATE, 0) == 0) ? false : true;
}

void TFont::removeFont(const TString& path)
{
	::RemoveFontResourceExW(path, FR_PRIVATE, 0);
}

HFONT TFont::getFontHandle()
{
	return hFont;
}

TFont::operator HFONT()const
{
	return hFont;
}

TFont::~TFont()
{
	if(hFont)
		::DeleteObject(hFont);
}