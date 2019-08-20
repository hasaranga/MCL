
#pragma once

#ifdef _WIN64
	#define MCL64
	#define MCL_PTR_SIZE 8
	#define MCL_NATIVE_INT __int64
#else
	#define MCL32
	#define MCL_PTR_SIZE 4
	#define MCL_NATIVE_INT int
#endif
