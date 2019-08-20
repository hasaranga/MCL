
#pragma once

#include <windows.h>

struct TMessage
{
	HWND hwnd;
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;
};

