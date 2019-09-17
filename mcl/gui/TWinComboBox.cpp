
/*
	MCL - TWinComboBox.cpp
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

#include "../mcl.h"
#include "TWinComboBox.h"

#define COMBOBOX_INITIAL_ITEM_ALLOCATION 50

TWinComboBox::TWinComboBox(bool sort)
{
	onChange = nullptr;
	itemIndexProperty = -1;

	classNameProperty.assignStaticText(TXT_WITH_LEN("COMBOBOX"));

	widthProperty = 100;
	heightProperty = 100;

	leftProperty = 0;
	heightProperty = 0;

	styleProperty |= WS_VSCROLL | CBS_DROPDOWNLIST | WS_TABSTOP;

	if(sort)
		styleProperty |= CBS_SORT;

	extendedStyleProperty = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

	stringList = new TPointerList<TString*>(COMBOBOX_INITIAL_ITEM_ALLOCATION);
}

void TWinComboBox::add(const TString& text)
{
	TString *str = new TString(text);
	stringList->addPointer(str);

	if(handleProperty)
		::SendMessageW(handleProperty, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void TWinComboBox::remove(int index)
{
	TString *text = stringList->getPointer(index);
	if (text)
		delete text;

	stringList->removePointer(index);

	if(handleProperty)
		::SendMessageW(handleProperty, CB_DELETESTRING, index, 0);
}

void TWinComboBox::removeByText(const TString& text)
{
	const int itemIndex = this->getItemIndexByText(text);
	if(itemIndex > -1)
		this->remove(itemIndex);
}

int TWinComboBox::getItemIndexByText(const TString& text)
{
	const int listSize = stringList->getSize();
	if(listSize)
	{
		for(int i = 0; i < listSize; i++)
		{
			if(stringList->getPointer(i)->compare(text))
				return i;
		}
	}
	return -1;
}

TString TWinComboBox::getItemTextByIndex(int index)
{
	TString *str = stringList->getPointer(index);

	if(str)
		return *str;

	return TString();
}

int TWinComboBox::getItemCountImpl()
{
	return stringList->getSize();
}

int TWinComboBox::getSelectedItemIndexImpl()
{
	return itemIndexProperty;
}

TString TWinComboBox::getSelectedItem()
{
	const int itemIndex = this->getSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->getPointer(itemIndex);
	return TString();
}

void TWinComboBox::clear()
{
	stringList->deleteAll(true);
	if(handleProperty)
		::SendMessageW(handleProperty, CB_RESETCONTENT, 0, 0);
}

void TWinComboBox::selectItemImpl(int index)
{
	itemIndexProperty = index;
	if(handleProperty)
		::SendMessageW(handleProperty, CB_SETCURSEL, index, 0);
}

bool TWinComboBox::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if ((message.msg == WM_COMMAND) && (HIWORD(message.wParam) == CBN_SELENDOK))
	{
		// update index property
		itemIndexProperty = (int)::SendMessageW(handleProperty, CB_GETCURSEL, 0, 0);

		if (onChange)
			onChange(this);

		result = 0;
		return true;
	}

	return TControl::notifyProcHandler(message, result);
}

bool TWinComboBox::create()
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
				::SendMessageW(handleProperty, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*stringList->getPointer(i));
		}

		if (itemIndexProperty > -1)
			::SendMessageW(handleProperty, CB_SETCURSEL, itemIndexProperty, 0);

		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

TWinComboBox::~TWinComboBox()
{
	stringList->deleteAll(false);
	delete stringList;
}


