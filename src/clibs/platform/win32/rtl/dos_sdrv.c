/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include <errno.h>
#include <windows.h>
#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

int _RTL_FUNC _dos_setdrive(int drive, int* nd)
{
    // fixme - figure out how many drives.
    char buf[5], *a, equals[256];
    buf[0] = '=';
    buf[1] = drive + '@';
    buf[2] = ':';
    buf[3] = '\\';
    buf[4] = 0;
    if (!GetEnvironmentVariable(buf, equals, 256))
    {
        if (!SetEnvironmentVariable(buf, buf + 1))
            return 1;
        if (!SetCurrentDirectory(buf + 1))
            return 1;
    }
    else if (!SetCurrentDirectory(equals))
        return 1;
    return 0;
}
