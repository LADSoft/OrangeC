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

#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <signal.h>
#include "libp.h"
extern sighandler_t __sigtab[NSIG];
extern const sighandler_t __defsigtab[NSIG];

sighandler_t _RTL_FUNC signal(int signum, sighandler_t func)
{
    sighandler_t temp;
   if (signum >= NSIG || signum < 1) {
        errno = EINVAL;
        return SIG_ERR;
    }
    temp = __sigtab[signum];
    if (func == SIG_DFL) 
        __sigtab[signum] = __defsigtab[signum];
    else    /* SIG_IGN && SIG_ERR gets used as-is, 
                   otherwise func is a valid func */
        __sigtab[signum] = func;
   __ll_signal(signum,func) ;
    return temp;
}
