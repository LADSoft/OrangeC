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

#include <cstddef>
#include "_rtti.h"
#include <string.h>
#include <stdio.h>

typedef unsigned char BYTE;

static RTTISTRUCT* getStructData(void* xt)
{
    RTTI* p = (RTTI*)xt;
    BYTE* b = (BYTE*)p->name + strlen(p->name) + 1;
    return (RTTISTRUCT*)b;
}

static int findoffs(RTTI* oldxt, RTTI* newxt, int* count)
{
    if (oldxt == newxt)
    {
        (*count)++;
        return 0;
    }
    RTTISTRUCT* oldrs = getStructData(oldxt);
    int offset = -1;
    while (oldrs->flags)
    {
        if (oldrs->flags & XD_CL_BASE)
        {
            if (oldrs->xt == newxt)
            {
                offset = oldrs->offset;
                (*count)++;
            }
            else
            {
                int offset1 = findoffs(oldrs->xt, newxt, count);
                if (offset1 != -1)
                    offset = oldrs->offset + offset1;
            }
        }
        oldrs++;
    }
    return offset;
}
void* _RTL_FUNC __dynamic_cast(void* __instance, void* __xceptptr, void* __oldxt, void* __newxt)
{
    BYTE* vtab;
    RTTI* instancext;
    if (!__instance)
        return __instance;

    int offset = ((int*)__xceptptr)[-2];
    __instance = (void*)(((BYTE*)__instance) - offset);
    // conversion to pointer to void gets outermost base
    if (!__newxt)
    {
        return __instance;
    }

    offset = ((int*)__xceptptr)[-1];
    vtab = (BYTE*)__xceptptr - offset - VTAB_XT_OFFS;  // get vtab pointer
    instancext = *(RTTI**)vtab;
    if (!instancext)
        return NULL;

    int count = 0;
    // try to find an offset for the func
    offset = findoffs((RTTI*)instancext, (RTTI*)__newxt, &count);

    // if we found it
    if (count == 1)
    {
        return ((BYTE*)__instance) + offset;
    }
    // no unique match, signal failure
    return NULL;
}