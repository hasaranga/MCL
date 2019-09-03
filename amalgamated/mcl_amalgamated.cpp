
// ========== Generated With RFC Amalgamator v1.0 ==========


#include"mcl_amalgamated.h"


// =========== mcl.cpp ===========

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

#include <process.h>

class InternalVariables
{
public:
	static TComponent *currentComponent;
	static HHOOK wnd_hook;
	static volatile int mclRefCount;

	static CRITICAL_SECTION csForCurrentComponent; // guard currentComponent!
};

TComponent*	InternalVariables::currentComponent = 0;
HHOOK InternalVariables::wnd_hook = 0;
volatile int InternalVariables::mclRefCount = 0;

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

		::GlobalDeleteAtom(InternalDefinitions::MCLPropAtom_Component);
		::GlobalDeleteAtom(InternalDefinitions::MCLPropAtom_OldProc);

		// delete all singletons!
		delete TPlatformUtil::getInstance();
	}
}

// =========== TApplication.cpp ===========

/*
	MCL - TApplication.cpp
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


HINSTANCE TApplication::hInstance = 0;

TApplication::TApplication()
{
}

int TApplication::main(TString** argv, int argc)
{
	return 0;
}

bool TApplication::allowMultipleInstances()
{
	return true;
}

int TApplication::anotherInstanceIsRunning(TString** argv, int argc)
{
	return 0;
}

const wchar_t* TApplication::getApplicationID()
{
	return L"MCL_APPLICATION";
}

void TApplication::run(bool handleTabKey)
{
	MSG msg;

	while (::GetMessageW(&msg, NULL, 0, 0))
	{
		if (handleTabKey)
		{
			if (::IsDialogMessage(::GetActiveWindow(), &msg))
				continue;
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
}

void TApplication::shouldQuit()
{
	::PostQuitMessage(0);
}

TApplication::~TApplication()
{
}

// =========== sha1.cpp ===========
/*
	100% free public domain implementation of the SHA-1 algorithm
	by Dominik Reichl <dominik.reichl@t-online.de>
	Web: http://www.dominik-reichl.de/

	Version 1.7 - 2006-12-21
	- Fixed buffer underrun warning which appeared when compiling with
	  Borland C Builder (thanks to Rex Bloom and Tim Gallagher for the
	  patch)
	- Breaking change: ReportHash writes the final hash to the start
	  of the buffer, i.e. it's not appending it to the string any more
	- Made some function parameters const
	- Added Visual Studio 2005 project files to demo project

	Version 1.6 - 2005-02-07 (thanks to Howard Kapustein for patches)
	- You can set the endianness in your files, no need to modify the
	  header file of the CSHA1 class any more
	- Aligned data support
	- Made support/compilation of the utility functions (ReportHash
	  and HashFile) optional (useful when bytes count, for example in
	  embedded environments)

	Version 1.5 - 2005-01-01
	- 64-bit compiler compatibility added
	- Made variable wiping optional (define SHA1_WIPE_VARIABLES)
	- Removed unnecessary variable initializations
	- ROL32 improvement for the Microsoft compiler (using _rotl)

	======== Test Vectors (from FIPS PUB 180-1) ========

	SHA1("abc") =
		A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D

	SHA1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq") =
		84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1

	SHA1(A million repetitions of "a") =
		34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/


using namespace ExtLibs;

#ifdef SHA1_UTILITY_FUNCTIONS
#define SHA1_MAX_FILE_BUFFER 8000
#endif

// Rotate x bits to the left
#ifndef ROL32
#ifdef _MSC_VER
#define ROL32(_val32, _nBits) _rotl(_val32, _nBits)
#else
#define ROL32(_val32, _nBits) (((_val32)<<(_nBits))|((_val32)>>(32-(_nBits))))
#endif
#endif

#ifdef SHA1_LITTLE_ENDIAN
#define SHABLK0(i) (m_block->l[i] = \
	(ROL32(m_block->l[i],24) & 0xFF00FF00) | (ROL32(m_block->l[i],8) & 0x00FF00FF))
#else
#define SHABLK0(i) (m_block->l[i])
#endif

#define SHABLK(i) (m_block->l[i&15] = ROL32(m_block->l[(i+13)&15] ^ m_block->l[(i+8)&15] \
	^ m_block->l[(i+2)&15] ^ m_block->l[i&15],1))

// SHA-1 rounds
#define _R0(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK0(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
#define _R1(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
#define _R2(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0x6ED9EBA1+ROL32(v,5); w=ROL32(w,30); }
#define _R3(v,w,x,y,z,i) { z+=(((w|x)&y)|(w&x))+SHABLK(i)+0x8F1BBCDC+ROL32(v,5); w=ROL32(w,30); }
#define _R4(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0xCA62C1D6+ROL32(v,5); w=ROL32(w,30); }

CSHA1::CSHA1()
{
	m_block = (SHA1_WORKSPACE_BLOCK*)m_workspace;

	Reset();
}

CSHA1::~CSHA1()
{
	Reset();
}

void CSHA1::Reset()
{
	// SHA1 initialization constants
	m_state[0] = 0x67452301;
	m_state[1] = 0xEFCDAB89;
	m_state[2] = 0x98BADCFE;
	m_state[3] = 0x10325476;
	m_state[4] = 0xC3D2E1F0;

	m_count[0] = 0;
	m_count[1] = 0;
}

void CSHA1::Transform(UINT_32* pState, const UINT_8* pBuffer)
{
	UINT_32 a = pState[0], b = pState[1], c = pState[2], d = pState[3], e = pState[4];

	memcpy(m_block, pBuffer, 64);

	// 4 rounds of 20 operations each. Loop unrolled.
	_R0(a,b,c,d,e, 0); _R0(e,a,b,c,d, 1); _R0(d,e,a,b,c, 2); _R0(c,d,e,a,b, 3);
	_R0(b,c,d,e,a, 4); _R0(a,b,c,d,e, 5); _R0(e,a,b,c,d, 6); _R0(d,e,a,b,c, 7);
	_R0(c,d,e,a,b, 8); _R0(b,c,d,e,a, 9); _R0(a,b,c,d,e,10); _R0(e,a,b,c,d,11);
	_R0(d,e,a,b,c,12); _R0(c,d,e,a,b,13); _R0(b,c,d,e,a,14); _R0(a,b,c,d,e,15);
	_R1(e,a,b,c,d,16); _R1(d,e,a,b,c,17); _R1(c,d,e,a,b,18); _R1(b,c,d,e,a,19);
	_R2(a,b,c,d,e,20); _R2(e,a,b,c,d,21); _R2(d,e,a,b,c,22); _R2(c,d,e,a,b,23);
	_R2(b,c,d,e,a,24); _R2(a,b,c,d,e,25); _R2(e,a,b,c,d,26); _R2(d,e,a,b,c,27);
	_R2(c,d,e,a,b,28); _R2(b,c,d,e,a,29); _R2(a,b,c,d,e,30); _R2(e,a,b,c,d,31);
	_R2(d,e,a,b,c,32); _R2(c,d,e,a,b,33); _R2(b,c,d,e,a,34); _R2(a,b,c,d,e,35);
	_R2(e,a,b,c,d,36); _R2(d,e,a,b,c,37); _R2(c,d,e,a,b,38); _R2(b,c,d,e,a,39);
	_R3(a,b,c,d,e,40); _R3(e,a,b,c,d,41); _R3(d,e,a,b,c,42); _R3(c,d,e,a,b,43);
	_R3(b,c,d,e,a,44); _R3(a,b,c,d,e,45); _R3(e,a,b,c,d,46); _R3(d,e,a,b,c,47);
	_R3(c,d,e,a,b,48); _R3(b,c,d,e,a,49); _R3(a,b,c,d,e,50); _R3(e,a,b,c,d,51);
	_R3(d,e,a,b,c,52); _R3(c,d,e,a,b,53); _R3(b,c,d,e,a,54); _R3(a,b,c,d,e,55);
	_R3(e,a,b,c,d,56); _R3(d,e,a,b,c,57); _R3(c,d,e,a,b,58); _R3(b,c,d,e,a,59);
	_R4(a,b,c,d,e,60); _R4(e,a,b,c,d,61); _R4(d,e,a,b,c,62); _R4(c,d,e,a,b,63);
	_R4(b,c,d,e,a,64); _R4(a,b,c,d,e,65); _R4(e,a,b,c,d,66); _R4(d,e,a,b,c,67);
	_R4(c,d,e,a,b,68); _R4(b,c,d,e,a,69); _R4(a,b,c,d,e,70); _R4(e,a,b,c,d,71);
	_R4(d,e,a,b,c,72); _R4(c,d,e,a,b,73); _R4(b,c,d,e,a,74); _R4(a,b,c,d,e,75);
	_R4(e,a,b,c,d,76); _R4(d,e,a,b,c,77); _R4(c,d,e,a,b,78); _R4(b,c,d,e,a,79);

	// Add the working vars back into state
	pState[0] += a;
	pState[1] += b;
	pState[2] += c;
	pState[3] += d;
	pState[4] += e;

	// Wipe variables
#ifdef SHA1_WIPE_VARIABLES
	a = b = c = d = e = 0;
#endif
}

// Use this function to hash in binary data and strings
void CSHA1::Update(const UINT_8* pData, UINT_32 uLen)
{
	UINT_32 i, j;

	j = (m_count[0] >> 3) & 63;

	if((m_count[0] += (uLen << 3)) < (uLen << 3))
		m_count[1]++;

	m_count[1] += (uLen >> 29);

	if((j + uLen) > 63)
	{
		i = 64 - j;
		memcpy(&m_buffer[j], pData, i);
		Transform(m_state, m_buffer);

		for( ; (i + 63) < uLen; i += 64)
			Transform(m_state, &pData[i]);

		j = 0;
	}
	else i = 0;

	if((uLen - i) != 0)
		memcpy(&m_buffer[j], &pData[i], uLen - i);
}

#ifdef SHA1_UTILITY_FUNCTIONS
// Hash in file contents
bool CSHA1::HashFile(const char* szFileName)
{
	unsigned long ulFileSize, ulRest, ulBlocks;
	unsigned long i;
	UINT_8 uData[SHA1_MAX_FILE_BUFFER];
	FILE* fIn;

	if(szFileName == NULL) return false;

	if(fopen_s(&fIn, szFileName, "rb") != NULL)
		return false;

	fseek(fIn, 0, SEEK_END);
	ulFileSize = (unsigned long)ftell(fIn);
	fseek(fIn, 0, SEEK_SET);

	if(ulFileSize != 0)
	{
		ulBlocks = ulFileSize / SHA1_MAX_FILE_BUFFER;
		ulRest = ulFileSize % SHA1_MAX_FILE_BUFFER;
	}
	else
	{
		ulBlocks = 0;
		ulRest = 0;
	}

	for(i = 0; i < ulBlocks; i++)
	{
		fread(uData, 1, SHA1_MAX_FILE_BUFFER, fIn);
		Update((UINT_8*)uData, SHA1_MAX_FILE_BUFFER);
	}

	if(ulRest != 0)
	{
		fread(uData, 1, ulRest, fIn);
		Update((UINT_8*)uData, ulRest);
	}

	fclose(fIn); fIn = NULL;
	return true;
}
#endif

void CSHA1::Final()
{
	UINT_32 i;
	UINT_8 finalcount[8];

	for(i = 0; i < 8; i++)
		finalcount[i] = (UINT_8)((m_count[((i >= 4) ? 0 : 1)]
			>> ((3 - (i & 3)) * 8) ) & 255); // Endian independent

	Update((UINT_8*)"\200", 1);

	while ((m_count[0] & 504) != 448)
		Update((UINT_8*)"\0", 1);

	Update(finalcount, 8); // Cause a SHA1Transform()

	for(i = 0; i < 20; i++)
		m_digest[i] = (UINT_8)((m_state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);

	// Wipe variables for security reasons
#ifdef SHA1_WIPE_VARIABLES
	memset(m_buffer, 0, 64);
	memset(m_state, 0, 20);
	memset(m_count, 0, 8);
	memset(finalcount, 0, 8);
	Transform(m_state, m_buffer);
#endif
}

#ifdef SHA1_UTILITY_FUNCTIONS
// Get the final hash as a pre-formatted string
void CSHA1::ReportHash(char* szReport, unsigned char uReportType) const
{
	unsigned char i;
	char szTemp[16];

	if(szReport == NULL) return;

	if(uReportType == REPORT_HEX)
	{
		sprintf_s(szTemp, 16, "%02X", m_digest[0]);
		strcpy_s(szReport, 17, szTemp);

		for(i = 1; i < 20; i++)
		{
			sprintf_s(szTemp, 16, "%02X", m_digest[i]);
			strcat_s(szReport, 17, szTemp);
		}
	}
	else if(uReportType == REPORT_DIGIT)
	{
		sprintf_s(szTemp, 16, "%u", m_digest[0]);
		strcpy_s(szReport, 17, szTemp);

		for(i = 1; i < 20; i++)
		{
			sprintf_s(szTemp, 16, " %u", m_digest[i]);
			strcat_s(szReport, 17, szTemp);
		}
	}
	else {
		strcpy_s(szReport, 28, "Error: Unknown report type!");
	}
}
#endif

// Get the raw message digest
void CSHA1::GetHash(UINT_8* puDest) const
{
	memcpy(puDest, m_digest, 20);
}

// =========== TBitmap.cpp ===========

/*
	MCL - TBitmap.cpp
	Copyright (C) 2013-2019 CrownSoft
  
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


TBitmap::TBitmap()
{
	MCL_INIT_VERIFIER;
	hBitmap = 0;
}

TBitmap::operator HBITMAP()const
{
	return hBitmap;
}

bool TBitmap::loadFromResource(WORD resourceID)
{
	hBitmap = (HBITMAP)::LoadImageW(TApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

bool TBitmap::loadFromFile(const TString& filePath)
{
	hBitmap = (HBITMAP)::LoadImageW(TApplication::hInstance, filePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hBitmap)
		return true;	
	return false;
}

void TBitmap::drawOnHDC(HDC hdc, int x, int y, int width, int height)
{
	HDC memHDC = ::CreateCompatibleDC(hdc);

	::SelectObject(memHDC, hBitmap);
	::BitBlt(hdc, x, y, width, height, memHDC, 0, 0, SRCCOPY);

	::DeleteDC(memHDC);
}

HBITMAP TBitmap::getHandle()
{
	return hBitmap;
}

TBitmap::~TBitmap()
{
	if(hBitmap)
		::DeleteObject(hBitmap);
}

// =========== TCursor.cpp ===========

/*
	MCL - TCursor.cpp
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


TCursor::TCursor()
{
	MCL_INIT_VERIFIER;
	hCursor = 0;
}

bool TCursor::loadFromResource(WORD resourceID)
{
	hCursor = (HCURSOR)::LoadImageW(TApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hCursor)
		return true;	
	return false;
}

bool TCursor::loadFromFile(const TString& filePath)
{
	hCursor = (HCURSOR)::LoadImageW(TApplication::hInstance, filePath, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hCursor)
		return true;	
	return false;
}

HCURSOR TCursor::getHandle()
{
	return hCursor;
}

TCursor::operator HCURSOR()const
{
	return hCursor;
}

TCursor::~TCursor()
{
	if(hCursor)
		::DestroyCursor(hCursor);
}

// =========== TFont.cpp ===========

/*
	MCL - TFont.cpp
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


TFont::TFont()
{
	MCL_INIT_VERIFIER;
	hFont = 0;
}

TFont::TFont(const TString& face, int size, bool bold, bool italic, bool antiAliased)
{
	MCL_INIT_VERIFIER;
	hFont = ::CreateFontW(size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE, 
		0, 0, DEFAULT_CHARSET, 0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, 
		VARIABLE_PITCH | FF_DONTCARE, face);
}

bool TFont::loadFont(const TString& face, int size, bool bold, bool italic, bool antiAliased)
{
	if(hFont)
		::DeleteObject(hFont);

	hFont = ::CreateFontW(size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic ? TRUE : FALSE,
		0, 0, DEFAULT_CHARSET, 0, 0, antiAliased ? DEFAULT_QUALITY : NONANTIALIASED_QUALITY, 
		VARIABLE_PITCH | FF_DONTCARE, face);

	return (hFont == NULL);
}

HFONT TFont::getDefaultFont()
{
	return (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
}

bool TFont::loadFont(const TString& path)
{
	return (::AddFontResourceExW(path, FR_PRIVATE, 0) == 0) ? false : true;
}

void TFont::removeFont(const TString& path)
{
	::RemoveFontResourceExW(path, FR_PRIVATE, 0);
}

HFONT TFont::getFontHandle()
{
	return hFont;
}

TFont::operator HFONT()const
{
	return hFont;
}

TFont::~TFont()
{
	if(hFont)
		::DeleteObject(hFont);
}

// =========== TGraphics.cpp ===========

/*
	MCL - TGraphics.cpp
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


TGraphics::TGraphics(){}

TGraphics::~TGraphics(){}

void TGraphics::draw3dVLine(HDC hdc, int startX, int startY, int height)
{
	TGraphics::draw3dRect(hdc, startX, startY, 2, height, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void TGraphics::draw3dHLine(HDC hdc, int startX, int startY, int width)
{
	TGraphics::draw3dRect(hdc, startX, startY, width, 2, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHIGHLIGHT));
}

void TGraphics::draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	TGraphics::draw3dRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}

void TGraphics::draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	TGraphics::fillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
	TGraphics::fillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
	TGraphics::fillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
	TGraphics::fillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void TGraphics::fillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color)
{
	RECT rect = { x, y, x + cx, y + cy };
	TGraphics::fillSolidRect(hdc, &rect, color);
}

void TGraphics::fillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color)
{
	const COLORREF clrOld = ::SetBkColor(hdc, color);

	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
	::SetBkColor(hdc, clrOld);
}

RECT TGraphics::calculateTextSize(const wchar_t *text, HFONT hFont)
{
	HDC hDC = ::CreateICW(L"DISPLAY", NULL, NULL, NULL);
	HGDIOBJ hOldFont = ::SelectObject(hDC, hFont);
	RECT sz = {0, 0, 0, 0};

	::DrawText(hDC, text, ::lstrlenW(text), &sz, DT_CALCRECT | DT_NOPREFIX);
	::SelectObject(hDC, hOldFont);

	::DeleteDC(hDC);
	return sz;
}

// =========== TIcon.cpp ===========

/*
	MCL - TIcon.cpp
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


TIcon::TIcon()
{
	MCL_INIT_VERIFIER;
	hIcon = 0;
}

bool TIcon::loadFromResource(WORD resourceID)
{
	hIcon = (HICON)::LoadImageW(TApplication::hInstance, MAKEINTRESOURCEW(resourceID), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hIcon)
		return true;	
	return false;
}

bool TIcon::loadFromFile(const TString& filePath)
{
	hIcon = (HICON)::LoadImageW(TApplication::hInstance, filePath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_DEFAULTCOLOR);
	if(hIcon)
		return true;	
	return false;
}

HICON TIcon::getHandle()
{
	return hIcon;
}

TIcon::operator HICON()const
{
	return hIcon;
}

TIcon::~TIcon()
{
	if(hIcon)
		::DestroyIcon(hIcon);
}

// =========== TButton.cpp ===========

/*
	MCL - TButton.cpp
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


TButton::TButton()
{
	onPress = nullptr;

	classNameProperty.assignStaticText(TXT_WITH_LEN("BUTTON"));
	textProperty.assignStaticText(TXT_WITH_LEN("Button"));

	widthProperty = 100;
	heightProperty = 30;

	styleProperty |= BS_NOTIFY | WS_TABSTOP;
	extendedStyleProperty = WS_EX_WINDOWEDGE;
}

bool TButton::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if ((message.msg == WM_COMMAND) && (HIWORD(message.wParam) == BN_CLICKED))
	{
		if (onPress)
			onPress(this);

		result = 0;
		return true;
	}

	return TControl::notifyProcHandler(message, result);
}

bool TButton::create()
{
	if(!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

TButton::~TButton()
{
}

// =========== TCheckBox.cpp ===========

/*
	MCL - TCheckBox.cpp
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


TCheckBox::TCheckBox()
{
	valueProperty = false;
	onCheck = nullptr;

	widthProperty = 100;
	heightProperty = 25;

	classNameProperty.assignStaticText(TXT_WITH_LEN("BUTTON"));
	textProperty.assignStaticText(TXT_WITH_LEN("CheckBox"));

	styleProperty |= BS_AUTOCHECKBOX | BS_NOTIFY | WS_TABSTOP;
}

bool TCheckBox::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::SendMessageW(handleProperty, BM_SETCHECK, valueProperty ? BST_CHECKED : BST_UNCHECKED, 0);
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

bool TCheckBox::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if ((message.msg == WM_COMMAND) && (HIWORD(message.wParam) == BN_CLICKED))
	{
		// update internal state
		if (::SendMessageW(handleProperty, BM_GETCHECK, 0, 0) == BST_CHECKED)
			valueProperty = true;
		else
			valueProperty = false;

		// inform to event
		if (onCheck)
			onCheck(this);

		result = 0;
		return true;
	}

	return TControl::notifyProcHandler(message, result);
}

bool TCheckBox::getValue()
{
	return valueProperty;
}

void TCheckBox::setValue(bool state)
{
	valueProperty = state;

	if(handleProperty)
		::SendMessageW(handleProperty, BM_SETCHECK, valueProperty ? BST_CHECKED : BST_UNCHECKED, 0);
}

TCheckBox::~TCheckBox()
{
}

// =========== TComboBox.cpp ===========

/*
	MCL - TComboBox.cpp
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


#define COMBOBOX_INITIAL_ITEM_ALLOCATION 50

TComboBox::TComboBox(bool sort)
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

void TComboBox::add(const TString& text)
{
	TString *str = new TString(text);
	stringList->addPointer(str);

	if(handleProperty)
		::SendMessageW(handleProperty, CB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void TComboBox::remove(int index)
{
	TString *text = stringList->getPointer(index);
	if (text)
		delete text;

	stringList->removePointer(index);

	if(handleProperty)
		::SendMessageW(handleProperty, CB_DELETESTRING, index, 0);
}

void TComboBox::removeByText(const TString& text)
{
	const int itemIndex = this->getItemIndexByText(text);
	if(itemIndex > -1)
		this->remove(itemIndex);
}

int TComboBox::getItemIndexByText(const TString& text)
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

TString TComboBox::getItemTextByIndex(int index)
{
	TString *str = stringList->getPointer(index);

	if(str)
		return *str;

	return TString();
}

int TComboBox::getItemCount()
{
	return stringList->getSize();
}

int TComboBox::getSelectedItemIndex()
{
	return itemIndexProperty;
}

TString TComboBox::getSelectedItem()
{
	const int itemIndex = this->getSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->getPointer(itemIndex);
	return TString();
}

void TComboBox::clear()
{
	stringList->deleteAll(true);
	if(handleProperty)
		::SendMessageW(handleProperty, CB_RESETCONTENT, 0, 0);
}

void TComboBox::selectItem(int index)
{
	itemIndexProperty = index;
	if(handleProperty)
		::SendMessageW(handleProperty, CB_SETCURSEL, index, 0);
}

bool TComboBox::notifyProcHandler(TMessage& message, LRESULT& result)
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

bool TComboBox::create()
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

TComboBox::~TComboBox()
{
	stringList->deleteAll(false);
	delete stringList;
}



// =========== TComponent.cpp ===========

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

void TComponent::setCursor(HCURSOR cursorHandle)
{
	cursorProperty = cursorHandle;
	if(handleProperty)
		::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);
}

HCURSOR TComponent::getCursor()
{
	return cursorProperty;
}

TString TComponent::getClassName()
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

void TComponent::setFont(HFONT fontHandle)
{
	fontProperty = fontHandle;
	if(handleProperty)
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0));
}

HFONT TComponent::getFont()
{
	return fontProperty;
}

TString TComponent::getText()
{
	return textProperty;
}

void TComponent::setText(const TString& caption)
{
	textProperty = caption;
	if(handleProperty)
		::SetWindowTextW(handleProperty, textProperty);
}

void TComponent::setHandle(HWND hwnd)
{
	handleProperty = hwnd;
}

HWND TComponent::getHandle()
{
	return handleProperty;
}

void TComponent::setParent(HWND parentHandle)
{
	parentProperty = parentHandle;
	if(handleProperty)
		::SetParent(handleProperty, parentProperty);
}

HWND TComponent::getParent()
{
	return parentProperty;
}

DWORD TComponent::getStyle()
{
	return styleProperty;
}

void TComponent::setStyle(DWORD compStyle)
{
	styleProperty = compStyle;
	if(handleProperty)
		::SetWindowLongPtrW(handleProperty, GWL_STYLE, styleProperty);
}

DWORD TComponent::getExtendedStyle()
{
	return extendedStyleProperty;
}

void TComponent::setExtendedStyle(DWORD compExStyle)
{
	extendedStyleProperty = compExStyle;
	if(handleProperty)
		::SetWindowLongPtrW(handleProperty, GWL_EXSTYLE, extendedStyleProperty);
}

int TComponent::getLeft()
{
	return leftProperty; 
}

int TComponent::getTop()
{
	return topProperty;
}

int TComponent::getWidth()
{
	return widthProperty;
}

int TComponent::getHeight()
{
	return heightProperty;
}

void TComponent::setLeft(int compLeft)
{
	this->setPosition(compLeft, topProperty);
}

void TComponent::setTop(int compTop)
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

void TComponent::setWidth(int compWidth)
{
	this->setSize(compWidth, heightProperty);
}

void TComponent::setHeight(int compHeight)
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

void TComponent::setVisible(bool state)
{
	visibleProperty = state;
	if(handleProperty)
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);
}

bool TComponent::getVisible()
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

bool TComponent::getEnabled()
{
	if (handleProperty)
		enabledProperty = (::IsWindowEnabled(handleProperty) == TRUE);

	return enabledProperty;
}

void TComponent::setEnabled(bool state)
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

// =========== TEditBox.cpp ===========

/*
	MCL - TEditBox.cpp
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


TEditBox::TEditBox()
{
	classNameProperty.assignStaticText(TXT_WITH_LEN("EDIT"));

	widthProperty = 100;
	heightProperty = 20;

	leftProperty = 0;
	topProperty = 0;

	readOnlyProperty = false;
	onChange = nullptr;

	styleProperty |= WS_TABSTOP | ES_AUTOHSCROLL;
	extendedStyleProperty = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;
}

bool TEditBox::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if ((message.msg == WM_COMMAND) && (HIWORD(message.wParam) == EN_CHANGE))
	{
		if (onChange)
			onChange(this);
	}

	return TControl::notifyProcHandler(message, result);
}

bool TEditBox::isReadOnly()
{
	return readOnlyProperty;
}

void TEditBox::setReadOnly(bool readOnly_)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	readOnlyProperty = readOnly_;

	if (readOnlyProperty)
		this->setStyle(styleProperty | ES_READONLY);
	else
		this->setStyle(styleProperty & ~ES_READONLY);

	if (destroyed)
		this->create();
}

void TEditBox::setLowercaseOnly(bool lowercaseOnly_)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	if (styleProperty & ES_UPPERCASE) // remove upper case style if already set
		styleProperty &= ~ES_UPPERCASE;

	if (lowercaseOnly_)
		this->setStyle(styleProperty | ES_LOWERCASE);
	else
		this->setStyle(styleProperty & ~ES_LOWERCASE);

	if (destroyed)
		this->create();
}

void TEditBox::setUppercaseOnly(bool uppercaseOnly_)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	if (styleProperty & ES_LOWERCASE) // remove lower case style if already set
		styleProperty &= ~ES_LOWERCASE;

	if (uppercaseOnly_)
		this->setStyle(styleProperty | ES_UPPERCASE);
	else
		this->setStyle(styleProperty & ~ES_UPPERCASE);

	if (destroyed)
		this->create();
}

TString TEditBox::getText()
{
	if(handleProperty)
	{
		const int length = ::GetWindowTextLengthW(handleProperty);
		if(length)
		{
			const int size = (length + 1) * sizeof(wchar_t);
			wchar_t* text = (wchar_t*)::malloc(size);
			text[0] = 0;
			::GetWindowTextW(handleProperty, text, size);
			textProperty = TString(text, TString::FREE_TEXT_WHEN_DONE);
		}else
		{
			textProperty = TString();
		}
	}
	return textProperty;
}

bool TEditBox::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

TEditBox::~TEditBox()
{
}

// =========== TGlyphButton.cpp ===========

/*
	MCL - TGlyphButton.cpp
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

#include <windows.h>

TGlyphButton::TGlyphButton()
{
	glyphFont = 0;
	glyphChar = 0;
	glyphLeft = 6;
}

TGlyphButton::~TGlyphButton()
{
}

void TGlyphButton::setGlyph(const wchar_t *glyphChar, HFONT glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->repaint();
}

bool TGlyphButton::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if (glyphFont)
	{
		if (message.msg == WM_NOTIFY)
		{
			if (((LPNMHDR)message.lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
			{
				LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW)message.lParam;

				result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

				if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
				{
					result = CDRF_NOTIFYPOSTPAINT;
				}
				else if (CDDS_POSTPAINT == lpNMCD->dwDrawStage) //  postpaint stage
				{
					const RECT rc = lpNMCD->rc;
					const bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED | CDIS_GRAYED)) != 0;

					HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, glyphFont);
					const COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);
					const int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

					RECT rcIcon = { rc.left + glyphLeft, rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw glyph

					::SetBkMode(lpNMCD->hdc, oldBkMode);
					::SetTextColor(lpNMCD->hdc, oldTextColor);
					::SelectObject(lpNMCD->hdc, oldFont);

					result = CDRF_DODEFAULT;
				}

				return true; // indicate that we processed this msg & result is valid.
			}
		}
	}

	return TButton::notifyProcHandler(message, result); // pass unprocessed messages to parent
}

// =========== TGridView.cpp ===========

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



// =========== TGroupBox.cpp ===========

/*
	MCL - TGroupBox.cpp
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


TGroupBox::TGroupBox()
{
	classNameProperty.assignStaticText(TXT_WITH_LEN("BUTTON"));
	textProperty.assignStaticText(TXT_WITH_LEN("GroupBox"));

	widthProperty = 100;
	heightProperty = 100;

	leftProperty = 0;
	topProperty = 0;

	styleProperty |= BS_GROUPBOX;
	extendedStyleProperty = WS_EX_WINDOWEDGE;
}

bool TGroupBox::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

TGroupBox::~TGroupBox()
{
}

// =========== TLabel.cpp ===========

/*
	MCL - TLabel.cpp
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


TLabel::TLabel()
{
	classNameProperty.assignStaticText(TXT_WITH_LEN("STATIC"));
	textProperty.assignStaticText(TXT_WITH_LEN("Label"));

	widthProperty = 100;
	heightProperty = 25;

	leftProperty = 0;
	topProperty = 0;

	styleProperty |= BS_NOTIFY;
	extendedStyleProperty = WS_EX_WINDOWEDGE;
}

bool TLabel::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

TLabel::~TLabel()
{
}

// =========== TListBox.cpp ===========

/*
	MCL - TListBox.cpp
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


#define INITIAL_LIST_ITEM_COUNT 100

TListBox::TListBox(bool multipleSelection, bool sort, bool vscroll)
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

void TListBox::addItem(const TString& text)
{
	TString *str = new TString(text);
	stringList->addPointer(str);

	if(handleProperty)
		::SendMessageW(handleProperty, LB_ADDSTRING, 0, (LPARAM)(const wchar_t*)*str);
}

void TListBox::removeItem(int index)
{
	TString *text = stringList->getPointer(index);
	if (text)
		delete text;

	stringList->removePointer(index);

	if(handleProperty)
		::SendMessageW(handleProperty, LB_DELETESTRING, index, 0);
}

void TListBox::removeItem(const TString& text)
{
	const int itemIndex = this->getItemIndex(text);
	if(itemIndex > -1)
		this->removeItem(itemIndex);
}

int TListBox::getItemIndex(const TString& text)
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

int TListBox::getItemCount()
{
	return stringList->getSize();
}

int TListBox::getSelectedItemIndex()
{
	if(handleProperty)
	{	 
		const int index = (int)::SendMessageW(handleProperty, LB_GETCURSEL, 0, 0);
		if(index != LB_ERR)
			return index;
	}
	return -1;	
}

TString TListBox::getSelectedItem()
{
	const int itemIndex = this->getSelectedItemIndex();
	if(itemIndex > -1)
		return *stringList->getPointer(itemIndex);
	return TString();
}

int TListBox::getSelectedItems(int* itemArray, int itemCountInArray)
{
	if(handleProperty)
	{	 
		const int items = (int)::SendMessageW(handleProperty, LB_GETSELITEMS, itemCountInArray, (LPARAM)itemArray);
		if(items != LB_ERR)
			return items;
	}
	return -1;
}

void TListBox::clearList()
{
	stringList->deleteAll(true);

	if(handleProperty)
		::SendMessageW(handleProperty, LB_RESETCONTENT, 0, 0);
}

void TListBox::selectItem(int index)
{
	selectedItemIndex = index;

	if(handleProperty)
		::SendMessageW(handleProperty, LB_SETCURSEL, index, 0);
}

void TListBox::selectItems(int start, int end)
{
	if(multipleSelection)
	{
		selectedItemIndex = start;
		selectedItemEnd = end;

		if(handleProperty)
			::SendMessageW(handleProperty, LB_SELITEMRANGE, TRUE, MAKELPARAM(start, end));
	}
}

bool TListBox::notifyProcHandler(TMessage& message, LRESULT& result)
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

bool TListBox::create()
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


TListBox::~TListBox()
{
	stringList->deleteAll(false);
	delete stringList;
}

// =========== TMenu.cpp ===========

/*
	MCL - TMenu.cpp
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



TMenu::TMenu()
{
	hMenu = ::CreatePopupMenu();
}

void TMenu::add(TMenuItem& menuItem)
{
	menuItem.addToMenu(hMenu);
}

void TMenu::addSubMenu(const TString& text, const TMenu& menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)(HMENU)menu, text);
}

void TMenu::addSeperator()
{
	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_TYPE;
	mii.fType = MFT_SEPARATOR;

	::InsertMenuItemW(hMenu, 0xFFFFFFFF, FALSE, &mii);
}

HMENU TMenu::getMenuHandle()
{
	return hMenu;
}

TMenu::operator HMENU()const
{
	return hMenu;
}

void TMenu::popUpMenu(TWindow& window)
{
	if(window)
	{
		POINT p;
		::GetCursorPos(&p);
		::TrackPopupMenu(hMenu, TPM_LEFTBUTTON, p.x, p.y, 0, window, NULL);
	}
}

TMenu::~TMenu()
{
	::DestroyMenu(hMenu);
}

// =========== TMenuBar.cpp ===========

/*
	MCL - TMenuBar.cpp
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


TMenuBar::TMenuBar()
{
	hMenu = ::CreateMenu();
}

void TMenuBar::add(const TString& text, const TMenu& menu)
{
	::InsertMenuW(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)(HMENU)menu, text);
}

HMENU TMenuBar::getHandle()
{
	return hMenu;
}

TMenuBar::operator HMENU()const
{
	return hMenu;
}

TMenuBar::~TMenuBar()
{
	::DestroyMenu(hMenu);
}

// =========== TMenuButton.cpp ===========

/*
	MCL - TMenuButton.cpp
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


TMenuButton::TMenuButton()
{
	buttonMenu = 0;
	glyphFont = 0;
	glyphChar = 0;
	glyphLeft = 6;
	arrowFont = new TFont(CONST_TXT("Webdings"), 18);
}	

TMenuButton::~TMenuButton()
{
	delete arrowFont;
}

void TMenuButton::setMenu(TMenu& buttonMenu_)
{
	buttonMenu = &buttonMenu_;
}

void TMenuButton::setGlyph(const wchar_t *glyphChar, HFONT glyphFont, COLORREF glyphColor, int glyphLeft)
{
	this->glyphChar = glyphChar;
	this->glyphFont = glyphFont;
	this->glyphColor = glyphColor;
	this->glyphLeft = glyphLeft;

	this->repaint();
}

bool TMenuButton::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if (message.msg == WM_NOTIFY)
	{		
		if (((LPNMHDR)message.lParam)->code == NM_CUSTOMDRAW) // custom drawing msg received for this component
		{
			LPNMCUSTOMDRAW lpNMCD = (LPNMCUSTOMDRAW)message.lParam;

			result = CDRF_DODEFAULT; // take the default processing unless we set this to something else below.

			if (CDDS_PREPAINT == lpNMCD->dwDrawStage) // it's the control's prepaint stage, tell Windows we want message after paint.
			{
				result = CDRF_NOTIFYPOSTPAINT;
			}
			else if ( CDDS_POSTPAINT== lpNMCD->dwDrawStage ) //  postpaint stage
			{
				const RECT rc = lpNMCD->rc;
				TGraphics::draw3dVLine(lpNMCD->hdc, rc.right - 22, rc.top + 6, rc.bottom - 12); // draw line

				const bool bDisabled = (lpNMCD->uItemState & (CDIS_DISABLED|CDIS_GRAYED)) != 0;

				HGDIOBJ oldFont = ::SelectObject(lpNMCD->hdc, arrowFont->getFontHandle());
				const COLORREF oldTextColor = ::SetTextColor(lpNMCD->hdc, ::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
				const int oldBkMode = ::SetBkMode(lpNMCD->hdc, TRANSPARENT);

				RECT rcIcon = { rc.right - 18, rc.top, rc.right, rc.bottom };
				::DrawTextW(lpNMCD->hdc, L"\x36", 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER); // draw arrow

				if (glyphFont) // draw glyph
				{
					::SelectObject(lpNMCD->hdc, glyphFont);
					::SetTextColor(lpNMCD->hdc, bDisabled ? ::GetSysColor(COLOR_GRAYTEXT) : glyphColor);

					rcIcon = { rc.left + glyphLeft, rc.top, rc.right, rc.bottom };
					::DrawTextW(lpNMCD->hdc, glyphChar, 1, &rcIcon, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
				}

				::SetBkMode(lpNMCD->hdc, oldBkMode);
				::SetTextColor(lpNMCD->hdc, oldTextColor);
				::SelectObject(lpNMCD->hdc, oldFont);

				result = CDRF_DODEFAULT;
			}

			return true; // indicate that we processed this msg & result is valid.
		}
	}
	else if ((message.msg == WM_COMMAND) && (HIWORD(message.wParam) == BN_CLICKED))
	{
		if (buttonMenu)
		{
			POINT point = { leftProperty, topProperty };
			::ClientToScreen(parentProperty, &point); // get screen cordinates

			::TrackPopupMenu(buttonMenu->getMenuHandle(), TPM_LEFTBUTTON, point.x, point.y + heightProperty, 0, parentProperty, NULL);
		}

		if (onPress)
			onPress(this);

		result = 0;
		return true;
	}

	return TButton::notifyProcHandler(message, result); // pass unprocessed messages to parent
}

// =========== TMenuItem.cpp ===========

/*
	MCL - TMenuItem.cpp
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


TMenuItem::TMenuItem()
{
	MCL_INIT_VERIFIER;
	hMenu = 0;
	onPress = nullptr;
	enabled = true;
	checked = false;
	itemID = TPlatformUtil::getInstance()->generateMenuItemID(this);
}

void TMenuItem::addToMenu(HMENU hMenu)
{
	this->hMenu = hMenu;

	MENUITEMINFOW mii;
	::ZeroMemory(&mii, sizeof(mii));

	mii.cbSize = sizeof(MENUITEMINFOW);
	mii.fMask = MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.dwTypeData = (LPWSTR)(const wchar_t*)textProperty;
	mii.cch = lstrlenW((LPWSTR)(const wchar_t*)textProperty);
	mii.fState = (enabled ? MFS_ENABLED : MFS_DISABLED) | (checked ? MFS_CHECKED : MFS_UNCHECKED);
	mii.wID = itemID;
	mii.dwItemData = (ULONG_PTR)this; // for future!

	::InsertMenuItemW(hMenu, itemID, FALSE, &mii);
}

bool TMenuItem::isChecked()
{
	return checked;
}

void TMenuItem::setCheckedState(bool state)
{
	checked = state;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = checked ? MFS_CHECKED : MFS_UNCHECKED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

bool TMenuItem::isEnabled()
{
	return enabled; 
}

void TMenuItem::setEnabled(bool state)
{
	enabled = state;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_STATE;
		mii.fState = enabled ? MFS_ENABLED : MFS_DISABLED;

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

void TMenuItem::setText(const TString& text)
{
	textProperty = text;
	if(hMenu) // it's alredy created menu item!
	{
		MENUITEMINFOW mii;
		::ZeroMemory(&mii, sizeof(mii));

		mii.cbSize = sizeof(MENUITEMINFOW);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.dwTypeData = (LPWSTR)(const wchar_t*)textProperty;
		mii.cch = lstrlenW((LPWSTR)(const wchar_t*)textProperty);

		::SetMenuItemInfoW(hMenu, itemID, FALSE, &mii);
	}
}

TString TMenuItem::getText()
{
	return textProperty;
}

UINT TMenuItem::getItemID()
{
	return itemID;
}

HMENU TMenuItem::getMenuHandle()
{
	return hMenu;
}

TMenuItem::~TMenuItem()
{
}


// =========== TNumericField.cpp ===========

/*
	MCL - TNumericField.cpp
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


TNumericField::TNumericField()
{
	styleProperty |= ES_NUMBER;
}

TNumericField::~TNumericField(){}

// =========== TPasswordBox.cpp ===========

/*
	MCL - TPasswordBox.cpp
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


TPasswordBox::TPasswordBox()
{
	passwordCharProperty = '*';
	styleProperty |= ES_PASSWORD;
}

void TPasswordBox::setPasswordChar(const char pwdChar)
{
	this->passwordCharProperty = pwdChar;
	if(handleProperty)
	{
		::SendMessageW(handleProperty, EM_SETPASSWORDCHAR, passwordCharProperty, 0);
		this->repaint();
	}
}

char TPasswordBox::getPasswordChar()
{
	return passwordCharProperty;
}

bool TPasswordBox::create()
{
	if(TEditBox::create())
	{
		::SendMessageW(handleProperty, EM_SETPASSWORDCHAR, passwordCharProperty, 0);
		return true;
	}
	return false;
}

TPasswordBox::~TPasswordBox()
{
}

// =========== TProgressBar.cpp ===========

/*
	MCL - TProgressBar.cpp
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


TProgressBar::TProgressBar()
{
	progress = 0;

	classNameProperty.assignStaticText(TXT_WITH_LEN("msctls_progress32"));

	widthProperty = 100;
	heightProperty = 20;

	leftProperty = 0;
	topProperty = 0;

	extendedStyleProperty = WS_EX_WINDOWEDGE;
	styleProperty |= PBS_SMOOTH;
}

void TProgressBar::setEnableSmooth(bool smooth_)
{
	if (smooth_)
		this->setStyle(styleProperty | PBS_SMOOTH);
	else
		this->setStyle(styleProperty & ~PBS_SMOOTH);
}

void TProgressBar::setOrientation(bool vertical_)
{
	if (vertical_)
		this->setStyle(styleProperty | PBS_VERTICAL);
	else
		this->setStyle(styleProperty & ~PBS_VERTICAL);
}

int TProgressBar::getValue()
{
	return progress;
}

void TProgressBar::setValue(int val)
{
	this->progress = val;

	if(handleProperty)
		::SendMessageW(handleProperty, PBM_SETPOS, progress, 0);
}

bool TProgressBar::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, PBM_SETRANGE, 0, MAKELPARAM(0, 100)); // set range between 0-100
		::SendMessageW(handleProperty, PBM_SETPOS, value, 0); // set current value!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);
		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

TProgressBar::~TProgressBar()
{
}

// =========== TPushButton.cpp ===========

/*
	MCL - TPushButton.cpp
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


TPushButton::TPushButton()
{
	classNameProperty.assignStaticText(TXT_WITH_LEN("Push Button"));
	styleProperty |= BS_PUSHLIKE;
}

TPushButton::~TPushButton()
{
}

// =========== TRadioButton.cpp ===========

/*
	MCL - TRadioButton.cpp
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


TRadioButton::TRadioButton()
{
	textProperty.assignStaticText(TXT_WITH_LEN("RadioButton"));

	styleProperty &= ~BS_AUTOCHECKBOX; // remove inherited checkbox style
	styleProperty |= BS_RADIOBUTTON;

}

TRadioButton::~TRadioButton()
{
}

// =========== TSlider.cpp ===========

/*
	MCL - TSlider.cpp
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



TSlider::TSlider()
{
	rangeMin = 0;
	rangeMax = 100;
	positionProperty = 0;
	onChange = nullptr;

	widthProperty = 100;
	heightProperty = 25;

	leftProperty = 0;
	topProperty = 0;

	styleProperty |= WS_TABSTOP | TBS_NOTICKS | TBS_HORZ;
	extendedStyleProperty = WS_EX_WINDOWEDGE;

	classNameProperty.assignStaticText(TXT_WITH_LEN("msctls_trackbar32"));
}

void TSlider::setEnableTicks(bool enableTicks)
{
	if (enableTicks)
		this->setStyle((styleProperty & ~TBS_NOTICKS) | TBS_AUTOTICKS);
	else
		this->setStyle((styleProperty & ~TBS_AUTOTICKS) | TBS_NOTICKS);
}

void TSlider::setOrientation(bool vertical)
{
	if (vertical)
		this->setStyle((styleProperty & ~TBS_HORZ) | TBS_VERT);
	else
		this->setStyle((styleProperty & ~TBS_VERT) | TBS_HORZ);
}

void TSlider::setRange(int min, int max)
{
	rangeMin = min;
	rangeMax = max;
	if(handleProperty)
		::SendMessageW(handleProperty, TBM_SETRANGE, TRUE, (LPARAM) MAKELONG(min, max));
}

void TSlider::setPosition(int pos)
{
	this->positionProperty = pos;
	if(handleProperty)
		::SendMessageW(handleProperty, TBM_SETPOS, TRUE, (LPARAM)positionProperty);
}

int TSlider::getPosition()
{
	return positionProperty;
}

bool TSlider::notifyProcHandler(TMessage& message, LRESULT& result)
{
	if( (message.msg == WM_HSCROLL) || (message.msg == WM_VSCROLL) )
	{
		const int nScrollCode = (int)LOWORD(message.wParam);

		if( (TB_THUMBTRACK == nScrollCode) || (TB_LINEDOWN == nScrollCode) || (TB_LINEUP == nScrollCode) || 
			(TB_BOTTOM == nScrollCode) || (TB_TOP == nScrollCode) || (TB_PAGEUP == nScrollCode) || 
			(TB_PAGEDOWN == nScrollCode) || (TB_THUMBPOSITION == nScrollCode)) // its trackbar!
		{
			positionProperty = (int)::SendMessageW(handleProperty, TBM_GETPOS, 0, 0);

			if (onChange)
				onChange(this, positionProperty);
			
			result = 0;
			return true;
		}
	}

	return TControl::notifyProcHandler(message, result);
}

bool TSlider::create()
{
	if (!parentProperty) // user must specify parent handle!
		return false;

	isRegistered = false; // we don't want to unregister this class.

	::CreateMCLComponent(this);

	if (handleProperty)
	{
		::SendMessageW(handleProperty, WM_SETFONT, (WPARAM)fontProperty, MAKELPARAM(true, 0)); // set font!
		::EnableWindow(handleProperty, enabledProperty ? TRUE : FALSE);

		::SendMessageW(handleProperty, TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(rangeMin, rangeMax));
		::SendMessageW(handleProperty, TBM_SETPOS, TRUE, (LPARAM)positionProperty);

		::ShowWindow(handleProperty, visibleProperty ? SW_SHOW : SW_HIDE);

		if (cursorProperty)
			::SetClassLongPtrW(handleProperty, GCLP_HCURSOR, (LONG_PTR)cursorProperty);

		return true;
	}
	return false;
}

TSlider::~TSlider()
{
}


// =========== TTextArea.cpp ===========

/*
	MCL - TTextArea.cpp
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


TTextArea::TTextArea()
{
	widthProperty = 200;
	heightProperty = 100;

	styleProperty |= ES_MULTILINE | ES_WANTRETURN | WS_HSCROLL | WS_VSCROLL;
	styleProperty = styleProperty & ~ES_AUTOHSCROLL; // remove ES_AUTOHSCROLL which comes from TEditBox
}

void TTextArea::setEnableVScrollBar(bool enableVScrollBar)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	if (enableVScrollBar)
		this->setStyle((styleProperty  & ~ES_AUTOVSCROLL) | WS_VSCROLL);
	else
		this->setStyle((styleProperty & ~WS_VSCROLL) | ES_AUTOVSCROLL);

	if(destroyed)
		this->create();
}

void TTextArea::setEnableHScrollBar(bool enableHScrollBar)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	if (enableHScrollBar)
		this->setStyle((styleProperty & ~ES_AUTOHSCROLL) | WS_HSCROLL);
	else
		this->setStyle((styleProperty & ~WS_HSCROLL) | ES_AUTOHSCROLL);

	if (destroyed)
		this->create();
}

void TTextArea::setEnableWordWrap(bool enableWordWrap)
{
	bool destroyed = false;

	if (handleProperty)
	{
		this->getText(); // this will update textProperty.
		this->destroy();
		destroyed = true;
	}

	if (enableWordWrap)
		this->setStyle((styleProperty & ~ES_AUTOHSCROLL) & ~WS_HSCROLL);
	else
		this->setStyle((styleProperty | WS_HSCROLL) & ~ES_AUTOHSCROLL);

	if (destroyed)
		this->create();
}

LRESULT TTextArea::windowProcHandler(TMessage& message)
{
	if(message.msg == WM_GETDLGCODE)
		return DLGC_WANTALLKEYS; // to catch TAB key

	return TEditBox::windowProcHandler(message);
}

TTextArea::~TTextArea()
{
}

// =========== TToolTip.cpp ===========

/*
	MCL - TToolTip.cpp
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


TToolTip::TToolTip()
{
	attachedControl = 0;
	classNameProperty.assignStaticText(TXT_WITH_LEN("tooltips_class32"));

	styleProperty = WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX;
}

TToolTip::~TToolTip()
{
}

void TToolTip::attachToControl(TWindow *parentWindow, TControl *attachedControl)
{
	parentProperty = parentWindow->getHandle();
	this->attachedControl = attachedControl->getHandle();

	handleProperty = ::CreateWindowExW(0, classNameProperty, NULL, styleProperty, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parentProperty,
		NULL, TApplication::hInstance, 0);

	if (handleProperty)
	{
		::SetWindowPos(handleProperty, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		::AttachMCLPropertiesToHWND(handleProperty, (TComponent*)this);

		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = parentProperty;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)this->attachedControl;
		toolInfo.lpszText = textProperty;

		SendMessageW(handleProperty, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);
	}
}

bool TToolTip::create()
{
	return false;
}

void TToolTip::setText(const TString& caption)
{
	textProperty = caption;

	if (handleProperty)
	{
		TOOLINFOW toolInfo = { 0 };
		toolInfo.cbSize = sizeof(TOOLINFOW);
		toolInfo.hwnd = parentProperty;
		toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		toolInfo.uId = (UINT_PTR)attachedControl;
		toolInfo.lpszText = textProperty;
		toolInfo.hinst = TApplication::hInstance;

		SendMessageW(handleProperty, TTM_UPDATETIPTEXT, 0, (LPARAM)&toolInfo);
	}
}


// =========== TWindow.cpp ===========

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

// =========== TDirectory.cpp ===========

/*
	MCL - TDirectory.cpp
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



TDirectory::TDirectory(){}

TDirectory::~TDirectory(){}

bool TDirectory::isDirExists(const TString& dirName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(dirName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool TDirectory::createDir(const TString& dirName)
{
	return (::CreateDirectoryW(dirName, NULL) == 0 ? false : true);
}

bool TDirectory::removeDir(const TString& dirName)
{
	return (::RemoveDirectoryW(dirName) == 0 ? false : true);
}

TString TDirectory::getModuleDir(HMODULE hModule)
{
	// assumes MAX_PATH * 2 is enough!

	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH * 2) * sizeof(wchar_t) );
	path[0] = 0;
	::GetModuleFileNameW(hModule, path, MAX_PATH * 2);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return TString(path, TString::FREE_TEXT_WHEN_DONE);
}

TString TDirectory::getParentDir(const TString& filePath)
{
	wchar_t *path = ::_wcsdup(filePath);

	wchar_t *p;
	for (p = path; *p; p++) {}	// find end
	for (; p > path && *p != L'\\'; p--) {} // back up to last backslash
	*p = 0;	// kill it

	return TString(path, TString::FREE_TEXT_WHEN_DONE);
}

TString TDirectory::getTempDir()
{
	wchar_t *path = (wchar_t*)::malloc( (MAX_PATH + 1) * sizeof(wchar_t) );
	path[0] = 0;
	::GetTempPathW(MAX_PATH + 1, path);

	return TString(path, TString::FREE_TEXT_WHEN_DONE);
}

TString TDirectory::getApplicationDataDir(bool isAllUsers)
{
	wchar_t *path = (wchar_t*)::malloc( MAX_PATH * sizeof(wchar_t) );
	path[0] = 0;
	::SHGetFolderPathW(NULL, isAllUsers ? CSIDL_COMMON_APPDATA : CSIDL_APPDATA, NULL, 0, path);

	return TString(path, TString::FREE_TEXT_WHEN_DONE);
}

// =========== TFile.cpp ===========

/*
	MCL - TFile.cpp
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


TFile::TFile()
{
	autoCloseHandle = false;
	desiredAccess = TFile::TBOTH;
	fileHandle = INVALID_HANDLE_VALUE;
}

TFile::TFile(const TString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

bool TFile::openFile(const TString& fileName, DWORD desiredAccess, bool autoCloseHandle)
{
	if (fileHandle != INVALID_HANDLE_VALUE) // close old file
		::CloseHandle(fileHandle);

	this->fileName = fileName;
	this->desiredAccess = desiredAccess;
	this->autoCloseHandle = autoCloseHandle;

	fileHandle = ::CreateFileW(fileName, desiredAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return (fileHandle == INVALID_HANDLE_VALUE) ? false : true;
}

bool TFile::closeFile()
{
	if (::CloseHandle(fileHandle) != 0)
	{
		fileHandle = INVALID_HANDLE_VALUE;
		return true;
	}
	return false;
}

HANDLE TFile::getFileHandle()
{
	return fileHandle;
}

TFile::operator HANDLE()const
{
	return fileHandle;
}

DWORD TFile::readFile(void* buffer, DWORD numberOfBytesToRead)
{
	DWORD numberOfBytesRead = 0;
	::ReadFile(fileHandle, buffer, numberOfBytesToRead, &numberOfBytesRead, NULL);

	return numberOfBytesRead;
}

DWORD TFile::writeFile(void* buffer, DWORD numberOfBytesToWrite)
{
	DWORD numberOfBytesWritten = 0;
	::WriteFile(fileHandle, buffer, numberOfBytesToWrite, &numberOfBytesWritten, NULL);

	return numberOfBytesWritten;
}

bool TFile::setFilePointerToStart()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) ? false : true;
}

bool TFile::setFilePointerTo(long distance, DWORD startingPoint)
{
	return (::SetFilePointer(fileHandle, distance, NULL, startingPoint) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD TFile::getFilePointerPosition()
{
	return ::SetFilePointer(fileHandle, 0, NULL, FILE_CURRENT);
}

bool TFile::setFilePointerToEnd()
{
	return (::SetFilePointer(fileHandle, 0, NULL, FILE_END) == INVALID_SET_FILE_POINTER) ? false : true;
}

DWORD TFile::getFileSize()
{
	const DWORD fileSize = ::GetFileSize(fileHandle, NULL);
	return (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;
}

void* TFile::readAsData()
{
	const DWORD fileSize = this->getFileSize();

	if (fileSize)
	{
		void* buffer = (void*)::malloc(fileSize);
		const DWORD numberOfBytesRead = this->readFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
			return buffer;

		::free(buffer); // cannot read entire file!
	}

	return NULL;
}

bool TFile::writeString(const TString& text, bool isUnicode)
{
	void* buffer = isUnicode ? (void*)(const wchar_t*)text : (void*)(const char*)text;
	const DWORD numberOfBytesToWrite = text.getLength() * ( isUnicode ? sizeof(wchar_t) : sizeof(char) );

	const DWORD numberOfBytesWritten = this->writeFile(buffer, numberOfBytesToWrite);

	return (numberOfBytesWritten == numberOfBytesToWrite);
}

TString TFile::readAsString()
{
	DWORD fileSize = this->getFileSize();

	if (fileSize)
	{
		char* buffer = (char*)::malloc(fileSize + 2); // +2 is for null
		const DWORD numberOfBytesRead = this->readFile(buffer, fileSize);

		if (numberOfBytesRead == fileSize)
		{
			buffer[fileSize] = 0; // null terminated string
			buffer[fileSize + 1] = 0; // null for the unicode encoding

			if (::IsTextUnicode(buffer, numberOfBytesRead, NULL) == TRUE)
			{
				return TString((const wchar_t*)buffer, TString::FREE_TEXT_WHEN_DONE);
			}
			else
			{
				TString strData((const char*)buffer);
				::free(buffer);
				return strData;
			}
		}

		::free(buffer); // cannot read entire file!
	}

	return TString();
}

bool TFile::deleteFile(const TString& fileName)
{
	return (::DeleteFileW(fileName) == 0) ? false : true;
}

bool TFile::copyFile(const TString& sourceFileName, const TString& destFileName)
{
	return (::CopyFileW(sourceFileName, destFileName, FALSE) == 0) ? false : true;
}

bool TFile::isFileExists(const TString& fileName)
{
	const DWORD dwAttrib = ::GetFileAttributesW(fileName);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

TFile::~TFile()
{
	if (autoCloseHandle)
		::CloseHandle(fileHandle);
}

// =========== TLogger.cpp ===========

/*
	MCL - TLogger.cpp
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


TLogger::TLogger(DWORD bufferSize)
{
	buffer = (char*)malloc(bufferSize);
	this->bufferSize = bufferSize;
	bufferIndex = 0;
	totalEvents = 0;
	totalMills = 0;
	bufferFull = false;
	isFirstCall = true;
}

bool TLogger::writeNewEvent(unsigned char eventType)
{
	if (!bufferFull)
	{
		if ((bufferIndex + 300) >= bufferSize) // assume each event data is not greater than 300 bytes
		{
			bufferFull = true;
			return false;
		}

		unsigned short secs = 0;
		unsigned short mills = 0;

		if (isFirstCall)
		{
			pCounter.startCounter();
			isFirstCall = false;
			totalMills = 0;
		}
		else{
			const double deltaMills = pCounter.endCounter();
			totalMills += (unsigned int)deltaMills;

			secs = (unsigned short)(totalMills/1000);
			mills = (unsigned short)(totalMills % 1000);

			pCounter.startCounter();
		}

		buffer[bufferIndex] = eventType; // write event type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = secs; // write secs
		bufferIndex += sizeof(unsigned short);

		*((unsigned short*)&buffer[bufferIndex]) = mills; // write mills
		bufferIndex += sizeof(unsigned short);

		totalEvents++;

		return true;
	}
	return false;
}

bool TLogger::endEvent()
{
	if (!bufferFull)
	{
		buffer[bufferIndex] = EVT_END; // write event end
		bufferIndex += sizeof(unsigned char);

		return true;
	}
	return false;
}

bool TLogger::addTextParam(const char *text, unsigned char textLength)
{
	if( (textLength < 256) && (!bufferFull) )
	{
		buffer[bufferIndex] = PARAM_STRING; // write param type
		bufferIndex += sizeof(unsigned char);

		buffer[bufferIndex] = textLength; // write data size
		bufferIndex += sizeof(unsigned char);

		for (int i = 0; i < textLength; i++) // write data
		{
			buffer[bufferIndex] = text[i];
			bufferIndex += sizeof(unsigned char);
		}

		return true;
	}
	return false;
}

bool TLogger::addIntParam(int value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_INT32; // write param type
		bufferIndex += sizeof(unsigned char);

		*((int*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(int);

		return true;
	}
	return false;
}

bool TLogger::addShortParam(unsigned short value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_SHORT16; // write param type
		bufferIndex += sizeof(unsigned char);

		*((unsigned short*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(unsigned short);

		return true;
	}
	return false;
}

bool TLogger::addFloatParam(float value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_FLOAT; // write param type
		bufferIndex += sizeof(unsigned char);

		*((float*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(float);

		return true;
	}
	return false;
}
	
bool TLogger::addDoubleParam(double value)
{
	if(!bufferFull)
	{
		buffer[bufferIndex] = PARAM_DOUBLE; // write param type
		bufferIndex += sizeof(unsigned char);

		*((double*)&buffer[bufferIndex]) = value; // write data
		bufferIndex += sizeof(double);

		return true;
	}
	return false;
}

bool TLogger::isBufferFull()
{
	return bufferFull;
}

bool TLogger::writeToFile(const TString &filePath)
{
	TFile file;

	if (TFile::isFileExists(filePath))
		TFile::deleteFile(filePath);

	if (file.openFile(filePath, TFile::TWRITE))
	{
		file.writeFile((void*)"TLOG", 4);
		file.writeFile(&totalEvents, 4);
		file.writeFile(buffer, bufferIndex);

		return true;
	}

	return false;
}

TLogger::~TLogger()
{
	free(buffer);
}

// =========== TSettingsReader.cpp ===========

/*
	MCL - TSettingsReader.cpp
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


TSettingsReader::TSettingsReader()
{

}

bool TSettingsReader::openFile(const TString& fileName, int formatID)
{
	if (!TFile::isFileExists(fileName))
		return false;

	if (!settingsFile.openFile(fileName, TFile::TREAD))
		return false;

	settingsFile.setFilePointerToStart();

	int fileFormatID = 0;
	settingsFile.readFile(&fileFormatID, sizeof(int));

	if (formatID != fileFormatID) // invalid settings file
		return false;

	return true;
}

void TSettingsReader::readData(DWORD size, void *buffer)
{
	if (buffer)
		settingsFile.readFile(buffer, size);
}

TString TSettingsReader::readString()
{
	int size = 0;
	settingsFile.readFile(&size, sizeof(int));

	if (size)
	{
		wchar_t *buffer = (wchar_t*)malloc(size);
		settingsFile.readFile(buffer, size);

		return TString(buffer, TString::FREE_TEXT_WHEN_DONE);
	}
	else
	{
		return TString();
	}
}

int TSettingsReader::readInt()
{
	int value = 0;
	settingsFile.readFile(&value, sizeof(int));

	return value;
}

float TSettingsReader::readFloat()
{
	float value = 0;
	settingsFile.readFile(&value, sizeof(float));

	return value;
}

double TSettingsReader::readDouble()
{
	double value = 0;
	settingsFile.readFile(&value, sizeof(double));

	return value;
}

bool TSettingsReader::readBool()
{
	bool value = 0;
	settingsFile.readFile(&value, sizeof(bool));

	return value;
}

TSettingsReader::~TSettingsReader()
{

}

// =========== TSettingsWriter.cpp ===========

/*
	MCL - TSettingsWriter.cpp
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


TSettingsWriter::TSettingsWriter()
{

}

bool TSettingsWriter::openFile(const TString& fileName, int formatID)
{
	if (TFile::isFileExists(fileName))
		TFile::deleteFile(fileName);

	if (!settingsFile.openFile(fileName, TFile::TWRITE))
		return false;

	settingsFile.setFilePointerToStart();
	settingsFile.writeFile(&formatID, sizeof(int));

	return true;
}

void TSettingsWriter::writeData(DWORD size, void* buffer)
{
	if (buffer)
		settingsFile.writeFile(buffer, size);
}

void TSettingsWriter::writeString(const TString& text)
{
	int size = text.getLength();
	if (size)
	{
		size = (size + 1) * sizeof(wchar_t);
		settingsFile.writeFile(&size, sizeof(int));

		settingsFile.writeFile((wchar_t*)text, size);
	}
	else // write only empty size
	{
		settingsFile.writeFile(&size, sizeof(int));
	}
}

void TSettingsWriter::writeInt(int value)
{
	settingsFile.writeFile(&value, sizeof(int));
}

void TSettingsWriter::writeFloat(float value)
{
	settingsFile.writeFile(&value, sizeof(float));
}

void TSettingsWriter::writeDouble(double value)
{
	settingsFile.writeFile(&value, sizeof(double));
}

void TSettingsWriter::writeBool(bool value)
{
	settingsFile.writeFile(&value, sizeof(bool));
}

TSettingsWriter::~TSettingsWriter()
{

}

// =========== TInternet.cpp ===========

/*
	MCL - TInternet.cpp
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

#include <string.h>
#include <stdio.h>

TInternet::TInternet(){}

TInternet::~TInternet(){}

void TInternet::applyProxySettings(const wchar_t* url, HINTERNET hInternet)
{
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig;
	WINHTTP_PROXY_INFO proxyInfoTemp, proxyInfo;
	WINHTTP_AUTOPROXY_OPTIONS OptPAC;

	::ZeroMemory(&proxyConfig, sizeof(WINHTTP_CURRENT_USER_IE_PROXY_CONFIG));
	::ZeroMemory(&proxyInfo, sizeof(WINHTTP_PROXY_INFO));

	if (::WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig))
	{
		if (proxyConfig.lpszProxy) {
			proxyInfo.lpszProxy = proxyConfig.lpszProxy;
			proxyInfo.dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
			proxyInfo.lpszProxyBypass = NULL;
		}

		if (proxyConfig.lpszAutoConfigUrl) {
			// Script proxy pac
			OptPAC.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
			OptPAC.lpszAutoConfigUrl = proxyConfig.lpszAutoConfigUrl;
			OptPAC.dwAutoDetectFlags = 0;
			OptPAC.fAutoLogonIfChallenged = TRUE;
			OptPAC.lpvReserved = 0;
			OptPAC.dwReserved = 0;

			if (::WinHttpGetProxyForUrl(hInternet, url, &OptPAC, &proxyInfoTemp))
				::memcpy(&proxyInfo, &proxyInfoTemp, sizeof(WINHTTP_PROXY_INFO));
		}

		if (proxyConfig.fAutoDetect) {
			// Autodetect proxy
			OptPAC.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
			OptPAC.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
			OptPAC.fAutoLogonIfChallenged = TRUE;
			OptPAC.lpszAutoConfigUrl = NULL;
			OptPAC.lpvReserved = 0;
			OptPAC.dwReserved = 0;

			if (::WinHttpGetProxyForUrl(hInternet, url, &OptPAC, &proxyInfoTemp))
				::memcpy(&proxyInfo, &proxyInfoTemp, sizeof(WINHTTP_PROXY_INFO));
		}

		if (proxyInfo.lpszProxy)
			::WinHttpSetOption(hInternet, WINHTTP_OPTION_PROXY, &proxyInfo, sizeof(WINHTTP_PROXY_INFO));
	}
}

TString TInternet::urlEncodeString(const TString &text)
{
	if (text.getLength() == 0)
		return TString();

	TString new_str;
	char c;
	int ic;
	const char* chars = text;
	char bufHex[10];
	int len = text.getLength();

	for (int i = 0; i < len; i++)
	{
		c = chars[i];
		ic = c;

		if (c == ' ')
		{
			new_str = new_str.appendStaticText(L"+", 1, true);
		}
		else if (::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		{		
			char tmp[] = { c, 0 };
			new_str = new_str.append(TString(tmp));
		}
		else
		{
			::sprintf_s(bufHex, 10,  "%X", c);

			if (ic < 16)
				new_str = new_str.appendStaticText(L"%0", 2, true);
			else
				new_str = new_str.appendStaticText(L"%", 1, true);

			new_str = new_str.append(TString(bufHex));
		}
	}
	return new_str;
}

TString TInternet::urlDecodeString(const TString& text)
{
	if (text.getLength() == 0)
		return TString();

	TString ret;
	const char* str = text;

	char ch;
	int i, ii, len = text.getLength();

	for (i = 0; i < len; i++)
	{
		if (str[i] != '%')
		{
			if (str[i] == '+')
			{
				ret = ret.appendStaticText(L" ", 1, true);
			}
			else
			{
				char tmp[] = { str[i], 0 };
				ret = ret.append(TString(tmp));
			}
		}
		else
		{
			TString sub(text.subString(i + 1, i + 2));
			::sscanf_s(sub, "%x", &ii);
			ch = static_cast<char>(ii);

			char tmp[] = { ch, 0 };
			ret = ret.append(TString(tmp));

			i = i + 2;
		}
	}
	return ret;
}

TString TInternet::postText(const wchar_t* url,
	const wchar_t* objectName,
	const bool isHttps,
	const char* postData,
	const int postDataLength,
	const bool ignoreCertificateErros,
	const wchar_t* userAgent)
{
	HINTERNET hInternet = 0, hConnect = 0, hRequest = 0;
	BOOL resultOK = FALSE;
	TString receivedText;

	//hInternet = ::WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	hInternet = ::WinHttpOpen(userAgent, WINHTTP_ACCESS_TYPE_NO_PROXY, 0, WINHTTP_NO_PROXY_BYPASS, 0);

	if (hInternet)
		TInternet::applyProxySettings(url, hInternet);

	if (hInternet)
		hConnect = ::WinHttpConnect(hInternet, url, INTERNET_DEFAULT_PORT, 0);

	if (hConnect)
		hRequest = ::WinHttpOpenRequest(hConnect, L"POST", objectName, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, isHttps ? (WINHTTP_FLAG_REFRESH | WINHTTP_FLAG_SECURE) : WINHTTP_FLAG_REFRESH);

	if (hRequest)
	{
		if (ignoreCertificateErros)
		{
			DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE | SECURITY_FLAG_IGNORE_CERT_CN_INVALID | SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
			::WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(DWORD));
		}

		resultOK = ::WinHttpSendRequest(hRequest, L"Content-Type: application/x-www-form-urlencoded\r\n", -1, (LPVOID)postData, postDataLength, postDataLength, 0); // Send a request
	}

	if (resultOK)
		resultOK = ::WinHttpReceiveResponse(hRequest, NULL);

	if (resultOK)
	{
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;

		do
		{
			dwSize = 0;
			if (::WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				char* outBuffer = new char[dwSize + 1];
				::ZeroMemory(outBuffer, dwSize + 1);

				if (::WinHttpReadData(hRequest, (LPVOID)outBuffer, dwSize, &dwDownloaded))
					receivedText = receivedText + TString(outBuffer);

				delete[] outBuffer;
			}

		} while (dwSize > 0);
	}

	if (hRequest)
		::WinHttpCloseHandle(hRequest);

	if (hConnect)
		::WinHttpCloseHandle(hConnect);

	if (hInternet)
		::WinHttpCloseHandle(hInternet);

	return receivedText;
}

// =========== TMD5.cpp ===========

/*
	MCL - TMD5.cpp
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


TMD5::TMD5()
{

}

TString TMD5::generateFromString(const TString& text)
{
	if (text.getLength() == 0)
		return TString();

	ExtLibs::MD5 md5;
	char *strMD5 = md5.digestString((char*)(const char*)text);

	return TString(strMD5);
}

TString TMD5::generateFromFile(const TString& fileName)
{
	if (fileName.getLength() == 0) // empty path
		return TString();

	if (!TFile::isFileExists(fileName)) // file does not exists
		return TString();

	TFile file;
	file.openFile(fileName, TFile::TREAD, false);

	const DWORD fileSize = file.getFileSize();
	file.closeFile();

	if (fileSize==0) // empty file
		return TString();

	ExtLibs::MD5 md5;
	char *strMD5 = md5.digestFile((char*)(const char*)fileName);

	return TString(strMD5);
}

TMD5::~TMD5()
{

}

// =========== TSHA1.cpp ===========

/*
	MCL - TSHA1.cpp
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


TSHA1::TSHA1()
{

}

TString TSHA1::generateFromString(const TString& text)
{
	if (text.getLength() == 0)
		return TString();

	ExtLibs::CSHA1 sha1;
	sha1.Update((const UINT_8*)(const char*)text, text.getLength());
	sha1.Final();

	char szReport[256];
	szReport[0] = 0;
	sha1.ReportHash(szReport, ExtLibs::CSHA1::REPORT_HEX);

	return TString(szReport);
}

TString TSHA1::generateFromFile(const TString& fileName)
{
	if (fileName.getLength() == 0) // empty path
		return TString();

	if (!TFile::isFileExists(fileName)) // file does not exists
		return TString();

	TFile file;
	file.openFile(fileName, TFile::TREAD, false);

	const DWORD fileSize = file.getFileSize();
	file.closeFile();

	if (fileSize == 0) // empty file
		return TString();

	ExtLibs::CSHA1 sha1;
	sha1.HashFile((const char*)fileName);
	sha1.Final();

	char szReport[256];
	szReport[0] = 0;
	sha1.ReportHash(szReport, ExtLibs::CSHA1::REPORT_HEX);

	return TString(szReport);
}

TSHA1::~TSHA1()
{

}

// =========== TString.cpp ===========

/*
	MCL - TString.cpp
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

#include <stdio.h>

const TString operator+ (const char* const string1, const TString& string2)
{
	TString s(string1);
	return s.append(string2);
}

const TString operator+ (const wchar_t* const string1, const TString& string2)
{
	TString s(string1);
	return s.append(string2);
}

const TString operator+ (const TString& string1, const TString& string2)
{
	return string1.append(string2);
}

TString::TString()
{
	isZeroLength = true;
	stringHolder = 0;
	isStaticText = false;
}

TString::TString(const TString& other)
{
	isZeroLength = other.isZeroLength;

	if (other.isStaticText)
	{
		stringHolder = 0;

		isStaticText = true;
		staticText = other.staticText;
		staticTextLength = other.staticTextLength;
	}
	else if (other.stringHolder)
	{
		other.stringHolder->addReference();
		stringHolder = other.stringHolder;

		isStaticText = false;
	}
	else
	{
		stringHolder = 0;
		isStaticText = false;
	}
}

TString::TString(const char* const text, UINT codePage)
{
	isStaticText = false;

	if (text != 0)
	{
		int count = ::MultiByteToWideChar(codePage, 0, text, -1, 0, 0); // get char count with null character
		if (count)
		{
			wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
			if (::MultiByteToWideChar(codePage, 0, text, -1, w_text, count))
			{
				count--; // ignore null character

				stringHolder = new TStringHolder(w_text, count);
				isZeroLength = (count == 0);
				return;
			}
			else
			{
				::free(w_text);
			}
		}
	}

	isZeroLength = true;
	stringHolder = 0;
}

TString::TString(const wchar_t* const text, unsigned char behaviour, int length)
{
	if (text != 0)
	{
		staticTextLength = ((length == -1) ? (int)::wcslen(text) : length);
		if (staticTextLength)
		{
			isZeroLength = false;
			isStaticText = (behaviour == STATIC_TEXT_DO_NOT_FREE);

			if (isStaticText)
			{
				staticText = (wchar_t*)text;
				stringHolder = 0;
				return;
			}

			stringHolder = new TStringHolder(((behaviour == FREE_TEXT_WHEN_DONE) ? (wchar_t*)text : _wcsdup(text)), staticTextLength);
			return;
		}
	}

	isZeroLength = true;
	isStaticText = false;
	stringHolder = 0;
}

TString::TString(const int value, const int radix)
{
	stringHolder = new TStringHolder((wchar_t *)::malloc(33 * sizeof(wchar_t)), 0); // max 32 digits
	::_itow_s(value, stringHolder->w_text, 33, radix);

	stringHolder->count = (int)::wcslen(stringHolder->w_text);
	isZeroLength = (stringHolder->count == 0);
	isStaticText = false;
}

TString::TString(const float value, const int numDecimals, bool compact)
{
	isStaticText = false;

	// round it to given digits
	char *str_fmtp = (char*)malloc(32);
	char *str_buf = (char*)malloc(64);

	sprintf_s(str_fmtp, 32, "%%.%df", numDecimals);
	sprintf_s(str_buf, 64, str_fmtp, value);

	if (compact)
	{
		int len = (int)strlen(str_buf) - 1;
		for (int i = 0; i < numDecimals; i++) // kill ending zeros
		{
			if (str_buf[len - i] == '0')
				str_buf[len - i] = 0; // kill it
			else
				break;
		}

		// kill if last char is dot
		len = (int)strlen(str_buf) - 1;
		if (str_buf[len] == '.')
			str_buf[len] = 0; // kill it
	}

	int count = ::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, 0, 0); // get char count with null character
	if (count)
	{
		wchar_t *w_text = (wchar_t *)::malloc(count * sizeof(wchar_t));
		if (::MultiByteToWideChar(CP_UTF8, 0, str_buf, -1, w_text, count))
		{
			count--; // ignore null character

			stringHolder = new TStringHolder(w_text, count);
			isZeroLength = (count == 0);

			::free(str_buf);
			::free(str_fmtp);
			return;
		}
		else
		{
			::free(w_text);
		}
	}

	::free(str_buf);
	::free(str_fmtp);

	isZeroLength = true;
	stringHolder = 0;
}

const TString& TString::operator= (const TString& other)
{
	if (stringHolder)
		stringHolder->releaseReference();

	if (other.isStaticText)
	{
		isStaticText = true;
		staticText = other.staticText;
		staticTextLength = other.staticTextLength;
	}
	else if (other.stringHolder)
	{
		other.stringHolder->addReference();
		isStaticText = false;
	}
	else // other is empty
	{
		isStaticText = false;
	}

	stringHolder = other.stringHolder;
	isZeroLength = other.isZeroLength;

	return *this;
}

const TString& TString::operator= (const wchar_t* const other)
{
	isStaticText = false;

	if (stringHolder)
		stringHolder->releaseReference();

	if (other != 0)
	{
		const int count = (int)::wcslen(other);
		if (count)
		{
			stringHolder = new TStringHolder(::_wcsdup(other), count);
			isZeroLength = false;
			return *this;
		}
	}

	isZeroLength = true;
	stringHolder = 0;
	return *this;
}

const TString TString::operator+ (const TString& stringToAppend)
{
	return append(stringToAppend);
}

const TString TString::operator+ (const wchar_t* const textToAppend)
{
	return append(TString(textToAppend, USE_COPY_OF_TEXT, -1));
}

void TString::convertToRefCountedStringIfStatic()const
{
	if (isStaticText)
	{
		isStaticText = false;
		stringHolder = new TStringHolder(::_wcsdup(staticText), staticTextLength);
	}
}

TString::operator const char*()const
{
	if (!isZeroLength)
	{
		this->convertToRefCountedStringIfStatic();
		return stringHolder->getAnsiVersion();
	}
	return "";
}

TString::operator const wchar_t*()const
{
	if (isStaticText)
	{
		return staticText;
	}
	else if (stringHolder)
	{
		return stringHolder->w_text;
	}
	else
	{
		return L"";
	}
}

TString::operator wchar_t*()const
{
	if (isStaticText)
	{
		return staticText;
	}
	else if (stringHolder)
	{
		return stringHolder->w_text;
	}
	else
	{
		return (wchar_t*)L"";
	}
}

const char TString::operator[](const int index)const
{
	if (!isZeroLength)
	{
		this->convertToRefCountedStringIfStatic();

		if ((0 <= index) && (index <= (stringHolder->count - 1)))
		{
			return stringHolder->getAnsiVersion()[index];
		}
	}
	return -1;
}

TString TString::append(const TString& otherString)const
{
	if (!otherString.isZeroLength)
	{
		if (!this->isZeroLength)
		{
			const int totalCount = (isStaticText ? staticTextLength : stringHolder->count) + (otherString.isStaticText ? otherString.staticTextLength : otherString.stringHolder->count);
			wchar_t* destText = (wchar_t*)::malloc((totalCount + 1) * sizeof(wchar_t));

			::wcscpy_s(destText, (totalCount + 1), isStaticText ? staticText : stringHolder->w_text);
			::wcscat_s(destText, (totalCount + 1), otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text);

			return TString(destText, FREE_TEXT_WHEN_DONE, totalCount);
		}
		else // this string is empty
		{
			return otherString;
		}
	}
	else // other string is empty
	{
		return *this;
	}
}

TString TString::appendStaticText(const wchar_t* const text, int length, bool appendToEnd)const
{
	if (!this->isZeroLength)
	{
		const int totalCount = (isStaticText ? staticTextLength : stringHolder->count) + length;
		wchar_t* destText = (wchar_t*)::malloc((totalCount + 1) * sizeof(wchar_t));

		::wcscpy_s(destText, (totalCount + 1), appendToEnd ? (isStaticText ? staticText : stringHolder->w_text) : text);
		::wcscat_s(destText, (totalCount + 1), appendToEnd ? text : (isStaticText ? staticText : stringHolder->w_text));

		return TString(destText, FREE_TEXT_WHEN_DONE, totalCount);
	}
	else // this string is empty
	{
		return TString(text, TString::STATIC_TEXT_DO_NOT_FREE, length);
	}
}

void TString::assignStaticText(const wchar_t* const text, int length)
{
	if (stringHolder)
		stringHolder->releaseReference();
	
	stringHolder = 0;
	isZeroLength = false;
	isStaticText = true;
	staticText = (wchar_t*)text;
	staticTextLength = length;
}

TString TString::subString(int start, int end)const
{
	const int count = this->getLength();

	if ((0 <= start) && (start <= (count - 1)))
	{
		if ((start < end) && (end <= (count - 1)))
		{
			int size = (end - start) + 1;
			wchar_t* buf = (wchar_t*)::malloc((size + 1) * sizeof(wchar_t));
			wchar_t* src = (isStaticText ? staticText : stringHolder->w_text);
			::wcsncpy_s(buf, (size + 1), &src[start], size);
			buf[size] = 0;

			return TString(buf, FREE_TEXT_WHEN_DONE, size);
		}
	}
	return TString();
}

bool TString::compareIgnoreCase(const TString& otherString)const
{
	if ((!otherString.isZeroLength) && (!this->isZeroLength))
		return (::_wcsicmp((isStaticText ? staticText : stringHolder->w_text), (otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text)) == 0);

	return false;
}

bool TString::compare(const TString& otherString)const
{
	if ((!otherString.isZeroLength) && (!this->isZeroLength))
		return (::wcscmp((isStaticText ? staticText : stringHolder->w_text), (otherString.isStaticText ? otherString.staticText : otherString.stringHolder->w_text)) == 0);

	return false;
}

bool TString::compareWithStaticText(const wchar_t* const text)const
{
	if (!this->isZeroLength)
		return (::wcscmp((isStaticText ? staticText : stringHolder->w_text), text) == 0);

	return false;
}

bool TString::startsWithChar(wchar_t character)const
{
	if (!this->isZeroLength)
		return (isStaticText ? (staticText[0] == character) : (stringHolder->w_text[0] == character));

	return false;
}

bool TString::startsWithChar(char character)const
{
	if (!this->isZeroLength)
	{
		this->convertToRefCountedStringIfStatic();
		return (stringHolder->getAnsiVersion()[0] == character);
	}
	return false;
}

bool TString::endsWithChar(wchar_t character)const
{
	if (!this->isZeroLength)
		return (isStaticText ? (staticText[staticTextLength - 1] == character) : (stringHolder->w_text[stringHolder->count - 1] == character));

	return false;
}

bool TString::endsWithChar(char character)const
{
	if (!this->isZeroLength)
	{
		this->convertToRefCountedStringIfStatic();
		return (stringHolder->getAnsiVersion()[stringHolder->count - 1] == character);
	}
	return false;
}

bool TString::isQuotedString()const
{
	if ((isStaticText && (staticTextLength > 1)) || ((stringHolder != 0) && (stringHolder->count > 1))) // not empty + count greater than 1
		return (startsWithChar(L'\"') && endsWithChar(L'\"'));

	return false;
}

wchar_t TString::getCharAt(int index)const
{
	const int count = this->getLength();

	if ((0 <= index) && (index <= (count - 1)))
		return (isStaticText ? staticText[index] : stringHolder->w_text[index]);

	return -1;
}

int TString::getLength()const
{
	return (isStaticText ? staticTextLength : ((stringHolder != 0) ? stringHolder->count : 0));
}

bool TString::isEmpty()const
{
	return isZeroLength;
}

bool TString::isNotEmpty()const
{
	return !isZeroLength;
}

int TString::getIntValue()const
{
	if (isZeroLength)
		return 0;

	return ::_wtoi(isStaticText ? staticText : stringHolder->w_text);
}

TString TString::toUpperCase()const
{
	if (this->getLength() == 0)
		return TString();

	TString result((const wchar_t*)*this, TString::USE_COPY_OF_TEXT);
	::CharUpperBuffW((wchar_t*)result, result.getLength());

	return result;
}

TString TString::toLowerCase()const
{
	if (this->getLength() == 0)
		return TString();

	TString result((const wchar_t*)*this, TString::USE_COPY_OF_TEXT);
	::CharLowerBuffW((wchar_t*)result, result.getLength());

	return result;
}

TString::~TString()
{
	if (stringHolder)
		stringHolder->releaseReference();
}

// =========== TStringHolder.cpp ===========

/*
	MCL - TStringHolder.cpp
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



TStringHolder::TStringHolder(wchar_t *w_text, int count)
{
	refCount = 1;
	a_text = 0;
	this->w_text = w_text;
	this->count = count;

	::InitializeCriticalSection(&cs_a_text);
}

TStringHolder::~TStringHolder()
{
	::DeleteCriticalSection(&cs_a_text);
}

void TStringHolder::addReference()
{
	::InterlockedIncrement(&refCount);
}

void TStringHolder::releaseReference()
{
	const LONG res = ::InterlockedDecrement(&refCount);
	if(res == 0)
	{
		if(a_text)
			::free(a_text);

		if(w_text)
			::free(w_text);

		delete this;
	}
}

const char* TStringHolder::getAnsiVersion(UINT codePage)
{
	::EnterCriticalSection(&cs_a_text);

	if(a_text)
	{
		::LeaveCriticalSection(&cs_a_text);
		return a_text;
	}else
	{
		const int length = ::WideCharToMultiByte(codePage, 0, w_text, -1, 0, 0, 0, 0);
		if (length)
		{
			a_text = (char*)::malloc(length);
			if (::WideCharToMultiByte(codePage, 0, w_text, -1, a_text, length, 0, 0))
			{
				::LeaveCriticalSection(&cs_a_text);
				return a_text;
			}
			::free(a_text);
			a_text = 0;
		}

		::LeaveCriticalSection(&cs_a_text);
		return 0; // conversion error
	}
}

// =========== TThread.cpp ===========

/*
	MCL - TThread.cpp
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



TThread::TThread()
{
	handle = 0; 
	onRun = nullptr;
	threadShouldStop = false;
}

void TThread::setHandle(HANDLE handle)
{
	this->handle = handle;
}

HANDLE TThread::getHandle()
{
	return handle;
}

TThread::operator HANDLE()const
{
	return handle;
}

bool TThread::shouldRun()
{
	return !threadShouldStop;
}

bool TThread::isThreadRunning()
{
	if (handle)
	{
		const DWORD result = ::WaitForSingleObject(handle, 0);
		return (result != WAIT_OBJECT_0);
	}

	return false;
}

void TThread::shouldStop()
{
	threadShouldStop = true;
}

void TThread::waitUntilThreadFinish()
{
	::WaitForSingleObject(handle, INFINITE);
}

bool TThread::startThread()
{
	threadShouldStop = false;

	if (handle) // close old handle
	{
		::CloseHandle(handle);
		handle = 0;
	}

	return ::CreateMCLThread(this);
}

void TThread::uSleep(int waitTime)
{
	__int64 time1 = 0, time2 = 0, freq = 0;

	QueryPerformanceCounter((LARGE_INTEGER *)&time1);
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);

	do {
		QueryPerformanceCounter((LARGE_INTEGER *)&time2);
	} while ((time2 - time1) < ((waitTime * freq) / 1000000));
}

TThread::~TThread()
{
	if (handle)
		::CloseHandle(handle);
}

// =========== TTimer.cpp ===========

/*
	MCL - TTimer.cpp
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


TTimer::TTimer()
{
	MCL_INIT_VERIFIER;

	onTimer = nullptr;
	resolution = 1000;
	started = false;
	ownerWindow = nullptr;

	timerID = TPlatformUtil::getInstance()->generateTimerID(this);
}

void TTimer::setInterval(int resolution)
{
	this->resolution = resolution;
}

int TTimer::getInterval()
{
	return resolution;
}

void TTimer::setTimerWindow(TWindow &window)
{
	ownerWindow = &window;
}

void TTimer::setTimerID(UINT timerID)
{
	this->timerID = timerID;
}

UINT TTimer::getTimerID()
{
	return timerID;
}

void TTimer::setEnabled(bool enable)
{
	if (enable)
	{
		if (started)
			return;

		if (ownerWindow)
		{
			HWND hwnd = ownerWindow->getHandle();
			if (hwnd)
			{
				::SetTimer(hwnd, timerID, resolution, 0);
				started = true;
			}
		}
	}
	else
	{
		if (ownerWindow)
		{
			HWND hwnd = ownerWindow->getHandle();
			if (hwnd)
			{
				if (started)
					::KillTimer(hwnd, timerID);

				started = false;
			}
		}
	}
}

bool TTimer::isTimerRunning()
{
	return started;
}

TTimer::~TTimer()
{
	if(started)
		this->setEnabled(false);
}

// =========== TPerformanceCounter.cpp ===========

/*
	MCL - TPerformanceCounter.cpp
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


TPerformanceCounter::TPerformanceCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceFrequency(&li);

	this->pcFreq = double(li.QuadPart) / 1000.0;
}

void TPerformanceCounter::startCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	this->counterStart = li.QuadPart;
}

double TPerformanceCounter::endCounter()
{
	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);

	return double(li.QuadPart - this->counterStart) / this->pcFreq;
}

TPerformanceCounter::~TPerformanceCounter()
{

}


// =========== TPlatformUtil.cpp ===========

/*
	MCL - TPlatformUtil.cpp
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


static const int mcl_InitialMenuItemCount	= 400;
static const int mcl_InitialTimerCount		= 40;
static const int mcl_InitialControlID		= 100;
static const int mcl_InitialMenuItemID		= 30000;
static const int mcl_InitialTimerID			= 1000;

TPlatformUtil* TPlatformUtil::_instance = 0;

TPlatformUtil::TPlatformUtil()
{
	MCL_INIT_VERIFIER;

	timerCount = 0;
	menuItemCount = 0;
	classCount = 0;
	controlCount = 0;
	menuItemList = 0;
	timerList = 0;

	::InitializeCriticalSection(&criticalSectionForCount);
}

TPlatformUtil* TPlatformUtil::getInstance()
{
	if(_instance)
		return _instance;
	_instance = new TPlatformUtil();
	return _instance;
}

UINT TPlatformUtil::generateControlID()
{
	::EnterCriticalSection(&criticalSectionForCount);

	++controlCount;
	UINT retVal = controlCount + mcl_InitialControlID;

	::LeaveCriticalSection(&criticalSectionForCount);

	return retVal;
}

UINT TPlatformUtil::generateMenuItemID(TMenuItem *menuItem)
{
	::EnterCriticalSection(&criticalSectionForCount);

	if (menuItemList == 0) // generate on first call
		menuItemList = new TPointerList<TMenuItem*>(mcl_InitialMenuItemCount);

	++menuItemCount;
	menuItemList->addPointer(menuItem);
	UINT retVal = menuItemCount + mcl_InitialMenuItemID;

	::LeaveCriticalSection(&criticalSectionForCount);

	return retVal;
}

TMenuItem* TPlatformUtil::getMenuItemByID(UINT id)
{
	if (menuItemList)
		return menuItemList->getPointer(id - (mcl_InitialMenuItemID + 1));
	return 0;
}

TString TPlatformUtil::generateClassName()
{ 
	::EnterCriticalSection(&criticalSectionForCount);

	wchar_t *className = (wchar_t*)::malloc(32 * sizeof(wchar_t));

	className[0] = L'M';
	className[1] = L'C';
	className[2] = L'L';
	className[3] = L'_';

	::_itow_s((int)(INT_PTR)TApplication::hInstance, &className[4], 28, 10);

	int lastPos = (int)::wcslen(className);
	className[lastPos] = L'_';

	::_itow_s(classCount, &className[lastPos + 1], 32 - (lastPos + 1 ), 10);

	/*
	#ifdef _MSC_VER
		::swprintf(className, 32, L"RFC_%d_%d", (int)hInstance, classCount);
	#else
		::swprintf(className,L"RFC_%d_%d", (int)hInstance, classCount);
	#endif */

	++classCount;
	::LeaveCriticalSection(&criticalSectionForCount);

	return TString(className, TString::FREE_TEXT_WHEN_DONE);
}

UINT TPlatformUtil::generateTimerID(TTimer *timer)
{
	::EnterCriticalSection(&criticalSectionForCount);

	if (timerList == 0) // generate on first call
		timerList = new TPointerList<TTimer*>(mcl_InitialTimerCount);

	++timerCount;
	timerList->addPointer(timer);

	UINT retVal = timerCount + mcl_InitialTimerID;
	::LeaveCriticalSection(&criticalSectionForCount);

	return retVal;
}

TTimer* TPlatformUtil::getTimerByID(UINT id)
{
	if (timerList)
		return timerList->getPointer(id - (mcl_InitialTimerID + 1));
	return 0;
}

TPlatformUtil::~TPlatformUtil()
{
	if (menuItemList)
		delete menuItemList;

	if (timerList)
		delete timerList;

	::DeleteCriticalSection(&criticalSectionForCount);
}

// =========== TRegistry.cpp ===========

/*
	MCL - TRegistry.cpp
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


TRegistry::TRegistry()
{

}

bool TRegistry::createKey(HKEY hKeyRoot, const TString& subKey)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	::RegCloseKey(hkey);
	return true;
}

bool TRegistry::deleteKey(HKEY hKeyRoot, const TString& subKey)
{
	return ::RegDeleteKeyW(hKeyRoot, subKey) == ERROR_SUCCESS ? true : false;
}

bool TRegistry::readString(HKEY hKeyRoot, const TString& subKey, const TString& valueName, TString* result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			if (requiredBytes == 0) // value might be empty
			{
				*result = TString();
			}
			else{
				void* buffer = ::malloc(requiredBytes + 1); // +1 for strings which doesn't have ending null
				::ZeroMemory(buffer, requiredBytes + 1); // ending null might not contain in register

				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)buffer, &requiredBytes);
				*result = TString((wchar_t*)buffer, TString::FREE_TEXT_WHEN_DONE);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool TRegistry::writeString(HKEY hKeyRoot, const TString& subKey, const TString& valueName, const TString& value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const int bCount = (value.getLength() * sizeof(wchar_t)) + 1; // +1 for ending null
	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_SZ, (LPBYTE)(const wchar_t*)value, bCount);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool TRegistry::readDWORD(HKEY hKeyRoot, const TString& subKey, const TString& valueName, DWORD* result)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);
		const LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, &dwType, (LPBYTE)result, &dwSize);
		::RegCloseKey(hkey);

		if (ret == ERROR_SUCCESS)
			return true;
	}
	return false;
}

bool TRegistry::writeDWORD(HKEY hKeyRoot, const TString& subKey, const TString& valueName, DWORD value)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const DWORD dwSize = sizeof(DWORD);
	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_DWORD, (LPBYTE)&value, dwSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

bool TRegistry::readBinary(HKEY hKeyRoot, const TString& subKey, const TString& valueName, void** buffer, DWORD* buffSize)
{
	HKEY hkey = 0;
	if (::RegOpenKeyExW(hKeyRoot, subKey, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
	{
		// get required buffer size
		DWORD requiredBytes = 0;
		LONG ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, NULL, &requiredBytes);

		if (ret == ERROR_SUCCESS)
		{
			*buffSize = requiredBytes;
			if (requiredBytes == 0) // value might be empty
			{
				*buffer = 0;
			}
			else{
				*buffer = ::malloc(requiredBytes);
				ret = ::RegQueryValueExW(hkey, valueName, NULL, NULL, (LPBYTE)*buffer, &requiredBytes);
			}

			::RegCloseKey(hkey);
			return true;
		}

		::RegCloseKey(hkey);
	}
	return false;
}

bool TRegistry::writeBinary(HKEY hKeyRoot, const TString& subKey, const TString& valueName, void* buffer, DWORD buffSize)
{
	HKEY hkey = 0;
	if (::RegCreateKeyExW(hKeyRoot, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL) != ERROR_SUCCESS)
		return false;

	const LONG ret = ::RegSetValueExW(hkey, valueName, 0, REG_BINARY, (LPBYTE)buffer, buffSize);
	::RegCloseKey(hkey);

	if (ret == ERROR_SUCCESS)
		return true;

	return false;
}

TRegistry::~TRegistry()
{

}