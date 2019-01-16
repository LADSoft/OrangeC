/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#undef errno
#include <wchar.h>
#include <locale.h>
#include "libp.h"

static int __rtlTlsIndex;
static struct __rtl_data* rd;

void __threadinit(void) { __rtlTlsIndex = TlsAlloc(); }
void __threadrundown(void) { TlsFree(__rtlTlsIndex); }
struct __rtl_data* __threadTlsAlloc(int cs)
{
    HLOCAL data = LocalAlloc(LPTR, sizeof(struct __rtl_data));
    struct __rtl_data* rv;
    if (!data)
    {
        printf(stderr, "out of memory");
        abort();
    }
    rv = LocalLock(data);
    rv->handle = (void*)data;
    TlsSetValue(__rtlTlsIndex, (void*)rv);
    if (cs)
        __ll_enter_critical();
    rv->link = rd;
    rd = rv;
    if (cs)
        __ll_exit_critical();
    return rv;
}
void __threadTlsFree(int cs)
{
    struct __rtl_data *rv = TlsGetValue(__rtlTlsIndex), **list = &rd;
    if (rv)
    {
        HLOCAL handle;
        handle = rv->handle;
        if (cs)
            __ll_enter_critical();
        while (*list)
        {
            if (*list == rv)
            {
                *list = (*list)->link;
                break;
            }
            list = &(*list)->link;
        }
        if (cs)
            __ll_exit_critical();
        LocalUnlock(handle);
        LocalFree(handle);
    }
    TlsSetValue(__rtlTlsIndex, 0);
}
void __threadTlsFreeAll(void)
{
    while (rd)
    {
        struct __rtl_data* next = rd->link;
        HLOCAL handle = rd->handle;
        LocalUnlock(handle);
        LocalFree(handle);
        rd = next;
    }
}
struct __rtl_data* __getRtlData(void)
{
    struct __rtl_data* rv = (struct __rtl_data*)TlsGetValue(__rtlTlsIndex);
    if (!rv)
        rv = __threadTlsAlloc(TRUE);
    return rv;
}
