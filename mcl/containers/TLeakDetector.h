
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
