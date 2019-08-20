
/*
	MCL - TDirectory.h
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
#include <shlobj.h>
#include "../text/TString.h"

/**
Can be use to manipulate dirs.
*/
class TDirectory
{
public:
	TDirectory();

	static bool isDirExists(const TString& dirName);

	/**
		returns false if directory already exists.
	*/
	static bool createDir(const TString& dirName);

	/**
		deletes an existing empty directory.
	*/
	static bool removeDir(const TString& dirName);

	/**
		returns the directory of given module. if HModule is NULL this function will return dir of exe.
		returns empty string on error.
	*/
	static TString getModuleDir(HMODULE hModule);

	/**
		returns the parent directory of given file.
	*/
	static TString getParentDir(const TString& filePath);

	/**
		returns the the directory for temporary files.
		returns empty string on error.
	*/
	static TString getTempDir();

	/**
		returns the the Application Data directory. if isAllUsers is true this function will return dir shared across all users.
		returns empty string on error.
	*/
	static TString getApplicationDataDir(bool isAllUsers = false);

	virtual ~TDirectory();

private:
	MCL_LEAK_DETECTOR(TDirectory)
};
