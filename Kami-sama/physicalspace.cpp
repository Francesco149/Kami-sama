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

#include "physicalspace.hpp"
#include "aobscan.hpp"
#include "utils.h"

#define basenull() (TSingleton_CWvsPhysicalSpace2D___ms_pInstance == NULL)

namespace maple
{
	boost::shared_ptr<physicalspace> physicalspace::inst;

	boost::shared_ptr<physicalspace> physicalspace::get()
	{
		if (!inst.get())
			inst.reset(new physicalspace);

		return inst;
	}

	physicalspace::physicalspace()
		: TSingleton_CWvsPhysicalSpace2D___ms_pInstance(NULL)
	{
		void *pmodule = maple::base();
		size_t cbmodule = maple::size();

		memory::aobscan base("A1 ? ? ? ? 8B 50 ? 83 C0", pmodule, cbmodule);

		if (!base.result())
			throw std::exception("physicalspace::physicalspace: could not find wall base!");

		TSingleton_CWvsPhysicalSpace2D___ms_pInstance = *reinterpret_cast<byte ** *>(base.result() + 1);
		offltwallx = *(base.result() + 7);
		offltwally = offltwallx + 4;
	}

	physicalspace::~physicalspace()
	{
		// empty
	}

	POINT physicalspace::getltwall()
	{
		try
		{
			POINT pt = {0};

			if (!basenull())
			{
				pt.x = *reinterpret_cast<long *>(*TSingleton_CWvsPhysicalSpace2D___ms_pInstance + offltwallx);
				pt.y = *reinterpret_cast<long *>(*TSingleton_CWvsPhysicalSpace2D___ms_pInstance + offltwally);
			}

			return pt;
		}
		catch (...)
		{
			POINT pt = {0};
			return pt;
		}
	}
}
