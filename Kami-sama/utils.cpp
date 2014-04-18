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

#include "utils.h"

#include <Windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <psapi.h>
#pragma  comment(lib, "dbghelp")
#pragma  comment(lib, "psapi")
#include <exception>

#define jmp(frm, to) (reinterpret_cast<int>(to) - reinterpret_cast<int>(frm) - 5)

namespace maple
{
	void *pmaplebase = NULL;
	size_t cbmaplesize = 0;

	HWND wnd()
	{
		TCHAR buf[256] = {0};
		DWORD procid;

		for (HWND hwnd = GetTopWindow(NULL); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT))
		{
			GetWindowThreadProcessId(hwnd, &procid);

			if (procid != GetCurrentProcessId()) 
				continue;

			if (!GetClassName(hwnd, buf, 256)) 
				continue;

			if (_tcscmp(buf, _T("MapleStoryClass")) != 0) 
				continue;

			return hwnd;
		}

		return NULL;
	}

	bool getmoduleinfo(void **pbase, size_t *psize)
	{
		return memory::getmodulesize(GetModuleHandle(NULL), pbase, psize);
	}

	void tryinitmoduleinfo()
	{
		if (pmaplebase != NULL)
			return;

		if (!getmoduleinfo(&pmaplebase, &cbmaplesize))
			throw std::exception("tryinitmoduleinfo: failed to retrieve module base / size!");

		if (pmaplebase == NULL || cbmaplesize <= 0)
			throw std::exception("tryinitmoduleinfo: invalid module base / size!");
	}

	void *base()
	{
		tryinitmoduleinfo();
		return pmaplebase;
	}

	size_t size()
	{
		tryinitmoduleinfo();
		return cbmaplesize;
	}
}

namespace memory
{
	bool getmodulesize(HMODULE hModule, void **pbase, size_t *psize)
	{
		if (hModule == GetModuleHandle(NULL))
		{
			PIMAGE_NT_HEADERS pImageNtHeaders = ImageNtHeader((PVOID)hModule);

			if (pImageNtHeaders == NULL)
				return false;

			*pbase = reinterpret_cast<void *>(hModule);
			*psize = pImageNtHeaders->OptionalHeader.SizeOfImage;
		}
		else
		{
			MODULEINFO ModuleInfo;

			if (!GetModuleInformation(GetCurrentProcess(), hModule, &ModuleInfo, sizeof(MODULEINFO)))
				return FALSE;

			*pbase = ModuleInfo.lpBaseOfDll;
			*psize = ModuleInfo.SizeOfImage;
		}

		return true;
	}

	dword makepagewritable(void *address, size_t cb, dword flprotect) 
	{
		MEMORY_BASIC_INFORMATION mbi = {0};
		VirtualQuery(address, &mbi, cb);

		if (mbi.Protect != flprotect)
		{
			DWORD oldprotect;
			VirtualProtect(address, cb, flprotect, &oldprotect);
			return oldprotect;
		}

		return flprotect;
	}

	void writeopcodewithdistance(byte opcode, byte *address, void *destination, size_t nops)
	{
		makepagewritable(address, 5 + nops);
		*address = 0xE9;
		*reinterpret_cast<dword *>(address + 1) = jmp(address, destination);
		memset(address + 5, 0x90, nops);
	}

	void writejmp(byte *address, void *hook, size_t nops)
	{
		writeopcodewithdistance(0xE9, address, hook, nops);
	}
}
