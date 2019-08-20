
/*
	MCL - TComboBox.h
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
#include "../containers/TPointerList.h"

class TComboBox : public TControl
{
protected:
	TPointerList<TString*> *stringList;
	int itemIndexProperty;

public:

	PROPERTY_DEF(int, itemIndex, getSelectedItemIndex, selectItem)

	PROPERTY_DEF_READONLY(int, itemCount, getItemCount)

	// void onChange(TComponent* sender)
	EVENT_DEF(void, onChange, TComponent*)

	TComboBox(bool sort = false);

	virtual void add(const TString& text);

	virtual void remove(int index);

	virtual void removeByText(const TString& text);

	virtual int getItemIndexByText(const TString& text);

	virtual TString getItemTextByIndex(int index);

	virtual int getItemCount();

	virtual int getSelectedItemIndex();

	virtual TString getSelectedItem();

	virtual void clear();

	virtual void selectItem(int index);

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual bool create() override;

	virtual ~TComboBox();
};
