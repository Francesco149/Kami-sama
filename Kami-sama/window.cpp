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

#include "window.hpp"
#include "aobscan.hpp"

#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Windows.h>

// virtual void __thiscall CWndMan::OnKey(unsigned int unknown, unsigned int key)
// A1 ? ? ? ? 85 C0 74 ? 8D ? ? 8B ? 8B ? FF
/*
mov ecx,[017F44A8] // CWndMan singleton 8B 15 ? ? ? ? 85 D2 74 23
push edx
push 00
call 00FEAC00 // CWndMan::OnKey
*/

// fake ret addy
// 90 C3 2nd result

#define basenull() (TSingleTon_CWndMan___ms_pInstance == NULL)
makesingletoninstance(maple::window)

namespace maple
{
	byte **window::TSingleTon_CWndMan___ms_pInstance = NULL;
	window::pfnCWndMan__OnKey window::CWndMan__OnKey = NULL;
	dword window::maplethreadid = NULL;

	window::window()
		//: TSingleTon_CWndMan___ms_pInstance(NULL), 
		  //CWndMan__OnKey(NULL)
	{
		void *pmodule = maple::base();
		size_t cbmodule = maple::size();

		memory::aobscan base("8B 15 ? ? ? ? 85 D2 74 23", pmodule, cbmodule); // thanks Waty for the AoB & pointer

		if (!base.result())
			throw std::exception("window::window: could not find window base!");

		TSingleTon_CWndMan___ms_pInstance = *reinterpret_cast<byte ** *>(base.result() + 2);

		memory::aobscan onkey("A1 ? ? ? ? 85 C0 74 ? 8D ? ? 8B ? 8B ? FF", pmodule, cbmodule);

		if (!onkey.result())
			throw std::exception("window::window: could not find CWndMan__OnKey!");

		CWndMan__OnKey = reinterpret_cast<pfnCWndMan__OnKey>(onkey.result());

		// TODO: move thread & return address spoofing to some external class for reuse
		memory::aobscan aobsomeret("90 C3", pmodule, cbmodule, 1);

		if (!aobsomeret.result())
			throw std::exception("window::window: could not find fake ret addy!");

		someret = reinterpret_cast<dword>(aobsomeret.result());

		boost::shared_ptr<boost::thread> t = boost::make_shared<boost::thread>(&window::getmaplethreadid);
	}

	window::~window()
	{
		// empty
	}

	dword window::someret = NULL;

	void __declspec(naked) window::trampoline_CWndMan__OnKey(unsigned int unknown, unsigned int key)
	{
		__asm
		{
			mov ecx, [TSingleTon_CWndMan___ms_pInstance]
			mov ecx, [ecx]

			push [key]
			push [unknown]
			push [someret]
			jmp [CWndMan__OnKey]
		}
	}

	void window::onkey(unsigned int unknown, unsigned int key, bool spoofthread)
	{
		try
		{
			if (spoofthread)
			{
				if (maplethreadid == 0)
					return;

				__writefsdword(0x06B8, maplethreadid);
				trampoline_CWndMan__OnKey(unknown, key); // fuck thread id checks
			}
			else
				CWndMan__OnKey(*TSingleTon_CWndMan___ms_pInstance, NULL, unknown, key);
		}
		catch (...)
		{
			return;
		}
	}

	void window::getmaplethreadid()
	{
		namespace tt = boost::this_thread;
		namespace pt = boost::posix_time;

		HWND hmoopla = NULL;

		while (!hmoopla)
		{
			hmoopla = maple::wnd();
			tt::sleep(pt::milliseconds(500));
		}

		maplethreadid = GetWindowThreadProcessId(hmoopla, NULL);
	}
}
