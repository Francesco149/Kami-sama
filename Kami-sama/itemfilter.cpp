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

#include "itemfilter.hpp"

makesingletoninstance(maple::itemfilter)

namespace maple
{
	itemfilter::itemfilter()
		: memory::memoryhack(std::string("8B CF 89 46 ? E8 ? ? ? ? 8B CF 89 46 ? E8 ? ? ? ? 0F B6 D0"), NULL, 5), 
		  meso(50000), 
		  mode(MODE_ACCEPT)
	{
		if (!address)
			throw std::exception("itemfilter::itemfilter: could not find filter hook addy!");

		// TODO: find a less ugly way to reuse code from memoryhack

		// calculate the jump distance and store jump opcode + distance in the modified memory array
		byte tmp[5] = {0};
		tmp[0] = 0xE9;
		*reinterpret_cast<dword *>(tmp + 1) = jmp(address, hook);
		std::copy(tmp, tmp + 5, modified.get());

		retaddy = reinterpret_cast<dword>(address + 5);
	}

	itemfilter::~itemfilter()
	{
		// empty
	}

	void itemfilter::push_back(const dword &itemid)
	{
		items.push_back(itemid);
	}

	void itemfilter::erase(const dword &itemid)
	{
		// remove moves elements to be removed to the end of the vector and returns
		// an iterator that points to the new end of the vector
		items.erase(std::remove(items.begin(), items.end(), itemid), items.end());
	}

	void itemfilter::setmeso(dword meso)
	{
		this->meso = meso;
	}

	void itemfilter::setmode(dword mode)
	{
		this->mode = mode;
	}

	bool itemfilter::contains(dword itemid)
	{
		return std::contains(items, itemid);
	}

	void __stdcall itemfilter::handlecall(dword *pitemid, dword *pmeso)
	{
		boost::shared_ptr<itemfilter> pthis = get();

		if (*pitemid <= pthis->meso)
		{
			*pmeso = 0;
			return;
		}

		bool filter = pthis->contains(*pitemid);

		// if the item is not in the list, then it must be ignored
		if (pthis->mode == MODE_ACCEPT)
			filter ^= true;

		if (filter && *pitemid > 60000)
			*pitemid = 0;
	}

	dword itemfilter::retaddy = 0;

	void __declspec(naked) itemfilter::hook()
	{
		__asm
		{
			// eax contains item id
			mov ecx,edi
			mov [esi+0x38],eax

			push ecx
			push edx

			mov edx, esi // meso (esi+34)
			add edx,0x34
			push edx

			mov edx, esi // itemid (esi+38)
			add edx,0x38
			push edx

			call handlecall

			pop edx
			pop ecx

			jmp [retaddy]
		}
	}
}
