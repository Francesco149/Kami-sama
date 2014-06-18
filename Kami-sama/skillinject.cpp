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

#include "skillinject.hpp"
#include <sstream>

// sorry about this mess, I got lazy at this point

makesingletoninstance(maple::skillinject)

namespace maple
{
	const byte skillinjectmem1[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	const size_t cbskillinjectmem1 = sizeof(skillinjectmem1);

	const byte skillinjectmem2[] = { 0xE9, 0xEC, 0x00, 0x00, 0x00, 0x90 };
	const size_t cbskillinjectmem2 = sizeof(skillinjectmem2);

	const byte skillinjectmem3[] = { 0x74 };
	const size_t cbskillinjectmem3 = sizeof(skillinjectmem3);

	const byte skillinjectmem4[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	const size_t cbskillinjectmem4 = sizeof(skillinjectmem4);

	const byte skillinjectmem5[] = { 0x7D, 0x00 };
	const size_t cbskillinjectmem5 = sizeof(skillinjectmem5);

	skillinject::skillinject()
		: memory::memoryhack(std::string("8B 8E ? ? ? ? 8B 54 24 ? 6A ? 6A ? 6A ? 8D 44 24 ? 50"), NULL, 6), 
		  skillinject1(std::string("0F 84 ? ? ? ? 2B 9E ? ? ? ? 0F 88 ? ? ? ? 8B 86 ? ? ? ? 50"), skillinjectmem1, cbskillinjectmem1), 
		  skillinject2(std::string("0F 84 ? ? ? ? 8B 16 8B 42 ? 8B CE FF D0 3D ? ? ? ? 74 ? 3D "
			"? ? ? ? 74 ? 3D ? ? ? ? 74 ? 3D ? ? ? ? 74 ? 3D ? ? ? ? 75 ? 8D 4C 24 ? 51"), skillinjectmem2, cbskillinjectmem2), 
		  skillinject3(std::string("75 ? 8B 86 ? ? ? ? 50 E8 ? ? ? ? 83 C4 ? 83 F8 ? 0F 85 ? ? ? "
			"? 8B 8E ? ? ? ? 8B 54 24 ? 6A ? 6A ? 6A ? 8D 44 24 ? 50 51"), skillinjectmem3, cbskillinjectmem3), 
		  skillinject4(std::string("0F 85 ? ? ? ? 8B 8E ? ? ? ? 8B 54 24 ? 6A ? 6A ? 6A ?"), skillinjectmem1, cbskillinjectmem1), 
		  skillinject5(std::string("0F 87 ? ? ? ? 0F B6 80 ? ? ? ? FF 24 85 ? ? ? ?"
			" 84 DB 74 ? 8B 8E ? ? ? ? 85 C9 "), skillinjectmem1, cbskillinjectmem1), 
		  skillinject6(std::string("FF 24 85 ? ? ? ? 84 DB 74 ? 8B 8E ? ? ? ? 85 C9"), skillinjectmem4, cbskillinjectmem4), 
		  skillinject7(std::string("75 ? C7 85 ? ? ? ? ? ? ? ? EB ? C7 85 ? ? ? ? ? ? ? ? 8B 95 "
			"? ? ? ? 89 55 ? 8B 85 ? ? ? ? 50"), skillinjectmem3, cbskillinjectmem3), 
		  skillinject8(std::string("7D ? 33 F6 89 74 24 ? 85 FF 7D ? 33 FF 81 FB ? ? ? "
			"? 75 ? 8B 6C 24 ?"), skillinjectmem5, cbskillinjectmem5)
	{
		int i = 1;

		// ugly shit, gotta make a proper hacklist generic class
		haxlist.push_back(&skillinject1);
		haxlist.push_back(&skillinject2);
		haxlist.push_back(&skillinject3);
		haxlist.push_back(&skillinject4);
		haxlist.push_back(&skillinject5);
		haxlist.push_back(&skillinject6);
		haxlist.push_back(&skillinject7);
		haxlist.push_back(&skillinject8);

		boost_foreach(memory::memoryhack *& hax, haxlist)
		{
			if (!hax->initialized())
			{
				std::ostringstream oss;
				oss << "skillinject::skillinject: could not find skillinjecy address " << i << ".";
				throw std::exception(oss.str().c_str());
			}

			i++;
		}

		// TODO: find a less ugly way to reuse code from memoryhack

		// calculate the jump distance and store jump opcode + distance in the modified memory array
		byte tmp[6] = { 0xE9, 0x00, 0x00, 0x00, 0x00, 0x90 };
		*reinterpret_cast<dword *>(tmp + 1) = jmp(address, hook);
		std::copy(tmp, tmp + 6, modified.get());

		retaddy = reinterpret_cast<dword>(address + 6);
		movoffset = *reinterpret_cast<dword *>(address + 2);
	}

	skillinject::~skillinject()
	{
		// empty
	}

	bool skillinject::enable(bool enabled)
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

		memory::memoryhack::enable(enabled);

		return res;
	}

	// TODO: don't make skillid & delay static
	void skillinject::setskillid(dword skillid)
	{
		skillinject::skillid = skillid;
	}

	void skillinject::setdelay(dword delay)
	{
		skillinject::delay = delay;
	}

	dword skillinject::skillid = 27001201;
	dword skillinject::delay = 1;
	dword skillinject::movoffset = 0;
	dword skillinject::retaddy = 0;
	dword skillinject::counter = 0;

	void __declspec(naked) skillinject::hook()
	{
		__asm
		{
			push eax
			mov eax,[counter]
			cmp eax,[delay]
			pop eax
			jge SkillInjection_inject
			inc [counter]
			mov ecx,esi
			add ecx,[movoffset]
			mov ecx,[ecx]
			jmp [retaddy]
			SkillInjection_inject:
			mov [counter],0
			mov ecx,[skillid]
			jmp [retaddy]
		}
	}
}
