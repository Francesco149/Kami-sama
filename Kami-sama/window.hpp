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
#include <boost/shared_ptr.hpp>

namespace maple
{
	// a wrapper around maple's CWndMan
	class window
	{
	public:
		enum
		{
			KEY_CTRL = 0x001D0000,
			KEY_SHIFT = 0x002A0000,
			KEY_INSERT = 0x01520000,
			KEY_DEL = 0x01530000,
			KEY_HOME = 0x01470000,
			KEY_END = 0x014F0000,
			KEY_PAGEUP = 0x01490000,
			KEY_PAGEDOWN = 0x01510000,
			KEY_ALT = 0x00380000,
			KEY_Z = 0x002C0000
		};

		static boost::shared_ptr<window> get();
		virtual ~window();
		void onkey(unsigned int unknown, unsigned int key, bool spoofthread = false);

	protected:
		static boost::shared_ptr<window> inst;
		
		static byte **TSingleTon_CWndMan___ms_pInstance;

		typedef void (__fastcall *pfnCWndMan__OnKey)(void *pthis, void *edx, unsigned int unknown, unsigned int key);
		static pfnCWndMan__OnKey CWndMan__OnKey;

		static dword someret;
		static void trampoline_CWndMan__OnKey(unsigned int unknown, unsigned int key);
		
		static dword maplethreadid;
		static void getmaplethreadid();

		window();
	};
}
