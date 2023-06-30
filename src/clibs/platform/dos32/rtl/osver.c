/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <dpmi.h>
 #include "llp.h"

unsigned char _RTL_DATA _osmajor, _RTL_DATA _osminor ;

#pragma startup checkver 31

/* this is only required if we don't use pmode/308 */

static void checkver(void)
{
    DPMI_REGS regs;
    __doscall(0x30,&regs);
    if (regs.b.al < 3) {
        fprintf(stderr,"This program requires DOS 3.0 or better");
        exit(255);
    }
   _osmajor = regs.b.al ;
   _osminor = regs.b.ah ;
}
