
/*
	MCL - TMD5.cpp
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

#include "TMD5.h"
#include "../io/TFile.h"
#include "../external/md5.h"

TMD5::TMD5()
{

}

TString TMD5::generateFromString(const TString& text)
{
	if (text.getLength() == 0)
		return TString();

	ExtLibs::MD5 md5;
	char *strMD5 = md5.digestString((char*)(const char*)text);

	return TString(strMD5);
}

TString TMD5::generateFromFile(const TString& fileName)
{
	if (fileName.getLength() == 0) // empty path
		return TString();

	if (!TFile::isFileExists(fileName)) // file does not exists
		return TString();

	TFile file;
	file.openFile(fileName, TFile::TREAD, false);

	const DWORD fileSize = file.getFileSize();
	file.closeFile();

	if (fileSize==0) // empty file
		return TString();

	ExtLibs::MD5 md5;
	char *strMD5 = md5.digestFile((char*)(const char*)fileName);

	return TString(strMD5);
}

TMD5::~TMD5()
{

}