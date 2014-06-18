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

#include "utils.hpp"
#include "memoryhack.hpp"
#include <list>

namespace maple
{
	class makesingleton(skillinject), public memory::memoryhack
	{
	public:
		skillinject();
		virtual ~skillinject();
		virtual bool enable(bool enabled);
		void setskillid(dword skillid);
		void setdelay(dword delay);

	protected:
		std::list<memory::memoryhack * const> haxlist;

		memory::memoryhack skillinject1, skillinject2, skillinject3, skillinject4, 
			skillinject5, skillinject6, skillinject7, skillinject8;

		// TODO: don't make these static and use getters/setters
		static dword skillid;
		static dword delay;
		static dword movoffset;
		static dword retaddy;
		static dword counter;
		static void hook();
	};
}
