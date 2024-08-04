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
#include <dir.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

void _RTL_FUNC 	_searchenv(const char *file, const char *inpath, char *output)
{
    char *path = getenv(inpath),*p=path,*q;
    FILE *t;
    strcpy(output,file);
    t = fopen(output,"rb");
    if (t) {
        fclose(t);
        if (!strchr(output,'\\')) {
            getcwd(output,255);
            strcat(output,"\\");
            strcat(output,file) ;
        }
        return ;
    }
    else while (*p) {
        q = output;
        while (*p != ';' && *p)
            *q++=*p++;
        if (*p) p++;
        if (*(q-1) != '\\')
            *q++ = '\\';
        strcpy(q,file);
        if ((t = fopen(output,"rb")) != 0) {
            fclose(t);
//			q[0] = 0 ;
            return ;
        }
    }
    *output = 0 ;
    return ;
}
