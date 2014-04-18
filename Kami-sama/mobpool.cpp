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

#include "mobpool.hpp"
#include "aobscan.hpp"
#include "utils.h"
#include <Windows.h>
//#include <wx/log.h>

// mob count: 8B 52 ? 83 C7 ? 8D 44 24 ? 50 8B CF FF D2 8B 00 89

// note: offsets from named funcs are off kmst, they might vary on ems

// mob 1:
// CMobPool__FindBodyAttackMob + 0x27: 8B 49 ? 33 ED 89 4C 24 ? 3B CD 0F 84 ? ? ? ? EB ? 8D 9B ? ? ? ? 8D 44 24 ? 50
// 8B 47 ? 89 44 24 ? 85 C0 74 ? EB ? 8D 49

// mob 2: 
// CMobPool__FindBodyAttackMob + 0x50: 8B 58 ? 83 C4 ? 89 5C 24 ? 39 6C 24 ? 74 ? 8B 8B ? ? ? ? 51
// CMobPool__Update + 0x7A: 8B 48 ? 8B 11 8B 02 83 C4 ? FF D0 83 7C 24 ? ? 75 ? 8B 47 ? 89 44 24 ? 85 C0
// CMobPool__GetMob + 0x25: 8B 70 ? F6 C3 ? 74 ? 83 7C 24 ? ? 74 ? 6A ? 8D 4C 24 ? E8 ? ? ? ? 8B C6

// NOTE: mob 4-invis-xy must be scanned while in game with all boxes ticked

// mob 3:
// 83 BF ? ? ? ? ? 0F 84 ? ? ? ? 83 CE ? 39 B3 ? ? ? ? 0F 8E ? ? ? ? 68 ? ? ? ? 8D 4C 24 ? E8 ? ? ? ? 68 ? ? ? ? 8D 4C 24 ? C7 44 24 ? ? ? ? ? E8 ? ? ? ? 8B 8F ? ? ? ? C6 44 24 ? ? 85 C9
// 8B 8F ? ? ? ? C7 44 24 ? ? ? ? ? 85 C9 75 ? 68 ? ? ? ? E8 ? ? ? ? 8D 54 24 ? 52 E8 ? ? ? ? 83 CE ? 66 83 7C 24 ? ? 89 74 24 ? 75 ? 33 C0
// 8B 8F ? ? ? ? C6 44 24 ? ? 85 C9 75 ? 68 ? ? ? ? E8 ? ? ? ? 8D 54 24 ? 8D 44 24 ? 52
// 8B 8F ? ? ? ? C6 44 24 ? ? 85 C9 75 ? 68 ? ? ? ? E8 ? ? ? ? 8D 54 24 ? 8D 44 24 ? 52 50 8B 43 ? F7 D8

// mob 4:
// 8B ? ? 3B ? 75 ? 8B ? ? 89 ? ? 8B ? ? 89 ? ? EB ? 8D
// 8B ? ? 3B ? 75 ? 8B ? ? 89 ? 33 ? EB ? 50
// 8B ? ? 33 ? 40 89 ? ? 85 ? 74 ? 8B ? 50

// invisible mob check:
// 89 ? ? 8B ? ? DD ? ? DD ? ? 89 ? ? 8B ? ? DD
// 8B ? ? 89 ? 8B ? ? 85 ? 74 ? 8B ? ? 89 ? 8B ? ? 85 ? 74 ? 8B ? ? 89 ? 8B ? ? 85 ? 74 ? 8B ? ? 89 ? 8B ? ? 85 ? 74 ? DD ? ? ? ? ? 83

// mob xy:
// 89 ? ? DD ? ? 8B ? ? DD ? ? DC ? ? 89 ? ? 8B ? ? 89 ? ? 8B ? ? DD

#define basenull() (TSingleton_CMobPool___ms_pInstance == NULL)

namespace maple
{
	boost::shared_ptr<mobpool> mobpool::inst;

	boost::shared_ptr<mobpool> mobpool::get()
	{
		if (!inst.get())
			inst.reset(new mobpool);

		return inst;
	}

	mobpool::mobpool()
		: TSingleton_CMobPool___ms_pInstance(NULL), 
		  offsize(0),
		  offmob1(0), 
		  offmob2(0), 
		  offmob3(0), 
		  offmob4(0x0024), // I'm gonna hope these are static for now 
		  offinvx(0x0060), 
		  offinvy(offinvx + 4), 
		  offmobx(0x0058), 
		  offmoby(offmobx + 4)
	{
		void *pmodule = maple::base();
		size_t cbmodule = maple::size();

		// mob base
		memory::aobscan base("8B 0D ? ? ? ? ? E8 ? ? ? ? 8B ? 85 ? 74 ? 8B ? ? 8B", pmodule, cbmodule);

		if (!base.result())
			throw std::exception("mobpool::mobpool: could not find mob base!");
		
		TSingleton_CMobPool___ms_pInstance = *reinterpret_cast<byte ** *>(base.result() + 2);

		// mob count
		memory::aobscan count("8B 52 ? 83 C7 ? 8D 44 24 ? 50 8B CF FF D2 8B 00 89", pmodule, cbmodule);

		if (!count.result())
			throw std::exception("mobpool::mobpool: could not find mob count offset!");
		
		offsize = *(count.result() + 2);

		// mob 1
		// note: offsets from named funcs are off kmst, they might vary on ems
		static const char *mob1aobs[2] =
		{
			"8B 49 ? 33 ED 89 4C 24 ? 3B CD 0F 84 ? ? ? ? EB ? 8D 9B ? ? ? ? 8D 44 24 ? 50", // CMobPool__FindBodyAttackMob + 0x27
			"8B 47 ? 89 44 24 ? 85 C0 74 ? EB ? 8D 49"
		};

		for (int i = 0; i < 2; i++)
		{
			memory::aobscan mob1(mob1aobs[i], pmodule, cbmodule);

			if (mob1.result())
			{
				offmob1 = *(mob1.result() + 2);
				break;
			}
		}

		if (offmob1 == 0)
			throw std::exception("mobpool::mobpool: could not find mob offset 1!");

		// mob 2
		static const char *mob2aobs[3] =
		{
			"8B 58 ? 83 C4 ? 89 5C 24 ? 39 6C 24 ? 74 ? 8B 8B ? ? ? ? 51", // CMobPool__FindBodyAttackMob + 0x50
			"8B 48 ? 8B 11 8B 02 83 C4 ? FF D0 83 7C 24 ? ? 75 ? 8B 47 ? 89 44 24 ? 85 C0", // CMobPool__Update + 0x7A
			"8B 70 ? F6 C3 ? 74 ? 83 7C 24 ? ? 74 ? 6A ? 8D 4C 24 ? E8 ? ? ? ? 8B C6" // CMobPool__GetMob + 0x25
		};

		for (int i = 0; i < 3; i++)
		{
			memory::aobscan mob2(mob2aobs[i], pmodule, cbmodule);

			if (mob2.result())
			{
				offmob2 = *(mob2.result() + 2);
				break;
			}
		}

		if (offmob2 == 0)
			throw std::exception("mobpool::mobpool: could not find mob offset 2!");

		// mob 3
		static const char *mob3aobs[4] =
		{
			"83 BF ? ? ? ? ? 0F 84 ? ? ? ? 83 CE ? 39 B3 ? ? ? ? 0F 8E ? ? ? ? 68 ? ? ? ? 8D 4C 24 ? E8 ? ? ? ? "
				"68 ? ? ? ? 8D 4C 24 ? C7 44 24 ? ? ? ? ? E8 ? ? ? ? 8B 8F ? ? ? ? C6 44 24 ? ? 85 C9",
			"8B 8F ? ? ? ? C7 44 24 ? ? ? ? ? 85 C9 75 ? 68 ? ? ? ? E8 ? ? ? ? 8D "
				"54 24 ? 52 E8 ? ? ? ? 83 CE ? 66 83 7C 24 ? ? 89 74 24 ? 75 ? 33 C0",
			"8B 8F ? ? ? ? C6 44 24 ? ? 85 C9 75 ? 68 ? ? ? ? E8 ? ? ? ? 8D 54 24 ? 8D 44 24 ? 52", 
			"8B 8F ? ? ? ? C6 44 24 ? ? 85 C9 75 ? 68 ? ? ? ? E8 ? ? ? ? 8D 54 24 ? 8D 44 24 ? 52 50 8B 43 ? F7 D8"
		};

		for (int i = 0; i < 4; i++)
		{
			memory::aobscan mob3(mob3aobs[i], pmodule, cbmodule);

			if (mob3.result())
			{
				offmob3 = *reinterpret_cast<word *>(mob3.result() + 2);
				break;
			}
		}

		if (offmob3 == 0)
			throw std::exception("mobpool::mobpool: could not find mob offset 3!");

		/*
		wxLogWarning("mobbase: %8X\r\ncount: %4X\r\noff1: %4X\r\noff2: %4X\r\noff3: %4X\r\noff4: %4X"
			"\r\ninvx: %4X\r\ninvy: %4X\r\nmobx: %4X\r\nmoby: %4X\r\n", 
			reinterpret_cast<dword>(TSingleton_CMobPool___ms_pInstance), offsize, offmob1, offmob2, offmob3, offmob4, offinvx, offinvy, offmobx, offmoby);
		*/
	}

	mobpool::~mobpool()
	{
		// empty
	}

	dword mobpool::size()
	{
		try
		{
			if (basenull())
				return 0;

			return *reinterpret_cast<dword *>(*TSingleton_CMobPool___ms_pInstance + offsize);
		}
		catch(...)
		{
			return 0;
		}
	}

	// TODO: call CMob * __thiscall CMobPool::FindNearestMob(struct tagPOINT, int) 
	// and find and call struct tagPOINT __thiscall CMob::GetPos() in EMS instead of using offsets
	bool mobpool::getcoords(POINT *ppt)
	{
		try
		{
			if (basenull())
				return false;

			if (!size())
				return false;
		
			byte *tmp = *reinterpret_cast<byte **>(*TSingleton_CMobPool___ms_pInstance + offmob1);
			byte *mob1 = tmp;

			if (tmp == NULL)
				return false;

			tmp = *reinterpret_cast<byte **>(tmp + offmob2);

			if (tmp == NULL)
				return false;

			tmp = *reinterpret_cast<byte **>(tmp + offmob3);

			if (tmp == NULL)
				return false;

			tmp = *reinterpret_cast<byte **>(tmp + offmob4);

			if (tmp == NULL)
				return false;

			dword invisiblex = *reinterpret_cast<dword *>(tmp + offinvx);
			dword invisibley = *reinterpret_cast<dword *>(tmp + offinvy);

			if (invisiblex || invisibley)
			{
				// first mob of the list is not invisible, so get that one
				ppt->x = *reinterpret_cast<long *>(tmp + offmobx);
				ppt->y = *reinterpret_cast<long *>(tmp + offmoby);
				return true;
			}

			mob1 = *reinterpret_cast<byte **>(mob1 - 0x0C);

			if (mob1 == NULL)
				return false;

			// iterate the mob list till a visible mob is found
			while (mob1 != NULL)
			{
				// next mob todo - find aob
				tmp = *reinterpret_cast<byte **>(mob1 + 0x14);

				if (tmp == NULL)
					return false;

				tmp = *reinterpret_cast<byte **>(tmp + offmob3);

				if (tmp == NULL)
					return false;

				tmp = *reinterpret_cast<byte **>(tmp + offmob4);

				if (tmp == NULL)
					return false;

				dword invisiblex = *reinterpret_cast<dword *>(tmp + offinvx);
				dword invisibley = *reinterpret_cast<dword *>(tmp + offinvy);

				if (invisiblex || invisibley)
				{
					ppt->x = *reinterpret_cast<long *>(tmp + offmobx);
					ppt->y = *reinterpret_cast<long *>(tmp + offmoby);
					return true;
				}

				mob1 = *reinterpret_cast<byte **>(mob1 + 0x04);

				if (mob1 == NULL)
					return false;
			}

			// no mobs found
			return false;
		}
		catch(...)
		{
			return false;
		}
	}
}
