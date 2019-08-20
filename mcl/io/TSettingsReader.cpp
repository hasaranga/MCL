
/*
	MCL - TSettingsReader.cpp
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

#include "TSettingsReader.h"

TSettingsReader::TSettingsReader()
{

}

bool TSettingsReader::openFile(const TString& fileName, int formatID)
{
	if (!TFile::isFileExists(fileName))
		return false;

	if (!settingsFile.openFile(fileName, TFile::TREAD))
		return false;

	settingsFile.setFilePointerToStart();

	int fileFormatID = 0;
	settingsFile.readFile(&fileFormatID, sizeof(int));

	if (formatID != fileFormatID) // invalid settings file
		return false;

	return true;
}

void TSettingsReader::readData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.readFile(buffer, size);
}

TString TSettingsReader::readString()
{
	int size = 0;
	settingsFile.readFile(&size, sizeof(int));

	if (size)
	{
		wchar_t *buffer = (wchar_t*)malloc(size);
		settingsFile.readFile(buffer, size);

		return TString(buffer, TString::FREE_TEXT_WHEN_DONE);
	}
	else
	{
		return TString();
	}
}

int TSettingsReader::readInt()
{
	int value = 0;
	settingsFile.readFile(&value, sizeof(int));

	return value;
}

float TSettingsReader::readFloat()
{
	float value = 0;
	settingsFile.readFile(&value, sizeof(float));

	return value;
}

double TSettingsReader::readDouble()
{
	double value = 0;
	settingsFile.readFile(&value, sizeof(double));

	return value;
}

bool TSettingsReader::readBool()
{
	bool value = 0;
	settingsFile.readFile(&value, sizeof(bool));

	return value;
}

TSettingsReader::~TSettingsReader()
{

}