/**
 * @file        Debug.h
 *
 * @note        This file is part of the "Synesthesia3D" graphics engine
 *
 * @copyright   Copyright (C) Iftode Bogdan-Marius <iftode.bogdan@gmail.com>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * @copyright
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * @copyright
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEBUG_H_

    #ifdef _DEBUG
        #define S3D_DBGPRINT(szDebugFormatString, ...) _S3D_DBGPRINT(__FILE__, __LINE__, szDebugFormatString, __VA_ARGS__)
        void _S3D_DBGPRINT(char* szFile, int nLineNumber, char* szDebugFormatString, ...);
    #else
        #define S3D_DBGPRINT( kwszDebugFormatString, ... ) ((void)0)
    #endif

    #ifndef assert
        #define assert(_Expression) ((void)0)
    #endif

#endif
