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

#include "memoryhack.hpp"
#include "utils.h"
#include <memory>

namespace memory
{
	memoryhack::memoryhack(byte *address, byte *modifiedmemory, size_t count)
		: address(address), count(count)
	{
		modified.reset(new byte[count]);
		clean.reset(new byte[count]);

		makepagewritable(address, count);

		std::copy(modifiedmemory, modifiedmemory + count, modified.get());
		std::copy(address, address + count, clean.get());
	}

	memoryhack::~memoryhack()
	{
		if (enabled)
			enable(false);
	}

	bool memoryhack::enable(bool enabled)
	{
		if (this->enabled == enabled)
			return false;

		if (enabled)
			std::copy(modified.get(), modified.get() + count, address);
		else
			std::copy(clean.get(), clean.get() + count, address);

		if (std::memcmp(address, enabled ? modified.get() : clean.get(), count) != 0)
			return false;

		this->enabled = enabled;
		return true;
	}
}
