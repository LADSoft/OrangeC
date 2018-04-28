/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include "libp.h"

void * _RTL_FUNC realloc(void *buf, size_t size)
{
    int oldsize, newsize = size ;
    char *ptr ;
    newsize +=7; /* must be the same as in malloc for comparison purposes */
    newsize &= 0xfffffff8;
    if (buf)
        oldsize = (((FREELIST *)buf)-1)->size - sizeof(FREELIST);
    else 
        oldsize = (size_t)-1 ;
    if (oldsize == newsize)
        return buf;
    ptr = malloc(newsize);
    if (ptr && buf) 
    {							/* borland C allows buf to be zero... */
        if (oldsize > size)
            oldsize = size ;
        memcpy(ptr,buf,oldsize);
        free(buf);
    }
    return ptr;
}