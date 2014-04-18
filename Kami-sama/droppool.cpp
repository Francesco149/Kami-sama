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

#include "droppool.hpp"
#include "aobscan.hpp"
#include "utils.h"

#define basenull() (TSingleton_CDropPool___ms_pInstance == NULL)

namespace maple
{
	boost::shared_ptr<droppool> droppool::inst;

	boost::shared_ptr<droppool> droppool::get()
	{
		if (!inst.get())
			inst.reset(new droppool);

		return inst;
	}

	droppool::droppool()
		: TSingleton_CDropPool___ms_pInstance(NULL)
	{
		void *pmodule = maple::base();
		size_t cbmodule = maple::size();

		memory::aobscan base("89 3D ? ? ? ? 8D 4E ? C7 06", pmodule, cbmodule);

		if (!base.result())
			throw std::exception("droppool::droppool: could not find item base!");

		TSingleton_CDropPool___ms_pInstance = *reinterpret_cast<byte ** *>(base.result() + 2);

		memory::aobscan size("8B 4C 24 ? 83 C1 F8 83 F9 50 77 ? 0F ? ? ? ? ? ? FF", pmodule, cbmodule);

		if (!size.result())
			throw std::exception("droppool::droppool: could not find item base!");

		offsize = *(size.result() + 3);
	}

	droppool::~droppool()
	{
		// empty
	}

	dword droppool::size()
	{
		try
		{
			if (basenull())
				return 0;
		
			return *reinterpret_cast<dword *>(*TSingleton_CDropPool___ms_pInstance + offsize);
		}
		catch (...)
		{
			return 0;
		}
	}
}
