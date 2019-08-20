
/*
	MCL - TSettingsReader.h
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
#include "TFile.h"
#include "../containers/TLeakDetector.h"

/**
	High performance configuration reading class.
*/
class TSettingsReader
{
protected:
	TFile settingsFile;

public:
	TSettingsReader();

	virtual bool openFile(const TString& fileName, int formatID);

	/**
		read struct, array or whatever...
	*/
	virtual void readData(DWORD size, void* buffer);

	virtual TString readString();

	virtual int readInt();

	virtual float readFloat();

	virtual double readDouble();

	virtual bool readBool();

	virtual ~TSettingsReader();

private:
	MCL_LEAK_DETECTOR(TSettingsReader)
};
