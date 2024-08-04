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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
#include <threads.h>
#include <sys\timeb.h>
#include "libp.h"

void _RTL_FUNC call_once(once_flag* flag, void (*func)(void))
{
    __ll_enter_critical();
    if (!flag->called)
    {
        flag->called = 1;
        __ll_exit_critical();
        (*func)();
    }
    else
    {
        __ll_exit_critical();
    }
}
int _RTL_FUNC timespec_get(struct timespec* xt, int base)
{
    if (base == TIME_UTC)
    {
        struct _timeb tb;
        __ll_xtime(&tb);
        xt->tv_nsec = tb.millitm * 1000000;
        xt->tv_sec = tb.time;
        return base;
    }
    return 0;  // error
}
