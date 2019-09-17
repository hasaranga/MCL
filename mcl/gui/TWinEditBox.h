
/*
	MCL - TWinEditBox.h
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

#include "TControl.h"

class TWinEditBox : public TControl
{
protected:
	bool readOnlyProperty;

public:

	PROPERTY_DEF(bool, readOnly, isReadOnly, setReadOnly)

	PROPERTY_DEF_WRITEONLY(bool, lowercaseOnly, setLowercaseOnly)

	PROPERTY_DEF_WRITEONLY(bool, uppercaseOnly, setUppercaseOnly)

	// void onChange(TComponent* sender)
	EVENT_DEF(void, onChange, TComponent*)

	TWinEditBox();

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline bool isReadOnly() { return isReadOnlyImpl(); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setReadOnly(bool readOnly_) { setReadOnlyImpl(readOnly_); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setLowercaseOnly(bool lowercaseOnly_) { setLowercaseOnlyImpl(lowercaseOnly_); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setUppercaseOnly(bool uppercaseOnly_) { setUppercaseOnlyImpl(uppercaseOnly_); }

	virtual bool create() override;

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual ~TWinEditBox();

private:
	virtual bool isReadOnlyImpl();
	virtual void setReadOnlyImpl(bool readOnly_);
	virtual void setLowercaseOnlyImpl(bool lowercaseOnly_);
	virtual void setUppercaseOnlyImpl(bool uppercaseOnly_);

	virtual TString getTextImpl() override;
};
