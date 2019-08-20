
/*
	MCL - TChildForm.h
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

#include "TForm.h"

class TChildForm : public TForm
{
public:

	// child windows are created when calling setParent method or asigning value to the parent property.
	TChildForm(int formType_ = TForm::TSTANDARD_WINDOW) : TForm(formType_, false)
	{
		this->setFormType(formType_);
	}

	virtual void setFormType(int formType_)
	{
		formTypeProperty = formType_;

		if (formTypeProperty == TSTANDARD_WINDOW)
		{
			this->setStyle(WS_CHILD | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		}
		else if (formTypeProperty == TDIALOG)
		{
			this->setStyle(WS_CHILD | WS_CAPTION | WS_SYSMENU);
		}
		else if (formTypeProperty == TTOOL_WINDOW)
		{
			this->setStyle(WS_CHILD | WS_CAPTION | WS_SYSMENU);
			this->setExtendedStyle(WS_EX_TOOLWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT);
		}
	}

	// this method will also create the window.
	virtual void setParent(HWND parentHandle) override
	{
		parentProperty = parentHandle;
		this->create();
	}
};
