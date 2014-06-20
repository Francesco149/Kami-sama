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

#include "moblag.hpp"

makesingletoninstance(maple::moblag)

namespace maple
{
	const byte lagmem[] = { 0xEB };
	const size_t cblagmem = sizeof(lagmem);

	moblag::moblag()
		: memory::memoryhack(std::string("77 ? 89 54 24 ? E9 ? ? ? ? 8B FF"), lagmem, cblagmem, 0, 1)
	{
		if (!initialized())
			throw std::exception("moblag::moblag: could not find lag address.");
	}

	moblag::~moblag()
	{
		// empty
	}
}
