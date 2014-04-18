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

namespace maple
{
	// wrapper to read and write data to TSingleton<CUserLocal>
	class player
	{
	public:
		static boost::shared_ptr<player> get();
		virtual ~player();

		// some class instance used to retrieve the instance to call CVecCtrlUser__OnTeleport
		// I'm not sure what it actually is.
		void *getteleportbase();

	protected:
		static boost::shared_ptr<player> inst;
		byte **TSingleton_CUserLocal___ms_pInstance; // TSingleton<CUserLocal> (char base)

		player();
	};
}
