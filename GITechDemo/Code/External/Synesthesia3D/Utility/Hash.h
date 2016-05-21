/**
*	@file		Hash.h
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

#ifndef HASH_H_
#define HASH_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define S3DH1(s,i,x)	(x*65599u+(uint8_t)s[(i)<strlen(s)?strlen(s)-1-(i):strlen(s)])	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
#define S3DH4(s,i,x)	S3DH1(s,i,S3DH1(s,i+1,S3DH1(s,i+2,S3DH1(s,i+3,x))))				/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
#define S3DH16(s,i,x)	S3DH4(s,i,S3DH4(s,i+4,S3DH4(s,i+8,S3DH4(s,i+12,x))))			/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
#define S3DH64(s,i,x)	S3DH16(s,i,S3DH16(s,i+16,S3DH16(s,i+32,S3DH16(s,i+48,x))))		/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */
#define S3DH256(s,i,x)	S3DH64(s,i,S3DH64(s,i+64,S3DH64(s,i+128,S3DH64(s,i+192,x))))	/**< @brief DO NOT USE! INTERNAL USAGE ONLY! */

/**
 * @brief Used for hashing shader input names.
 * @see Synesthesia3D::ShaderInput::GetInputHandleByNameHash()
 */
#define S3DHASH(s)		((uint32_t)(S3DH256(s,0,0)^(S3DH256(s,0,0)>>16)))

#endif // HASH_H_
