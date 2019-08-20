
/*
    MCL - TRegistry.h
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

#include "../config.h"
#include <windows.h>
#include "../text/TString.h"

class TRegistry
{

public:
	TRegistry();

	// returns true on success or if the key already exists.
	static bool createKey(HKEY hKeyRoot, const TString& subKey);

	// the subkey to be deleted must not have subkeys. 
	static bool deleteKey(HKEY hKeyRoot, const TString& subKey);

	static bool readString(HKEY hKeyRoot, const TString& subKey, const TString& valueName, TString* result);

	static bool writeString(HKEY hKeyRoot, const TString& subKey, const TString& valueName, const TString& value);

	static bool readDWORD(HKEY hKeyRoot, const TString& subKey, const TString& valueName, DWORD* result);

	static bool writeDWORD(HKEY hKeyRoot, const TString& subKey, const TString& valueName, DWORD value);

	/**
		you must free the buffer when you are done with it.

		e.g. @code
		void* buffer;
		DWORD bufferSize;
		if(TRegistry::ReadBinary(xxx, xxx, xxx, &buffer, &buffSize))
		{
			// do your thing here...

			free(buffer);
		}
		@endcode
	*/
	static bool readBinary(HKEY hKeyRoot, const TString& subKey, const TString& valueName, void** buffer, DWORD* buffSize);

	static bool writeBinary(HKEY hKeyRoot, const TString& subKey, const TString& valueName, void* buffer, DWORD buffSize);

	virtual ~TRegistry();

};
