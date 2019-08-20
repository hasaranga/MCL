
/*
	MCL - TSettingsWriter.cpp
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

#include "TSettingsWriter.h"

TSettingsWriter::TSettingsWriter()
{

}

bool TSettingsWriter::openFile(const TString& fileName, int formatID)
{
	if (TFile::isFileExists(fileName))
		TFile::deleteFile(fileName);

	if (!settingsFile.openFile(fileName, TFile::TWRITE))
		return false;

	settingsFile.setFilePointerToStart();
	settingsFile.writeFile(&formatID, sizeof(int));

	return true;
}

void TSettingsWriter::writeData(DWORD size, void* buffer)
{
	if (buffer)
		settingsFile.writeFile(buffer, size);
}

void TSettingsWriter::writeString(const TString& text)
{
	int size = text.getLength();
	if (size)
	{
		size = (size + 1) * sizeof(wchar_t);
		settingsFile.writeFile(&size, sizeof(int));

		settingsFile.writeFile((wchar_t*)text, size);
	}
	else // write only empty size
	{
		settingsFile.writeFile(&size, sizeof(int));
	}
}

void TSettingsWriter::writeInt(int value)
{
	settingsFile.writeFile(&value, sizeof(int));
}

void TSettingsWriter::writeFloat(float value)
{
	settingsFile.writeFile(&value, sizeof(float));
}

void TSettingsWriter::writeDouble(double value)
{
	settingsFile.writeFile(&value, sizeof(double));
}

void TSettingsWriter::writeBool(bool value)
{
	settingsFile.writeFile(&value, sizeof(bool));
}

TSettingsWriter::~TSettingsWriter()
{

}