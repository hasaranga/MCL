
/*
	MCL - TGridView.cpp
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

#include "TGridView.h"
#include "../mcl.h"

TGridView::TGridView(bool sortItems)
{
	itemCount = 0;
	colCount = 0;

	onItemSelect = nullptr;
	onItemRightClick = nullptr;
	onItemDoubleClick = nullptr;

	classNameProperty.assignStaticText(TXT_WITH_LEN("SysListView32"));

	widthProperty = 300;
	heightProperty = 200;

	leftProperty = 0;
	topProperty = 0;

	styleProperty |= WS_TABSTOP | WS_BORDER | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;
	extendedStyleProperty = WS_EX_WINDOWEDGE;

	if (sortItems)
		styleProperty |= LVS_SORTASCENDING;
}

TGridView::~TGridView(){}

void TGridView::insertRecord(TString **columnsData)
{
	LVITEMW lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (*columnsData[0]);
	lvi.iItem = itemCount;

	const int row = (int)::SendMessageW(handleProperty, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvItem = { 0 };
		lvItem.iSubItem = i;
		lvItem.pszText = (*columnsData[i]);

		::SendMessageW(handleProperty, LVM_SETITEMTEXTW, (WPARAM)row, (LPARAM)&lvItem);
	}

	++itemCount;
}

void TGridView::insertRecordTo(int rowIndex, TString **columnsData)
{
	LVITEMW lvi = { 0 };
	lvi.mask = LVIF_TEXT;
	lvi.pszText = (*columnsData[0]);
	lvi.iItem = rowIndex;

	const int row = (int)::SendMessageW(handleProperty, LVM_INSERTITEMW, 0, (LPARAM)&lvi);

	for (int i = 1; i < colCount; i++) // first column already added, lets add the others
	{
		LV_ITEMW lvItem= { 0 };
		lvItem.iSubItem = i;
		lvItem.pszText = (*columnsData[i]);

		::SendMessageW(handleProperty, LVM_SETITEMTEXTW, (WPARAM)row, (LPARAM)&lvItem);
	}

	++itemCount;
}

TString TGridView::getRecordAt(int rowIndex, int columnIndex)
{
	wchar_t *buffer = (wchar_t*)::malloc(512 * sizeof(wchar_t));
	buffer[0] = 0;

	LV_ITEMW lvi = { 0 };
	lvi.iSubItem = columnIndex;
	lvi.cchTextMax = 512;
	lvi.pszText = buffer;

	::SendMessageW(handleProperty, LVM_GETITEMTEXTW, (WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_GetItemText macro. it relies on preprocessor defs.

	return TString(buffer, TString::FREE_TEXT_WHEN_DONE);
}

int TGridView::getSelectedRow()
{
	return ListView_GetNextItem(handleProperty, -1, LVNI_SELECTED);
}

void TGridView::removeRecordAt(int rowIndex)
{
	if (ListView_DeleteItem(handleProperty, rowIndex))
		--itemCount;
}

void TGridView::removeAll()
{
	ListView_DeleteAllItems(handleProperty);
	itemCount = 0;
}

void TGridView::updateRecordAt(int rowIndex, int columnIndex, const TString& text)
{
	LV_ITEMW lvi = { 0 };
	lvi.iSubItem = columnIndex;
	lvi.pszText = text;

	::SendMessageW(handleProperty, LVM_SETITEMTEXTW, (WPARAM)rowIndex, (LPARAM)&lvi); // explicity call unicode version. we can't use ListView_SetItemText macro. it relies on preprocessor defs.
}

void TGridView::setColumnWidth(int columnIndex, int columnWidth)
{
	ListView_SetColumnWidth(handleProperty, columnIndex, columnWidth);
}

int TGridView::getColumnWidth(int columnIndex)
{
	return ListView_GetColumnWidth(handleProperty, columnIndex);
}

void TGridView::createColumn(const TString& text, int columnWidth)
{
	LVCOLUMN lvc = { 0 };

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = columnWidth;
	lvc.pszText = text;
	lvc.iSubItem = colCount;

	::SendMessageW(handleProperty, LVM_INSERTCOLUMNW, (WPARAM)colCount, (LPARAM)&lvc);

	++colCount;
}

bool TGridView::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if (message.msg == WM_NOTIFY)
	{
		if (((LPNMHDR)message.lParam)->code == LVN_ITEMCHANGED) // List view item selection changed (mouse or keyboard)
		{
			LPNMLISTVIEW pNMListView = (LPNMLISTVIEW)message.lParam;
			if ((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVIS_SELECTED))
			{
				if (onItemSelect)
					onItemSelect(this, this->getSelectedRow());

				result = 0;
				return true;
			}
		}
		else if (((LPNMHDR)message.lParam)->code == NM_RCLICK) // List view item right click
		{
			if(onItemRightClick)
				onItemRightClick(this, this->getSelectedRow());

			result = 0;
			return true;
		}
		else if (((LPNMHDR)message.lParam)->code == NM_DBLCLK) // List view item double click
		{
			if(onItemDoubleClick)
				onItemDoubleClick(this, this->getSelectedRow());

			result = 0;
			return true;
		}
	}

	return TControl::notifyProcHandler(message, result);
}

bool TGridView::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		ListView_SetExtendedListViewStyle(handleProperty, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}


