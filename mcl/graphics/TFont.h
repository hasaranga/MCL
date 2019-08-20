
/*
	MCL - TFont.h
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

#include <windows.h>
#include "../text/TString.h"
#include "../containers/TLeakDetector.h"

/**
	Can be use to create system default font or custom font. Once created, you cannot change font properties.
*/
class TFont
{
protected:
	HFONT hFont;

public:
	/**
		Constructs empty Font object.
	*/
	TFont();

	/**
		Constructs custom font object.
	*/
	TFont(const TString& face, int size = 14, bool bold = false, bool italic = false, bool antiAliased=false);

	bool loadFont(const TString& face, int size, bool bold, bool italic, bool antiAliased);

	/**
		If you want to use system default font, then use this static method. Do not delete returned handle!
	*/
	static HFONT getDefaultFont();

	/**
		Loads font from a file. make sure to call RemoveFont when done.
	*/
	static bool loadFont(const TString& path);

	static void removeFont(const TString& path);

	/**
		Returns font handle.
	*/
	virtual HFONT getFontHandle();

	operator HFONT()const;

	virtual ~TFont();

private:
	MCL_LEAK_DETECTOR(TFont)
};
