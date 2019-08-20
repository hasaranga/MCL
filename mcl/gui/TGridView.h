
/*
	MCL - TGridView.h
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

class TGridView : public TControl
{
protected:
	int colCount;
	int itemCount;

public:

	// void onItemSelect(TComponent* sender, int selectedRow)
	EVENT_DEF(void, onItemSelect, TComponent*, int)

	// void onItemRightClick(TComponent* sender, int selectedRow)
	EVENT_DEF(void, onItemRightClick, TComponent*, int)

	// void onItemDoubleClick(TComponent* sender, int selectedRow)
	EVENT_DEF(void, onItemDoubleClick, TComponent*, int)

	TGridView(bool sortItems = false);

	virtual void insertRecord(TString **columnsData);

	virtual void insertRecordTo(int rowIndex, TString **columnsData);

	virtual TString getRecordAt(int rowIndex, int columnIndex);

	/**
		returns -1 if nothing selected.
	*/
	virtual int getSelectedRow();

	virtual void removeRecordAt(int rowIndex);

	virtual void removeAll();

	virtual void updateRecordAt(int rowIndex, int columnIndex, const TString& text);

	virtual void setColumnWidth(int columnIndex, int columnWidth);

	virtual int getColumnWidth(int columnIndex);

	virtual void createColumn(const TString& text, int columnWidth = 100);

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual bool create() override;

	virtual ~TGridView();
};
