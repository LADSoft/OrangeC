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
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <dpmi.h>
#include "llp.h"

int __ll_rename(wchar_t *old , wchar_t *new)
{
    char buf[256], *p = buf, buf1[256], *q= buf1;
    while (*old)
        *p++ = *old++;
    *p = *old;
    while (*new)
        *q++ = *new++;
    *q = *new;
    DPMI_REGS regs;
    regs.h.dx = __nametodta(buf,0);
    regs.h.di = __nametodta(buf1,256);
    __doscall(0x56,&regs);
    if (regs.h.flags & 1)
    {
        errno = regs.h.ax & 255;
        return 1;
    }
    
    return 0;
}
