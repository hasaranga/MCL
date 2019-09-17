
/*
	MCL - TWinForm.h
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

#include "TWindow.h"
#include "TWinMenuBar.h"

class TWinForm : public TWindow
{
protected:
	int formTypeProperty;
	bool maximizeButtonProperty;
	bool minimizeButtonProperty;
	bool closeButtonProperty;
	bool resizableProperty;
	bool alwaysOnTopProperty;
	TWinMenuBar* menuProperty;
	TWinMenu* contextMenuProperty;

public:

	int closeBehaviour;
	bool exitModalLoop;

	PROPERTY_DEF(int, formType, getFormType, setFormType)

	PROPERTY_DEF(bool, maximizeButton, hasMaximizeButton, setEnableMaximizeButton)

	PROPERTY_DEF(bool, minimizeButton, hasMinimizeButton, setEnableMinimizeButton)

	PROPERTY_DEF(bool, closeButton, hasCloseButton, setEnableCloseButton)

	PROPERTY_DEF(bool, resizable, isResizable, setEnableResizable)

	PROPERTY_DEF(bool, alwaysOnTop, isAlwaysOnTop, setEnableAlwaysOnTop)

	PROPERTY_DEF_WRITEONLY(TWinMenuBar&, menu, setMenu)

	PROPERTY_DEF_WRITEONLY(TWinMenu&, contextMenu, setContextMenu)

	// void onClose(TComponent* sender)
	EVENT_DEF(void, onClose, TComponent*)

	// void onMove(TComponent* sender)
	EVENT_DEF(void, onMove, TComponent*)

	// void onResize(TComponent* sender)
	EVENT_DEF(void, onResize, TComponent*)

	// void onShow(TComponent* sender)
	EVENT_DEF(void, onShow, TComponent*)

	// void onHide(TComponent* sender)
	EVENT_DEF(void, onHide, TComponent*)

	enum CloseBehaviour
	{
		HIDE_ON_CLOSE = 0,
		DESTROY_ON_CLOSE = 1,
		DESTROY_AND_EXIT_ON_CLOSE = 3,
		DO_NOTHING = 4
	};

	enum FormTypes
	{
		TSTANDARD_WINDOW = 0, // title bar + max + min + close buttons
		TDIALOG = 1, // title bar + close button + no resizable
		TTOOL_WINDOW = 2, // tool window
		TPOPUP_WINDOW = 3 // window without title bar + no taskbar button
	};

	TWinForm(int formType_ = TWinForm::FormTypes::TSTANDARD_WINDOW, bool createWindow = true)
	{
		menuProperty = 0;
		contextMenuProperty = 0;
		closeBehaviour = CloseBehaviour::DESTROY_AND_EXIT_ON_CLOSE;
		textProperty.assignStaticText(TXT_WITH_LEN("TWinForm"));

		onClose = nullptr;
		onMove = nullptr;
		onResize = nullptr;
		onShow = nullptr;
		onHide = nullptr;

		this->setFormType(formType_);

		if (createWindow)
			this->create();
	}

	virtual ~TWinForm() {}

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setContextMenu(TWinMenu& contextMenu) { setContextMenuImpl(contextMenu); }

	virtual bool create() override
	{
		if (TWindow::create())
		{
			if (menuProperty)
				::SetMenu(handleProperty, menuProperty->getHandle());

			return true;
		}
		return false;
	}

	virtual void close()
	{
		if (handleProperty)
			::SendMessageW(handleProperty, WM_CLOSE, 0, 0);
	}

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setMenu(TWinMenuBar& menu) { setMenuImpl(menu); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline bool isAlwaysOnTop() { return isAlwaysOnTopImpl(); }
		
	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setEnableAlwaysOnTop(bool enableAlwaysOnTop) { setEnableAlwaysOnTopImpl(enableAlwaysOnTop); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline bool isResizable() { return isResizableImpl(); }
		
	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setEnableResizable(bool enableResizeBox) { setEnableResizableImpl(enableResizeBox); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline bool hasCloseButton() { return hasCloseButtonImpl(); }
	
	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setEnableCloseButton(bool enableCloseButton) { setEnableCloseButtonImpl(enableCloseButton); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline bool hasMinimizeButton() { return hasMinimizeButtonImpl(); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setEnableMinimizeButton(bool enableMinimizeButton) { setEnableMinimizeButtonImpl(enableMinimizeButton); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline bool hasMaximizeButton() { return hasMaximizeButtonImpl(); }
	
	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setEnableMaximizeButton(bool enableMaximizeButton) { setEnableMaximizeButtonImpl(enableMaximizeButton); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline int getFormType(){ return getFormTypeImpl(); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setFormType(int formType_) { setFormTypeImpl(formType_); }

	// form must be created before calling this method
	virtual bool setClientAreaSize(int width, int height)
	{
		if (handleProperty)
		{
			RECT wndRect;
			::GetClientRect(handleProperty, &wndRect);

			wndRect.right = wndRect.left + width;
			wndRect.bottom = wndRect.top + height;

			::AdjustWindowRect(&wndRect, styleProperty, ::GetMenu(handleProperty) == NULL ? FALSE : TRUE);
			this->setWidth(wndRect.right - wndRect.left);
			this ->setHeight(wndRect.bottom - wndRect.top);

			return true;
		}
		return false;
	}

	// form must be created before calling this method
	virtual void flash()
	{
		if (handleProperty)
			::FlashWindow(handleProperty, TRUE);
	}

	virtual void setIcon(HICON icon)
	{
		wc.hIcon = icon;
		if(handleProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HICON, (LONG_PTR)icon);
	}

	virtual void centerScreen()
	{
		if (parentProperty) // put the window on same monitor as parent + centered
		{
			HMONITOR hmon = ::MonitorFromWindow(parentProperty, MONITOR_DEFAULTTONEAREST);

			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(MONITORINFO);
			::GetMonitorInfo(hmon, &monitorInfo);

			const int posX = monitorInfo.rcMonitor.left + 
				(((monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left) - widthProperty) / 2);

			this->setPosition(posX, 
				((monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top) - heightProperty) / 2);
		}
		else // center on default monitor
		{
			this->setPosition((::GetSystemMetrics(SM_CXSCREEN) - widthProperty) / 2,
				(::GetSystemMetrics(SM_CYSCREEN) - heightProperty) / 2);
		}
	}

	virtual int showAsModal(bool hideOnEscapeKey = true)
	{
		this->setVisible(true);

		if (parentProperty)
			::EnableWindow(parentProperty, FALSE);

		MSG msg;
		for (exitModalLoop = false; !exitModalLoop; ::WaitMessage())
		{
			while (::PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					exitModalLoop = true;
					::PostMessageW(NULL, WM_QUIT, 0, 0);
					break;
				}

				if (hideOnEscapeKey)
				{
					if ((msg.message == WM_KEYDOWN) && (msg.wParam == VK_ESCAPE))
					{
						exitModalLoop = true;
						break;
					}
				}

				if (!::IsDialogMessage(handleProperty, &msg))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}

			if (exitModalLoop)
				break;
		}

		if (parentProperty)
			::EnableWindow(parentProperty, TRUE);

		this->setVisible(false);

		return 0;
	}

	// form must be created before calling this method
	virtual bool getClientAreaSize(int& width, int& height)
	{
		if (handleProperty)
		{
			RECT wndRect;
			::GetClientRect(handleProperty, &wndRect);

			width = wndRect.right - wndRect.left;
			height = wndRect.bottom - wndRect.top;

			return true;
		}
		return false;
	}

	virtual LRESULT windowProcHandler(TMessage& message) override
	{
		switch (message.msg)
		{
		case WM_CLOSE:
			if (onClose)
				onClose(this);

			if (closeBehaviour == HIDE_ON_CLOSE)
				this->setVisible(false);
			else if ((closeBehaviour == DESTROY_ON_CLOSE) || (closeBehaviour == DESTROY_AND_EXIT_ON_CLOSE))
				this->destroy();
				
			break;

		case WM_DESTROY:
			if (onDestroy)
				onDestroy(this);

			if (closeBehaviour == DESTROY_AND_EXIT_ON_CLOSE)
				TApplication::shouldQuit();		

			break;

		case WM_MOVE:
			{
				LRESULT ret = TWindow::windowProcHandler(message); // updates left & top

				if (onMove)
					onMove(this);

				return ret;
			}

		case WM_SIZE:
			{
				LRESULT ret = TWindow::windowProcHandler(message); // updates width & height

				if (onResize)
					onResize(this);

				return ret;
			}

		case WM_SHOWWINDOW:
			if (message.wParam == TRUE)
			{
				if (onShow)
					onShow(this);
			}
			else
			{
				if (onHide)
					onHide(this);
			}
			return TWindow::windowProcHandler(message);

		case WM_CONTEXTMENU:
			if ((contextMenuProperty) && ((HWND)message.wParam == handleProperty)) // do not show for child controls
			{
				POINT p;
				::GetCursorPos(&p);
				::TrackPopupMenu(contextMenuProperty->getMenuHandle(), TPM_LEFTBUTTON, p.x, p.y, 0, handleProperty, NULL);
				break;
			}
			return TWindow::windowProcHandler(message);

		default:
			return TWindow::windowProcHandler(message);
		}

		return 0;
	}

private:
	virtual int getFormTypeImpl()
	{
		return formTypeProperty;
	}

	virtual void setFormTypeImpl(int formType_)
	{
		formTypeProperty = formType_;

		closeButtonProperty = true;
		alwaysOnTopProperty = false;

		if (formTypeProperty == TSTANDARD_WINDOW)
		{
			this->setStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX);
			minimizeButtonProperty = true;
			maximizeButtonProperty = true;
			resizableProperty = true;
		}
		else if (formTypeProperty == TDIALOG)
		{
			this->setStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU);
			maximizeButtonProperty = false;
			minimizeButtonProperty = false;
			resizableProperty = false;
		}
		else if (formTypeProperty == TTOOL_WINDOW)
		{
			this->setStyle(WS_POPUP | WS_CAPTION | WS_SYSMENU);
			this->setExtendedStyle(WS_EX_TOOLWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT);
			maximizeButtonProperty = false;
			minimizeButtonProperty = false;
			resizableProperty = false;
		}
	}

	virtual bool hasMinimizeButtonImpl()
	{
		return minimizeButtonProperty;
	}

	virtual void setEnableMinimizeButtonImpl(bool enableMinimizeButton)
	{
		minimizeButtonProperty = enableMinimizeButton;

		if (enableMinimizeButton)
			this->setStyle(styleProperty | WS_MINIMIZEBOX);
		else
			this->setStyle(styleProperty & ~WS_MINIMIZEBOX);
	}

	virtual bool hasMaximizeButtonImpl()
	{
		return maximizeButtonProperty;
	}

	virtual void setEnableMaximizeButtonImpl(bool enableMaximizeButton)
	{
		maximizeButtonProperty = enableMaximizeButton;

		if (enableMaximizeButton)
			this->setStyle(styleProperty | WS_MAXIMIZEBOX);
		else
			this->setStyle(styleProperty & ~WS_MAXIMIZEBOX);
	}

	virtual bool hasCloseButtonImpl()
	{
		return closeButtonProperty;
	}

	virtual void setEnableCloseButtonImpl(bool enableCloseButton)
	{
		closeButtonProperty = enableCloseButton;

		if (enableCloseButton)
			::EnableMenuItem(::GetSystemMenu(handleProperty, FALSE), SC_CLOSE,
				MF_BYCOMMAND | MF_ENABLED);
		else
			::EnableMenuItem(::GetSystemMenu(handleProperty, FALSE), SC_CLOSE,
				MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}

	virtual bool isResizableImpl()
	{
		return resizableProperty;
	}

	virtual void setEnableResizableImpl(bool enableResizeBox)
	{
		resizableProperty = enableResizeBox;

		if (enableResizeBox)
			this->setStyle(styleProperty | WS_SIZEBOX);
		else
			this->setStyle(styleProperty & ~WS_SIZEBOX);
	}

	virtual bool isAlwaysOnTopImpl()
	{
		return alwaysOnTopProperty;
	}

	virtual void setEnableAlwaysOnTopImpl(bool enableAlwaysOnTop)
	{
		alwaysOnTopProperty = enableAlwaysOnTop;

		if (enableAlwaysOnTop)
			::SetWindowPos(handleProperty, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			::SetWindowPos(handleProperty, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	virtual void setMenuImpl(TWinMenuBar& menu)
	{
		menuProperty = &menu; // this points to the real object!

		if (handleProperty)
			::SetMenu(handleProperty, menuProperty->getHandle());
	}

	virtual void setContextMenuImpl(TWinMenu& contextMenu)
	{
		contextMenuProperty = &contextMenu;  // this points to the real object!
	}
};
