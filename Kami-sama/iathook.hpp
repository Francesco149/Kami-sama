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
#include <cstddef>

namespace memory
{
	// abstract class for IAT hooks
	class iathook
	{
	public:
		iathook(void **pfunction = NULL, void *destination = NULL);
		virtual ~iathook();
		void setpfunction(void **pfunction);
		void setdestination(void *destination);
		void *getpointervalue();

		// returns false if pfunction or destination are not initialized or of memory is not writable
		// or if you're trying to disable an hook that was never enabled
		bool enable(bool enabled);

	protected:
		void **pfunction;
		void *destination;
		void *originalvalue;
	};
}
