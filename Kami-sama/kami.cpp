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

#include "kami.hpp"
#include "aobscan.hpp"

#include <boost/make_shared.hpp>

// GetCVecCtrlUser?
// 8B ? ? ? ? ? 85 ? 74 ? 83 ? ? 74 ? 83 ? ? C3
// mov eax,[ecx+00007xxxx], usually the last result (8th)
// or just get the value of edx at FF D2 89 84 24 ? ? ? ? 8D 84 24 ? ? ? ? 50 8B CF

// CVecCtrlUser__OnTeleport?
// 1st result: 8B ? 24 ? 8B ? ? 8B ? ? ? 8D ? ? 8B ? ? ? ? ? ? FF ? 85 C0 ? ? ? ? ? ? ? ? ? E8

// kami hook
// Pointer: C7 ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? 8B ? ? ? ? ? 8B ? ? ? ? ? 50 C7 ? ? ? ? ? ? ? E8 ? ? ? ? 8B ? ? ? ? ? 33 [Address in mov]
// Offset: 8B ? ? FF ? 8B ? B8 ? ? ? ? F7 ? C1 ? ? 8B ? C1 ? ? 03 ? 83 ? ? 74 ? 81 ? ? ? ? ? 75 ? 80

// kami hook return addy check
// 8B C8 B8 ? ? ? ? F7 E9 C1 FA ? 8B C2 C1 E8 ? 03 C2 83 F8 ? 74 ? 81 F9 ? ? ? ? 75 ? 80 BE ? ? ? ? ? 74 ? 8B 56 ? 8B 42

makesingletoninstance(maple::kami)

namespace maple
{
	kami::kami()
		#ifdef BYPASSLESS_KAMI
		: memory::iathook(NULL, kamihook),
		#else
		: memory::memoryhack(std::string("8B 06 8B 50 ? FF D2 8B C8 B8 ? ? ? ? F7 E9 C1 FA ? 8B C2 C1 E8 ? "
			"03 C2 83 F8 ? 74 ? 81 F9 ? ? ? ? 75 ? 80 BE ? ? ? ? ? 74 ? 8B 56 ? 8B 42 ? 8D 4E ? FF D0"), NULL, 5), 
		#endif
		  GetCVecCtrlUser(NULL), 
		  CVecCtrlUser__OnTeleport(NULL), 
		  pitemhook(itemhook::get()), 
		  pplayer(player::get()), 
		  pmobpool(mobpool::get()), 
		  pphysicalspace(physicalspace::get()), 
		  pdroppool(droppool::get()), 
		  pwindow(window::get()), 
		  lastteleport(pt::microsec_clock::universal_time())
	{
		void *pmodule = maple::base();
		size_t cbmodule = maple::size();

		// GetCVecCtrlUser
		memory::aobscan getcvec("8B ? ? ? ? ? 85 ? 74 ? 83 ? ? 74 ? 83 ? ? C3", pmodule, cbmodule, 8);

		if (!getcvec.result())
			throw std::exception("kami::kami: could not find GetCVecCtrlUser!");

		GetCVecCtrlUser = reinterpret_cast<pfnGetCVecCtrlUser>(getcvec.result());

		// CVecCtrlUser__OnTeleport
		memory::aobscan onteleport("8B ? 24 ? 8B ? ? 8B ? ? ? 8D ? ? 8B ? ? ? ? ? ? FF ? 85 C0 ? ? ? ? ? ? ? ? ? E8", pmodule, cbmodule);

		if (!onteleport.result())
			throw std::exception("kami::kami: could not find CVecCtrlUser__OnTeleport!");

		CVecCtrlUser__OnTeleport = reinterpret_cast<pfnCVecCtrlUser__OnTeleport>(onteleport.result());

		// block movement
		memory::aobscan blockmov("75 ? 8D 55 ? 52 E8 ? ? ? ? 83 C4 ? 0F B6 45 ? 85 C0", pmodule, cbmodule);

		if (!blockmov.result())
			throw std::exception("kami::kami: could not find block movement!");

		byte blockmovmemory[] = { 0xEB };
		blockmovement.reset(new memory::memoryhack(blockmov.result(), blockmovmemory, 1));

		// kami hook
#ifdef BYPASSLESS_KAMI
		memory::aobscan hookpointer("C7 ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? ? ? ? 8B"
			" ? ? ? ? ? 8B ? ? ? ? ? 50 C7 ? ? ? ? ? ? ? E8 ? ? ? ? 8B ? ? ? ? ? 33", pmodule, cbmodule);

		if (!hookpointer.result())
			throw std::exception("kami::kami: could not find kami hook base!");

		byte *phookbase = *reinterpret_cast<byte **>(hookpointer.result() + 2);

		memory::aobscan hookoffset("8B ? ? FF ? 8B ? B8 ? ? ? ? F7 ? C1 ? ? 8B ? C1 ? ?"
			" 03 ? 83 ? ? 74 ? 81 ? ? ? ? ? 75 ? 80", pmodule, cbmodule);

		if (!hookoffset.result())
			throw std::exception("kami::kami: could not find kami hook offset!");

		word offkamihook = *(hookoffset.result() + 2);

		setpfunction(reinterpret_cast<void **>(phookbase + offkamihook));

		memory::aobscan retaddycheck("8B C8 B8 ? ? ? ? F7 E9 C1 FA ? 8B C2 C1 E8 ? 03 C2 83 F8 ? "
			"74 ? 81 F9 ? ? ? ? 75 ? 80 BE ? ? ? ? ? 74 ? 8B 56 ? 8B 42", pmodule, cbmodule);

		if (!retaddycheck.result())
			throw std::exception("kami::kami: could not find kami hook return address!");

		kamiretcheck = reinterpret_cast<dword>(retaddycheck.result());
#else
		// TODO: find a less ugly way to reuse code from memoryhack
		if (!address)
			throw std::exception("kami::kami: could not find kami hook addy!");

		// calculate the jump distance and store jump opcode + distance in the modified memory array
		byte tmp[5] = {0};
		tmp[0] = 0xE9;
		*reinterpret_cast<dword *>(tmp + 1) = jmp(address, kamihook);
		std::copy(tmp, tmp + 5, modified.get());
		kamiret = reinterpret_cast<dword>(address) + 5;
#endif
	}

	kami::~kami()
	{
		// empty
	}

	void kami::teleport(long x, long y)
	{
		void *teleportbase = pplayer->getteleportbase();

		if (teleportbase == NULL)
			return;

		try
		{
			void *pthis = GetCVecCtrlUser(teleportbase, NULL);
			CVecCtrlUser__OnTeleport(pthis, NULL, NULL, x, y); // not sure what the first argument is, but passing zero works
		}
		catch (...)
		{
			return;
		}
	}

	void kami::teleporttomobs(bool loot, long xoff, long yoff, bool autoattack, bool spoofthread)
	{
		POINT pt = {0};

		if (pmobpool->getcoords(&pt))
		{
			pt.x -= xoff;
			pt.y -= yoff;

			if (autoattack)
				pwindow->onkey(0, window::KEY_CTRL, spoofthread);
		}
		else
		{
			if (loot && pdroppool->size() > 0)
			{
				pwindow->onkey(0, window::KEY_Z, spoofthread);
				pt = pitemhook->getcoords();
			}
			else
				pt = pphysicalspace->getltwall();
		}

		teleport(pt.x, pt.y);
	}

	void kami::worker(bool loot, long xoff, long yoff, dword microsecondsdelay, bool autoattack)
	{
		namespace tt = boost::this_thread;
		namespace pt = boost::posix_time;

		while (true)
		{
			teleporttomobs(loot, xoff, yoff, autoattack, true);
			tt::sleep(pt::microseconds(microsecondsdelay));
		}
	}

	void kami::startsticking(bool loot, long xoff, long yoff, dword microsecondsdelay, bool clientsided, bool autoattack)
	{
		stopsticking();

		if (clientsided)
			blockmovement->enable(true);

		hworker = boost::make_shared<boost::thread>(
			boost::bind(&kami::worker, this, loot, xoff, yoff, microsecondsdelay, autoattack)
		);
	}

	void kami::stopsticking()
	{
		blockmovement->enable(false);

		if (hworker.get())
		{
			hworker->interrupt();
			hworker.reset();
		}
	}

	void kami::hook(bool loot, long xoff, long yoff, dword microsecondsdelay, bool clientsided, bool autoattack)
	{
		if (clientsided)
			blockmovement->enable(true);

		this->loot = loot;
		this->xoff = xoff;
		this->yoff = yoff;
		this->microsecondsdelay = microsecondsdelay;
		this->autoattack = autoattack;

#ifdef BYPASSLESS_KAMI
		kamiret = reinterpret_cast<dword>(getpointervalue());
#endif
		enable(true);
	}

	void kami::unhook()
	{
		enable(false);
		blockmovement->enable(false);
	}

	void kami::dokamistep()
	{
		if (pt::microsec_clock::universal_time() - lastteleport >= pt::microseconds(microsecondsdelay))
		{
			teleporttomobs(loot, xoff, yoff, autoattack, false);
			lastteleport = pt::microsec_clock::universal_time();
		}
	}

	void kami::handlekamicall()
	{
		get()->dokamistep();
	}

	dword kami::kamiret = NULL;

#ifdef BYPASSLESS_KAMI
	dword kami::kamiretcheck = NULL;

	void __declspec(naked) kami::kamihook()
	{
		__asm
		{
			mov eax, [kamiretcheck]
			cmp dword ptr [esp], eax
			pushad
			jne KamiExit
			call handlekamicall
		KamiExit:
			popad
			jmp [kamiret]
		}
	}
#else
	void __declspec(naked) kami::kamihook()
	{
		__asm
		{
			mov eax,[esi]
			mov edx,[eax+0x60]
			pushad
			call handlekamicall
			popad
			jmp [kamiret]
		}
	}
#endif
}
