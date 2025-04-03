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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include "libp.h"
#include <dpmi.h>
#include <dos.h>

void far* _RTL_FUNC farmalloc(size_t size)
{
    SELECTOR seg = 0;
    void* s = malloc(size);
    if (s)
    {
        if (!dpmi_alloc_descriptors(&seg, 1))
        {
            dpmi_set_sel_access_rights(seg, 0xc092);
            dpmi_set_sel_limit(seg, size - 1);
            dpmi_set_sel_base(seg, (ULONG)s);
        }
        else
        {
            seg = 0;
            free(s);
        }
    }
    return MK_FP(seg, 0);
}
void _RTL_FUNC farfree(void far* ptr)
{
    SELECTOR seg = FP_SEG(ptr);
    ULONG base;
    if (seg == __CS || seg == __DS || seg == __SS)
        return;
    if (!dpmi_get_sel_base(&base, seg))
    {
        free((void*)base);
        dpmi_free_selector(seg);
    }
}