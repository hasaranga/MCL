
/*
	MCL - TWinListBox.cpp
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

#include "TWinListBox.h"
#include "../mcl.h"

#define INITIAL_LIST_ITEM_COUNT 100

TWinListBox::TWinListBox(bool multipleSelection, bool sort, bool vscroll)
{
	this->multipleSelection = multipleSelection;

	onItemSelect = nullptr;
	onItemDoubleClick = nullptr;

	selectedItemIndex = -1;
	selectedItemEnd = -1;

	classNameProperty.assignStaticText(TXT_WITH_LEN("LISTBOX"));

	widthProperty = 100;
	heightProperty = 100;

	leftProperty = 0;
	topProperty = 0;

	styleProperty |= LBS_NOTIFY | WS_TABSTOP;
	extendedStyleProperty = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

	if(multipleSelection)
		styleProperty |= LBS_MULTIPLESEL;
	if(sort)
		styleProperty |= LBS_SORT;
	if(vscroll)
		styleProperty |= WS_VSCROLL;

	stringList = new TPointerList<TString*>(INITIAL_LIST_ITEM_COUNT);
}

void TWinListBox::addItem(const TString& text)
{
	TString *str = new TString(text);
	stringList->addPointer(str);

	if(handleProperty)
		::SendMessageW(handleProperty, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void TWinListBox::removeItem(int index)
{
	TString *text = stringList->getPointer(index);
	if (text)
		delete text;

	stringList->removePointer(index);

	if(handleProperty)
		::SendMessageW(handleProperty, LB_DELETESTRING, index, 0);
}

void TWinListBox::removeItem(const TString& text)
{
	const int itemIndex = this->getItemIndex(text);
	if(itemIndex > -1)
		this->removeItem(itemIndex);
}

int TWinListBox::getItemIndex(const TString& text)
{
	const int listSize = stringList->getSize();
	if(listSize)
	{
		for(int i = 0; i < listSize; i++)
		{
			if (stringList->getPointer(i)->compare(text))
				return i;
		}
	}
	return -1;
}

int TWinListBox::getItemCount()
{
	return stringList->getSize();
}

int TWinListBox::getSelectedItemIndex()
{
	if(handleProperty)
	{	 
		const int index = (int)::SendMessageW(handleProperty, LB_GETCURSEL, 0, 0);
		if(index != LB_ERR)
			return index;
	}
	return -1;	
}

TString TWinListBox::getSelectedItem()
{
	const int itemIndex = this->getSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->getPointer(itemIndex);
	return TString();
}

int TWinListBox::getSelectedItems(int* itemArray, int itemCountInArray)
{
	if(handleProperty)
	{	 
		const int items = (int)::SendMessageW(handleProperty, LB_GETSELITEMS, itemCountInArray, (LPARAM)itemArray);
		if(items != LB_ERR)
			return items;
	}
	return -1;
}

void TWinListBox::clearList()
{
	stringList->deleteAll(true);

	if(handleProperty)
		::SendMessageW(handleProperty, LB_RESETCONTENT, 0, 0);
}

void TWinListBox::selectItem(int index)
{
	selectedItemIndex = index;

	if(handleProperty)
		::SendMessageW(handleProperty, LB_SETCURSEL, index, 0);
}

void TWinListBox::selectItems(int start, int end)
{
	if(multipleSelection)
	{
		selectedItemIndex = start;
		selectedItemEnd = end;

		if(handleProperty)
			::SendMessageW(handleProperty, LB_SELITEMRANGE, TRUE, MAKELPARAM(start, end));
	}
}

bool TWinListBox::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if (message.msg == WM_COMMAND)
	{
		if (HIWORD(message.wParam) == LBN_SELCHANGE) // listbox sel change!
		{
			if (onItemSelect)
				onItemSelect(this, this->getSelectedItemIndex());

			result = 0;
			return true;
		}
		else if (HIWORD(message.wParam) == LBN_DBLCLK) // listbox double click
		{
			if(onItemDoubleClick)
				onItemDoubleClick(this, this->getSelectedItemIndex());

			result = 0;
			return true;
		}
	}

	return TControl::notifyProcHandler(message, result);
}

bool TWinListBox::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);

		const int listSize = stringList->getSize();
		if (listSize)
		{
			for (int i = 0; i < listSize; i++)
				::SendMessageW(handleProperty, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*stringList->getPointer(i));
		}

		if (!multipleSelection) // single selction!
		{
			if (selectedItemIndex > -1)
				::SendMessageW(handleProperty, LB_SETCURSEL, selectedItemIndex, 0);
		}
		else
		{
			if (selectedItemIndex > -1)
				::SendMessageW(handleProperty, LB_SELITEMRANGE, TRUE, MAKELPARAM(selectedItemIndex, selectedItemEnd));
		}

		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}


TWinListBox::~TWinListBox()
{
	stringList->deleteAll(false);
	delete stringList;
}