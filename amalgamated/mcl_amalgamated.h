
#ifndef _MCL_AMALGAMATED_H_
#define _MCL_AMALGAMATED_H_ 

#define AMALGAMATED_VERSION


// =========== config.h ===========

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

// =========== TMessage.h ===========

#pragma once

#include <windows.h>

struct TMessage
{
	HWND hwnd;
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;
};


// =========== TMouse.h ===========

/*
	MCL - TMouse.h
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

class TMouse
{
public:

	enum Buttons
	{
		LEFT_BUTTON = MK_LBUTTON,
		RIGHT_BUTTON = MK_RBUTTON,
		MIDDLE_BUTTON = MK_MBUTTON
	};
};



// =========== sha1.h ===========

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

#pragma once

#if !defined(SHA1_UTILITY_FUNCTIONS) && !defined(SHA1_NO_UTILITY_FUNCTIONS)
#define SHA1_UTILITY_FUNCTIONS
#endif

#include <memory.h> // Required for memset and memcpy

#ifdef SHA1_UTILITY_FUNCTIONS
#include <stdio.h>  // Required for file access and sprintf
#include <string.h> // Required for strcat and strcpy
#endif

#ifdef _MSC_VER
#include <stdlib.h>
#endif


namespace ExtLibs{

	// You can define the endian mode in your files, without modifying the SHA1
	// source files. Just #define SHA1_LITTLE_ENDIAN or #define SHA1_BIG_ENDIAN
	// in your files, before including the SHA1.h header file. If you don't
	// define anything, the class defaults to little endian.
#if !defined(SHA1_LITTLE_ENDIAN) && !defined(SHA1_BIG_ENDIAN)
#define SHA1_LITTLE_ENDIAN
#endif

	// Same here. If you want variable wiping, #define SHA1_WIPE_VARIABLES, if
	// not, #define SHA1_NO_WIPE_VARIABLES. If you don't define anything, it
	// defaults to wiping.
#if !defined(SHA1_WIPE_VARIABLES) && !defined(SHA1_NO_WIPE_VARIABLES)
#define SHA1_WIPE_VARIABLES
#endif

	/////////////////////////////////////////////////////////////////////////////
	// Define 8- and 32-bit variables

#ifndef UINT_32

#ifdef _MSC_VER // Compiling with Microsoft compiler

#define UINT_8  unsigned __int8
#define UINT_32 unsigned __int32

#else // !_MSC_VER

#define UINT_8 unsigned char

#if (ULONG_MAX == 0xFFFFFFFF)
#define UINT_32 unsigned long
#else
#define UINT_32 unsigned int
#endif

#endif // _MSC_VER
#endif // UINT_32

	/////////////////////////////////////////////////////////////////////////////
	// Declare SHA1 workspace

	typedef union
	{
		UINT_8 c[64];
		UINT_32 l[16];
	} SHA1_WORKSPACE_BLOCK;

	class CSHA1
	{
	public:
#ifdef SHA1_UTILITY_FUNCTIONS
		// Two different formats for ReportHash(...)
		enum
		{
			REPORT_HEX = 0,
			REPORT_DIGIT = 1
		};
#endif

		// Constructor and destructor
		CSHA1();
		~CSHA1();

		UINT_32 m_state[5];
		UINT_32 m_count[2];
		UINT_32 m_reserved1[1]; // Memory alignment padding
		UINT_8 m_buffer[64];
		UINT_8 m_digest[20];
		UINT_32 m_reserved2[3]; // Memory alignment padding

		void Reset();

		// Update the hash value
		void Update(const UINT_8* pData, UINT_32 uLen);
#ifdef SHA1_UTILITY_FUNCTIONS
		bool HashFile(const char* szFileName);
#endif

		// Finalize hash and report
		void Final();

		// Report functions: as pre-formatted and raw data
#ifdef SHA1_UTILITY_FUNCTIONS
		void ReportHash(char* szReport, unsigned char uReportType = REPORT_HEX) const;
#endif
		void GetHash(UINT_8* puDest) const;

	private:
		// Private SHA-1 transformation
		void Transform(UINT_32* pState, const UINT_8* pBuffer);

		// Member variables
		UINT_8 m_workspace[64];
		SHA1_WORKSPACE_BLOCK* m_block; // SHA1 pointer to the byte array above
	};

}


// =========== TScopedStructPointer.h ===========

/*
	MCL - TScopedStructPointer.h
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

#include <malloc.h>
#include <Objbase.h>

template<class StructType>
class TReleaseUsingFree
{
public:
	static void free(StructType* structPtr)
	{
		::free(structPtr);
	}
};

template<class StructType>
class TReleaseUsingTaskMemFree
{
public:
	static void free(StructType* memory)
	{
		::CoTaskMemFree(memory);
	}
};

/**
	This class holds a pointer to the struct which is automatically freed when this object goes
	out of scope. 
*/
template<class StructType, class ReleaseMethod = TReleaseUsingFree<StructType>>
class TScopedStructPointer
{
private:
	StructType* structPointer;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline TScopedStructPointer()
	{
		structPointer = NULL;
	}

	inline TScopedStructPointer(StructType* structPointer)
	{
		this->structPointer = structPointer;
	}

	TScopedStructPointer(TScopedStructPointer& structPointerToTransferFrom)
	{
		this->structPointer = structPointerToTransferFrom.structPointer;
		structPointerToTransferFrom.structPointer = NULL;
	}

	bool isNull()
	{
		return (structPointer == NULL);
	}

	/** 
		Removes the current struct pointer from this TScopedStructPointer without freeing it.
		This will return the current struct pointer, and set the TScopedStructPointer to a null pointer.
	*/
	StructType* detach()
	{ 
		StructType* m = structPointer;
		structPointer = NULL;
		return m; 
	}

	~TScopedStructPointer()
	{
		if (structPointer)
			ReleaseMethod::Free(structPointer);
	}

	/** 
		Changes this TScopedStructPointer to point to a new struct.

		If this TScopedStructPointer already points to a struct, that struct
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	TScopedStructPointer& operator= (StructType* const newStructPointer)
	{
		if (structPointer != newStructPointer)
		{
			StructType* const oldStructPointer = structPointer;
			structPointer = newStructPointer;

			if (oldStructPointer)
				ReleaseMethod::Free(oldStructPointer);
		}

		return *this;
	}

	inline StructType** operator&() { return &structPointer; }

	/** Returns the struct pointer that this KScopedStructPointer refers to. */
	inline operator StructType*() const { return structPointer; }

	/** Returns the struct pointer that this KScopedStructPointer refers to. */
	inline StructType& operator*() const { return *structPointer; }

	/** Lets you access properties of the struct that this KScopedStructPointer refers to. */
	inline StructType* operator->() const { return structPointer; }

};

// =========== TScopedMemoryBlock.h ===========

/*
	MCL - TScopedMemoryBlock.h
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

#include <malloc.h>

/**
	This class holds a memory which is automatically freed when this object goes
	out of scope.

	Once a memory has been passed to a TScopedMemoryBlock, it will make sure that the memory
	gets freed when the TScopedMemoryBlock is deleted. Using the TScopedMemoryBlock on the stack or
	as member variables is a good way to use RAII to avoid accidentally leaking dynamically
	allocated memory.

	A TScopedMemoryBlock can be used in pretty much the same way that you'd use a normal pointer
	to a memory block. If you use the assignment operator to assign a different memory to a
	TScopedMemoryBlock, the old one will be automatically freed.

	If you need to get a memory block out of a TScopedMemoryBlock without it being freed, you
	can use the Release() method.

	(ref: ScopedPointer class of JUCE)

	e.g. @code
	TScopedMemoryBlock<float*> a = (float*)malloc(512 * sizeof(float)); // slow
	TScopedMemoryBlock<float*> b( (float*)malloc(512 * sizeof(float)) ); // fast
	@endcode
*/
template<class T>
class TScopedMemoryBlock
{
private:
	T memoryBlock;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline TScopedMemoryBlock()
	{
		memoryBlock = NULL;
	}

	inline TScopedMemoryBlock(T memoryBlock)
	{
		this->memoryBlock = memoryBlock;
	}

	TScopedMemoryBlock(TScopedMemoryBlock& memoryBlockToTransferFrom)
	{
		this->memoryBlock = memoryBlockToTransferFrom.memoryBlock;
		memoryBlockToTransferFrom.memoryBlock = NULL;
	}

	bool IsNull()
	{
		return (memoryBlock == NULL);
	}

	/** 
		Removes the current memory block from this TScopedMemoryBlock without freeing it.
		This will return the current memory block, and set the TScopedMemoryBlock to a null pointer.
	*/
	T detach()
	{ 
		T m = memoryBlock;
		memoryBlock = NULL;
		return m; 
	}

	~TScopedMemoryBlock()
	{
		if (memoryBlock)
			::free(memoryBlock);
	}

	/** 
		Changes this TScopedMemoryBlock to point to a new memory block.

		If this TScopedMemoryBlock already points to a memory, that memory
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	TScopedMemoryBlock& operator= (T const newMemoryBlock)
	{
		if (memoryBlock != newMemoryBlock)
		{
			T const oldMemoryBlock = memoryBlock;
			memoryBlock = newMemoryBlock;

			if (oldMemoryBlock)
				::free(oldMemoryBlock);
		}

		return *this;
	}

	/** Returns the memory block that this TScopedMemoryBlock refers to. */
	inline operator T() const { return memoryBlock; }

};

// =========== TScopedHandle.h ===========

/*
	MCL - TScopedHandle.h
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

/**
	This class holds a handle which is automatically closed when this object goes
	out of scope.
*/
class TScopedHandle
{
private:
	HANDLE handle;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	TScopedHandle()
	{
		handle = 0;
	}

	TScopedHandle(HANDLE handle)
	{
		this->handle = handle;
	}

	HANDLE detach()
	{
		HANDLE h = handle;
		handle = 0;
		return h;
	}

	TScopedHandle& operator= (HANDLE newHandle)
	{
		if (handle != newHandle)
		{
			HANDLE oldHandle = handle;
			handle = newHandle;

			if (oldHandle)
				::CloseHandle(oldHandle);
		}

		return *this;
	}

	bool isNull()
	{
		return (handle == 0);
	}

	~TScopedHandle()
	{
		if (handle)
			::CloseHandle(handle);
	}

	inline operator HANDLE() const { return handle; }

	inline HANDLE* operator&() { return &handle; }

};

// =========== TScopedGdiObject.h ===========

/*
	MCL - TScopedGdiObject.h
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

/**
	This class holds a gdi object which is automatically freed when this object goes
	out of scope.

	If you need to get a gdi object out of a TScopedGdiObject without it being freed, you
	can use the Release() method.

	(ref: ScopedPointer class of JUCE)

	e.g. @code
	TScopedGdiObject<HBRUSH> a = ::CreateSolidBrush(RGB(255, 255, 255)); // slow
	TScopedGdiObject<HBRUSH> b(::CreateSolidBrush(RGB(255, 0, 0))); // fast
	@endcode,
*/
template<class T>
class TScopedGdiObject
{
private:
	T gdiObject;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline TScopedGdiObject()
	{
		gdiObject = 0;
	}

	inline TScopedGdiObject(T gdiObject)
	{
		this->gdiObject = gdiObject;
	}

	TScopedGdiObject(TScopedGdiObject& gdiObjectToTransferFrom)
	{
		this->gdiObject = gdiObjectToTransferFrom.gdiObject;
		gdiObjectToTransferFrom.gdiObject = 0;
	}

	/** 
		Removes the current gdi object from this TScopedGdiObject without freeing it.
		This will return the current gdi object, and set the TScopedGdiObject to a null value.
	*/
	T detach()
	{ 
		T g = gdiObject;
		gdiObject = 0;
		return g; 
	}

	~TScopedGdiObject()
	{
		if (gdiObject)
			::DeleteObject(gdiObject);
	}

	/** 
		Changes this TScopedGdiObject to point to a new gdi object.

		If this TScopedGdiObject already points to a gdi object, that object
		will first be freed.

		The object that you pass in may be a zero.
	*/
	TScopedGdiObject& operator= (T const newGdiObject)
	{
		if (gdiObject != newGdiObject)
		{
			T const oldgdiObject = gdiObject;
			gdiObject = newGdiObject;

			if (oldgdiObject)
				::DeleteObject(oldgdiObject);
		}

		return *this;
	}

	/** Returns the gdi object that this TScopedGdiObject refers to. */
	inline operator T() const { return gdiObject; }

};

// =========== md5.h ===========

	// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
	// rights reserved.

	// License to copy and use this software is granted provided that it
	// is identified as the "RSA Data Security, Inc. MD5 Message-Digest
	// Algorithm" in all material mentioning or referencing this software
	// or this function.
	//
	// License is also granted to make and use derivative works provided
	// that such works are identified as "derived from the RSA Data
	// Security, Inc. MD5 Message-Digest Algorithm" in all material
	// mentioning or referencing the derived work.
	//
	// RSA Data Security, Inc. makes no representations concerning either
	// the merchantability of this software or the suitability of this
	// software for any particular purpose. It is provided "as is"
	// without express or implied warranty of any kind.
	//
	// These notices must be retained in any copies of any part of this
	// documentation and/or software.



	// The original md5 implementation avoids external libraries.
	// This version has dependency on stdio.h for file input and
	// string.h for memcpy.

#pragma once

#include <stdio.h>
#include <string.h>

namespace ExtLibs{

#pragma region MD5 defines
	// Constants for MD5Transform routine.
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

	static unsigned char PADDING[64] = {
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	// F, G, H and I are basic MD5 functions.
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

	// ROTATE_LEFT rotates x left n bits.
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

	// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
	// Rotation is separate from addition to prevent recomputation.
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F((b), (c), (d)) + (x)+(UINT4)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
	}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G((b), (c), (d)) + (x)+(UINT4)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
		}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H((b), (c), (d)) + (x)+(UINT4)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
		}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I((b), (c), (d)) + (x)+(UINT4)(ac); \
	(a) = ROTATE_LEFT((a), (s)); \
	(a) += (b); \
		}
#pragma endregion

	typedef unsigned char BYTE;

	// POINTER defines a generic pointer type
	typedef unsigned char *POINTER;

	// UINT2 defines a two byte word
	typedef unsigned short int UINT2;

	// UINT4 defines a four byte word
	typedef unsigned long int UINT4;


	// convenient object that wraps
	// the C-functions for use in C++ only
	class MD5
	{
	private:
		struct __context_t {
			UINT4 state[4];                                   /* state (ABCD) */
			UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
			unsigned char buffer[64];                         /* input buffer */
		} context;

#pragma region static helper functions
		// The core of the MD5 algorithm is here.
		// MD5 basic transformation. Transforms state based on block.
		static void MD5Transform(UINT4 state[4], unsigned char block[64])
		{
			UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

			Decode(x, block, 64);

			/* Round 1 */
			FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
			FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
			FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
			FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
			FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
			FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
			FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
			FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
			FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
			FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
			FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
			FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
			FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
			FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
			FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
			FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

			/* Round 2 */
			GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
			GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
			GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
			GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
			GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
			GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
			GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
			GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
			GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
			GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
			GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
			GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
			GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
			GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
			GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
			GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

			/* Round 3 */
			HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
			HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
			HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
			HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
			HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
			HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
			HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
			HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
			HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
			HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
			HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
			HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
			HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
			HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
			HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
			HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

			/* Round 4 */
			II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
			II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
			II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
			II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
			II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
			II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
			II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
			II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
			II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
			II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
			II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
			II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
			II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
			II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
			II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
			II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

			state[0] += a;
			state[1] += b;
			state[2] += c;
			state[3] += d;

			// Zeroize sensitive information.
			memset((POINTER)x, 0, sizeof (x));
		}

		// Encodes input (UINT4) into output (unsigned char). Assumes len is
		// a multiple of 4.
		static void Encode(unsigned char *output, UINT4 *input, unsigned int len)
		{
			unsigned int i, j;

			for (i = 0, j = 0; j < len; i++, j += 4) {
				output[j] = (unsigned char)(input[i] & 0xff);
				output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
				output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
				output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
			}
		}

		// Decodes input (unsigned char) into output (UINT4). Assumes len is
		// a multiple of 4.
		static void Decode(UINT4 *output, unsigned char *input, unsigned int len)
		{
			unsigned int i, j;

			for (i = 0, j = 0; j < len; i++, j += 4)
				output[i] = ((UINT4)input[j]) | (((UINT4)input[j + 1]) << 8) |
				(((UINT4)input[j + 2]) << 16) | (((UINT4)input[j + 3]) << 24);
		}
#pragma endregion


	public:
		// MAIN FUNCTIONS
		MD5()
		{
			Init();
		}

		// MD5 initialization. Begins an MD5 operation, writing a new context.
		void Init()
		{
			context.count[0] = context.count[1] = 0;

			// Load magic initialization constants.
			context.state[0] = 0x67452301;
			context.state[1] = 0xefcdab89;
			context.state[2] = 0x98badcfe;
			context.state[3] = 0x10325476;
		}

		// MD5 block update operation. Continues an MD5 message-digest
		// operation, processing another message block, and updating the
		// context.
		void Update(
			unsigned char *input,   // input block
			unsigned int inputLen) // length of input block
		{
			unsigned int i, index, partLen;

			// Compute number of bytes mod 64
			index = (unsigned int)((context.count[0] >> 3) & 0x3F);

			// Update number of bits
			if ((context.count[0] += ((UINT4)inputLen << 3))
				< ((UINT4)inputLen << 3))
				context.count[1]++;
			context.count[1] += ((UINT4)inputLen >> 29);

			partLen = 64 - index;

			// Transform as many times as possible.
			if (inputLen >= partLen) {
				memcpy((POINTER)&context.buffer[index], (POINTER)input, partLen);
				MD5Transform(context.state, context.buffer);

				for (i = partLen; i + 63 < inputLen; i += 64)
					MD5Transform(context.state, &input[i]);

				index = 0;
			}
			else
				i = 0;

			/* Buffer remaining input */
			memcpy((POINTER)&context.buffer[index], (POINTER)&input[i], inputLen - i);
		}

		// MD5 finalization. Ends an MD5 message-digest operation, writing the
		// the message digest and zeroizing the context.
		// Writes to digestRaw
		void Final()
		{
			unsigned char bits[8];
			unsigned int index, padLen;

			// Save number of bits
			Encode(bits, context.count, 8);

			// Pad out to 56 mod 64.
			index = (unsigned int)((context.count[0] >> 3) & 0x3f);
			padLen = (index < 56) ? (56 - index) : (120 - index);
			Update(PADDING, padLen);

			// Append length (before padding)
			Update(bits, 8);

			// Store state in digest
			Encode(digestRaw, context.state, 16);

			// Zeroize sensitive information.
			memset((POINTER)&context, 0, sizeof (context));

			writeToString();
		}

		// Buffer must be 32+1 (nul) = 33 chars long at least 
		void writeToString()
		{
			int pos;

			for (pos = 0; pos < 16; pos++)
				sprintf_s(digestChars + (pos * 2), 33, "%02x", digestRaw[pos]);
		}


	public:
		// an MD5 digest is a 16-byte number (32 hex digits)
		BYTE digestRaw[16];

		// This version of the digest is actually
		// a "printf'd" version of the digest.
		char digestChars[33];

		/// Load a file from disk and digest it
		// Digests a file and returns the result.
		char* digestFile(char *filename)
		{
			Init();

			FILE *file;

			int len;
			unsigned char buffer[1024];

			if (fopen_s(&file, filename, "rb") != 0) // file open failed
			{
				digestChars[0] = 0; 
			}
			else
			{
				while (len = (int)fread(buffer, 1, 1024, file))
					Update(buffer, len);
				Final();

				fclose(file);
			}

			return digestChars;
		}

		/// Digests a byte-array already in memory
		char* digestMemory(BYTE *memchunk, int len)
		{
			Init();
			Update(memchunk, len);
			Final();

			return digestChars;
		}
		
		// Digests a string and prints the result.
		char* digestString(char *string)
		{
			Init();
			Update((unsigned char*)string, (unsigned int)strlen(string));
			Final();

			return digestChars;
		}
	};


}

// =========== TScopedComPointer.h ===========

/*
	MCL - TScopedComPointer.h
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

/**
	This class holds a COM pointer which is automatically released when this object goes
	out of scope.
*/
template<class T>
class TScopedComPointer
{
private:
	T* object;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline TScopedComPointer()
	{
		object = NULL;
	}

	inline TScopedComPointer(T* object)
	{
		this->object = object;

		if (this->object)
			this->object->AddRef();
	}

	TScopedComPointer(TScopedComPointer& objectToTransferFrom)
	{
		object = objectToTransferFrom.object;

		if (object)
			object->AddRef();
	}

	bool isNull()
	{
		return (object == NULL);
	}

	/** 
		Removes the current COM object from this TScopedComPointer without releasing it.
		This will return the current object, and set the TScopedComPointer to a null pointer.
	*/
	T* detach()
	{ 
		T* o = object; 
		object = NULL; 
		return o; 
	}

	~TScopedComPointer()
	{
		if (object)
			object->Release();

		object = NULL;
	}

	inline T** operator&() { return &object; }

	/** Returns the object that this TScopedComPointer refers to. */
	inline operator T*() const { return object; }

	/** Returns the object that this TScopedComPointer refers to. */
	inline T& operator*() const { return *object; }

	/** Lets you access methods and properties of the object that this TScopedComPointer refers to. */
	inline T* operator->() const { return object; }

};

// =========== TScopedClassPointer.h ===========

/*
	MCL - TScopedClassPointer.h
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

/**
	This class holds a pointer which is automatically deleted when this object goes
	out of scope.

	Once a pointer has been passed to a TScopedClassPointer, it will make sure that the pointer
	gets deleted when the TScopedClassPointer is deleted. Using the TScopedClassPointer on the stack or
	as member variables is a good way to use RAII to avoid accidentally leaking dynamically
	created objects.

	A TScopedClassPointer can be used in pretty much the same way that you'd use a normal pointer
	to an object. If you use the assignment operator to assign a different object to a
	TScopedClassPointer, the old one will be automatically deleted.

	Important note: The class is designed to hold a pointer to an object, NOT to an array!
	It calls delete on its payload, not delete[], so do not give it an array to hold!

	If you need to get a pointer out of a TScopedClassPointer without it being deleted, you
	can use the Release() method.

	(ref: ScopedPointer class of JUCE)

	e.g. @code
	TScopedClassPointer<MyClass> a = new MyClass(); // slow
	a->myMethod();
	a = new MyClass(); // old object will be deleted
	TScopedClassPointer<MyClass> b( new MyClass() ); // fast
	@endcode

*/
template<class T>
class TScopedClassPointer
{
private:
	T* object;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline TScopedClassPointer()
	{
		object = 0;
	}

	inline TScopedClassPointer(T* object)
	{
		this->object = object;
	}

	TScopedClassPointer(TScopedClassPointer& objectToTransferFrom)
	{
		this->object = objectToTransferFrom.object;
		objectToTransferFrom.object = 0;
	}

	bool isNull()
	{
		return (object == NULL);
	}

	/** 
		Removes the current object from this TScopedClassPointer without deleting it.
		This will return the current object, and set the TScopedClassPointer to a null pointer.
	*/
	T* detach()
	{ 
		T* o = object; 
		object = 0; 
		return o; 
	}

	~TScopedClassPointer()
	{
		if (object)
			delete object;
	}

	/** 
		Changes this TScopedClassPointer to point to a new object.

		If this TScopedClassPointer already points to an object, that object
		will first be deleted.

		The pointer that you pass in may be a nullptr.
	*/
	TScopedClassPointer& operator= (T* const newObject)
	{
		if (object != newObject)
		{
			T* const oldObject = object;
			object = newObject;

			if (oldObject)
				delete oldObject;
		}

		return *this;
	}

	/** Returns the object that this TScopedClassPointer refers to. */
	inline operator T*() const { return object; }

	/** Returns the object that this TScopedClassPointer refers to. */
	inline T& operator*() const { return *object; }

	/** Lets you access methods and properties of the object that this TScopedClassPointer refers to. */
	inline T* operator->() const { return object; }
};

// =========== TLeakDetector.h ===========

/*
	MCL - TLeakDetector.h
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
#include <string.h>  
#include <stdlib.h>  

/**
	Embedding an instance of this class inside another class can be used as a
	low-overhead way of detecting leaked instances.

	This class keeps an internal static count of the number of instances that are
	active, so that when the app is shutdown and the static destructors are called,
	it can check whether there are any left-over instances that may have been leaked.

	To use it, simply declare MCL_LEAK_DETECTOR(YourClassName) inside a private section
	of the class declaration.

	(ref: LeakedObjectDetector class of JUCE)

	@code
	class MyClass
	{
	public:
		MyClass();
		void method1();

	private:
		MCL_LEAK_DETECTOR (MyClass)
	};
	@endcode
*/
template <class T>
class TLeakDetector
{
public:
	TLeakDetector()
	{
		::InterlockedIncrement(&getCounter().numObjects);
	}

	TLeakDetector(const TLeakDetector&)
	{
		::InterlockedIncrement(&getCounter().numObjects);
	}

	~TLeakDetector()
	{
		::InterlockedDecrement(&getCounter().numObjects);
	}

private:
	class LeakCounter
	{
	public:
		LeakCounter()
		{
			numObjects = 0;
		}

		~LeakCounter()
		{
			if (numObjects > 0) // If you hit this, then you've leaked one or more objects of the type specified by the template parameter
			{
				char textBuffer[128];
				::strcpy_s(textBuffer, 128, "Leaked objects detected : ");

				char intBuffer[16];
				::_itoa_s((int)numObjects, intBuffer, 16, 10);

				::strcat_s(textBuffer, 128, intBuffer);
				::strcat_s(textBuffer, 128, " instance(s) of class ");
				::strcat_s(textBuffer, 128, getLeakedClassName());

				::MessageBoxA(0, textBuffer, "Warning", MB_ICONWARNING);
			}
		}

		volatile long numObjects;
	};

	static const char* getLeakedClassName()
	{
		return T::mcl_getLeakedClassName();
	}

	static LeakCounter& getCounter()
	{
		static LeakCounter counter;
		return counter;
	}
};

#ifdef _DEBUG
	#define MCL_LEAK_DETECTOR(Class) \
			friend class TLeakDetector<Class>; \
			TLeakDetector<Class> mcl_leakDetector; \
			static const char* mcl_getLeakedClassName() { return #Class; }
#else 
	#define MCL_LEAK_DETECTOR(Class)
#endif

// =========== eventprop.h ===========

/*
	MCL - eventprop.h
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

#include <functional>

template <class C, typename Ret, typename ... Ts>
std::function<Ret(Ts...)> __EVENT(C* c, Ret(C::*m)(Ts...))
{
	return [=](auto&&... args) { return (c->*m)(std::forward<decltype(args)>(args)...); };
}

template <class C, typename Ret, typename ... Ts>
std::function<Ret(Ts...)> __EVENT(const C* c, Ret(C::*m)(Ts...) const)
{
	return [=](auto&&... args) { return (c->*m)(std::forward<decltype(args)>(args)...); };
}

#define EVENT(obj, method) __EVENT(obj, &method)
#define EVENT_DEF(_ret, _name, ...) std::function<_ret(##__VA_ARGS__)> _name;

#define PROPERTY_DEF(_type, _name, _get, _set) __declspec(property(get=_get, put=_set)) _type _name;
#define PROPERTY_DEF_READONLY(_type, _name, _get) __declspec(property(get=_get)) _type _name;
#define PROPERTY_DEF_WRITEONLY(_type, _name, _set) __declspec(property(put=_set)) _type _name;

// =========== TScopedCriticalSection.h ===========

/*
	MCL - TScopedCriticalSection.h
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

/**
	This class holds a pointer to CRITICAL_SECTION which is automatically released when this object goes
	out of scope.
*/
class TScopedCriticalSection
{
private:
	CRITICAL_SECTION *criticalSection;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	TScopedCriticalSection(CRITICAL_SECTION *criticalSection)
	{
		this->criticalSection = criticalSection;
		::EnterCriticalSection(criticalSection);
	}

	// does not call LeaveCriticalSection
	CRITICAL_SECTION* detach()
	{ 
		CRITICAL_SECTION *c = criticalSection;
		criticalSection = NULL;
		return c; 
	}

	~TScopedCriticalSection()
	{
		if (criticalSection)
			::LeaveCriticalSection(criticalSection);
	}

	inline operator CRITICAL_SECTION*() const { return criticalSection; }

};

// =========== TTime.h ===========

/*
	MCL - TTime.h
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

class TTime
{
public:

	TTime(){}

	// Returns time difference in units of 100 us.
	static _int64 delta100us(const SYSTEMTIME &time1, const SYSTEMTIME &time2)
	{
		union timeunion {
			FILETIME fileTime;
			ULARGE_INTEGER ul;
		};

		timeunion ft1;
		timeunion ft2;

		SystemTimeToFileTime(&time1, &ft1.fileTime);
		SystemTimeToFileTime(&time2, &ft2.fileTime);

		return ft2.ul.QuadPart - ft1.ul.QuadPart;
	}

	// Returns time difference in seconds.
	static _int64 deltaSeconds(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		return (delta100us(time1, time2) / 10000000);
	}

	// Returns time difference in minutes.
	static _int64 deltaMinutes(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		return (deltaSeconds(time1, time2) / 60);
	}

	// Returns time difference in hours.
	static _int64 deltaHours(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		return (deltaMinutes(time1, time2) / 60);
	}

	static void getNow(SYSTEMTIME* time, const bool isLocalTime = true)
	{
		if (isLocalTime)
			::GetLocalTime(time);
		else
			::GetSystemTime(time);
	}

	virtual ~TTime(){}

private:
	MCL_LEAK_DETECTOR(TTime)
};

// =========== TStringHolder.h ===========

/*
	MCL - TStringHolder.h
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

/**
	This class holds reference counted string.
*/
class TStringHolder
{
	volatile LONG refCount;
	char *a_text; // ansi version

	CRITICAL_SECTION cs_a_text; // to guard ansi string creation

public:
	wchar_t *w_text; // unicode version
	int count; // character count

	TStringHolder(wchar_t *w_text, int count);

	~TStringHolder();

	/**
		Make sure to call this method if you contruct new KStringHolder or keep reference to another KStringHolder object.
	*/
	void addReference();

	/**
		Make sure to call this method if you clear reference to KStringHolder object. it will release allocated memory for string.
	*/
	void releaseReference();

	/**
		ANSI version available only when needed.
	*/
	const char* getAnsiVersion(UINT codePage = CP_UTF8);

private:
	MCL_LEAK_DETECTOR(TStringHolder)
};


// =========== TThread.h ===========

/*
	MCL - TThread.h
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

class TThread
{
protected:
	HANDLE handle;
	volatile bool threadShouldStop;

public:

	// void onRun(TThread* thread)
	EVENT_DEF(void, onRun, TThread*)

	TThread();

	/**
		For internal use only!
	*/
	virtual void setHandle(HANDLE handle);

	/**
		Returns handle of the thread
	*/
	virtual HANDLE getHandle();

	operator HANDLE()const;

	/**
		Starts thread
	*/
	virtual bool startThread();

	/**
		Another thread can signal this thread should stop. 
	*/
	virtual void shouldStop();

	/**
		@returns true if thread should run
	*/
	virtual bool shouldRun();

	/**
		@returns true if thread is still running
	*/
	virtual bool isThreadRunning();

	/**
		Calling thread is not return until this thread finish.
	*/
	virtual void waitUntilThreadFinish();

	/**
		Sleeps calling thread to given micro seconds.
	*/
	static void uSleep(int waitTime);

	virtual ~TThread();

private:
	MCL_LEAK_DETECTOR(TThread)
};

// =========== TPerformanceCounter.h ===========

/*
	MCL - TPerformanceCounter.h
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

class TPerformanceCounter
{
protected:
	double pcFreq;
	__int64 counterStart;

public:
	TPerformanceCounter();

	virtual void startCounter();

	/**
		returns delta time(milliseconds) between StartCounter and EndCounter calls.
	*/
	virtual double endCounter();

	virtual ~TPerformanceCounter();

private:
	MCL_LEAK_DETECTOR(TPerformanceCounter)
};

// =========== TPointerList.h ===========

/*
	MCL - TPointerList.h
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

#include <malloc.h>
#include <windows.h>

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma warning(disable:4311)
	#pragma warning(disable:4312)
#endif


/**
	Holds a resizable list of pointers.
	To make all the methods thread-safe, pass true for isThreadSafe parameter of constructor.
	Duplicated items allowed!
	index is between 0 to (item count-1)

	e.g. @code
	TButton btn1;
	TPointerList<TButton*> btnList(1024,true);
	btnList.addPointer(&btn1);
	btnList.addPointer(&btn1);
	@endcode
*/
template<class T>
class TPointerList
{
protected:
	int size;
	int roomCount;
	int roomIncrement;
	T* list;

	CRITICAL_SECTION criticalSection;
	volatile bool isThreadSafe;

public:
	/**
		Constructs PointerList object.
		@param roomIncrement initial and reallocation size of internal memory block in DWORDS
		@param isThreadSafe make all the methods thread-safe
	*/
	TPointerList(const int roomIncrement = 1024, const bool isThreadSafe = false) // 1024*4=4096 = default alignment!
	{
		roomCount = roomIncrement;
		this->roomIncrement = roomIncrement;
		this->isThreadSafe = isThreadSafe;
		size = 0;
		list = (T*)::malloc(roomCount * MCL_PTR_SIZE);
		
		if(isThreadSafe)
			::InitializeCriticalSection(&criticalSection);
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool addPointer(T pointer)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection); // thread safe!

		if(roomCount >= (size + 1) ) // no need reallocation. coz room count is enough!
		{
			list[size] = pointer;
			size++;

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return true;
		}
		else // require reallocation!
		{
			roomCount += roomIncrement;
			void* retVal = ::realloc((void*)list, roomCount * MCL_PTR_SIZE);
			if(retVal)
			{
				list = (T*)retVal;
				list[size] = pointer;
				size++;

				if(isThreadSafe)
					::LeaveCriticalSection(&criticalSection);

				return true;
			}
			else // memory allocation failed!
			{
				if(isThreadSafe)
					::LeaveCriticalSection(&criticalSection);

				return false;
			}
		}
	}

	/**
		Get pointer at id.
		@returns 0 if id is out of range!
	*/
	T getPointer(const int id)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if( (0 <= id) & (id < size) ) // checks for valid range!
		{	
			T object = list[id];

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return object;
		}
		else // out of range!
		{
			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return NULL;
		}
	}

	/**
		Get pointer at id.
		@returns 0 if id is out of range!
	*/
	T operator[](const int id)
	{
		return getPointer(id);
	}

	/**
		Replace pointer of given id with new pointer
		@returns false if id is out of range!
	*/
	bool setPointer(const int id, T pointer)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if( (0 <= id) & (id < size) )
		{	
			list[id] = pointer;

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return true;
		}
		else // out of range!
		{
			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return false;
		}
	}

	/**
		Remove pointer of given id
		@returns false if id is out of range!
	*/
	bool removePointer(const int id)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		if( (0 <= id) & (id < size) )
		{	
			const int newRoomCount = (((size - 1) / roomIncrement) + 1) * roomIncrement;
			T* newList = (T*)::malloc(newRoomCount * MCL_PTR_SIZE);

			for(int i = 0, j = 0; i < size; i++)
			{
				if(i != id)
				{
					newList[j] = list[i];
					j++;
				}	
			}
			::free((void*)list); // free old list!
			list = newList;
			roomCount = newRoomCount;
			size--;

			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return true;

		}
		else // out of range!
		{
			if(isThreadSafe)
				::LeaveCriticalSection(&criticalSection);

			return false;
		}

	}

	/**
		Clears the list!
	*/
	void removeAll(bool reallocate = true)// remove all pointers from list!
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		::free((void*)list);
		roomCount = roomIncrement;
		list = reallocate ? (T*)::malloc(roomCount * MCL_PTR_SIZE) : NULL;
		size = 0;

		if(isThreadSafe)
			::LeaveCriticalSection(&criticalSection);
	}

	/**
		Call destructors of all objects which are pointed by pointers in the list.
		Also clears the list.
	*/
	void deleteAll(bool reallocate = true)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		for(int i = 0; i < size; i++)
		{
			T object = list[i];
			delete object;
		}

		::free((void*)list);

		roomCount = roomIncrement;
		list = reallocate ? (T*)::malloc(roomCount * MCL_PTR_SIZE) : NULL;
		size = 0;

		if(isThreadSafe)
			::LeaveCriticalSection(&criticalSection);
	}

	/**
		Finds the id of the first pointer which matches the pointer passed in.
		@returns -1 if not found!
	*/
	int getID(T pointer)
	{
		if(isThreadSafe)
			::EnterCriticalSection(&criticalSection);

		for(int i = 0; i < size; i++)
		{
			if (list[i] == pointer)
			{
				if(isThreadSafe)
					::LeaveCriticalSection(&criticalSection);

				return i;
			}
		}

		if(isThreadSafe)
			::LeaveCriticalSection(&criticalSection);

		return -1;
	}

	/**
		@returns item count in the list
	*/
	int getSize()
	{
		return size;
	}

	/** Destructs PointerList object.*/
	~TPointerList()
	{
		if (list)
			::free((void*)list);

		if(isThreadSafe)
			::DeleteCriticalSection(&criticalSection);
	}

private:
	MCL_LEAK_DETECTOR(TPointerList)
};

// =========== TGraphics.h ===========

/*
	MCL - TGraphics.h
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

class TGraphics
{
public:
	TGraphics();

	virtual ~TGraphics();

	static void draw3dVLine(HDC hdc, int startX, int startY, int height);

	static void draw3dHLine(HDC hdc, int startX, int startY, int width);

	static void draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void fillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color);

	static void fillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color);

	static RECT calculateTextSize(const wchar_t *text, HFONT hFont);

private:
	MCL_LEAK_DETECTOR(TGraphics)
};

// =========== TString.h ===========

/*
	MCL - TString.h
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
#include <string.h>
#include <malloc.h>

/**
	Using a reference-counted internal representation, these strings are fast and efficient. <BR>
	TString was optimized to use with unicode strings. So, use unicode strings instead of ansi. <BR>
	TString does not support for multiple zero terminated strings. <BR>

	Optimization tips: <BR>
	use CONST_TXT macro when using statically typed text. <BR>
	use constructor instead of assignment (if can). <BR>
	use "append" method instead of "+" operator. <BR>
	use "appendStaticText" method instead of "append" if you are using statically typed text. <BR>
	use "compareWithStaticText" method instead of "compare" if you are comparing statically typed text. <BR>

	@code
	TString result1 = str1 + L"1234"; // slow
	TString result2 = str1 + CONST_TXT("1234"); // fast
	TString result3( str1 + CONST_TXT("1234") ); // more fast
	TString result4( str1.append(CONST_TXT("1234")) ); // bit more fast
	TString result5( str1.appendStaticText(TXT_WITH_LEN("1234")) ); // that's all you can have ;-)
	@endcode
*/
class TString
{
protected:
	mutable TStringHolder *stringHolder; // for empty string: stringHolder=0 && isStaticText=false
	bool isZeroLength; // true if empty string or staticText, stringHolder are zero length
	mutable bool isStaticText; // staticText & staticTextLength are valid only if this field is true. stringHolder can be zero even this filed is false.
	wchar_t *staticText;
	int staticTextLength;

	void convertToRefCountedStringIfStatic()const; // generates StringHolder object from static text

public:

	enum TextTypes
	{
		STATIC_TEXT_DO_NOT_FREE = 1,
		FREE_TEXT_WHEN_DONE = 2,
		USE_COPY_OF_TEXT = 3,
	};

	/**
		Constructs an empty string
	*/
	TString();

	/**
		Constructs copy of another string
	*/
	TString(const TString& other);

	/**
		Constructs String object using ansi string
	*/
	TString(const char* const text, UINT codePage = CP_UTF8);

	/**
		Constructs String object using unicode string
	*/
	TString(const wchar_t* const text, unsigned char behaviour = USE_COPY_OF_TEXT, int length = -1);

	/**
		Constructs String object using integer
	*/
	TString(const int value, const int radix = 10);

	/**
		Constructs String object using float
		@param compact   removes ending decimal zeros if true
	*/
	TString(const float value, const int numDecimals, bool compact = false);

	/** 
		Replaces this string's contents with another string.
	*/
	const TString& operator= (const TString& other);

	/** 
		Replaces this string's contents with unicode string. 
	*/
	const TString& operator= (const wchar_t* const other);


	/** 
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	const TString operator+ (const TString& stringToAppend);

	/** 
		Appends a unicode string at the end of this one.
		@returns     the concatenated string
	*/
	const TString operator+ (const wchar_t* const textToAppend);
	/**
		Returns ansi version of this string
	*/
	operator const char*()const;

	/**
		Returns const unicode version of this string
	*/
	operator const wchar_t*()const;

	/**
		Returns unicode version of this string
	*/
	operator wchar_t*()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	const char operator[](const int index)const;

	/**
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	TString append(const TString& otherString)const;

	/**
		Appends a statically typed string to begining or end of this one.
		@param text			statically typed text
		@param length		text length. should not be zero.
		@param appendToEnd	appends to begining if false
		@returns			the concatenated string
	*/
	TString appendStaticText(const wchar_t* const text, int length, bool appendToEnd = true)const;

	/**
		Assigns a statically typed string.
		@param text			statically typed text
		@param length		text length. should not be zero.
	*/
	void assignStaticText(const wchar_t* const text, int length);

	/** 
		Returns a subsection of the string.

		If the range specified is beyond the limits of the string, empty string
		will be return.

		@param start   the index of the start of the substring needed
		@param end     all characters from start up to this index are returned
	*/
	TString subString(int start, int end)const;

	/**
		Case-insensitive comparison with another string. Slower than "Compare" method.
		@returns     true if the two strings are identical, false if not
	*/
	bool compareIgnoreCase(const TString& otherString)const;

	/** 
		Case-sensitive comparison with another string.
		@returns     true if the two strings are identical, false if not
	*/
	bool compare(const TString& otherString)const;

	/** 
		Case-sensitive comparison with statically typed string.
		@param text		statically typed text.
		@returns		true if the two strings are identical, false if not
	*/
	bool compareWithStaticText(const wchar_t* const text)const;

	/**
		Compare first character with given unicode character
	*/
	bool startsWithChar(wchar_t character)const;

	/**
		Compare first character with given ansi character
	*/
	bool startsWithChar(char character)const;

	/**
		Compare last character with given unicode character
	*/
	bool endsWithChar(wchar_t character)const;

	/**
		Compare last character with given ansi character
	*/
	bool endsWithChar(char character)const;

	/**
		Check if string is quoted or not
	*/
	bool isQuotedString()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	wchar_t getCharAt(int index)const;

	/**
		Returns number of characters in string
	*/
	int getLength()const;

	/**
		Returns true if string is empty
	*/
	bool isEmpty()const;

	bool isNotEmpty()const;

	/**
		Returns value of string
	*/
	int getIntValue()const;

	/** 
		Returns an upper-case version of this string.
	*/
	TString toUpperCase()const;

	/** 
		Returns an lower-case version of this string. 
	*/
	TString toLowerCase()const;

	virtual ~TString();

private:
	MCL_LEAK_DETECTOR(TString)
};

const TString operator+ (const char* const string1, const TString& string2);

const TString operator+ (const wchar_t* const string1, const TString& string2);

const TString operator+ (const TString& string1, const TString& string2);

#define LEN_UNI_STR(X) (sizeof(X) / sizeof(wchar_t)) - 1

#define LEN_ANSI_STR(X) (sizeof(X) / sizeof(char)) - 1

// do not make a copy + do not free + do not calculate length
#define CONST_TXT(X) TString(L##X, TString::STATIC_TEXT_DO_NOT_FREE, LEN_UNI_STR(L##X))

// do not make a copy + do not free + calculate length
#define STATIC_TXT(X) TString(L##X, TString::STATIC_TEXT_DO_NOT_FREE, -1)

// do not make a copy + free when done + calculate length
#define BUFFER_TXT(X) TString(X, TString::FREE_TEXT_WHEN_DONE, -1)

// can be use like this: KString str(CONST_TXT_PARAMS("Hello World"));
#define CONST_TXT_PARAMS(X) L##X, TString::STATIC_TEXT_DO_NOT_FREE, LEN_UNI_STR(L##X)

#define TXT_WITH_LEN(X) L##X, LEN_UNI_STR(L##X)

// =========== TComponent.h ===========

/*
	MCL - TComponent.h
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

/**
	Base class of all W32 gui objects.
*/
class TComponent
{
protected:
	TString classNameProperty;
	TString textProperty;
	HWND handleProperty;
	HWND parentProperty;
	DWORD styleProperty;
	DWORD extendedStyleProperty;
	UINT controlID;
	int leftProperty;
	int topProperty;
	int widthProperty;
	int heightProperty;
	bool visibleProperty;
	bool enabledProperty;
	bool isRegistered;
	HFONT fontProperty;
	HCURSOR cursorProperty;

public:
	WNDCLASSEXW wc;

	PROPERTY_DEF_READONLY(TString, className, getClassName)
	PROPERTY_DEF_READONLY(HWND, handle, getHandle)
	PROPERTY_DEF(TString, text, getText, setText)
	PROPERTY_DEF(HWND, parent, getParent, setParent)
	PROPERTY_DEF(DWORD, style, getStyle, setStyle)
	PROPERTY_DEF(DWORD, extendedStyle, getExtendedStyle, setExtendedStyle)
	PROPERTY_DEF(int, left, getLeft, setLeft)
	PROPERTY_DEF(int, top, getTop, setTop)
	PROPERTY_DEF(int, width, getWidth, setWidth)
	PROPERTY_DEF(int, height, getHeight, setHeight)
	PROPERTY_DEF(bool, visible, getVisible, setVisible)
	PROPERTY_DEF(bool, enabled, getEnabled, setEnabled)
	PROPERTY_DEF(HFONT, font, getFont, setFont)
	PROPERTY_DEF(HCURSOR, cursor, getCursor, setCursor)

	// LRESULT windowProc(TMessage& message)
	EVENT_DEF(LRESULT, windowProc, TMessage&)

	// bool notifyProc(TMessage& message, LRESULT& result)
	EVENT_DEF(bool, notifyProc, TMessage&, LRESULT&)

	// void onCreate(TComponent* sender)
	EVENT_DEF(void, onCreate, TComponent*)

	// void onDestroy(TComponent* sender)
	EVENT_DEF(void, onDestroy, TComponent*)

	// void onMouseDown(TComponent* sender, int mouseButton, int x, int y)
	EVENT_DEF(void, onMouseDown, TComponent*, int, int, int)

	// void onMouseUp(TComponent* sender, int mouseButton, int x, int y)
	EVENT_DEF(void, onMouseUp, TComponent*, int, int, int)

	// void onMouseMove(TComponent* sender, int x, int y)
	EVENT_DEF(void, onMouseMove, TComponent*, int, int)

	// void onMouseLeave(TComponent* sender)
	EVENT_DEF(void, onMouseLeave, TComponent*)

	// void onMouseWheel(TComponent* sender, int wheelDelta, int x, int y)
	EVENT_DEF(void, onMouseWheel, TComponent*, int, int, int)

	// void onDblClick(TComponent* sender, int mouseButton, int x, int y)
	EVENT_DEF(void, onDblClick, TComponent*, int, int, int)

	// void onKeyPress(TComponent* sender, char key)
	EVENT_DEF(void, onKeyPress, TComponent*, char)

	// void onKeyDown(TComponent* sender, short virtualKeyCode)
	EVENT_DEF(void, onKeyDown, TComponent*, short)

	// void onKeyUp(TComponent* sender, short virtualKeyCode)
	EVENT_DEF(void, onKeyUp, TComponent*, short)

	/**
		Constructs a standard win32 component.
	*/
	TComponent();
	
	/**
		Returns HWND of this component
	*/
	operator HWND()const;

	/**
		Sets mouse cursor of this component.
	*/
	virtual void setCursor(HCURSOR cursorHandle);

	virtual HCURSOR getCursor();

	/**
		@returns class name(autogenerated unique) for this component
	*/
	virtual TString getClassName();

	/**
		Registers the class name and creates the component. 
		@returns false if registration failed or component creation failed.
	*/
	virtual bool create();

	virtual void destroy();

	/**
		Handles internal window messages. (subclassed window proc)
		Important: Pass unprocessed messages to parent if you override this method.
	*/
	virtual LRESULT windowProcHandler(TMessage& message);

	/**
		Receives notification messages like WM_COMMAND, WM_NOTIFY, WM_DRAWITEM from the parent window. (if it belongs to this component)
		Pass unprocessed messages to parent if you override this method.
		@returns true if msg processed.
	*/
	virtual bool notifyProcHandler(TMessage& message, LRESULT& result);

	/**
		send unprocessed messages into default window proc
	*/
	virtual LRESULT dispatchToDefaultProc(TMessage& message);

	/**
		Identifier of the child component which can be used with WM_MEASUREITEM like messages.
		@returns zero for top level windows
	*/
	virtual UINT getControlID();

	/**
		Sets font of this component
	*/
	virtual void setFont(HFONT fontHandle);

	virtual HFONT getFont();

	/**
		Returns caption of this component
	*/
	virtual TString getText();

	/**
		Sets caption of this component
	*/
	virtual void setText(const TString& caption);

	/**
		For internal use only!
	*/
	virtual void setHandle(HWND hwnd);

	/**
		Returns HWND of this component
	*/
	virtual HWND getHandle();

	/**
		Changes parent of this component
	*/
	virtual void setParent(HWND parentHandle);

	/**
		Returns parent of this component
	*/
	virtual HWND getParent();

	/**
		Returns style of this component
	*/
	virtual DWORD getStyle();

	/**
		Sets style of this component
	*/
	virtual void setStyle(DWORD compStyle);

	/**
		Returns extended style of this component
	*/
	virtual DWORD getExtendedStyle();

	/**
		Sets extended style of this component
	*/
	virtual void setExtendedStyle(DWORD compExStyle);

	/**
		Returns x position of this component which is relative to parent component.
	*/
	virtual int getLeft();

	/**
		Returns y position of this component which is relative to parent component.
	*/
	virtual int getTop();

	/**
		Sets x position of the component.
	*/
	virtual void setLeft(int compLeft);

	/**
		Sets y position of the component.
	*/
	virtual void setTop(int compTop);

	virtual void setPosition(int compLeft, int compTop);

	/**
		Returns width of the component.
	*/
	virtual int getWidth();

	/**
		Returns height of the component.
	*/
	virtual int getHeight();

	/**
		Sets width of the component.
	*/
	virtual void setWidth(int compWidth);

	/**
		Sets height of the component.
	*/
	virtual void setHeight(int compHeight);

	virtual void setSize(int compWidth, int compHeight);

	/**
		Sets visible state of the component
	*/
	virtual void setVisible(bool state);

	/**
		Returns visible state of the component
	*/
	virtual bool getVisible();

	virtual void show();

	virtual void hide();

	/**
		Returns the component is ready for user input or not
	*/
	virtual bool getEnabled();

	/**
		Sets component's user input reading state
	*/
	virtual void setEnabled(bool state);

	/**
		Brings component to front
	*/
	virtual void bringToFront();

	/**
		Grabs keyboard focus into this component
	*/
	virtual void grabKeyboardFocus();

	/**
		Repaints the component
	*/
	virtual void repaint();

	virtual ~TComponent();

private:
	MCL_LEAK_DETECTOR(TComponent)
};

// =========== TDirectory.h ===========

/*
	MCL - TDirectory.h
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
#include <shlobj.h>

/**
Can be use to manipulate dirs.
*/
class TDirectory
{
public:
	TDirectory();

	static bool isDirExists(const TString& dirName);

	/**
		returns false if directory already exists.
	*/
	static bool createDir(const TString& dirName);

	/**
		deletes an existing empty directory.
	*/
	static bool removeDir(const TString& dirName);

	/**
		returns the directory of given module. if HModule is NULL this function will return dir of exe.
		returns empty string on error.
	*/
	static TString getModuleDir(HMODULE hModule);

	/**
		returns the parent directory of given file.
	*/
	static TString getParentDir(const TString& filePath);

	/**
		returns the the directory for temporary files.
		returns empty string on error.
	*/
	static TString getTempDir();

	/**
		returns the the Application Data directory. if isAllUsers is true this function will return dir shared across all users.
		returns empty string on error.
	*/
	static TString getApplicationDataDir(bool isAllUsers = false);

	virtual ~TDirectory();

private:
	MCL_LEAK_DETECTOR(TDirectory)
};

// =========== TWindow.h ===========

/*
	MCL - TWindow.h
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


class TWindow : public TComponent
{
protected:
	HWND lastFocusedChild;

public:
	TWindow();

	// change the owner of this window
	virtual void setParent(HWND parentHandle) override;

	virtual LRESULT windowProcHandler(TMessage& message) override;

	virtual ~TWindow();
};

// =========== TControl.h ===========

/*
	MCL - TControl.h
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


/**
	Base class of all W32 child components
*/
class TControl : public TComponent
{
public:
	TControl()
	{
		leftProperty = 0;
		topProperty = 0;

		styleProperty = WS_CHILD | WS_CLIPSIBLINGS;
	}

	// control will create itself when you assign parent to it!
	virtual void setParent(HWND parentHandle) override
	{
		parentProperty = parentHandle;
		this->create();
	}
};

// =========== TButton.h ===========

/*
	MCL - TButton.h
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


class TButton : public TControl
{
public:

	// void onPress(TComponent* sender)
	EVENT_DEF(void, onPress, TComponent*)

	TButton();

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual bool create() override;

	virtual ~TButton();
};

// =========== TCheckBox.h ===========

/*
	MCL - TCheckBox.h
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


class TCheckBox : public TControl
{
protected:
	bool valueProperty;

public:

	PROPERTY_DEF(bool, value, getValue, setValue)

	// void onCheck(TComponent* sender)
	EVENT_DEF(void, onCheck, TComponent*)

	TCheckBox();

	virtual bool create() override;

	virtual bool getValue();

	virtual void setValue(bool state);

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual ~TCheckBox();
};

// =========== TFile.h ===========

/*
	MCL - TFile.h
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

/**
	Can be use to read/write data from a file easily.
*/
class TFile
{
protected:
	TString fileName;
	HANDLE fileHandle;
	bool autoCloseHandle;
	DWORD desiredAccess;

public:
	TFile();

	/** 
		Used in file opening, to specify whether to open as read or write or both.
	*/
	enum FileAccessTypes
	{
		TREAD = GENERIC_READ,
		TWRITE = GENERIC_WRITE,
		TBOTH = GENERIC_READ | GENERIC_WRITE,
	};

	/** 
		If the file does not exist, it will be created.
	*/
	TFile(const TString& fileName, DWORD desiredAccess = TFile::TBOTH, bool autoCloseHandle = true);

	/** 
		If the file does not exist, it will be created.
	*/
	virtual bool openFile(const TString& fileName, DWORD desiredAccess = TFile::TBOTH, bool autoCloseHandle = true);

	virtual bool closeFile();

	virtual HANDLE getFileHandle();

	operator HANDLE()const;

	/** 
		fills given buffer and returns number of bytes read.
	*/
	virtual DWORD readFile(void* buffer, DWORD numberOfBytesToRead);

	/** 
		You must free the returned buffer yourself. To get the size of buffer, use GetFileSize method. return value will be null on read error.
	*/
	virtual void* readAsData();

	/**
		auto detects unicode & ansi encodings.
	*/
	virtual TString readAsString();

	/**
		returns number of bytes written.
	*/
	virtual DWORD writeFile(void* buffer, DWORD numberOfBytesToWrite);

	virtual bool writeString(const TString& text, bool isUnicode = true);

	virtual bool setFilePointerToStart();

	/**
		moves file pointer to given distance from "startingPoint".
		"startingPoint" can be FILE_BEGIN, FILE_CURRENT or FILE_END
		"distance" can be negative.
	*/
	virtual bool setFilePointerTo(long distance, DWORD startingPoint = FILE_BEGIN);

	virtual DWORD getFilePointerPosition();

	virtual bool setFilePointerToEnd();

	/**
		returns zero on error
	*/
	virtual DWORD getFileSize();

	static bool deleteFile(const TString& fileName);

	static bool isFileExists(const TString& fileName);

	static bool copyFile(const TString& sourceFileName, const TString& destFileName);

	virtual ~TFile();

private:
	MCL_LEAK_DETECTOR(TFile)
};

// =========== TSettingsReader.h ===========

/*
	MCL - TSettingsReader.h
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

/**
	High performance configuration reading class.
*/
class TSettingsReader
{
protected:
	TFile settingsFile;

public:
	TSettingsReader();

	virtual bool openFile(const TString& fileName, int formatID);

	/**
		read struct, array or whatever...
	*/
	virtual void readData(DWORD size, void* buffer);

	virtual TString readString();

	virtual int readInt();

	virtual float readFloat();

	virtual double readDouble();

	virtual bool readBool();

	virtual ~TSettingsReader();

private:
	MCL_LEAK_DETECTOR(TSettingsReader)
};

// =========== TSettingsWriter.h ===========

/*
	MCL - TSettingsWriter.h
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

/**
	High performance configuration writing class.
*/
class TSettingsWriter
{
protected:
	TFile settingsFile;

public:
	TSettingsWriter();

	virtual bool openFile(const TString& fileName, int formatID);

	/**
		save struct, array or whatever...
	*/
	virtual void writeData(DWORD size, void* buffer);

	virtual void writeString(const TString& text);

	virtual void writeInt(int value);

	virtual void writeFloat(float value);

	virtual void writeDouble(double value);

	virtual void writeBool(bool value);

	virtual ~TSettingsWriter();

private:
	MCL_LEAK_DETECTOR(TSettingsWriter)
};

// =========== TInternet.h ===========

/*
	MCL - TInternet.h
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
#include <winhttp.h>

class TInternet
{
public:

	TInternet();

	/**
		this method automatically applies the browser proxy settings if available.
		no need to use win8.1 only WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY flag when calling WinHttpOpen.
	*/
	static void applyProxySettings(const wchar_t* url, HINTERNET hInternet);

	/**
		convert string into url format.
	*/
	static TString urlEncodeString(const TString& text);

	/**
		converts url formatted string into regular string.
	*/
	static TString urlDecodeString(const TString& text);

	/**
		this method posts data to given url. post data must be in url enocoded format.
		url is domain name without "http://" prefix.
		objectName is prefixed with "/".
		postData should be like this "data1=Hello&data2=World".
		ignoreCertificateErros parameter will be ignored if isHttps is false.
		this method automatically applies the browser proxy settings if available.
	*/
	static TString postText(const wchar_t* url,
		const wchar_t* objectName,
		const bool isHttps,
		const char* postData,
		const int postDataLength,
		const bool ignoreCertificateErros = true,
		const wchar_t* userAgent = L"MCL Application/1.0");

	virtual ~TInternet();

private:
	MCL_LEAK_DETECTOR(TInternet)
};

// =========== TMD5.h ===========

/*
	MCL - TMD5.h
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


class TMD5
{
public:
	TMD5();

	static TString generateFromString(const TString& text);

	static TString generateFromFile(const TString& fileName);

	virtual ~TMD5();

private:
	MCL_LEAK_DETECTOR(TMD5)
};

// =========== TSHA1.h ===========

/*
	MCL - TSHA1.h
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


class TSHA1
{
public:
	TSHA1();

	static TString generateFromString(const TString& text);

	static TString generateFromFile(const TString& fileName);

	virtual ~TSHA1();

private:
	MCL_LEAK_DETECTOR(TSHA1)
};

// =========== TTimer.h ===========

/*
	MCL - TTimer.h
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

/**
	Encapsulates a timer.

	The timer can be started with the startTimer() method
	and controlled with various other methods. Before you start timer, you must set 
	timer window by calling setTimerWindow method.
*/
class TTimer
{
protected:
	UINT timerID;
	int resolution;
	bool started;
	TWindow *window;

public:

	// void onTimer(TTimer* timer)
	EVENT_DEF(void, onTimer, TTimer*)

	TTimer();

	/**
		@param resolution timer interval
	*/
	virtual void setInterval(int resolution);

	virtual int getInterval();

	/**
		Call this method before you start the timer
	*/
	virtual void setTimerWindow(TWindow *window);

	virtual void setTimerID(UINT timerID);

	/**
		@returns unique id of this timer
	*/
	virtual UINT getTimerID();

	/**
		Starts timer
	*/
	virtual void startTimer();

	/**
		Stops the timer. You can restart it by calling startTimer() method.
	*/
	virtual void stopTimer();

	virtual bool isTimerRunning();

	virtual ~TTimer();

private:
	MCL_LEAK_DETECTOR(TTimer)
};

// =========== TGuid.h ===========

/*
	MCL - TGuid.h
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

class TGuid
{
public:

	TGuid(){}

	static bool generateGUID(GUID *pGUID)
	{
		return (::CoCreateGuid(pGUID) == S_OK);
	}

	static TString generateGUID()
	{
		GUID guid;

		if (TGuid::generateGUID(&guid))
			return TGuid::guidToString(&guid);

		return TString();
	}

	static TString guidToString(GUID *pGUID)
	{
		wchar_t* strGuid = NULL;
		::UuidToStringW(pGUID, (RPC_WSTR*)&strGuid);

		TString result(strGuid, TString::USE_COPY_OF_TEXT);
		RpcStringFreeW((RPC_WSTR*)&strGuid);

		return result;
	}

	virtual ~TGuid(){}

private:
	MCL_LEAK_DETECTOR(TGuid)
};

// =========== TApplication.h ===========

/*
	MCL - TApplication.h
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

/**
	Derive your application object from this class!
	Override Main method and put your application code there.
	Use START_APPLICATION macro somewhere in a cpp file to declare an instance of this class.

	e.g. @code
	class MyApp : public TApplication
	{
		public:
			MyApp(){}
			~MyApp(){}

			int main(TString** argv, int argc)
			{
				// your app code goes here...
				return 0;
			}
	};

	START_APPLICATION(MyApp)
	@endcode
*/
class TApplication
{
public:

	/**
		Use this field to get HINSTANCE of your application.
		(This field will set when calling InitMCL function.)
	*/
	static HINSTANCE hInstance;

	/** 
		Constructs an TApplication object.
	*/
	TApplication();

	/** 
		Called when the application starts.
		Put your application code here and if you create a window, 
		then make sure to call DoMessagePump method before you return.

		@param argv array of command-line arguments! access them like this TString* arg1=argv[0];
		@param argc number of arguments
	*/
	virtual int main(TString** argv, int argc);

	/**
		Return false if your application is single instance only.
		Single instance applications must implement "GetApplicationID" method.
	*/
	virtual bool allowMultipleInstances();

	/**
		This method will be called if the application is single instance only and another instance is already running.
		("Main" method will not be called.)
	*/
	virtual int anotherInstanceIsRunning(TString** argv, int argc);

	/**
		Unique id of your application which is limited to MAX_PATH characters.
		Single instance applications must implement this method.
	*/
	virtual const wchar_t* getApplicationID();

	static void run(bool handleTabKey = true);

	static void shouldQuit();

	/** 
		Destructs an Application object.
	*/
	virtual ~TApplication();

private:
	MCL_LEAK_DETECTOR(TApplication)
};

// =========== TToolTip.h ===========

/*
	MCL - TToolTip.h
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


class TToolTip : public TControl
{
protected:
	HWND attachedControl;

public:
	TToolTip();

	virtual ~TToolTip();

	/**
		parentWindow must be created before you call this method.
		attachedControl must be created before you call this method.
		do not attach same tooltip into multiple controls.
	*/
	virtual void attachToControl(TWindow *parentWindow, TControl *attachedControl);

	/**
		calling this method has no effect.
	*/
	virtual bool create() override;

	virtual void setText(const TString& caption) override;
};



// =========== TBitmap.h ===========

/*
	MCL - TBitmap.h
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

/**
	Can be use to load bitmap image from file or resource.
*/
class TBitmap
{
protected:
	HBITMAP hBitmap;

public:
	TBitmap();

	/**
		Loads bitmap image from resource
		@param resourceID resource ID of image
		@returns false if image load fails
	*/
	bool loadFromResource(WORD resourceID);

	/**
		Loads bitmap image from file
		@param filePath path to image
		@returns false if image load fails
	*/
	bool loadFromFile(const TString& filePath);

	void drawOnHDC(HDC hdc, int x, int y, int width, int height);

	/**
		Returns bitmap handle
	*/
	HBITMAP getHandle();

	/**
		Returns bitmap handle
	*/
	operator HBITMAP()const;

	virtual ~TBitmap();

private:
	MCL_LEAK_DETECTOR(TBitmap)
};



// =========== TCursor.h ===========

/*
	MCL - TCursor.h
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

/**
	Can be use to load cursor from file or resource.
*/
class TCursor
{
protected:
	HCURSOR hCursor;

public:
	TCursor();

	/**
		Loads cursor from resource
		@param resourceID resource ID of cursor file
		@returns false if cursor load fails
	*/
	bool loadFromResource(WORD resourceID);

	/**
		Loads cursor from file
		@param filePath path to cursor file
		@returns false if cursor load fails
	*/
	bool loadFromFile(const TString& filePath);

	/**
		Returns cursor handle
	*/
	HCURSOR getHandle();

	/**
		Returns cursor handle
	*/
	operator HCURSOR()const;

	virtual ~TCursor();

private:
	MCL_LEAK_DETECTOR(TCursor)
};

// =========== TListBox.h ===========


/*
	MCL - TListBox.h
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


class TListBox : public TControl
{
protected:
	TPointerList<TString*> *stringList;
	int selectedItemIndex;
	int selectedItemEnd;
	bool multipleSelection;

public:

	// void onItemSelect(TComponent* sender, int selectedItemIndex)
	EVENT_DEF(void, onItemSelect, TComponent*, int)

	// void onItemDoubleClick(TComponent* sender, int selectedItemIndex)
	EVENT_DEF(void, onItemDoubleClick, TComponent*, int)

	TListBox(bool multipleSelection = false, bool sort = false, bool vscroll = true);

	virtual void addItem(const TString& text);

	virtual void removeItem(int index);

	virtual void removeItem(const TString& text);

	virtual int getItemIndex(const TString& text);

	virtual int getItemCount();

	virtual int getSelectedItemIndex();

	virtual TString getSelectedItem();

	virtual int getSelectedItems(int* itemArray, int itemCountInArray);

	virtual void clearList();

	virtual void selectItem(int index);

	virtual void selectItems(int start, int end);

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual bool create() override;

	virtual ~TListBox();
};

// =========== TFont.h ===========

/*
	MCL - TFont.h
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

/**
	Can be use to create system default font or custom font. Once created, you cannot change font properties.
*/
class TFont
{
protected:
	HFONT hFont;

public:
	/**
		Constructs empty Font object.
	*/
	TFont();

	/**
		Constructs custom font object.
	*/
	TFont(const TString& face, int size = 14, bool bold = false, bool italic = false, bool antiAliased=false);

	bool loadFont(const TString& face, int size, bool bold, bool italic, bool antiAliased);

	/**
		If you want to use system default font, then use this static method. Do not delete returned handle!
	*/
	static HFONT getDefaultFont();

	/**
		Loads font from a file. make sure to call RemoveFont when done.
	*/
	static bool loadFont(const TString& path);

	static void removeFont(const TString& path);

	/**
		Returns font handle.
	*/
	virtual HFONT getFontHandle();

	operator HFONT()const;

	virtual ~TFont();

private:
	MCL_LEAK_DETECTOR(TFont)
};

// =========== TRegistry.h ===========

/*
    MCL - TRegistry.h
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

class TRegistry
{

public:
	TRegistry();

	// returns true on success or if the key already exists.
	static bool createKey(HKEY hKeyRoot, const TString& subKey);

	// the subkey to be deleted must not have subkeys. 
	static bool deleteKey(HKEY hKeyRoot, const TString& subKey);

	static bool readString(HKEY hKeyRoot, const TString& subKey, const TString& valueName, TString* result);

	static bool writeString(HKEY hKeyRoot, const TString& subKey, const TString& valueName, const TString& value);

	static bool readDWORD(HKEY hKeyRoot, const TString& subKey, const TString& valueName, DWORD* result);

	static bool writeDWORD(HKEY hKeyRoot, const TString& subKey, const TString& valueName, DWORD value);

	/**
		you must free the buffer when you are done with it.

		e.g. @code
		void* buffer;
		DWORD bufferSize;
		if(TRegistry::ReadBinary(xxx, xxx, xxx, &buffer, &buffSize))
		{
			// do your thing here...

			free(buffer);
		}
		@endcode
	*/
	static bool readBinary(HKEY hKeyRoot, const TString& subKey, const TString& valueName, void** buffer, DWORD* buffSize);

	static bool writeBinary(HKEY hKeyRoot, const TString& subKey, const TString& valueName, void* buffer, DWORD buffSize);

	virtual ~TRegistry();

};

// =========== TIcon.h ===========

/*
	MCL - TIcon.h
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

/**
	Can be use to load icon from file or resource.
*/
class TIcon
{
protected:
	HICON hIcon;

public:
	TIcon();

	/**
		Loads icon from resource
		@param resourceID resource ID of icon file
		@returns false if icon load fails
	*/
	bool loadFromResource(WORD resourceID);

	/**
		Loads icon from file
		@param filePath path to icon file
		@returns false if icon load fails
	*/
	bool loadFromFile(const TString& filePath);

	/**
		Returns icon handle
	*/
	HICON getHandle();

	operator HICON()const;

	virtual ~TIcon();

private:
	MCL_LEAK_DETECTOR(TIcon)
};

// =========== TComboBox.h ===========

/*
	MCL - TComboBox.h
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


class TComboBox : public TControl
{
protected:
	TPointerList<TString*> *stringList;
	int itemIndexProperty;

public:

	PROPERTY_DEF(int, itemIndex, getSelectedItemIndex, selectItem)

	PROPERTY_DEF_READONLY(int, itemCount, getItemCount)

	// void onChange(TComponent* sender)
	EVENT_DEF(void, onChange, TComponent*)

	TComboBox(bool sort = false);

	virtual void add(const TString& text);

	virtual void remove(int index);

	virtual void removeByText(const TString& text);

	virtual int getItemIndexByText(const TString& text);

	virtual TString getItemTextByIndex(int index);

	virtual int getItemCount();

	virtual int getSelectedItemIndex();

	virtual TString getSelectedItem();

	virtual void clear();

	virtual void selectItem(int index);

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual bool create() override;

	virtual ~TComboBox();
};

// =========== TMenuItem.h ===========

/*
	MCL - TMenuItem.h
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

class TMenuItem
{
protected:
	HMENU hMenu;
	UINT itemID;
	TString textProperty;
	bool enabled;
	bool checked;

public:

	PROPERTY_DEF(TString, text, getText, setText)

	// void onPress(TMenuItem* sender)
	EVENT_DEF(void, onPress, TMenuItem*)

	TMenuItem();

	virtual void addToMenu(HMENU hMenu);

	virtual bool isChecked();

	virtual void setCheckedState(bool state);

	virtual bool isEnabled();

	virtual void setEnabled(bool state);

	virtual void setText(const TString& text);

	virtual TString getText();

	virtual UINT getItemID();

	virtual HMENU getMenuHandle();

	virtual ~TMenuItem();

private:
	MCL_LEAK_DETECTOR(TMenuItem)
};

// =========== TLogger.h ===========

/*
	MCL - TLogger.h
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

/**
	Super fast logging class for logging within a (audio)loop. (not thread safe)
	Writes logging data into ram & dumps data into file when needed.
	You can use this class instead of OutputDebugString API.(OutputDebugString is too slow & ETW is too complex?)
	Use Log Viewer tool to view generated log file.

	Log File Format:
		file header:				'T' 'L' 'O' 'G'
		event count:				int32					; (event count)
		event start packet format:	byte|short16|short16	; (event type|secs|mills)
		event param number format:	byte|data				; (param type|data)
		event param string format:	byte|byte|data			; (param type|data size[max 255]|data)
		event end packet format:	byte					; (EVT_END)
*/
class TLogger
{
protected:
	DWORD bufferSize;
	DWORD bufferIndex;
	unsigned int totalMills;
	unsigned int totalEvents;
	char *buffer;
	bool bufferFull, isFirstCall;
	TPerformanceCounter pCounter;

public:

	enum ByteSizes
	{
		SZ_MEGABYTE = 1024 * 1024,
		SZ_KILOBYTE = 1024,
	};

	enum EventTypes
	{
		EVT_END = 0,
		EVT_INFORMATION = 1,
		EVT_WARNING = 2,
		EVT_ERROR = 3,
	};

	enum ParamTypes
	{
		// skipped value zero. because parser will fail to recognize EVT_END.
		PARAM_STRING = 1,
		PARAM_INT32 = 2,
		PARAM_SHORT16 = 3,
		PARAM_FLOAT = 4,
		PARAM_DOUBLE = 5,
	};

	TLogger(DWORD bufferSize = (SZ_MEGABYTE * 10));

	virtual bool writeNewEvent(unsigned char eventType = EVT_INFORMATION);

	virtual bool endEvent();

	/**
		textLength is number of chars. max value is 255.
	*/
	virtual bool addTextParam(const char *text, unsigned char textLength);

	virtual bool addIntParam(int value);

	virtual bool addShortParam(unsigned short value);

	virtual bool addFloatParam(float value);
	
	virtual bool addDoubleParam(double value);

	virtual bool isBufferFull();

	virtual bool writeToFile(const TString& filePath);

	virtual ~TLogger();

private:
	MCL_LEAK_DETECTOR(TLogger)
};

// =========== TMenu.h ===========

/*
	MCL - TMenu.h
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


class TMenu
{
protected:
	HMENU hMenu;

public:
	TMenu();

	virtual void add(TMenuItem& menuItem);

	virtual void addSubMenu(const TString& text, const TMenu& menu);

	virtual void addSeperator();

	virtual HMENU getMenuHandle();

	operator HMENU()const;

	virtual void popUpMenu(TWindow& window);

	virtual ~TMenu();

private:
	MCL_LEAK_DETECTOR(TMenu)
};

// =========== TOpenDialog.h ===========

/*
	MCL - TOpenDialog.h
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


/**
	Filter string should be like this "Text Files (*.txt)|*.txt"
	Multiple filters can be write like this "Text Files (*.txt)|*.txt|All Files (*.*)|*.*"
	"settingsGUID" is valid only if "saveLastLocation" is true.
*/
class TOpenDialog
{
protected:
	virtual bool showDialog(OPENFILENAMEW* ofn)
	{
		return ( ::GetOpenFileNameW(ofn) == TRUE );
	}

public:

	TString title;
	TString filter;
	TString fileName;
	int filterIndex;
	bool saveLastLocation;
	TString settingsGUID;

	const wchar_t* const MCL_OSD_REG_LOCATION = L"Software\\CrownSoft\\MCL\\OSD";

	TOpenDialog()
	{
		title = L"Open File...";
		filter = L"All Files (*.*)|*.*";
		filterIndex = 0;
		saveLastLocation = false;
	}

	virtual ~TOpenDialog()
	{

	}

	virtual bool show(TWindow& window)
	{
		// assumes MAX_PATH * 2 is enough!	
		wchar_t *buff = (wchar_t*)::malloc((MAX_PATH * 2) * sizeof(wchar_t));
		buff[0] = 0;

		OPENFILENAMEW ofn;
		::ZeroMemory(&ofn, sizeof(OPENFILENAMEW));

		TString lastLocation;
		if (saveLastLocation)
		{
			TRegistry::readString(HKEY_CURRENT_USER, MCL_OSD_REG_LOCATION, settingsGUID, &lastLocation);

			if (lastLocation.getLength() > 0)
				ofn.lpstrInitialDir = (const wchar_t*)lastLocation;
		}

		// replace pipe character with null
		wchar_t* strFilter = _wcsdup(filter.appendStaticText(TXT_WITH_LEN("|")));
		const int strFilterLength = filter.getLength() + 1; // +1 appended pipe character
		for (int i = 0; i < strFilterLength; ++i)
		{
			if (strFilter[i] == L'|')
				strFilter[i] = L'\0';
		}

		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.hwndOwner = (window != NULL) ? window : NULL;
		ofn.lpstrFilter = strFilter;
		ofn.lpstrFile = buff;
		ofn.nMaxFile = MAX_PATH * 2;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
		ofn.lpstrTitle = title;
		ofn.nFilterIndex = filterIndex;

		if (this->showDialog(&ofn))
		{
			fileName = TString(buff, TString::FREE_TEXT_WHEN_DONE);
			filterIndex = ofn.nFilterIndex;

			if (saveLastLocation)
			{
				TString parentDir(TDirectory::getParentDir(fileName).appendStaticText(L"\\", 1, true));

				TRegistry::createKey(HKEY_CURRENT_USER, MCL_OSD_REG_LOCATION);	// if not exists
				TRegistry::writeString(HKEY_CURRENT_USER, MCL_OSD_REG_LOCATION, settingsGUID, parentDir);
			}

			::free(strFilter);
			return true;
		}
		else
		{
			::free(buff);
			::free(strFilter);
			return false;
		}
	}

};

// =========== TPlatformUtil.h ===========

/*
	MCL - TPlatformUtil.h
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
#include <stdio.h>

/**
	Singleton class which can be use to generate class names, timer ids etc...
	(this class is for internal use)
*/
class TPlatformUtil
{
private:
	MCL_LEAK_DETECTOR(TPlatformUtil)

	static TPlatformUtil *_instance;
	TPlatformUtil();

protected:
	volatile int classCount;
	volatile int timerCount;
	volatile int controlCount;
	volatile UINT menuItemCount;

	TPointerList<TMenuItem*> *menuItemList;
	TPointerList<TTimer*> *timerList;

	CRITICAL_SECTION criticalSectionForCount;

public:

	static TPlatformUtil* getInstance();

	UINT generateControlID();

	// TApplication:hInstance must be valid before calling this method
	TString generateClassName();

	UINT generateMenuItemID(TMenuItem *menuItem);
	TMenuItem* getMenuItemByID(UINT id);

	UINT generateTimerID(TTimer *timer);
	TTimer* getTimerByID(UINT id);

	~TPlatformUtil();

};

// =========== TGroupBox.h ===========

/*
	MCL - TGroupBox.h
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


class TGroupBox : public TControl
{
public:
	TGroupBox();

	virtual bool create() override;

	virtual ~TGroupBox();
};

// =========== TLabel.h ===========

/*
	MCL - TLabel.h
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


class TLabel : public TControl
{
public:
	TLabel();

	virtual bool create() override;

	virtual ~TLabel();
};

// =========== TPushButton.h ===========

/*
	MCL - TPushButton.h
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


class TPushButton : public TCheckBox
{
public:
	TPushButton();

	virtual ~TPushButton();
};

// =========== TRadioButton.h ===========

/*
	MCL - TRadioButton.h
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


class TRadioButton : public TCheckBox
{
public:
	TRadioButton();

	virtual ~TRadioButton();
};

// =========== TSaveDialog.h ===========

/*
	MCL - TSaveDialog.h
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


class TSaveDialog : TOpenDialog
{
protected:
	virtual bool showDialog(OPENFILENAMEW* ofn)
	{
		return (::GetSaveFileNameW(ofn) == TRUE);
	}

public:

	TSaveDialog()
	{

	}

	virtual ~TSaveDialog()
	{

	}
};

// =========== TSlider.h ===========

/*
	MCL - TSlider.h
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


class TSlider : public TControl
{
protected:
	int rangeMin;
	int rangeMax;
	int positionProperty;

public:

	PROPERTY_DEF(int, position, getPosition, setPosition)

	PROPERTY_DEF_WRITEONLY(bool, showTicks, setEnableTicks)

	PROPERTY_DEF_WRITEONLY(bool, vertical, setOrientation)

	// void onChange(TComponent* sender, int position)
	EVENT_DEF(void, onChange, TComponent*, int position)

	TSlider();

	virtual void setEnableTicks(bool enableTicks);

	virtual void setOrientation(bool vertical);

	/**
		Range between 0 to 100
	*/
	virtual void setRange(int min, int max);

	virtual void setPosition(int pos);

	virtual int getPosition();

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual bool create() override;

	virtual ~TSlider();
};



// =========== TGridView.h ===========

/*
	MCL - TGridView.h
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


class TGridView : public TControl
{
protected:
	int colCount;
	int itemCount;

public:

	// void onItemSelect(TComponent* sender, int selectedRow)
	EVENT_DEF(void, onItemSelect, TComponent*, int)

	// void onItemRightClick(TComponent* sender, int selectedRow)
	EVENT_DEF(void, onItemRightClick, TComponent*, int)

	// void onItemDoubleClick(TComponent* sender, int selectedRow)
	EVENT_DEF(void, onItemDoubleClick, TComponent*, int)

	TGridView(bool sortItems = false);

	virtual void insertRecord(TString **columnsData);

	virtual void insertRecordTo(int rowIndex, TString **columnsData);

	virtual TString getRecordAt(int rowIndex, int columnIndex);

	/**
		returns -1 if nothing selected.
	*/
	virtual int getSelectedRow();

	virtual void removeRecordAt(int rowIndex);

	virtual void removeAll();

	virtual void updateRecordAt(int rowIndex, int columnIndex, const TString& text);

	virtual void setColumnWidth(int columnIndex, int columnWidth);

	virtual int getColumnWidth(int columnIndex);

	virtual void createColumn(const TString& text, int columnWidth = 100);

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual bool create() override;

	virtual ~TGridView();
};

// =========== TProgressBar.h ===========

/*
	MCL - TProgressBar.h
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


class TProgressBar : public TControl
{
protected:
	int progress;

public:

	PROPERTY_DEF(int, value, getValue, setValue)

	PROPERTY_DEF_WRITEONLY(bool, smooth, setEnableSmooth)

	PROPERTY_DEF_WRITEONLY(bool, vertical, setOrientation)

	TProgressBar();

	virtual void setEnableSmooth(bool smooth_);

	virtual void setOrientation(bool vertical_);

	virtual int getValue();

	virtual void setValue(int val);

	virtual bool create();

	virtual ~TProgressBar();
};

// =========== TGlyphButton.h ===========

/*
	MCL - TGlyphButton.h
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


class TGlyphButton : public TButton
{
protected:
	HFONT glyphFont;
	const wchar_t *glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:
	TGlyphButton();

	virtual ~TGlyphButton();

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor not specified.
	*/
	virtual void setGlyph(const wchar_t *glyphChar, HFONT glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

};

// =========== TEditBox.h ===========

/*
	MCL - TEditBox.h
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


class TEditBox : public TControl
{
protected:
	bool readOnlyProperty;

public:

	PROPERTY_DEF(bool, readOnly, isReadOnly, setReadOnly)

	PROPERTY_DEF_WRITEONLY(bool, lowercaseOnly, setLowercaseOnly)

	PROPERTY_DEF_WRITEONLY(bool, uppercaseOnly, setUppercaseOnly)

	// void onChange(TComponent* sender)
	EVENT_DEF(void, onChange, TComponent*)

	TEditBox();

	virtual bool isReadOnly();

	virtual void setReadOnly(bool readOnly_);

	virtual void setLowercaseOnly(bool lowercaseOnly_);

	virtual void setUppercaseOnly(bool uppercaseOnly_);

	virtual TString getText() override;

	virtual bool create() override;

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual ~TEditBox();
};

// =========== TMenuBar.h ===========

/*
	MCL - TMenuBar.h
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


class TMenuBar
{
protected:
	HMENU hMenu;

public:
	TMenuBar();

	virtual void add(const TString& text, const TMenu& menu);

	virtual HMENU getHandle();

	operator HMENU()const;

	virtual ~TMenuBar();

private:
	MCL_LEAK_DETECTOR(TMenuBar)
};

// =========== TMenuButton.h ===========

/*
	MCL - TMenuButton.h
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


class TMenuButton : public TButton
{
protected:
	TFont* arrowFont;
	HFONT glyphFont;
	TMenu* buttonMenu;
	const wchar_t *glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:

	PROPERTY_DEF_WRITEONLY(TMenu&, menu, setMenu)

	TMenuButton();

	virtual ~TMenuButton();

	virtual void setMenu(TMenu& buttonMenu_);

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor is not specified.
	*/
	virtual void setGlyph(const wchar_t *glyphChar, HFONT glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;
};

// =========== TNumericField.h ===========

/*
	MCL - TNumericField.h
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


class TNumericField : public TEditBox
{
public:
	TNumericField();

	virtual ~TNumericField();
};

// =========== TPasswordBox.h ===========

/*
	MCL - TPasswordBox.h
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


class TPasswordBox : public TEditBox
{
protected:
	char passwordCharProperty;

public:

	PROPERTY_DEF(char, passwordChar, getPasswordChar, setPasswordChar)

	TPasswordBox();

	virtual void setPasswordChar(const char pwdChar);

	virtual char getPasswordChar();

	virtual bool create();

	virtual ~TPasswordBox();
};

// =========== TTextArea.h ===========

/*
	MCL - TTextArea.h
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


class TTextArea : public TEditBox
{
public:

	PROPERTY_DEF_WRITEONLY(bool, verticalScrollBar, setEnableVScrollBar)
	PROPERTY_DEF_WRITEONLY(bool, horizontalScrollBar, setEnableHScrollBar)
	PROPERTY_DEF_WRITEONLY(bool, wordWrap, setEnableWordWrap)

	TTextArea();

	virtual void setEnableVScrollBar(bool enableVScrollBar);

	virtual void setEnableHScrollBar(bool enableHScrollBar);

	virtual void setEnableWordWrap(bool enableWordWrap);

	virtual LRESULT windowProcHandler(TMessage& message) override;

	virtual ~TTextArea();
};

// =========== TForm.h ===========

/*
	MCL - TForm.h
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


class TForm : public TWindow
{
protected:
	int formTypeProperty;
	bool maximizeButtonProperty;
	bool minimizeButtonProperty;
	bool closeButtonProperty;
	bool resizableProperty;
	bool alwaysOnTopProperty;
	TMenuBar* menuProperty;
	TMenu* contextMenuProperty;

public:

	int closeBehaviour;
	bool exitModalLoop;

	PROPERTY_DEF(int, formType, getFormType, setFormType)

	PROPERTY_DEF(bool, maximizeButton, hasMaximizeButton, setEnableMaximizeButton)

	PROPERTY_DEF(bool, minimizeButton, hasMinimizeButton, setEnableMinimizeButton)

	PROPERTY_DEF(bool, closeButton, hasCloseButton, setEnableCloseButton)

	PROPERTY_DEF(bool, resizable, isResizable, setEnableResizable)

	PROPERTY_DEF(bool, alwaysOnTop, isAlwaysOnTop, setEnableAlwaysOnTop)

	PROPERTY_DEF_WRITEONLY(TMenuBar&, menu, setMenu)

	PROPERTY_DEF_WRITEONLY(TMenu&, contextMenu, setContextMenu)

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

	TForm(int formType_ = TForm::TSTANDARD_WINDOW, bool createWindow = true)
	{
		menuProperty = 0;
		contextMenuProperty = 0;
		closeBehaviour = DESTROY_AND_EXIT_ON_CLOSE;
		textProperty.assignStaticText(TXT_WITH_LEN("TForm"));

		onClose = nullptr;
		onMove = nullptr;
		onResize = nullptr;
		onShow = nullptr;
		onHide = nullptr;

		this->setFormType(formType_);

		if (createWindow)
			this->create();
	}

	virtual ~TForm() {}

	virtual void setContextMenu(TMenu& contextMenu)
	{
		contextMenuProperty = &contextMenu;  // this points to the real object!
	}

	virtual bool create()
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

	virtual void setMenu(TMenuBar& menu)
	{
		menuProperty = &menu; // this points to the real object!

		if(handleProperty)
			::SetMenu(handleProperty, menuProperty->getHandle());
	}

	virtual bool isAlwaysOnTop()
	{
		return alwaysOnTopProperty;
	}
		
	virtual void setEnableAlwaysOnTop(bool enableAlwaysOnTop)
	{
		alwaysOnTopProperty = enableAlwaysOnTop;

		if(enableAlwaysOnTop)
			::SetWindowPos(handleProperty, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			::SetWindowPos(handleProperty, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	virtual bool isResizable()
	{
		return resizableProperty;
	}
		
	virtual void setEnableResizable(bool enableResizeBox)
	{
		resizableProperty = enableResizeBox;

		if (enableResizeBox)
			this->setStyle(styleProperty | WS_SIZEBOX);
		else
			this->setStyle(styleProperty & ~WS_SIZEBOX);
	}

	virtual bool hasCloseButton()
	{
		return closeButtonProperty;
	}
		
	virtual void setEnableCloseButton(bool enableCloseButton)
	{
		closeButtonProperty = enableCloseButton;

		if(enableCloseButton)
			::EnableMenuItem(::GetSystemMenu(handleProperty, FALSE), SC_CLOSE, 
				MF_BYCOMMAND | MF_ENABLED);
		else
			::EnableMenuItem(::GetSystemMenu(handleProperty, FALSE), SC_CLOSE, 
				MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}

	virtual bool hasMinimizeButton()
	{
		return minimizeButtonProperty;
	}
		
	virtual void setEnableMinimizeButton(bool enableMinimizeButton)
	{
		minimizeButtonProperty = enableMinimizeButton;

		if (enableMinimizeButton)
			this->setStyle(styleProperty | WS_MINIMIZEBOX);
		else
			this->setStyle(styleProperty & ~WS_MINIMIZEBOX);
	}

	virtual bool hasMaximizeButton()
	{
		return maximizeButtonProperty;
	}
		
	virtual void setEnableMaximizeButton(bool enableMaximizeButton)
	{
		maximizeButtonProperty = enableMaximizeButton;
		
		if (enableMaximizeButton)
			this->setStyle(styleProperty | WS_MAXIMIZEBOX); 
		else
			this->setStyle(styleProperty & ~WS_MAXIMIZEBOX);
	}

	virtual int getFormType()
	{
		return formTypeProperty;
	}

	virtual void setFormType(int formType_)
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
};

// =========== TChildForm.h ===========

/*
	MCL - TChildForm.h
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


class TChildForm : public TForm
{
public:

	// child windows are created when calling setParent method or asigning value to the parent property.
	TChildForm(int formType_ = TForm::TSTANDARD_WINDOW) : TForm(formType_, false)
	{
		this->setFormType(formType_);
	}

	virtual void setFormType(int formType_)
	{
		formTypeProperty = formType_;

		if (formTypeProperty == TSTANDARD_WINDOW)
		{
			this->setStyle(WS_CHILD | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
		}
		else if (formTypeProperty == TDIALOG)
		{
			this->setStyle(WS_CHILD | WS_CAPTION | WS_SYSMENU);
		}
		else if (formTypeProperty == TTOOL_WINDOW)
		{
			this->setStyle(WS_CHILD | WS_CAPTION | WS_SYSMENU);
			this->setExtendedStyle(WS_EX_TOOLWINDOW | WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT);
		}
	}

	// this method will also create the window.
	virtual void setParent(HWND parentHandle) override
	{
		parentProperty = parentHandle;
		this->create();
	}
};

// =========== mcl.h ===========

// todo: TCustomControl with onPaint

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

#endif

