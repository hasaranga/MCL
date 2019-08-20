
/*
	MCL - TPasswordBox.cpp
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

#include "TPasswordBox.h"

TPasswordBox::TPasswordBox()
{
	passwordCharProperty = '*';
	styleProperty |= ES_PASSWORD;
}

void TPasswordBox::setPasswordChar(const char pwdChar)
{
	this->passwordCharProperty = pwdChar;
	if(handleProperty)
	{
		::SendMessageW(handleProperty, EM_SETPASSWORDCHAR, passwordCharProperty, 0);
		this->repaint();
	}
}

char TPasswordBox::getPasswordChar()
{
	return passwordCharProperty;
}

bool TPasswordBox::create()
{
	if(TEditBox::create())
	{
		::SendMessageW(handleProperty, EM_SETPASSWORDCHAR, passwordCharProperty, 0);
		return true;
	}
	return false;
}

TPasswordBox::~TPasswordBox()
{
}