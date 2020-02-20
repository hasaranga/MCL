
/*
	MCL - mcl.cpp
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

#include "mcl.h"
#include <process.h>

class InternalVariables
{
public:
	static TComponent *currentComponent;
	static HHOOK wnd_hook;
	static volatile int mclRefCount;
	static Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	static ULONG_PTR gdiplusToken;

	static CRITICAL_SECTION csForCurrentComponent; // guard currentComponent!
};

TComponent*	InternalVariables::currentComponent = 0;
HHOOK InternalVariables::wnd_hook = 0;
volatile int InternalVariables::mclRefCount = 0;
Gdiplus::GdiplusStartupInput InternalVariables::gdiplusStartupInput;
ULONG_PTR InternalVariables::gdiplusToken;

CRITICAL_SECTION InternalVariables::csForCurrentComponent;

ATOM InternalDefinitions::MCLPropAtom_Component;
ATOM InternalDefinitions::MCLPropAtom_OldProc;

void AttachMCLPropertiesToHWND(HWND hwnd, TComponent* component)
{
	::SetPropW(hwnd, MAKEINTATOM(InternalDefinitions::MCLPropAtom_Component), (HANDLE)component);

	FARPROC lpfnOldWndProc = (FARPROC)::GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
	::SetPropW(hwnd, MAKEINTATOM(InternalDefinitions::MCLPropAtom_OldProc), (HANDLE)lpfnOldWndProc);

	if (lpfnOldWndProc != (void*)GlobalWnd_Proc) // sublcass only if window proc is not GlobalWnd_Proc (common control or dialog)
		::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)::GlobalWnd_Proc); // subclassing...
}

LRESULT CALLBACK MCL_CTL_CBTProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	if(nCode < 0)
		return ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);

	if(nCode==HCBT_CREATEWND){
		HWND hwnd = (HWND)wParam;
		LPCBT_CREATEWNDW cbtCreateWnd = (LPCBT_CREATEWNDW)lParam;
		if (cbtCreateWnd)
		{
			if (cbtCreateWnd->lpcs)
			{
				if (cbtCreateWnd->lpcs->lpCreateParams == InternalVariables::currentComponent) // only catch what we created. ignore unknown windows.
				{
					::AttachMCLPropertiesToHWND(hwnd, InternalVariables::currentComponent);

					// Call the next hook, if there is one
					const LRESULT result = ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);

					// we subclassed what we created. so remove the hook.
					::UnhookWindowsHookEx(InternalVariables::wnd_hook); // unhooking at here will allow child creation at WM_CREATE. otherwise this will hook child also!

					return result;
				}
			}
		}
	}

	// Call the next hook, if there is one
	return ::CallNextHookEx(InternalVariables::wnd_hook, nCode, wParam, lParam);
}

LRESULT CALLBACK GlobalWnd_Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	TComponent *component = (TComponent*)::GetPropW(hwnd, MAKEINTATOM(InternalDefinitions::MCLPropAtom_Component));

	if(!component) // for safe!
		return ::DefWindowProcW( hwnd, msg, wParam, lParam );

	if(!component->getHandle()) // window recieve msg for the first time!
		component->setHandle(hwnd);

	if(msg == WM_NCDESTROY){

		component->setHandle(0); // make handleProperty null, So, we can create the component again!
		::RemovePropW(hwnd, MAKEINTATOM(InternalDefinitions::MCLPropAtom_Component));

		FARPROC lpfnOldWndProc = (FARPROC)::GetPropW(hwnd, MAKEINTATOM(InternalDefinitions::MCLPropAtom_OldProc));
		::RemovePropW(hwnd,  MAKEINTATOM(InternalDefinitions::MCLPropAtom_OldProc));

		if (lpfnOldWndProc)
		{
			if (lpfnOldWndProc != (void*)GlobalWnd_Proc) // common control or dialog
			{
				::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)lpfnOldWndProc); // restore default wnd proc!
				return ::CallWindowProcW((WNDPROC)lpfnOldWndProc, hwnd, msg, wParam, lParam);
			}
		}
		 // lpfnOldWndProc is not set or window or custom control
		return ::DefWindowProcW( hwnd, msg, wParam, lParam );		
	}

	TMessage message = { hwnd, msg, wParam, lParam };

	if (component->windowProc) // is event assigned
		return component->windowProc(message);

	return component->windowProcHandler(message);
}

HWND CreateMCLComponent(TComponent* component)
{	
	::EnterCriticalSection(&InternalVariables::csForCurrentComponent);

	InternalVariables::currentComponent = component;

	// install hook to get called before WM_CREATE_WINDOW msg!
	InternalVariables::wnd_hook = ::SetWindowsHookExW(WH_CBT, &MCL_CTL_CBTProc, 0, ::GetCurrentThreadId());

	// pass current component as lpParam. so CBT proc can ignore other unknown windows.
	HWND hwnd = ::CreateWindowExW(component->extendedStyle, component->className, component->text, 
		component->style, component->left, component->top, component->width, component->height, 
		component->parent, (HMENU)component->getControlID(), TApplication::hInstance, (LPVOID)component);

	// unhook at here will cause catching childs which are created at WM_CREATE. so, unhook at CBT proc!
	//::UnhookWindowsHookEx(InternalVariables::wnd_hook);

	::LeaveCriticalSection(&InternalVariables::csForCurrentComponent);

	return hwnd;
}

unsigned __stdcall GlobalThread_Proc(void* lpParameter)
{
	if(lpParameter == 0) // for safe!
		return 0;

	TThread* thread = (TThread*)lpParameter;
	if (thread->onRun)
		thread->onRun(thread);

	return 0;	
}

bool CreateMCLThread(TThread* thread)
{
	if(thread)
	{
		// create thread in suspended state. so we can set the handle field.
		HANDLE handle = (HANDLE)::_beginthreadex(NULL, 0, GlobalThread_Proc, thread, CREATE_SUSPENDED, NULL);
		//HANDLE handle = ::CreateThread(NULL, 0, ::GlobalThread_Proc, thread, CREATE_SUSPENDED, NULL);

		if (handle)
		{
			thread->setHandle(handle);
			::ResumeThread(handle);

			return true;
		}
	}
	return false;
}

void InitMCL(HINSTANCE hInstance)
{
	if (!InternalVariables::mclRefCount)
	{
		if (!hInstance)
		{
			//hInstance = ::GetModuleHandleW(NULL); // not work for dll
			::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&InitMCL, &hInstance);
		}

		TApplication::hInstance = hInstance;

		TPlatformUtil::getInstance(); // create instance for first time
		
		INITCOMMONCONTROLSEX icx;
		icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icx.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&icx);

		::CoInitialize(NULL); //Initializes COM as STA.

		::InitializeCriticalSection(&InternalVariables::csForCurrentComponent);

		Gdiplus::GdiplusStartup(&InternalVariables::gdiplusToken, &InternalVariables::gdiplusStartupInput, NULL); // Initialize GDI+.

		InternalDefinitions::MCLPropAtom_Component = ::GlobalAddAtomW(L"MCLComponent");
		InternalDefinitions::MCLPropAtom_OldProc = ::GlobalAddAtomW(L"MCLOldProc");
	}
	++InternalVariables::mclRefCount;
}

void DeInitMCL()
{
	MCL_INIT_VERIFIER;

	--InternalVariables::mclRefCount;
	if (!InternalVariables::mclRefCount)
	{
		::CoUninitialize();
		::DeleteCriticalSection(&InternalVariables::csForCurrentComponent);

		Gdiplus::GdiplusShutdown(InternalVariables::gdiplusToken);

		::GlobalDeleteAtom(InternalDefinitions::MCLPropAtom_Component);
		::GlobalDeleteAtom(InternalDefinitions::MCLPropAtom_OldProc);

		// delete all singletons!
		delete TPlatformUtil::getInstance();
	}
}