
/*
	MCL - TComponent.cpp
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


#include "TComponent.h"
#include "../mcl.h"
#include "TMouse.h"
#include <windowsx.h>

TComponent::TComponent()
{
	MCL_INIT_VERIFIER;
	isRegistered = false;

	TPlatformUtil *platformUtil = TPlatformUtil::getInstance();
	controlID = platformUtil->generateControlID();

	handleProperty = 0;
	parentProperty = 0;
	styleProperty = 0;
	extendedStyleProperty = 0;
	cursorProperty = 0;
	leftProperty = CW_USEDEFAULT;
	topProperty = CW_USEDEFAULT;
	widthProperty = CW_USEDEFAULT;
	heightProperty = CW_USEDEFAULT;
	visibleProperty = true;
	enabledProperty = true;

	windowProc = nullptr;
	notifyProc = nullptr;
	onCreate = nullptr;
	onDestroy = nullptr;
	onMouseUp = nullptr;
	onMouseDown = nullptr;
	onMouseMove = nullptr;
	onMouseLeave = nullptr;
	onMouseWheel = nullptr;
	onDblClick = nullptr;
	onKeyPress = nullptr;
	onKeyDown = nullptr;
	onKeyUp = nullptr;

	classNameProperty = platformUtil->generateClassName();
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = 0;
	wc.lpszMenuName = 0;
	wc.hbrBackground = (HBRUSH)::GetSysColorBrush(COLOR_BTNFACE);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIconSm = 0;
	wc.style = 0;
	wc.hInstance = TApplication::hInstance;
	wc.lpszClassName = classNameProperty;

	wc.lpfnWndProc = ::GlobalWnd_Proc;
	
	fontProperty = TFont::getDefaultFont();
}

TComponent::operator HWND()const
{
	return handleProperty;
}

UINT TComponent::getControlID()
{
	return controlID;
}

void TComponent::setCursorImpl(HCURSOR cursorHandle)
{
	cursorProperty = cursorHandle;
	if(handleProperty)
		::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);
}

HCURSOR TComponent::getCursorImpl()
{
	return cursorProperty;
}

TString TComponent::getClassNameImpl()
{
	return classNameProperty;
}

bool TComponent::create()
{
	if(!::RegisterClassExW(&wc))
		return false;

	isRegistered = true;

	::CreateMCLComponent(this);

	if(handleProperty)
	{
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if(cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

void TComponent::destroy()
{
	if (handleProperty)
		::DestroyWindow(handleProperty);
}

LRESULT TComponent::windowProcHandler(TMessage& message)
{
	switch (message.msg)
	{
	case WM_MOUSEMOVE:
		if (onMouseMove)
			onMouseMove(this, GET_X_LPARAM(message.lParam), GET_Y_LPARAM(message.lParam));
		break;

	case WM_MOUSELEAVE:
		if (onMouseLeave)
			onMouseLeave(this);
		break;

	case WM_MOUSEWHEEL:
		if (onMouseWheel)
			onMouseWheel(this, GET_WHEEL_DELTA_WPARAM(message.wParam), GET_X_LPARAM(message.lParam), GET_Y_LPARAM(message.lParam));
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		if (onMouseUp)
			onMouseUp(this, (message.msg == WM_LBUTTONUP) ? TMouse::Buttons::LEFT_BUTTON
				: (message.msg == WM_RBUTTONUP) ? TMouse::Buttons::RIGHT_BUTTON
				: TMouse::Buttons::MIDDLE_BUTTON,
				GET_X_LPARAM(message.lParam), GET_Y_LPARAM(message.lParam));
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		if (onMouseDown)
			onMouseDown(this, (message.msg == WM_LBUTTONDOWN) ? TMouse::Buttons::LEFT_BUTTON
				: (message.msg == WM_RBUTTONDOWN) ? TMouse::Buttons::RIGHT_BUTTON
				: TMouse::Buttons::MIDDLE_BUTTON,
				GET_X_LPARAM(message.lParam), GET_Y_LPARAM(message.lParam));
		break;

	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
		if (onDblClick)
			onDblClick(this, (message.msg == WM_LBUTTONUP) ? TMouse::Buttons::LEFT_BUTTON
				: (message.msg == WM_RBUTTONUP) ? TMouse::Buttons::RIGHT_BUTTON
				: TMouse::Buttons::MIDDLE_BUTTON,
				GET_X_LPARAM(message.lParam), GET_Y_LPARAM(message.lParam));
		break;

	case WM_CHAR:
		if (onKeyPress)
			onKeyPress(this, (char)message.wParam);
		break;

	case WM_KEYDOWN:
		if (onKeyDown)
			onKeyDown(this, (short)message.wParam);
		break;

	case WM_KEYUP:
		if (onKeyUp)
			onKeyUp(this, (short)message.wParam);
		break;

	case WM_CREATE:
		if (onCreate)
			onCreate(this);
		break;

	case WM_DESTROY:
		if (onDestroy)
			onDestroy(this);
		break;
	}

	return dispatchToDefaultProc(message);
}

bool TComponent::notifyProcHandler(TMessage& message, LRESULT& result)
{
	return false;
}

LRESULT TComponent::dispatchToDefaultProc(TMessage& message)
{
	FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(message.hwnd, MAKEINTATOM(InternalDefinitions::MCLPropAtom_OldProc));
	if (lpfnOldWndProc)
		if ((void*)lpfnOldWndProc != (void*)::GlobalWnd_Proc) // it's a subclassed common control! MCLOldProc of subclassed control|dialog is not GlobalWnd_Proc function.
			return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, message.hwnd, message.msg, message.wParam, message.lParam);
	return ::DefWindowProcW(message.hwnd, message.msg, message.wParam, message.lParam); // custom control or window
}

void TComponent::setFontImpl(HFONT fontHandle)
{
	fontProperty = fontHandle;
	if(handleProperty)
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0));
}

HFONT TComponent::getFontImpl()
{
	return fontProperty;
}

TString TComponent::getTextImpl()
{
	return textProperty;
}

void TComponent::setTextImpl(const TString& caption)
{
	textProperty = caption;
	if(handleProperty)
		::SetWindowTextW(handleProperty, textProperty);
}

void TComponent::setHandle(HWND hwnd)
{
	handleProperty = hwnd;
}

HWND TComponent::getHandleImpl()
{
	return handleProperty;
}

void TComponent::setParentImpl(HWND parentHandle)
{
	parentProperty = parentHandle;
	if(handleProperty)
		::SetParent(handleProperty, parentProperty);
}

HWND TComponent::getParentImpl()
{
	return parentProperty;
}

DWORD TComponent::getStyleImpl()
{
	return styleProperty;
}

void TComponent::setStyleImpl(DWORD compStyle)
{
	styleProperty = compStyle;
	if(handleProperty)
		::SetWindowLongPtrW(handleProperty, GWL_STYLE, styleProperty);
}

DWORD TComponent::getExtendedStyleImpl()
{
	return extendedStyleProperty;
}

void TComponent::setExtendedStyleImpl(DWORD compExStyle)
{
	extendedStyleProperty = compExStyle;
	if(handleProperty)
		::SetWindowLongPtrW(handleProperty, GWL_EXSTYLE, extendedStyleProperty);
}

int TComponent::getLeftImpl()
{
	return leftProperty; 
}

int TComponent::getTopImpl()
{
	return topProperty;
}

int TComponent::getWidthImpl()
{
	return widthProperty;
}

int TComponent::getHeightImpl()
{
	return heightProperty;
}

void TComponent::setLeftImpl(int compLeft)
{
	this->setPosition(compLeft, topProperty);
}

void TComponent::setTopImpl(int compTop)
{
	this->setPosition(leftProperty, compTop);
}

void TComponent::setPosition(int compLeft, int compTop)
{
	leftProperty = compLeft;
	topProperty = compTop;

	if (handleProperty)
		::SetWindowPos(handleProperty, 0, leftProperty, topProperty, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void TComponent::setWidthImpl(int compWidth)
{
	this->setSize(compWidth, heightProperty);
}

void TComponent::setHeightImpl(int compHeight)
{
	this->setSize(widthProperty, compHeight);
}

void TComponent::setSize(int compWidth, int compHeight)
{
	widthProperty = compWidth;
	heightProperty = compHeight;

	if (handleProperty)
		::SetWindowPos(handleProperty, 0, 0, 0, widthProperty, heightProperty, SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOACTIVATE | SWP_NOZORDER);
}

void TComponent::setVisibleImpl(bool state)
{
	visibleProperty = state;
	if(handleProperty)
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);
}

bool TComponent::getVisibleImpl()
{
	if (handleProperty)
		visibleProperty = (::IsWindowVisible(handleProperty) == TRUE);

	return visibleProperty;
}

void TComponent::show()
{
	this->setVisible(true);
}

void TComponent::hide()
{
	this->setVisible(false);
}

bool TComponent::getEnabledImpl()
{
	if (handleProperty)
		enabledProperty = (::IsWindowEnabled(handleProperty) == TRUE);

	return enabledProperty;
}

void TComponent::setEnabledImpl(bool state)
{
	enabledProperty = state;

	if(handleProperty)
		::EnableWindow(handleProperty, enabledProperty);
}

void TComponent::bringToFront()
{
	if(handleProperty)
		::BringWindowToTop(handleProperty);
}

void TComponent::grabKeyboardFocus()
{
	if(handleProperty)
		::SetFocus(handleProperty);
}

void TComponent::repaint()
{
	if (handleProperty)
	{
		::InvalidateRect(handleProperty, NULL, TRUE);
		::UpdateWindow(handleProperty); // instant update
	}
}

TComponent::~TComponent()
{
	if(isRegistered)
		::UnregisterClassW(classNameProperty, TApplication::hInstance);
}