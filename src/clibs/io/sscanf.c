/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <string.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

int _RTL_FUNC scanf(const char* format, ...) { return __scanf(stdin, format, ((char*)&format + sizeof(char*))); }

int _RTL_FUNC vsscanf(char* restrict buf, const char* restrict format, va_list list)
{
    int rv;
    FILE fil;
    struct __file2 fil2;
    memset(&fil, 0, sizeof(fil));
    memset(&fil2, 0, sizeof(fil2));
    fil.level = strlen(buf) + 1;
    fil.flags = _F_IN | _F_READ | _F_BUFFEREDSTRING;
    fil.bsize = strlen(buf);
    fil.buffer = fil.curp = buf;
    fil.token = FILTOK;
    fil.extended = &fil2;
    return __scanf_unlocked(&fil, format, list);
}
int _RTL_FUNC sscanf(char* restrict buf, const char* restrict format, ...)
{
    return vsscanf(buf, format, (((char*)&format) + sizeof(char*)));
}
int vfscanf(FILE* restrict fil, const char* restrict format, va_list arglist) { return __scanf(fil, format, arglist); }
int vscanf(const char* restrict format, va_list arglist) { return __scanf(stdin, format, arglist); }
int fscanf(FILE* restrict fil, const char* restrict format, ...) { return __scanf(fil, format, ((char*)&format + sizeof(char*))); }
