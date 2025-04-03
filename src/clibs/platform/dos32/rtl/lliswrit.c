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
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <time.h>
#include <dpmi.h>
#include <wchar.h>
#include <locale.h>
#include <dos.h>
#include "libp.h"

int __ll_writeable(const wchar_t *file)
{
    char buf[260], *p = buf;
    while (*file)
        *p++ = *file++;
    *p = *file;
    DPMI_REGS regs;
   regs.b.al = 0 ;
   regs.h.dx = __nametodta(buf,0) ;
   __doscall(0x43,&regs);
   if (regs.b.al != 0)
      return 1 ;
   return !(regs.h.cx & 1) ;
}
