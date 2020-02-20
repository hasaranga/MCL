
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
	static void release(StructType* structPtr)
	{
		::free(structPtr);
	}
};

template<class StructType>
class TReleaseUsingTaskMemFree
{
public:
	static void release(StructType* memory)
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
			ReleaseMethod::release(structPointer);
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
				ReleaseMethod::release(oldStructPointer);
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
