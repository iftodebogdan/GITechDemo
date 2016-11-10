/**
 *	@file		HalfFloat.h
 *
 *	@note		This file is part of the "Synesthesia3D" graphics engine
 *
 *	@copyright	Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
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

#ifndef HALFFLOAT_H
#define HALFFLOAT_H

namespace Synesthesia3D
{
	/**
	 * @brief	Utility class for encoding and decoding 16 bit floating-point numbers.
	 */
	class HalfFloat
	{
		unsigned short m_hValue;	/**< @brief 16 bit floating-point value stored in an unsigned short. */

	public:
		/**
		 * @brief Constructor
		 */
		HalfFloat() { m_hValue = 0; }

		/**
		 * @brief Destructor
		 */
		~HalfFloat() { m_hValue = 0; }

		/**
		 * @brief Constructor
		 *
		 * @param[in]	value	Source 32 bit floating-point value to be encoded.
		 */
		HalfFloat(const float value);

		/**
		 * @brief Operator used for converting a 16 bit floating-point value into a 32 bit floating-point value.
		 */
		operator float() const;
	};
}

#endif // HALFFLOAT_H
