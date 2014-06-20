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

#include "fullgodmode.hpp"

makesingletoninstance(maple::fullgodmode)

namespace maple
{
	const byte fullgmmem[] = { 0xC2, 0x04, 0x00 };
	const size_t cbfullgmmem = sizeof(fullgmmem);

	fullgodmode::fullgodmode()
		: fullgm1(std::string("6A ? 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 EC ? ? ? ? 53 55 56 57 A1 ? ? ? ? 33 C4 50 8D 84 24 "
			"? ? ? ? 64 A3 ? ? ? ? 8B F1 C7 44 24 ? ? ? ? ? 8B 2D ? ? ? ? 8D 84 24"), fullgmmem, cbfullgmmem)
	{
		if (!fullgm1.initialized())
			throw std::exception("fullgodmode::fullgodmode: could not find first address.");
	}

	fullgodmode::~fullgodmode()
	{
		// empty
	}

	bool fullgodmode::enable(bool enabled)
	{
		bool res = fullgm1.enable(enabled);

		if (!res)
			fullgm1.enable(!enabled);

		return res;
	}
}
