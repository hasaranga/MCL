
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
