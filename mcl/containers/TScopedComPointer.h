
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
