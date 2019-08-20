
/*
	MCL - TOpenDialog.h
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

#include "TWindow.h"
#include "../text/TString.h"
#include "../utils/TRegistry.h"
#include "../io/TDirectory.h"

/**
	Filter string should be like this "Text Files (*.txt)|*.txt"
	Multiple filters can be write like this "Text Files (*.txt)|*.txt|All Files (*.*)|*.*"
	"settingsGUID" is valid only if "saveLastLocation" is true.
*/
class TOpenDialog
{
protected:
	virtual bool showDialog(OPENFILENAMEW* ofn)
	{
		return ( ::GetOpenFileNameW(ofn) == TRUE );
	}

public:

	TString title;
	TString filter;
	TString fileName;
	int filterIndex;
	bool saveLastLocation;
	TString settingsGUID;

	const wchar_t* const MCL_OSD_REG_LOCATION = L"Software\\CrownSoft\\MCL\\OSD";

	TOpenDialog()
	{
		title = L"Open File...";
		filter = L"All Files (*.*)|*.*";
		filterIndex = 0;
		saveLastLocation = false;
	}

	virtual ~TOpenDialog()
	{

	}

	virtual bool show(TWindow& window)
	{
		// assumes MAX_PATH * 2 is enough!	
		wchar_t *buff = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
		buff[0] = 0;

		OPENFILENAMEW ofn;
		::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

		TString lastLocation;
		if (saveLastLocation)
		{
			TRegistry::readString(HKEY_CURRENT_USER, MCL_OSD_REG_LOCATION, settingsGUID, &lastLocation);

			if (lastLocation.getLength() > 0)
				ofn.lpstrInitialDir = (const wchar_t*)lastLocation;
		}

		// replace pipe character with null
		wchar_t* strFilter = _wcsdup(filter.appendStaticText(TXT_WITH_LEN("|")));
		const int strFilterLength = filter.getLength() + 1; // +1 appended pipe character
		for (int i = 0; i < strFilterLength; ++i)
		{
			if (strFilter[i] == L'|')
				strFilter[i] = L'\0';
		}

		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.hwndOwner = (window != NULL) ? window : NULL;
		ofn.lpstrFilter = strFilter;
		ofn.lpstrFile = buff;
		ofn.nMaxFile = MAX_PATH * 2;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
		ofn.lpstrTitle = title;
		ofn.nFilterIndex = filterIndex;

		if (this->showDialog(&ofn))
		{
			fileName = TString(buff, TString::FREE_TEXT_WHEN_DONE);
			filterIndex = ofn.nFilterIndex;

			if (saveLastLocation)
			{
				TString parentDir(TDirectory::getParentDir(fileName).appendStaticText(L"\\", 1, true));

				TRegistry::createKey(HKEY_CURRENT_USER, MCL_OSD_REG_LOCATION);	// if not exists
				TRegistry::writeString(HKEY_CURRENT_USER, MCL_OSD_REG_LOCATION, settingsGUID, parentDir);
			}

			::free(strFilter);
			return true;
		}
		else
		{
			::free(buff);
			::free(strFilter);
			return false;
		}
	}

};
