
/*
	MCL - TString.h
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
#include "TStringHolder.h"
#include "../containers/TLeakDetector.h"

#include <windows.h>
#include <string.h>
#include <malloc.h>

/**
	Using a reference-counted internal representation, these strings are fast and efficient. <BR>
	TString was optimized to use with unicode strings. So, use unicode strings instead of ansi. <BR>
	TString does not support for multiple zero terminated strings. <BR>

	Optimization tips: <BR>
	use CONST_TXT macro when using statically typed text. <BR>
	use constructor instead of assignment (if can). <BR>
	use "append" method instead of "+" operator. <BR>
	use "appendStaticText" method instead of "append" if you are using statically typed text. <BR>
	use "compareWithStaticText" method instead of "compare" if you are comparing statically typed text. <BR>

	@code
	TString result1 = str1 + L"1234"; // slow
	TString result2 = str1 + CONST_TXT("1234"); // fast
	TString result3( str1 + CONST_TXT("1234") ); // more fast
	TString result4( str1.append(CONST_TXT("1234")) ); // bit more fast
	TString result5( str1.appendStaticText(TXT_WITH_LEN("1234")) ); // that's all you can have ;-)
	@endcode
*/
class TString
{
protected:
	mutable TStringHolder *stringHolder; // for empty string: stringHolder=0 && isStaticText=false
	bool isZeroLength; // true if empty string or staticText, stringHolder are zero length
	mutable bool isStaticText; // staticText & staticTextLength are valid only if this field is true. stringHolder can be zero even this filed is false.
	wchar_t *staticText;
	int staticTextLength;

	void convertToRefCountedStringIfStatic()const; // generates StringHolder object from static text

public:

	enum TextTypes
	{
		STATIC_TEXT_DO_NOT_FREE = 1,
		FREE_TEXT_WHEN_DONE = 2,
		USE_COPY_OF_TEXT = 3,
	};

	/**
		Constructs an empty string
	*/
	TString();

	/**
		Constructs copy of another string
	*/
	TString(const TString& other);

	/**
		Constructs String object using ansi string
	*/
	TString(const char* const text, UINT codePage = CP_UTF8);

	/**
		Constructs String object using unicode string
	*/
	TString(const wchar_t* const text, unsigned char behaviour = USE_COPY_OF_TEXT, int length = -1);

	/**
		Constructs String object using integer
	*/
	TString(const int value, const int radix = 10);

	/**
		Constructs String object using float
		@param compact   removes ending decimal zeros if true
	*/
	TString(const float value, const int numDecimals, bool compact = false);

	/** 
		Replaces this string's contents with another string.
	*/
	const TString& operator= (const TString& other);

	/** 
		Replaces this string's contents with unicode string. 
	*/
	const TString& operator= (const wchar_t* const other);


	/** 
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	const TString operator+ (const TString& stringToAppend);

	/** 
		Appends a unicode string at the end of this one.
		@returns     the concatenated string
	*/
	const TString operator+ (const wchar_t* const textToAppend);
	/**
		Returns ansi version of this string
	*/
	operator const char*()const;

	/**
		Returns const unicode version of this string
	*/
	operator const wchar_t*()const;

	/**
		Returns unicode version of this string
	*/
	operator wchar_t*()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	const char operator[](const int index)const;

	/**
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	TString append(const TString& otherString)const;

	/**
		Appends a statically typed string to begining or end of this one.
		@param text			statically typed text
		@param length		text length. should not be zero.
		@param appendToEnd	appends to begining if false
		@returns			the concatenated string
	*/
	TString appendStaticText(const wchar_t* const text, int length, bool appendToEnd = true)const;

	/**
		Assigns a statically typed string.
		@param text			statically typed text
		@param length		text length. should not be zero.
	*/
	void assignStaticText(const wchar_t* const text, int length);

	/** 
		Returns a subsection of the string.

		If the range specified is beyond the limits of the string, empty string
		will be return.

		@param start   the index of the start of the substring needed
		@param end     all characters from start up to this index are returned
	*/
	TString subString(int start, int end)const;

	/**
		Case-insensitive comparison with another string. Slower than "Compare" method.
		@returns     true if the two strings are identical, false if not
	*/
	bool compareIgnoreCase(const TString& otherString)const;

	/** 
		Case-sensitive comparison with another string.
		@returns     true if the two strings are identical, false if not
	*/
	bool compare(const TString& otherString)const;

	/** 
		Case-sensitive comparison with statically typed string.
		@param text		statically typed text.
		@returns		true if the two strings are identical, false if not
	*/
	bool compareWithStaticText(const wchar_t* const text)const;

	/**
		Compare first character with given unicode character
	*/
	bool startsWithChar(wchar_t character)const;

	/**
		Compare first character with given ansi character
	*/
	bool startsWithChar(char character)const;

	/**
		Compare last character with given unicode character
	*/
	bool endsWithChar(wchar_t character)const;

	/**
		Compare last character with given ansi character
	*/
	bool endsWithChar(char character)const;

	/**
		Check if string is quoted or not
	*/
	bool isQuotedString()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	wchar_t getCharAt(int index)const;

	/**
		Returns number of characters in string
	*/
	int getLength()const;

	/**
		Returns true if string is empty
	*/
	bool isEmpty()const;

	bool isNotEmpty()const;

	/**
		Returns value of string
	*/
	int getIntValue()const;

	/** 
		Returns an upper-case version of this string.
	*/
	TString toUpperCase()const;

	/** 
		Returns an lower-case version of this string. 
	*/
	TString toLowerCase()const;

	virtual ~TString();

private:
	MCL_LEAK_DETECTOR(TString)
};

const TString operator+ (const char* const string1, const TString& string2);

const TString operator+ (const wchar_t* const string1, const TString& string2);

const TString operator+ (const TString& string1, const TString& string2);

#define LEN_UNI_STR(X) (sizeof(X) / sizeof(wchar_t)) - 1

#define LEN_ANSI_STR(X) (sizeof(X) / sizeof(char)) - 1

// do not make a copy + do not free + do not calculate length
#define CONST_TXT(X) TString(L##X, TString::STATIC_TEXT_DO_NOT_FREE, LEN_UNI_STR(L##X))

// do not make a copy + do not free + calculate length
#define STATIC_TXT(X) TString(L##X, TString::STATIC_TEXT_DO_NOT_FREE, -1)

// do not make a copy + free when done + calculate length
#define BUFFER_TXT(X) TString(X, TString::FREE_TEXT_WHEN_DONE, -1)

// can be use like this: KString str(CONST_TXT_PARAMS("Hello World"));
#define CONST_TXT_PARAMS(X) L##X, TString::STATIC_TEXT_DO_NOT_FREE, LEN_UNI_STR(L##X)

#define TXT_WITH_LEN(X) L##X, LEN_UNI_STR(L##X)
