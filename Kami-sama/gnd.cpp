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

#include "gnd.hpp"

makesingletoninstance(maple::gnd)

namespace maple
{
	const byte gndmem[] = { 0x01 };
	const size_t cbgndmem = sizeof(gndmem);

	gnd::gnd()
		: memory::memoryhack(std::string("00 00 00 00 8B 95 ? ? ? ? 89 55 ? 8B 85 ? ? ? ? 50 E8 ? ? ? ? 83 C4 ? 85 C0"), gndmem, cbgndmem)
	{
		if (!initialized())
			throw std::exception("gnd::gnd: could not find gnd address.");
	}

	gnd::~gnd()
	{
		// empty
	}
}
