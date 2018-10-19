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
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <locale.h>
#include <wchar.h>
#include "libp.h"

extern FREELIST* __mallocchains[MEMCHAINS];

void _RTL_FUNC free(void* buf)
{
    FREELIST *p, **c;
    if (!buf)
        return;
    p = buf;

    p--;
    int n = p->size;
    if (n < 0)
    {
        p = (char*)buf + n;
        p--;
    }
    if (p->size == INT_MIN)
        return;
    if (p->next)  // already freed
        return;
    __ll_enter_critical();
    c = &__mallocchains[(p->size >> 2) % MEMCHAINS];
    p->next = *c;
    (*c) = p;
    __ll_exit_critical();
}