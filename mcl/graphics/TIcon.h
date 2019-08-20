
/*
	MCL - TIcon.h
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

#include "../text/TString.h"
#include <windows.h>
#include "../containers/TLeakDetector.h"

/**
	Can be use to load icon from file or resource.
*/
class TIcon
{
protected:
	HICON hIcon;

public:
	TIcon();

	/**
		Loads icon from resource
		@param resourceID resource ID of icon file
		@returns false if icon load fails
	*/
	bool loadFromResource(WORD resourceID);

	/**
		Loads icon from file
		@param filePath path to icon file
		@returns false if icon load fails
	*/
	bool loadFromFile(const TString& filePath);

	/**
		Returns icon handle
	*/
	HICON getHandle();

	operator HICON()const;

	virtual ~TIcon();

private:
	MCL_LEAK_DETECTOR(TIcon)
};
