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

#include "unlimitedmp.hpp"

makesingletoninstance(maple::unlimitedmp)

namespace maple
{
	const byte unlimitedmpmem[] = { 0x90, 0x90 };
	const size_t cbunlimitedmpmem = sizeof(unlimitedmpmem);

	unlimitedmp::unlimitedmp()
		: memory::memoryhack(std::string("7D ? 33 FF 81 FB ? ? ? ? 75 ? 8B 6C 24 ? 8B CD"), unlimitedmpmem, cbunlimitedmpmem)
	{
		if (!initialized())
			throw std::exception("unlimitedmp::unlimitedmp: could not find unlimited mp address.");
	}

	unlimitedmp::~unlimitedmp()
	{
		// empty
	}
}
