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

#include "fusionattack.hpp"

makesingletoninstance(maple::fusionattack)

namespace maple
{
	fusionattack::fusionattack()
		: memory::memoryhack(std::string("89 34 81 40 89 44 24 ? 8B 44 24 ? 85 C0"), NULL, 5 + 3)
	{
		// TODO: find a less ugly way to reuse code from memoryhack

		// calculate the jump distance and store jump opcode + distance in the modified memory array
		byte tmp[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90 };
		tmp[0] = 0xE9;
		*reinterpret_cast<dword *>(tmp + 1) = jmp(address, hook);
		std::copy(tmp, tmp + 8, modified.get());

		retaddy = reinterpret_cast<dword>(address + 8);
	}

	fusionattack::~fusionattack()
	{
		// empty
	}

	dword fusionattack::retaddy = 0;

	void __declspec(naked) fusionattack::hook()
	{
		__asm
		{
			mov [ecx+eax*4],esi
			inc eax
			cmp eax,[esp+0x68]
			jl hook
			mov [esp+0x1C],eax
			jmp [retaddy]
		}
	}
}
