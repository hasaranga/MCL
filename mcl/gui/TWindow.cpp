
/*
	MCL - TWindow.cpp
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

#include "TWindow.h"
#include "../mcl.h"

TWindow::TWindow()
{
	textProperty.assignStaticText(TXT_WITH_LEN("TWindow"));

	widthProperty = 400;
	heightProperty = 200;

	visibleProperty = false;

	styleProperty = WS_POPUP;
	extendedStyleProperty = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT;
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

	controlID = 0; // control id is zero for top level window
	lastFocusedChild = 0;
}

void TWindow::setParent(HWND parentHandle)
{
	parentProperty = parentHandle;
	if (handleProperty)
		::SetWindowLongPtrW(handleProperty, GWLP_HWNDPARENT, (LONG_PTR)parentProperty);
}

LRESULT TWindow::windowProcHandler(TMessage& message)
{
	switch(message.msg)
	{
		case WM_DRAWITEM: // owner-drawn button, combo box and list box... (menu ignored. use windowProc of parent window if you want to draw menu)
			{
				if (message.wParam != 0) // ignore menus
				{
					TComponent *component = (TComponent*)::GetPropW(((LPDRAWITEMSTRUCT)message.lParam)->hwndItem, MAKEINTATOM(InternalDefinitions::MCLPropAtom_Component));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->notifyProc)
						{
							if (component->notifyProc(message, result))
								return result;
						}
						else
						{
							if (component->notifyProcHandler(message, result))
								return result;
						}
					}
				}
			}
			return TComponent::windowProcHandler(message);

		case WM_NOTIFY: // GridView, Custom drawing etc...
			{
				TComponent *component = (TComponent*)::GetPropW(((LPNMHDR)message.lParam)->hwndFrom, MAKEINTATOM(InternalDefinitions::MCLPropAtom_Component));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->notifyProc)
					{
						if (component->notifyProc(message, result))
							return result;
					}
					else
					{
						if (component->notifyProcHandler(message, result))
							return result;
					}
				}
			}
			return TComponent::windowProcHandler(message);

		case WM_VKEYTOITEM:
		case WM_CHARTOITEM:
		case WM_HSCROLL: // trackbar
		case WM_VSCROLL:
		case WM_CTLCOLORBTN: // buttons 
		case WM_CTLCOLOREDIT: // edit controls 
		case WM_CTLCOLORLISTBOX: // listbox controls 
		case WM_CTLCOLORSCROLLBAR: // scroll bar controls 
		case WM_CTLCOLORSTATIC: // static controls
			{
				TComponent *component = (TComponent*)::GetPropW((HWND)message.lParam, MAKEINTATOM(InternalDefinitions::MCLPropAtom_Component));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->notifyProc)
					{
						if (component->notifyProc(message, result))
							return result;
					}
					else
					{
						if (component->notifyProcHandler(message, result))
							return result;
					}
				}				
			}
			return TComponent::windowProcHandler(message);

		case WM_MEASUREITEM: // combo box, list box, list-view control... (menu ignored. use windowProc of parent window if you want to set the size of menu)
			{
				if (message.wParam != 0) // ignore menus
				{
					TComponent *component = (TComponent*)::GetPropW(GetDlgItem(message.hwnd,((LPMEASUREITEMSTRUCT)message.lParam)->CtlID), MAKEINTATOM(InternalDefinitions::MCLPropAtom_Component));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->notifyProc)
						{
							if (component->notifyProc(message, result))
								return result;
						}
						else
						{
							if (component->notifyProcHandler(message, result))
								return result;
						}
					}
				}			
			}
			return TComponent::windowProcHandler(message);

		case WM_COMPAREITEM: // owner-drawn combo box or list box
			{
				TComponent *component = (TComponent*)::GetPropW(((LPCOMPAREITEMSTRUCT)message.lParam)->hwndItem, MAKEINTATOM(InternalDefinitions::MCLPropAtom_Component));
				if (component)
				{
					LRESULT result = 0; // just for safe
					if (component->notifyProc)
					{
						if (component->notifyProc(message, result))
							return result;
					}
					else
					{
						if (component->notifyProcHandler(message, result))
							return result;
					}
				}
			}
			return TComponent::windowProcHandler(message);

		case WM_TIMER:
			{
				TTimer *timer = TPlatformUtil::getInstance()->getTimerByID((UINT)message.wParam);
				if(timer)
				{
					if (timer->onTimer)
						timer->onTimer(timer);

					break;
				}
			}
			return TComponent::windowProcHandler(message);

		case WM_SIZE: // window has been resized! we can't use lparam since it's giving client area size instead of window size.
			{
				RECT rect;
				::GetWindowRect(handleProperty, &rect);

				widthProperty = rect.right - rect.left;
				heightProperty = rect.bottom - rect.top;
			}
			return TComponent::windowProcHandler(message);

		case WM_MOVE: // window has been moved! we can't use lparam since it's giving client area pos instead of window position.
			{
				RECT rect;
				::GetWindowRect(handleProperty, &rect);

				leftProperty = rect.left;
				topProperty = rect.top;
			}
			return TComponent::windowProcHandler(message);

		case WM_COMMAND: // button, checkbox, radio button, listbox, combobox or menu-item
			{
				if( (HIWORD(message.wParam) == 0) && (message.lParam == 0) ) // its menu item! unfortunately windows does not send menu handle with clicked event!
				{
					TMenuItem *menuItem = TPlatformUtil::getInstance()->getMenuItemByID(LOWORD(message.wParam));
					if(menuItem)
					{
						if (menuItem->onPress) // is event assigned
							menuItem->onPress(menuItem);

						break;
					}
				}
				else if(message.lParam)// send to appropriate component
				{
					TComponent *component = (TComponent*)::GetPropW((HWND)message.lParam, MAKEINTATOM(InternalDefinitions::MCLPropAtom_Component));
					if (component)
					{
						LRESULT result = 0; // just for safe
						if (component->notifyProc)
						{
							if (component->notifyProc(message, result))
								return result;
						}
						else
						{
							if (component->notifyProcHandler(message, result))
								return result;
						}
					}
				}
				else if (LOWORD(message.wParam) == IDOK) // enter key pressed. (lParam does not contain current comp hwnd)
				{
					HWND currentComponent = ::GetFocus();

					// simulate enter key pressed event into current component. (might be a window)
					::SendMessageW(currentComponent, WM_KEYDOWN, VK_RETURN, 0);
					::SendMessageW(currentComponent, WM_KEYUP, VK_RETURN, 0);
					::SendMessageW(currentComponent, WM_CHAR, VK_RETURN, 0); 

					return 0;
				}
				else if (LOWORD(message.wParam) == IDCANCEL) // Esc key pressed. (lParam does not contain current comp hwnd)
				{
					HWND currentComponent = ::GetFocus();

					// simulate esc key pressed event into current component. (might be a window)
					::SendMessageW(currentComponent, WM_KEYDOWN, VK_ESCAPE, 0);
					::SendMessageW(currentComponent, WM_KEYUP, VK_ESCAPE, 0);
					::SendMessageW(currentComponent, WM_CHAR, VK_ESCAPE, 0); 

					return 0;
				}
			}
			return TComponent::windowProcHandler(message);

		case WM_ACTIVATE: // save last focused item when inactive
			if (message.wParam != WA_INACTIVE)
				return TComponent::windowProcHandler(message);
			this->lastFocusedChild = ::GetFocus();
			break;

		case WM_SETFOCUS:
			if (this->lastFocusedChild) // set focus to last item
				::SetFocus(this->lastFocusedChild);
			else // set focus to first child
				::SetFocus(::GetNextDlgTabItem(handleProperty, NULL, FALSE));
			break;

		default:
			return TComponent::windowProcHandler(message);
	}
	return 0;
}

TWindow::~TWindow()
{

}