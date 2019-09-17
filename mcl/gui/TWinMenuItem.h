
/*
	MCL - TWinMenuItem.h
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

#include "../text/TString.h"
#include <windows.h>
#include "../containers/TLeakDetector.h"
#include "TControl.h"

class TWinMenuItem
{
protected:
	HMENU hMenu;
	UINT itemID;
	TString textProperty;
	bool enabled;
	bool checked;

public:

	PROPERTY_DEF(TString, text, getText, setText)

	// void onPress(TWinMenuItem* sender)
	EVENT_DEF(void, onPress, TWinMenuItem*)

	TWinMenuItem();

	virtual void addToMenu(HMENU hMenu);

	virtual bool isChecked();

	virtual void setCheckedState(bool state);

	virtual bool isEnabled();

	virtual void setEnabled(bool state);

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setText(const TString& text) { setTextImpl(text); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline TString getText() { return getTextImpl(); }

	virtual UINT getItemID();

	virtual HMENU getMenuHandle();

	virtual ~TWinMenuItem();

private:
	virtual void setTextImpl(const TString& text);
	virtual TString getTextImpl();

	MCL_LEAK_DETECTOR(TWinMenuItem)
};
