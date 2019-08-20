
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
