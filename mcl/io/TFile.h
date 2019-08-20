
/*
	MCL - TFile.h
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
#include "../text/TString.h"
#include "../containers/TLeakDetector.h"

/**
	Can be use to read/write data from a file easily.
*/
class TFile
{
protected:
	TString fileName;
	HANDLE fileHandle;
	bool autoCloseHandle;
	DWORD desiredAccess;

public:
	TFile();

	/** 
		Used in file opening, to specify whether to open as read or write or both.
	*/
	enum FileAccessTypes
	{
		TREAD = GENERIC_READ,
		TWRITE = GENERIC_WRITE,
		TBOTH = GENERIC_READ | GENERIC_WRITE,
	};

	/** 
		If the file does not exist, it will be created.
	*/
	TFile(const TString& fileName, DWORD desiredAccess = TFile::TBOTH, bool autoCloseHandle = true);

	/** 
		If the file does not exist, it will be created.
	*/
	virtual bool openFile(const TString& fileName, DWORD desiredAccess = TFile::TBOTH, bool autoCloseHandle = true);

	virtual bool closeFile();

	virtual HANDLE getFileHandle();

	operator HANDLE()const;

	/** 
		fills given buffer and returns number of bytes read.
	*/
	virtual DWORD readFile(void* buffer, DWORD numberOfBytesToRead);

	/** 
		You must free the returned buffer yourself. To get the size of buffer, use GetFileSize method. return value will be null on read error.
	*/
	virtual void* readAsData();

	/**
		auto detects unicode & ansi encodings.
	*/
	virtual TString readAsString();

	/**
		returns number of bytes written.
	*/
	virtual DWORD writeFile(void* buffer, DWORD numberOfBytesToWrite);

	virtual bool writeString(const TString& text, bool isUnicode = true);

	virtual bool setFilePointerToStart();

	/**
		moves file pointer to given distance from "startingPoint".
		"startingPoint" can be FILE_BEGIN, FILE_CURRENT or FILE_END
		"distance" can be negative.
	*/
	virtual bool setFilePointerTo(long distance, DWORD startingPoint = FILE_BEGIN);

	virtual DWORD getFilePointerPosition();

	virtual bool setFilePointerToEnd();

	/**
		returns zero on error
	*/
	virtual DWORD getFileSize();

	static bool deleteFile(const TString& fileName);

	static bool isFileExists(const TString& fileName);

	static bool copyFile(const TString& sourceFileName, const TString& destFileName);

	virtual ~TFile();

private:
	MCL_LEAK_DETECTOR(TFile)
};
