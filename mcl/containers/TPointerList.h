
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
#include "../config.h"
#include "TLeakDetector.h"

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
