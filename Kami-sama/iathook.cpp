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

#include "iathook.hpp"
#include "utils.h"

namespace memory
{
	iathook::iathook(void **pfunction, void *destination)
		: pfunction(pfunction), 
		  destination(destination)
	{
		// empty
	}

	iathook::~iathook()
	{
		if (*pfunction == destination)
			enable(false);
	}

	void iathook::setpfunction(void **pfunction)
	{
		this->pfunction = pfunction;
	}

	void iathook::setdestination(void *destination)
	{
		this->destination = destination;
	}

	void *iathook::getpointervalue()
	{
		return *pfunction;
	}

	bool iathook::enable(bool enabled)
	{
		if (pfunction == NULL || destination == NULL)
			return false;

		//if (!enabled && originalvalue == NULL) // disabling a hook that was never enabled
			//return false;

		memory::makepagewritable(pfunction, sizeof(void *));

		void *valuetowrite = enabled ? destination : originalvalue;
		originalvalue = enabled ? *pfunction : NULL;
		*pfunction = valuetowrite;

		return *pfunction == valuetowrite;
	}
}
