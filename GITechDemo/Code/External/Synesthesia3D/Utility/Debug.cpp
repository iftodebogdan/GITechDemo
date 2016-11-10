/**
 * @file        Debug.cpp
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

#include "stdafx.h"

#include "Debug.h"

#ifdef WIN32
    #include <Strsafe.h>
#endif

#ifdef _DEBUG
    void _S3D_DBGPRINT(char* szFile, int nLineNumber, char* szDebugFormatString, ...)
    {
    #ifdef WIN32
        #define DBGPRINT_HEADER "%s(%d): [S3D] "
        INT cbFormatString = 0;
        va_list args;
        STRSAFE_LPSTR szDebugString = NULL;
        size_t st_Offset = 0;
    
        va_start(args, szDebugFormatString);
    
        cbFormatString = _scprintf(DBGPRINT_HEADER, szFile, nLineNumber) * sizeof(STRSAFE_LPSTR);
        cbFormatString += _vscprintf(szDebugFormatString, args) * sizeof(STRSAFE_LPSTR) + 2;
    
        /* Depending on the size of the format string, allocate space on the stack or the heap. */
        szDebugString = (STRSAFE_LPSTR)_malloca(cbFormatString);
    
        /* Populate the buffer with the contents of the format string. */
        StringCbPrintf(szDebugString, cbFormatString, DBGPRINT_HEADER, szFile, nLineNumber);
        StringCbLength(szDebugString, cbFormatString, &st_Offset);
        StringCbVPrintf(&szDebugString[st_Offset], cbFormatString - st_Offset, szDebugFormatString, args);
    
        OutputDebugString(szDebugString);
    
        _freea(szDebugString);
        va_end(args);
    #endif
    }
#endif
