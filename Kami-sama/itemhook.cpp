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

#include "itemhook.hpp"
#include "aobscan.hpp"
#include <Windows.h>

// AoBs:
// retaddy: add 6 to result: FF 15 ? ? ? ? 85 C0 75 ? 8D 4C 24 ? C7 44 24 ? ? ? ? ? E8 ? ? ? ? EB ? 83 C6 ? 8D 7E ? 57
// pfunction: pointer called above retaddy

makesingletoninstance(maple::itemhook)

namespace maple
{
	itemhook::itemhook()
		: memory::iathook(
			  NULL, 
			  PartyInRectum
		  )
	{
		void *pmodule = maple::base();
		size_t cbmodule = maple::size();

		SecureZeroMemory(&pt, sizeof(POINT));

		memory::aobscan aobhook("FF 15 ? ? ? ? 85 C0 75 ? 8D 4C 24 ? C7 44 24 ? ? ? "
			"? ? E8 ? ? ? ? EB ? 83 C6 ? 8D 7E ? 57", pmodule, cbmodule);

		if (!aobhook.result())
			throw std::exception("itemhook::itemhook: could not find item hook pointer!");

		setpfunction(*reinterpret_cast<void ** *>(aobhook.result() + 2));
		retaddy = aobhook.result() + 6;
	}

	itemhook::~itemhook()
	{
		boost::mutex::scoped_lock lock(mut); // ensures no other concurrent calls to ptinrect are running
	}

	POINT itemhook::getcoords()
	{
		boost::mutex::scoped_lock lock(mut);
		return pt;
	}

	BOOL itemhook::handle_PtInRect(void *returnaddress, _In_ const RECT *lprc, _In_ POINT pt)
	{
		// this is the call we're looking for, the game is checking if the item is colliding with the character
		if (returnaddress == retaddy)
		{
			boost::mutex::scoped_lock lock(mut);
			this->pt = pt; // store item X/Y
		}

		return PtInRect(lprc, pt);
	}

	BOOL WINAPI itemhook::PartyInRectum(_In_ const RECT *lprc, _In_ POINT pt)
	{
		void *ret = _ReturnAddress();
		return get()->handle_PtInRect(ret, lprc, pt);
	}
}
