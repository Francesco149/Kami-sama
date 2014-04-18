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

#include "aobscan.hpp"

#include <cctype>
#include <cstdlib>

namespace memory
{
	aobscan::aobscan(const std::string &pattern, void *pimagebase, size_t imagesize, int index)
		: pimagebase(reinterpret_cast<byte *>(pimagebase)),
		  imagesize(imagesize),
		  pattern(pattern),
		  presult(NULL),
		  res(aobscan::none),
		  mask(reinterpret_cast<char *>(NULL)), // not sure why I have to cast here
		  data(reinterpret_cast<byte *>(NULL))

	{
		bool invalid = false;

		countpatternbytes();
		invalid = invalid || !length;

		// initialize mask & data buffers
		mask.reset(new char[length + 1]);
		data.reset(new byte[length]);
		memset(mask.get(), 0, length + 1);
		memset(data.get(), 0, length);

		invalid = invalid || !makepatternmask();
		invalid = invalid || !makepatternbytes();

		if (invalid)
			res = aobscan::invalid;

		searchpattern(index);

		if (!presult)
			res = aobscan::not_found;

		res = aobscan::none;
	}

	aobscan::~aobscan()
	{
		// empty
	}

	byte *aobscan::result() const
	{
		return presult;
	}

	aobscan::error aobscan::geterror() const
	{
		return res;
	}

	std::string aobscan::string() const
	{
		return pattern;
	}

	size_t aobscan::bytecount() const
	{
		return length;
	}

	boost::shared_array<byte> aobscan::bytearray() const
	{
		return data;
	}

	boost::shared_array<char> aobscan::maskstring() const
	{
		return mask;
	}

	void aobscan::countpatternbytes()
	{
		size_t cb = 0;
		bool firstnibble = false;

		for (size_t i = 0; i < pattern.length(); i++)
		{
			char c;
		
			c = std::toupper(pattern[i]);

			if (c == ' ') // ignore whitespace
				continue;

			if (c == '?') // wildcard bytes
			{
				if (firstnibble) // unexpected ? after a nibble
				{
					length = 0;
					return;
				}

				cb++; // wildcard byte - increase count
			}

			else // regular bytes
			{
				if (!std::isxdigit(c)) // invalid non-hex byte
				{
					length = 0;
					return;
				}

				if (firstnibble) // regular byte - increase count
					cb++;

				firstnibble ^= true;
			}
		}

		if (firstnibble) // invalid truncated last nibble
		{
			length = 0;
			return;
		}

		length = cb;
	}

	bool aobscan::makepatternmask()
	{
		bool firstnibble = false;

		for (size_t i = 0; i < pattern.length(); i++)
		{
			char c;
		
			c = std::toupper(pattern[i]);
		
			if (c == ' ') // ignore whitespace
				continue;

			if (c == '?') // wildcard bytes
			{
				if (firstnibble) // invalid ? after a nibble
					return false;

				strcat_s(mask.get(), length + 1, "?"); // wildcard
			}

			else // regular bytes
			{
				if (!std::isxdigit(c)) // invalid non-hex byte
					return false;

				if (firstnibble)
					strcat_s(mask.get(), length + 1, "x"); // regular byte

				firstnibble ^= true;
			}	
		}

		if (firstnibble) // invalid truncated last nibble
			return false;

		return true;
	}

	bool aobscan::makepatternbytes()
	{
		bool firstnibble = false;
		size_t count = 0;

		for (size_t i = 0; i < pattern.length(); i++)
		{
			char c;

			c = toupper(pattern[i]);
		
			if (c == ' ') // ignore whitespace
				continue;

			if (c == '?')
			{
				if (firstnibble) // invalid ? after a nibble
					return false;

				data[count] = 0x00; // wildcard byte
				count++;
			}

			else
			{
				if (!std::isxdigit(c)) // invalid non-hex digit
					return false;

				if (firstnibble)
				{
					char szbyte[3] = {0};

					// convert byte string to byte
					szbyte[0] = pattern[i - 1];
					szbyte[1] = c;
					szbyte[2] = '\0';
					data[count] = static_cast<byte>(std::strtol(szbyte, NULL, 16));
					count++;
				}

				firstnibble ^= true;
			}
		}

		if (firstnibble)
			return false;

		return true;
	}

	void aobscan::searchpattern(int index)
	{
		presult = NULL;
		int n = 0;

		for (byte *i = pimagebase; i < pimagebase + imagesize; i++)
		{
			bool found = true;

			for (size_t j = 0; j < length; j++)
			{
				if (mask[j] != 'x')
					continue; // whitespace or wildcard

				// check byte value
				if (*reinterpret_cast<byte *>(i + j) != data[j])
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				if (n == index)
				{
					presult = i;
					return;
				}
				else n++;
			}
		}
	}
}
