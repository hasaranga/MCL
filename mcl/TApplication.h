
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

#include "config.h"
#include "text/TString.h"
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
