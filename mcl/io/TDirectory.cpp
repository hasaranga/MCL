
/*
	MCL - TDirectory.cpp
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

#include "TDirectory.h"


TDirectory::TDirectory(){}

TDirectory::~TDirectory(){}

bool TDirectory::isDirExists(const TString& dirName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(dirName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool TDirectory::createDir(const TString& dirName)
{
	return (::CreateDirectoryW(dirName, NULL) == 0 ? false : true);
}

bool TDirectory::removeDir(const TString& dirName)
{
	return (::RemoveDirectoryW(dirName) == 0 ? false : true);
}

TString TDirectory::getModuleDir(HMODULE hModule)
{
	// assumes MAX_PATH * 2 is enough!

	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	path[0] = 0;
	::GetModuleFileNameW(hModule, path, MAX_PATH * 2);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return TString(path, TString::FREE_TEXT_WHEN_DONE);
}

TString TDirectory::getParentDir(const TString& filePath)
{
	wchar_t *path = ::_wcsdup(filePath);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return TString(path, TString::FREE_TEXT_WHEN_DONE);
}

TString TDirectory::getTempDir()
{
	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH + 1) * sizeof(wchar_t) );
	path[0] = 0;
	::GetTempPathW(MAX_PATH + 1, path);

	return TString(path, TString::FREE_TEXT_WHEN_DONE);
}

TString TDirectory::getApplicationDataDir(bool isAllUsers)
{
	wchar_t *path = (wchar_t*)::malloc( MAX_PATH * sizeof(wchar_t) );
	path[0] = 0;
	::SHGetFolderPathW(NULL, isAllUsers ? CSIDL_COMMON_APPDATA : CSIDL_APPDATA, NULL, 0, path);

	return TString(path, TString::FREE_TEXT_WHEN_DONE);
}