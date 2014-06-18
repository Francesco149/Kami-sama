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
#include "utils.hpp"
#include "iathook.hpp"

#include <boost/thread/mutex.hpp>

#include <Windows.h>
#include <wx/msw/winundef.h>

namespace maple
{
	class makesingleton(itemhook), public memory::iathook
	{
	public:
		itemhook();
		virtual ~itemhook();
		POINT getcoords();

	protected:
		boost::mutex mut;
		void *retaddy;
		POINT pt;

		static BOOL WINAPI PartyInRectum(_In_ const RECT *lprc, _In_ POINT pt);
		BOOL handle_PtInRect(void *returnaddress, _In_ const RECT *lprc, _In_ POINT pt);
	};
}
