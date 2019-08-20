
/*
	MCL - TFile.cpp
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

#include "TFile.h"

TFile::TFile()
{
	autoCloseHandle = false;
	desiredAccess = TFile::TBOTH;
	fileHandle = INVALID_HANDLE_VALUE;
}

TFile::TFile(const TString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

bool TFile::openFile(const TString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	if (fileHandle != INVALID_HANDLE_VALUE) // close old file
		::CloseHandle(fileHandle);

	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return (fileHandle == INVALID_HANDLE_VALUE) ? false : true;
}

bool TFile::closeFile()
{
	if (::CloseHandle(fileHandle) != 0)
	{
		fileHandle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

HANDLE TFile::getFileHandle()
{
	return fileHandle;
}

TFile::operator HANDLE()const
{
	return fileHandle;
}

DWORD TFile::readFile(void* buffer, DWORD numberOfBytesToRead)
{
	DWORD numberOfBytesRead = 0;
	::ReadFile(fileHandle, buffer, numberOfBytesToRead, &numberOfBytesRead, NULL);

	return numberOfBytesRead;
}

DWORD TFile::writeFile(void* buffer, DWORD numberOfBytesToWrite)
{
	DWORD numberOfBytesWritten = 0;
	::WriteFile(fileHandle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL);

	return numberOfBytesWritten;
}

bool TFile::setFilePointerToStart()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ? false : true;
}

bool TFile::setFilePointerTo(long distance, DWORD startingPoint)
{
	return (::SetFilePointer(fileHandle, distance, NULL, startingPoint) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD TFile::getFilePointerPosition()
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
}

bool TFile::setFilePointerToEnd()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD TFile::getFileSize()
{
	const DWORD fileSize = ::GetFileSize(fileHandle, NULL);
	return (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;
}

void* TFile::readAsData()
{
	const DWORD fileSize = this->getFileSize();

	if (fileSize)
	{
		void* buffer = (void*)::malloc(fileSize);
		const DWORD numberOfBytesRead = this->readFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
			return buffer;

		::free(buffer); // cannot read entire file!
	}

	return NULL;
}

bool TFile::writeString(const TString& text, bool isUnicode)
{
	void* buffer = isUnicode ? (void*)(const wchar_t*)text : (void*)(const char*)text;
	const DWORD numberOfBytesToWrite = text.getLength() * ( isUnicode ? sizeof(wchar_t) : sizeof(char) );

	const DWORD numberOfBytesWritten = this->writeFile(buffer, numberOfBytesToWrite);

	return (numberOfBytesWritten == numberOfBytesToWrite);
}

TString TFile::readAsString()
{
	DWORD fileSize = this->getFileSize();

	if (fileSize)
	{
		char* buffer = (char*)::malloc(fileSize + 2); // +2 is for null
		const DWORD numberOfBytesRead = this->readFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
		{
			buffer[fileSize] = 0; // null terminated string
			buffer[fileSize + 1] = 0; // null for the unicode encoding

			if (::IsTextUnicode(buffer, numberOfBytesRead, NULL) == TRUE)
			{
				return TString((const wchar_t*)buffer, TString::FREE_TEXT_WHEN_DONE);
			}
			else
			{
				TString strData((const char*)buffer);
				::free(buffer);
				return strData;
			}
		}

		::free(buffer); // cannot read entire file!
	}

	return TString();
}

bool TFile::deleteFile(const TString& fileName)
{
	return (::DeleteFileW(fileName) == 0) ? false : true;
}

bool TFile::copyFile(const TString& sourceFileName, const TString& destFileName)
{
	return (::CopyFileW(sourceFileName, destFileName, FALSE) == 0) ? false : true;
}

bool TFile::isFileExists(const TString& fileName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(fileName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

TFile::~TFile()
{
	if (autoCloseHandle)
		::CloseHandle(fileHandle);
}