/**
 *	@file		HalfFloat.cpp
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) 2014-2016 Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 *	@copyright
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	@copyright
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *	@copyright
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"

#include "HalfFloat.h"
using namespace Synesthesia3D;

HalfFloat::HalfFloat(const float value)
{
	assert(sizeof(HalfFloat) == sizeof(unsigned short));

	unsigned int bits = *(unsigned int*)&value;
	unsigned short biasExp = (unsigned short)((bits & 0x7F800000) >> 23);
	if (biasExp >= 0x0071)
	{
		if (biasExp <= 0x008E)
		{
			if (biasExp != 0)
			{
				// Truncate 23-bit mantissa to 10 bits.
				unsigned short signBit	= (unsigned short)((bits & 0x80000000) >> 16);
				unsigned short mantissa	= (unsigned short)((bits & 0x007FFFFF) >> 13);
				biasExp = (biasExp - 0x0070) << 10;
				m_hValue = signBit | biasExp | mantissa;
			}
			else
			{
				// E = 0 (alternate encoding of zero, M does not matter)
				m_hValue = (unsigned short)0;
			}
		}
		else
		{
			// E = 30, M = 1023 (largest magnitude half-float)
			unsigned short signBit =
				(unsigned short)((bits & 0x80000000) >> 16);
			m_hValue = signBit | (unsigned short)0x7BFF;
		}
	}
	else
	{
		// E = 1, M = 0 (smallest magnitude half-float)
		unsigned short signBit = (unsigned short)((bits & 0x80000000) >> 16);
		m_hValue = signBit | (unsigned short)0x0400;
	}
}

HalfFloat::operator float() const
{
	assert(sizeof(HalfFloat) == sizeof(unsigned short));

	unsigned int biasExp = (unsigned int)(m_hValue & 0x7C00) >> 10;
	if (biasExp != 0)
	{
		unsigned int signBit = (unsigned int)(m_hValue & 0x8000) << 16;
		unsigned int mantissa = (unsigned int)(m_hValue & 0x03FF) << 13;
		biasExp = (biasExp + 0x0070) << 23;
		unsigned int result = signBit | biasExp | mantissa;
		return *(float*)&result;
	}
	else
	{
		// E = 0 (alternate encoding of zero, M does not matter)
		return 0.0f;
	}
}
