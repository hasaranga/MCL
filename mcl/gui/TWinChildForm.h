
/*
	MCL - TWinChildForm.h
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

#include "TWinForm.h"

class TWinChildForm : public TWinForm
{
public:

	// child windows are created when calling setParent method or asigning value to the parent property.
	TWinChildForm(int formType_ = TWinForm::TSTANDARD_WINDOW) : TWinForm(formType_, false)
	{

	}

private:

	// change WS_POPUP to WS_CHILD
	virtual void setFormTypeImpl(int formType_) override
	{
		formTypeProperty = formType_;

		closeButtonProperty = true;
		alwaysOnTopProperty = false;

		if (formTypeProperty == TSTANDARD_WINDOW)
		{
			this->setStyle(WS_CHILD | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);
			minimizeButtonProperty = true;
			maximizeButtonProperty = true;
			resizableProperty = true;
		}
		else if (formTypeProperty == TDIALOG)
		{
			this->setStyle(WS_CHILD | WS_CAPTION | WS_SYSMENU);
			maximizeButtonProperty = false;
			minimizeButtonProperty = false;
			resizableProperty = false;
		}
		else if (formTypeProperty == TTOOL_WINDOW)
		{
			this->setStyle(WS_CHILD | WS_CAPTION | WS_SYSMENU);
			this->setExtendedStyle(WS_EX_TOOLWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT);
			maximizeButtonProperty = false;
			minimizeButtonProperty = false;
			resizableProperty = false;
		}
	}

	// this method will also create the window.
	virtual void setParentImpl(HWND parentHandle) override
	{
		parentProperty = parentHandle;
		this->create();
	}
};
