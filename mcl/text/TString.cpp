
/*
	MCL - TString.cpp
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

#include "TString.h"
#include <stdio.h>

const TString operator+ (const char* const string1, const TString& string2)
{
	TString s(string1);
	return s.append(string2);
}

const TString operator+ (const wchar_t* const string1, const TString& string2)
{
	TString s(string1);
	return s.append(string2);
}

const TString operator+ (const TString& string1, const TString& string2)
{
	return string1.append(string2);
}

TString::TString()
{
	isZeroLength = true;
	stringHolder = 0;
	isStaticText = false;
}

TString::TString(const TString& other)
{
	isZeroLength = other.isZeroLength;

	if (other.isStaticText)
	{
		stringHolder = 0;

		isStaticText = true;
		staticText = other.staticText;
		staticTextLength = other.staticTextLength;
	}
	else if (other.stringHolder)
	{
		other.stringHolder->addReference();
		stringHolder = other.stringHolder;

		isStaticText = false;
	}
	else
	{
		stringHolder = 0;
		isStaticText = false;
	}
}

TString::TString(const char* const text, UINT codePage)
{
	isStaticText = false;

	if (text != 0)
	{
		int count = ::MultiByteToWideChar(codePage, 0, text, -1, 0, 0); // get char count with null character
		if (count)
		{
			wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
			if (::MultiByteToWideChar(codePage, 0, text, -1, w_text, count))
			{
				count--; // ignore null character

				stringHolder = new TStringHolder(w_text, count);
				isZeroLength = (count == 0);
				return;
			}
			else
			{
				::free(w_text);
			}
		}
	}

	isZeroLength = true;
	stringHolder = 0;
}

TString::TString(const wchar_t* const text, unsigned char behaviour, int length)
{
	if (text != 0)
	{
		staticTextLength = ((length == -1) ? (int)::wcslen(text) : length);
		if (staticTextLength)
		{
			isZeroLength = false;
			isStaticText = (behaviour == STATIC_TEXT_DO_NOT_FREE);

			if (isStaticText)
			{
				staticText = (wchar_t*)text;
				stringHolder = 0;
				return;
			}

			stringHolder = new TStringHolder(((behaviour == FREE_TEXT_WHEN_DONE) ? (wchar_t*)text : _wcsdup(text)), staticTextLength);
			return;
		}
	}

	isZeroLength = true;
	isStaticText = false;
	stringHolder = 0;
}

TString::TString(const int value, const int radix)
{
	stringHolder = new TStringHolder((wchar_t *)::malloc(33 * sizeof(wchar_t)), 0); // max 32 digits
	::_itow_s(value, stringHolder->w_text, 33, radix);

	stringHolder->count = (int)::wcslen(stringHolder->w_text);
	isZeroLength = (stringHolder->count == 0);
	isStaticText = false;
}

TString::TString(const float value, const int numDecimals, bool compact)
{
	isStaticText = false;

	// round it to given digits
	char *str_fmtp = (char*)malloc(32);
	char *str_buf = (char*)malloc(64);

	sprintf_s(str_fmtp, 32, "%%.%df", numDecimals);
	sprintf_s(str_buf, 64, str_fmtp, value);

	if (compact)
	{
		int len = (int)strlen(str_buf) - 1;
		for (int i = 0; i < numDecimals; i++) // kill ending zeros
		{
			if (str_buf[len - i] == '0')
				str_buf[len - i] = 0; // kill it
			else
				break;
		}

		// kill if last char is dot
		len = (int)strlen(str_buf) - 1;
		if (str_buf[len] == '.')
			str_buf[len] = 0; // kill it
	}

	int count = ::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, 0, 0); // get char count with null character
	if (count)
	{
		wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
		if (::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, w_text, count))
		{
			count--; // ignore null character

			stringHolder = new TStringHolder(w_text, count);
			isZeroLength = (count == 0);

			::free(str_buf);
			::free(str_fmtp);
			return;
		}
		else
		{
			::free(w_text);
		}
	}

	::free(str_buf);
	::free(str_fmtp);

	isZeroLength = true;
	stringHolder = 0;
}

const TString& TString::operator= (const TString& other)
{
	if (stringHolder)
		stringHolder->releaseReference();

	if (other.isStaticText)
	{
		isStaticText = true;
		staticText = other.staticText;
		staticTextLength = other.staticTextLength;
	}
	else if (other.stringHolder)
	{
		other.stringHolder->addReference();
		isStaticText = false;
	}
	else // other is empty
	{
		isStaticText = false;
	}

	stringHolder = other.stringHolder;
	isZeroLength = other.isZeroLength;

	return *this;
}

const TString& TString::operator= (const wchar_t* const other)
{
	isStaticText = false;

	if (stringHolder)
		stringHolder->releaseReference();

	if (other != 0)
	{
		const int count = (int)::wcslen(other);
		if (count)
		{
			stringHolder = new TStringHolder(::_wcsdup(other), count);
			isZeroLength = false;
			return *this;
		}
	}

	isZeroLength = true;
	stringHolder = 0;
	return *this;
}

const TString TString::operator+ (const TString& stringToAppend)
{
	return append(stringToAppend);
}

const TString TString::operator+ (const wchar_t* const textToAppend)
{
	return append(TString(textToAppend, USE_COPY_OF_TEXT, -1));
}

void TString::convertToRefCountedStringIfStatic()const
{
	if (isStaticText)
	{
		isStaticText = false;
		stringHolder = new TStringHolder(::_wcsdup(staticText), staticTextLength);
	}
}

TString::operator const char*()const
{
	if (!isZeroLength)
	{
		this->convertToRefCountedStringIfStatic();
		return stringHolder->getAnsiVersion();
	}
	return "";
}

TString::operator const wchar_t*()const
{
	if (isStaticText)
	{
		return staticText;
	}
	else if (stringHolder)
	{
		return stringHolder->w_text;
	}
	else
	{
		return L"";
	}
}

TString::operator wchar_t*()const
{
	if (isStaticText)
	{
		return staticText;
	}
	else if (stringHolder)
	{
		return stringHolder->w_text;
	}
	else
	{
		return (wchar_t*)L"";
	}
}

const char TString::operator[](const int index)const
{
	if (!isZeroLength)
	{
		this->convertToRefCountedStringIfStatic();

		if ((0 <= index) && (index <= (stringHolder->count - 1)))
		{
			return stringHolder->getAnsiVersion()[index];
		}
	}
	return -1;
}

TString TString::append(const TString& otherString)const
{
	if (!otherString.isZeroLength)
	{
		if (!this->isZeroLength)
		{
			const int totalCount = (isStaticText ? staticTextLength : stringHolder->count) + (otherString.isStaticText ? otherString.staticTextLength : otherString.stringHolder->count);
			wchar_t* destText = (wchar_t*)::malloc((totalCount + 1) * sizeof(wchar_t));

			::wcscpy_s(destText, (totalCount + 1), isStaticText ? staticText : stringHolder->w_text);
			::wcscat_s(destText, (totalCount + 1), otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text);

			return TString(destText, FREE_TEXT_WHEN_DONE, totalCount);
		}
		else // this string is empty
		{
			return otherString;
		}
	}
	else // other string is empty
	{
		return *this;
	}
}

TString TString::appendStaticText(const wchar_t* const text, int length, bool appendToEnd)const
{
	if (!this->isZeroLength)
	{
		const int totalCount = (isStaticText ? staticTextLength : stringHolder->count) + length;
		wchar_t* destText = (wchar_t*)::malloc((totalCount + 1) * sizeof(wchar_t));

		::wcscpy_s(destText, (totalCount + 1), appendToEnd ? (isStaticText ? staticText : stringHolder->w_text) : text);
		::wcscat_s(destText, (totalCount + 1), appendToEnd ? text : (isStaticText ? staticText : stringHolder->w_text));

		return TString(destText, FREE_TEXT_WHEN_DONE, totalCount);
	}
	else // this string is empty
	{
		return TString(text, TString::STATIC_TEXT_DO_NOT_FREE, length);
	}
}

void TString::assignStaticText(const wchar_t* const text, int length)
{
	if (stringHolder)
		stringHolder->releaseReference();
	
	stringHolder = 0;
	isZeroLength = false;
	isStaticText = true;
	staticText = (wchar_t*)text;
	staticTextLength = length;
}

TString TString::subString(int start, int end)const
{
	const int count = this->getLength();

	if ((0 <= start) && (start <= (count - 1)))
	{
		if ((start < end) && (end <= (count - 1)))
		{
			int size = (end - start) + 1;
			wchar_t* buf = (wchar_t*)::malloc((size + 1) * sizeof(wchar_t));
			wchar_t* src = (isStaticText ? staticText : stringHolder->w_text);
			::wcsncpy_s(buf, (size + 1), &src[start], size);
			buf[size] = 0;

			return TString(buf, FREE_TEXT_WHEN_DONE, size);
		}
	}
	return TString();
}

bool TString::compareIgnoreCase(const TString& otherString)const
{
	if ((!otherString.isZeroLength) && (!this->isZeroLength))
		return (::_wcsicmp((isStaticText ? staticText : stringHolder->w_text), (otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text)) == 0);

	return false;
}

bool TString::compare(const TString& otherString)const
{
	if ((!otherString.isZeroLength) && (!this->isZeroLength))
		return (::wcscmp((isStaticText ? staticText : stringHolder->w_text), (otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text)) == 0);

	return false;
}

bool TString::compareWithStaticText(const wchar_t* const text)const
{
	if (!this->isZeroLength)
		return (::wcscmp((isStaticText ? staticText : stringHolder->w_text), text) == 0);

	return false;
}

bool TString::startsWithChar(wchar_t character)const
{
	if (!this->isZeroLength)
		return (isStaticText ? (staticText[0] == character) : (stringHolder->w_text[0] == character));

	return false;
}

bool TString::startsWithChar(char character)const
{
	if (!this->isZeroLength)
	{
		this->convertToRefCountedStringIfStatic();
		return (stringHolder->getAnsiVersion()[0] == character);
	}
	return false;
}

bool TString::endsWithChar(wchar_t character)const
{
	if (!this->isZeroLength)
		return (isStaticText ? (staticText[staticTextLength - 1] == character) : (stringHolder->w_text[stringHolder->count - 1] == character));

	return false;
}

bool TString::endsWithChar(char character)const
{
	if (!this->isZeroLength)
	{
		this->convertToRefCountedStringIfStatic();
		return (stringHolder->getAnsiVersion()[stringHolder->count - 1] == character);
	}
	return false;
}

bool TString::isQuotedString()const
{
	if ((isStaticText && (staticTextLength > 1)) || ((stringHolder != 0) && (stringHolder->count > 1))) // not empty + count greater than 1
		return (startsWithChar(L'\"') && endsWithChar(L'\"'));

	return false;
}

wchar_t TString::getCharAt(int index)const
{
	const int count = this->getLength();

	if ((0 <= index) && (index <= (count - 1)))
		return (isStaticText ? staticText[index] : stringHolder->w_text[index]);

	return -1;
}

int TString::getLength()const
{
	return (isStaticText ? staticTextLength : ((stringHolder != 0) ? stringHolder->count : 0));
}

bool TString::isEmpty()const
{
	return isZeroLength;
}

bool TString::isNotEmpty()const
{
	return !isZeroLength;
}

int TString::getIntValue()const
{
	if (isZeroLength)
		return 0;

	return ::_wtoi(isStaticText ? staticText : stringHolder->w_text);
}

TString TString::toUpperCase()const
{
	if (this->getLength() == 0)
		return TString();

	TString result((const wchar_t*)*this, TString::USE_COPY_OF_TEXT);
	::CharUpperBuffW((wchar_t*)result, result.getLength());

	return result;
}

TString TString::toLowerCase()const
{
	if (this->getLength() == 0)
		return TString();

	TString result((const wchar_t*)*this, TString::USE_COPY_OF_TEXT);
	::CharLowerBuffW((wchar_t*)result, result.getLength());

	return result;
}

TString::~TString()
{
	if (stringHolder)
		stringHolder->releaseReference();
}