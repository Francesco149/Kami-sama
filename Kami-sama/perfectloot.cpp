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

#include "perfectloot.hpp"

makesingletoninstance(maple::perfectloot)

namespace maple
{
	const byte tubimem[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	const size_t cbtubimem = sizeof(tubimem);

	const byte instadropmem[] = { 0x25 };
	const size_t cbinstadropmem = sizeof(instadropmem);

	const byte nolootanimmem[] = { 0x81, 0xFE, 0x00, 0x00, 0x00, 0x00 };
	const size_t cbnolootanimmem = sizeof(nolootanimmem);

	perfectloot::perfectloot()
		: tubi(std::string("89 86 ? ? ? ? E8 ? ? ? ? 89 86 ? ? ? ? 5E C2 ? ? CC CC CC CC"), tubimem, cbtubimem), 
		  instadrop(std::string("0D ? ? ? ? 83 C4 ? E9 ? ? ? ? DD 05 ? ? ? ? DC C9"), instadropmem, cbinstadropmem), 
		  nolootanim(std::string("81 FE ? ? ? ? 0F 8D ? ? ? ? 85 ED 0F 84 ? ? ? ? 81 "
			"FE ? ? ? ? BF ? ? ? ? 7E ? B8 ? ? ? ? 2B C6"), nolootanimmem, cbnolootanimmem)
	{
		if (!tubi.initialized())
			throw std::exception("perfectloot::perfectloot: could not find tubi address.");

		if (!instadrop.initialized())
			throw std::exception("perfectloot::perfectloot: could not find instadrop address.");

		if (!nolootanim.initialized())
			throw std::exception("perfectloot::perfectloot: could not find nolootanim address.");
	}

	perfectloot::~perfectloot()
	{
		// empty
	}

	bool perfectloot::enable(bool enabled)
	{
		bool res = tubi.enable(enabled) && instadrop.enable(enabled) && nolootanim.enable(enabled);

		if (!res)
		{
			tubi.enable(!enabled);
			instadrop.enable(!enabled);
			nolootanim.enable(!enabled);
		}

		return res;
	}
}
