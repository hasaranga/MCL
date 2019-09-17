
/*
	MCL - TWinPasswordBox.h
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

#include "TWinEditBox.h"

class TWinPasswordBox : public TWinEditBox
{
protected:
	char passwordCharProperty;

public:

	PROPERTY_DEF(char, passwordChar, getPasswordChar, setPasswordChar)

	TWinPasswordBox();

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setPasswordChar(const char pwdChar) { setPasswordCharImpl(pwdChar); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline char getPasswordChar() { return getPasswordCharImpl(); }

	virtual bool create();

	virtual ~TWinPasswordBox();

private:
	virtual void setPasswordCharImpl(const char pwdChar);
	virtual char getPasswordCharImpl();
};
