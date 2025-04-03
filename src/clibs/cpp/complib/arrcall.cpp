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

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <uchar.h>
#include "libp.h"

typedef void (*CONSDEST)(void*);
void _RTL_FUNC __arrCall(void* instance, void* cons, void* dest, int elems, int size)
{
    void* pos = instance;
    if (cons)
    {
        try
        {
            for (int i = 0; i < elems; i++)
            {
                CONSDEST xx = (CONSDEST)cons;
                (*xx)(pos);
                pos = (void*)((BYTE*)pos + size);
            }
        }
        catch (...)
        {
            if (dest)
            {
                pos = (void*)((BYTE*)pos - size);
                while (pos >= instance)
                {
                    CONSDEST xx = (CONSDEST)dest;
                    (*xx)(pos);
                    pos = (void*)((BYTE*)pos - size);
                }
            }
            throw;
        }
    }
    else if (dest)
    {
        if (!elems)
        {
            // if we get here it was a new'd array...
            elems = (((FREELIST*)instance)[-1].size - 8) / size;  // get #elems from the RTL's size field
        }
        pos = (void*)((BYTE*)pos + (elems - 1) * size);
        while (pos >= instance)
        {
            CONSDEST xx = (CONSDEST)dest;
            (*xx)(pos);
            pos = (void*)((BYTE*)pos - size);
        }
    }
}