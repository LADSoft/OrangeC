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

#include <errno.h>
#include <windows.h>
#include <dos.h>
#include <dir.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

char  *_RTL_FUNC      getcwd( char  *__buf, int __buflen )
{
    int drv,rv;
    char ibuf[265];
    drv = getdisk() ;
    rv = getcurdir(0,ibuf+3);
    ibuf[0] = drv + 'A';
    ibuf[1] = ':';
    ibuf[2] = '\\';
    if (!__buf)
        __buf = strdup(ibuf) ;
    else {
        memcpy(__buf,ibuf,__buflen);
        __buf[__buflen-1] = 0;
    }
   return __buf;
}
char  *_RTL_FUNC      _getcwd( char  *__buf, int __buflen )
{
    return getcwd(__buf, __buflen);
}
