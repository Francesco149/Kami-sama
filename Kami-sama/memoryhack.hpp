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

#pragma once
#include "common.h"
#include <boost/scoped_array.hpp>

namespace memory
{
	class memoryhack
	{
	public:
		memoryhack(byte *address, byte *modifiedmemory, size_t count);
		virtual ~memoryhack();
		bool enable(bool enabled); // returns false if it fails to write the memory

	protected:
		bool enabled;
		byte *address;
		size_t count;
		boost::scoped_array<byte> modified;
		boost::scoped_array<byte> clean;
	};
}
