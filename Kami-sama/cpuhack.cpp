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

#include "cpuhack.hpp"

makesingletoninstance(maple::cpuhack)

namespace maple
{
	const byte cpumem1[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	const size_t cbcpumem1 = sizeof(cpumem1);

	const byte cpumem2[] = { 0xC2, 0x08, 0x00 };
	const size_t cbcpumem2 = sizeof(cpumem2);

	const byte nodmgmem1[] = { 0x7C, 0x00 };
	const size_t cbnodmgmem1 = sizeof(nodmgmem1);

	const byte nodmgmem2[] = { 0x90, 0xE9 };
	const size_t cbnodmgmem2 = sizeof(nodmgmem2);

	const byte nodmgmem3[] = { 0xEB };
	const size_t cbnodmgmem3 = sizeof(nodmgmem3);

	cpuhack::cpuhack()
		: obj2(std::string("E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF "
			"E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 51 8B C4"), cpumem1, cbcpumem1, 7), 
		  // todo: figure out why using obj2.getaddress() + 7 crashes
		  tiles(std::string("E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF "
			"E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 51 8B C4"), cpumem1, cbcpumem1, 14),   
		  obj(std::string("E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF "
			"E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 51 8B C4"), cpumem1, cbcpumem1, 21), 
		  bg(std::string("E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF "
			"E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B CF E8 ? ? ? ? 51 8B C4"), cpumem1, cbcpumem1, 28),  
		  clouds(std::string("55 8D 6C 24 ? 83 EC ? 6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC ? A1 "
			"? ? ? ? 33 C5 89 45 ? 53 56 57 50 8D 45 ? 64 A3 ? ? ? ? 89 4D ? 51"), cpumem2, cbcpumem2), 
		  nodmg1(std::string("7C ? B8 ? ? ? ? E8 ? ? ? ? 8B C4 6A ? 50"), nodmgmem1, cbnodmgmem1), 
		  nodmg2(std::string("0F 84 ? ? ? ? 2D ? ? ? ? 75 ? 85 DB 74 ? 8B 13"), nodmgmem2, cbnodmgmem2), 
		  nodmg3(std::string("7E ? 8B 54 24 ? 52 8B 54 24 ? 52 51"), nodmgmem3, cbnodmgmem3), 
		  nodmg4(std::string("75 ? 8B 44 24 ? 50 6A ? 51"), nodmgmem3, cbnodmgmem3), 
		  nodmg5(std::string("7D ? 8B 4C 24 ? 8B 54 24 ? 51 52"), nodmgmem3, cbnodmgmem3)
	{
		// ugly as fuck, gotta make a proper hacklist class for hacks with this many memhacks

		if (!bg.initialized())
			throw std::exception("cpuhack::cpuhack: could not find nobg address.");

		if (!obj.initialized())
			throw std::exception("cpuhack::cpuhack: could not find noobj address.");

		if (!tiles.initialized())
			throw std::exception("cpuhack::cpuhack: could not find notiles address.");

		if (!obj2.initialized())
			throw std::exception("cpuhack::cpuhack: could not find noobj2 address.");

		if (!clouds.initialized())
			throw std::exception("cpuhack::cpuhack: could not find clouds address.");

		if (!nodmg1.initialized())
			throw std::exception("cpuhack::cpuhack: could not find 1st nodmg address.");

		if (!nodmg2.initialized())
			throw std::exception("cpuhack::cpuhack: could not find 2nd nodmg address.");

		if (!nodmg3.initialized())
			throw std::exception("cpuhack::cpuhack: could not find 3rd nodmg address.");

		if (!nodmg4.initialized())
			throw std::exception("cpuhack::cpuhack: could not find 4th nodmg address.");

		if (!nodmg5.initialized())
			throw std::exception("cpuhack::cpuhack: could not find 5th nodmg address.");

		haxlist.push_back(&bg);
		haxlist.push_back(&obj);
		haxlist.push_back(&tiles);
		haxlist.push_back(&obj2);
		haxlist.push_back(&clouds);
		haxlist.push_back(&nodmg1);
		haxlist.push_back(&nodmg2);
		haxlist.push_back(&nodmg3);
		haxlist.push_back(&nodmg4);
		haxlist.push_back(&nodmg5);
	}

	cpuhack::~cpuhack()
	{
		// empty
	}

	bool cpuhack::enable(bool enabled)
	{
		// TODO: proper generic hacklist base class

		bool res = true;

		boost_foreach(memory::memoryhack *& hax, haxlist)
			res = res && hax->enable(enabled);

		if (!res)
		{
			boost_foreach(memory::memoryhack *& hax, haxlist)
				hax->enable(!enabled);
		}

		return res;
	}
}
