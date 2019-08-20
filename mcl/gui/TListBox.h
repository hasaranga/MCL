

/*
	MCL - TListBox.h
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

class TListBox : public TControl
{
protected:
	TPointerList<TString*> *stringList;
	int selectedItemIndex;
	int selectedItemEnd;
	bool multipleSelection;

public:

	// void onItemSelect(TComponent* sender, int selectedItemIndex)
	EVENT_DEF(void, onItemSelect, TComponent*, int)

	// void onItemDoubleClick(TComponent* sender, int selectedItemIndex)
	EVENT_DEF(void, onItemDoubleClick, TComponent*, int)

	TListBox(bool multipleSelection = false, bool sort = false, bool vscroll = true);

	virtual void addItem(const TString& text);

	virtual void removeItem(int index);

	virtual void removeItem(const TString& text);

	virtual int getItemIndex(const TString& text);

	virtual int getItemCount();

	virtual int getSelectedItemIndex();

	virtual TString getSelectedItem();

	virtual int getSelectedItems(int* itemArray, int itemCountInArray);

	virtual void clearList();

	virtual void selectItem(int index);

	virtual void selectItems(int start, int end);

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual bool create() override;

	virtual ~TListBox();
};
