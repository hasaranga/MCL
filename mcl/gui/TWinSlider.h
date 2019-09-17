
/*
	MCL - TWinSlider.h
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

#include "TControl.h"

class TWinSlider : public TControl
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

	TWinSlider();

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setEnableTicks(bool enableTicks) { setEnableTicksImpl(enableTicks); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setOrientation(bool vertical) { setOrientationImpl(vertical); }

	/**
		Range between 0 to 100
	*/
	virtual void setRange(int min, int max);

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline void setPosition(int pos) { setPositionImpl(pos); }

	/**
		(Please override the "Impl" method to change the behaviour!)
	*/
	inline int getPosition() { return getPositionImpl(); }

	virtual bool notifyProcHandler(TMessage& message, LRESULT& result) override;

	virtual bool create() override;

	virtual ~TWinSlider();

private:
	virtual void setPositionImpl(int pos);
	virtual int getPositionImpl();
	virtual void setEnableTicksImpl(bool enableTicks);
	virtual void setOrientationImpl(bool vertical);
};


