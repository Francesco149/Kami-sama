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

#include "player.hpp"
#include "aobscan.hpp"

#define basenull() (TSingleton_CUserLocal___ms_pInstance == NULL)
makesingletoninstance(maple::player)

namespace maple
{
	player::player()
		: TSingleton_CUserLocal___ms_pInstance(NULL)
	{
		void *pmodule = maple::base();
		size_t cbmodule = maple::size();

		memory::aobscan base("8B 3D ? ? ? ? 8B 40 08 8B 17", pmodule, cbmodule);

		if (!base.result())
			throw std::exception("player::player: could not find player base!");

		TSingleton_CUserLocal___ms_pInstance = *reinterpret_cast<byte ** *>(base.result() + 2);
	}

	player::~player()
	{
		// empty
	}

	void *player::getteleportbase()
	{
		try
		{
			if (basenull())
				return NULL;

			// this offset is static
			return *TSingleton_CUserLocal___ms_pInstance + 4;
		}
		catch (...)
		{
			return NULL;
		}
	}
}
