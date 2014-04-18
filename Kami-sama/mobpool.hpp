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
#include <boost/shared_ptr.hpp>

struct tagPOINT;
typedef tagPOINT POINT;

namespace maple
{
	// wrapper to read and write data to TSingleton<CMobPool>
	class mobpool
	{
	public:
		static boost::shared_ptr<mobpool> get();
		virtual ~mobpool();

		dword size();
		bool getcoords(POINT *ppt);

	protected:
		static boost::shared_ptr<mobpool> inst;
		byte **TSingleton_CMobPool___ms_pInstance; // TSingleton<CMobPool> (mob base)
		word offsize;
		word offmob1;
		word offmob2;
		word offmob3;
		word offmob4;
		word offinvx;
		word offinvy;
		word offmobx;
		word offmoby;

		mobpool();
	};
}
