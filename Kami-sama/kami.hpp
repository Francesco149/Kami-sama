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

#include "itemhook.hpp"
#include "memoryhack.hpp"
#ifdef BYPASSLESS_KAMI
#include "iathook.hpp"
#endif

#include "player.hpp"
#include "mobpool.hpp"
#include "physicalspace.hpp"
#include "droppool.hpp"
#include "window.hpp"

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Windows.h>
#include <wx/msw/winundef.h>

namespace maple
{
	namespace pt = boost::posix_time;

#ifdef BYPASSLESS_KAMI
	class makesingleton(kami), public memory::iathook
#else
	class makesingleton(kami), public memory::memoryhack
#endif
	{
	public:
		kami();
		virtual ~kami();
		void teleport(long x, long y);
		void teleporttomobs(bool loot, long xoff, long yoff, bool autoattack, bool spoofthread);
		void startsticking(bool loot, long xoff, long yoff, dword microsecondsdelay, bool clientsided, bool autoattack);
		void stopsticking();
		void hook(bool loot, long xoff, long yoff, dword microsecondsdelay, bool clientsided, bool autoattack);
		void unhook();

	protected:
		boost::shared_ptr<itemhook> pitemhook;
		boost::shared_ptr<player> pplayer;
		boost::shared_ptr<mobpool> pmobpool;
		boost::shared_ptr<physicalspace> pphysicalspace;
		boost::shared_ptr<droppool> pdroppool;
		boost::shared_ptr<window> pwindow;

		boost::shared_ptr<boost::thread> hworker;

		// NOTE: I'm not 100% sure these funcs are actually working on a CVecCtrlUser instance because
		// I couldn't find OnTeleport on the unpacked client, but OnTeleport is close to CVecCtrlUser::OnSit in memory so it could be correct
		// it could also be a totally changed version of CUserLocal::TryDoingTeleport

		// some func that returns a CVecCtrlUser instance, not sure what the real name is, it takes (*charbase + 4) as instance
		typedef void * (__fastcall *pfnGetCVecCtrlUser)(void *instance, void *edx);

		// void CVecCtrlUser::OnTeleport(long, long). Not sure if it's the real name of the function, it basically looks the same as OnSit
		typedef void (__fastcall *pfnCVecCtrlUser__OnTeleport)(void *instance, void *edx, void *, long x, long y);

		pfnGetCVecCtrlUser GetCVecCtrlUser;
		pfnCVecCtrlUser__OnTeleport CVecCtrlUser__OnTeleport;

		boost::shared_ptr<memory::memoryhack> blockmovement;

		void worker(bool loot, long xoff, long yoff, dword microsecondsdelay, bool autoattack);

		bool loot; 
		long xoff; 
		long yoff; 
		dword microsecondsdelay; 
		bool autoattack;
		pt::ptime lastteleport;

		void dokamistep();
		static void handlekamicall();

		static dword kamiret;
		static dword kamiretcheck;
		static void kamihook();
	};
}
