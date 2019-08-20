
/*
	MCL Framework v0.1
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

#include <windows.h>
#include <commctrl.h>

#ifdef _MSC_VER
	#include <crtdbg.h>
#else
	#include <assert.h>
#endif

#ifdef _MSC_VER
	#pragma comment(lib, "Comctl32.lib")
	#pragma comment(lib, "Rpcrt4.lib")
	#pragma comment(lib, "Winhttp.lib")
#endif

#include "config.h"
#include "eventprop.h"

#include "utils/TPlatformUtil.h"
#include "utils/TPerformanceCounter.h"
#include "utils/TRegistry.h"
#include "utils/TTime.h"
#include "utils/TGuid.h"
#include "TApplication.h"
#include "text/TString.h"
#include "graphics/TFont.h"
#include "graphics/TIcon.h"
#include "graphics/TBitmap.h"
#include "graphics/TCursor.h"
#include "graphics/TGraphics.h"
#include "gui/TMouse.h"
#include "gui/TMessage.h"
#include "gui/TControl.h"
#include "gui/TComponent.h"
#include "gui/TWindow.h"
#include "gui/TForm.h"
#include "gui/TChildForm.h"
#include "gui/TButton.h"
#include "gui/TLabel.h"
#include "gui/TCheckBox.h"
#include "gui/TRadioButton.h"
#include "gui/TPushButton.h"
#include "gui/TGroupBox.h"
#include "gui/TProgressBar.h"
#include "gui/TMenuItem.h"
#include "gui/TMenu.h"
#include "gui/TMenuBar.h"
#include "gui/TEditBox.h"
#include "gui/TPasswordBox.h"
#include "gui/TTextArea.h"
#include "gui/TListBox.h"
#include "gui/TComboBox.h"
#include "gui/TSlider.h"
#include "gui/TOpenDialog.h"
#include "gui/TSaveDialog.h"
#include "gui/TNumericField.h"
#include "gui/TGridView.h"
#include "gui/TGlyphButton.h"
#include "gui/TMenuButton.h"
#include "gui/TToolTip.h"
#include "threads/TThread.h"
#include "threads/TTimer.h"
#include "io/TFile.h"
#include "io/TDirectory.h"
#include "io/TSettingsWriter.h"
#include "io/TSettingsReader.h"
#include "io/TLogger.h"
#include "security/TMD5.h"
#include "security/TSHA1.h"
#include "containers/TPointerList.h"
#include "containers/TScopedClassPointer.h"
#include "containers/TScopedMemoryBlock.h"
#include "containers/TScopedCriticalSection.h"
#include "containers/TLeakDetector.h"
#include "containers/TScopedGdiObject.h"
#include "containers/TScopedComPointer.h"
#include "containers/TScopedStructPointer.h"
#include "network/TInternet.h"

LRESULT CALLBACK GlobalWnd_Proc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK GlobalDlg_Proc(HWND, UINT, WPARAM, LPARAM);
unsigned __stdcall GlobalThread_Proc(void*);

HWND CreateMCLComponent(TComponent* component);
bool CreateMCLThread(TThread* thread);

/**
	Important: hInstance is current module HINSTANCE.
	If you are in EXE, then hInstance is HINSTANCE provided by WinMain.
	If you are in DLL, then hInstance is HINSTANCE provided by DllMain or HMODULE of the DLL.
	If you are in Console app, then pass zero.
*/
void InitMCL(HINSTANCE hInstance);
void DeInitMCL();

/** 
	hwnd can be window, custom control, dialog or common control.
	hwnd will be subclassed if it common control or dialog.
*/
void AttachMCLPropertiesToHWND(HWND hwnd, TComponent* component);

#define START_APPLICATION(AppClass) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	::InitMCL(hInstance); \
	int argc = 0; \
	LPWSTR *args = ::CommandLineToArgvW(GetCommandLineW(), &argc); \
	TString **str_argv = (TString**)::malloc(argc * MCL_PTR_SIZE); \
	for(int i = 0; i < argc; i++){str_argv[i] = new TString(args[i], TString::STATIC_TEXT_DO_NOT_FREE);} \
	AppClass* application = new AppClass(); \
	int retVal = 0; \
	if (application->allowMultipleInstances()){ \
		retVal = application->main(str_argv, argc); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application->getApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application->main(str_argv, argc); \
		}else{ \
			retVal = application->anotherInstanceIsRunning(str_argv, argc); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	delete application; \
	for(int i = 0; i < argc; i++){delete str_argv[i];} \
	::DeInitMCL(); \
	::free((void*)str_argv); \
	::GlobalFree(args); \
	return retVal; \
}

// require to support XP/Vista styles.
#ifdef _MSC_VER
	#ifndef MCL_NO_MANIFEST
		#ifdef MCL64
			#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
		#else
			#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
		#endif
	#endif
#endif

#define BEGIN_MSG_HANDLER \
	virtual LRESULT windowProcHandler(TMessage& message) \
	{\
	switch(message.msg)\
	{

#define ON_MSG(_TMsg,_TMsgHandler) \
	case _TMsg: return _TMsgHandler(message);

#define END_MSG_HANDLER(_TComponentParentClass) \
	default: return _TComponentParentClass::windowProcHandler(message); \
	}\
	}

#define TFORMAT_ID(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
	(((DWORD)(ch4)& 0xFF00) << 8) | \
	(((DWORD)(ch4)& 0xFF0000) >> 8) | \
	(((DWORD)(ch4)& 0xFF000000) >> 24))

// generates filter text for TFILE_FILTER("Text Files", "txt") as follows: L"Text Files\0*.txt\0"
#define TFILE_FILTER(desc, ext) L##desc L"\0*." L##ext L"\0"

class InternalDefinitions
{
public:
	static ATOM MCLPropAtom_Component;
	static ATOM MCLPropAtom_OldProc;
};

#ifdef _DEBUG
	#ifdef _MSC_VER
		#define MCL_INIT_VERIFIER _ASSERT_EXPR((TApplication::hInstance != 0), L"##### MCL Framework used before being initialized! Did you forget to call the InitMCL function? Or did you declared MCL class as a global variable? #####")
	#else
		#define MCL_INIT_VERIFIER assert((TApplication::hInstance != 0) && "##### MCL Framework used before being initialized! Did you forget to call the InitMCL function? Or did you declared MCL class as a global variable? #####")
	#endif
#else
	#define MCL_INIT_VERIFIER
#endif
