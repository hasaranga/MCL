
/*
	MCL - TSHA1.cpp
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

#include "TSHA1.h"
#include "../io/TFile.h"
#include "../external/sha1.h"

TSHA1::TSHA1()
{

}

TString TSHA1::generateFromString(const TString& text)
{
	if (text.getLength() == 0)
		return TString();

	ExtLibs::CSHA1 sha1;
	sha1.Update((const UINT_8*)(const char*)text, text.getLength());
	sha1.Final();

	char szReport[256];
	szReport[0] = 0;
	sha1.ReportHash(szReport, ExtLibs::CSHA1::REPORT_HEX);

	return TString(szReport);
}

TString TSHA1::generateFromFile(const TString& fileName)
{
	if (fileName.getLength() == 0) // empty path
		return TString();

	if (!TFile::isFileExists(fileName)) // file does not exists
		return TString();

	TFile file;
	file.openFile(fileName, TFile::TREAD, false);

	const DWORD fileSize = file.getFileSize();
	file.closeFile();

	if (fileSize == 0) // empty file
		return TString();

	ExtLibs::CSHA1 sha1;
	sha1.HashFile((const char*)fileName);
	sha1.Final();

	char szReport[256];
	szReport[0] = 0;
	sha1.ReportHash(szReport, ExtLibs::CSHA1::REPORT_HEX);

	return TString(szReport);
}

TSHA1::~TSHA1()
{

}