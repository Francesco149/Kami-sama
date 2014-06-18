/*
	Copyright 2014 Francesco "Franc[e]sco" Noferi (francesco149@gmail.com)

	This file is part of Kami-sama.

	Kami-sama is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Kami-sama is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Kami-sama. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include "common.h"
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

// hack to avoid including Windows.h and having to undefine everything to avoid conflicts with wx
struct HWND__; 
typedef HWND__ *HWND;
struct HINSTANCE__;
typedef HINSTANCE__ *HINSTANCE;
typedef HINSTANCE HMODULE;

#define jmp(frm, to) (reinterpret_cast<int>(to) - reinterpret_cast<int>(frm) - 5)
#define boost_foreach BOOST_FOREACH
#define makesingleton(classname) classname : public utils::singleton<classname>
#define makesingletoninstance(classname) boost::shared_ptr<classname> utils::singleton<classname>::inst;

namespace std
{
	template <class T>
	bool contains(vector<T> v, T x)
	{
		return find(v.begin(), v.end(), x) != v.end();
	}
}

// maplestory utility funcs
namespace maple
{
	HWND wnd(); // returns a handle (HWND) to the in-game window
	void *base(); // returns the module base address
	size_t size(); // returns the module size
}

// utilities to read & write memory
namespace memory
{
	bool getmodulesize(HMODULE hModule, void **pbase, size_t *psize);
	dword makepagewritable(void *address, size_t cb, dword flprotect = 0x40);
	void writejmp(byte *address, void *hook, size_t nops = 0);
}

namespace utils
{
	template <class T>
	class singleton
	{
	public:
		virtual ~singleton() { }

		static boost::shared_ptr<T> get()
		{
			if (!inst.get())
				inst.reset(new T);

			return inst;
		}

	protected:
		static boost::shared_ptr<T> inst;

		singleton() { }
	};
}
