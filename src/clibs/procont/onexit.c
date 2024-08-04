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

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern int _RTL_DATA _abterm;
static struct _onx_l
{
    struct _onx_l* next;
    onexit_t func;
} * onx_list;

#pragma rundown __onprocexit 225

void __onprocexit(void)
{
    int i;
    __ll_enter_critical();
    if (!_abterm)
    {
        while (onx_list)
        {
            (*onx_list->func)();
            onx_list = onx_list->next;
        }
    }
    __ll_exit_critical();
}

onexit_t _RTL_FUNC onexit(onexit_t func)
{
    struct _onx_l* l = malloc(sizeof(struct _onx_l));
    if (!l)
        return NULL;
    __ll_enter_critical();
    l->next = onx_list;
    onx_list = l;
    l->func = func;
    __ll_exit_critical();
    return func;
}
_onexit_t _RTL_FUNC _onexit(_onexit_t func) { return onexit(func); }
