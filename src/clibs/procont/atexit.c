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

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

#define MAX_ATEXIT 100

extern int _RTL_DATA _abterm;
extern int __quick_exit;

static int atexitval;
static void (*funclist[MAX_ATEXIT])(void);

#pragma rundown __procexit 225

void __procexit(void)
{
    int i;
    __ll_enter_critical();
    if (!_abterm && !__quick_exit)
    {
        for (i = atexitval; i > 0; i--)
            (*funclist[i - 1])();
    }
    __ll_exit_critical();
}

int _RTL_FUNC atexit(atexit_t func)
{
    __ll_enter_critical();
    if (atexitval < MAX_ATEXIT)
    {
        funclist[atexitval++] = func;
        __ll_exit_critical();
        return 0;
    }
    __ll_exit_critical();
    return -1;
}
