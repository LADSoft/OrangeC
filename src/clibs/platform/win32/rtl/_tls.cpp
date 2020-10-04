/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
#include "LocalAlloc.h"
#include <set>
#include <stdio.h>

static int __rtlTlsIndex;

static LocalAllocAllocator<__rtl_data> DataAllocator;

static LocalAllocAllocator<std::set<__rtl_data*, std::less<__rtl_data*>, LocalAllocAllocator<__rtl_data*>>> SetAllocator;

static std::set<__rtl_data*, std::less<__rtl_data*>, LocalAllocAllocator<__rtl_data*>> *RtlDataSet;

extern "C" void __threadinit(void) 
{ 
    RtlDataSet = SetAllocator.allocate(1);
    SetAllocator.construct(RtlDataSet);
    __rtlTlsIndex = TlsAlloc(); 
}
extern "C" void __threadrundown(void) 
{ 
    TlsFree(__rtlTlsIndex);
    SetAllocator.destroy(RtlDataSet);
    SetAllocator.deallocate(RtlDataSet,1);
}
extern "C" struct __rtl_data* __threadTlsAlloc(int cs)
{
DWORD temp;
WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "A\n", 2, &temp, 0);
    struct __rtl_data* rv = DataAllocator.allocate(1);
    if (!rv)
    {
WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "B\n", 2, &temp, 0);
        fprintf(stderr, "out of memory");
        abort();
    }
WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "C\n", 2, &temp, 0);
    DataAllocator.construct(rv);
WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "D\n", 2, &temp, 0);
    TlsSetValue(__rtlTlsIndex, (void*)rv);
WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "e\n", 2, &temp, 0);
    if (cs)
        __ll_enter_critical();
WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "f\n", 2, &temp, 0);
    RtlDataSet->insert(rv);
WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "g\n", 2, &temp, 0);
    if (cs)
        __ll_exit_critical();
WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "h\n", 2, &temp, 0);
    return rv;
}
extern "C" void __threadTlsFree(int cs)
{
    struct __rtl_data *rv = (struct __rtl_data*)TlsGetValue(__rtlTlsIndex);
    if (rv)
    {
        if (cs)
            __ll_enter_critical();
        auto data = RtlDataSet->find(rv);
        if (data != RtlDataSet->end())
        {
            __rtl_data *temp = *data;
            RtlDataSet->erase(data);
            DataAllocator.destroy(temp);
            DataAllocator.deallocate(temp,1);
        }
        if (cs)
            __ll_exit_critical();
    }
    TlsSetValue(__rtlTlsIndex, 0);
}
extern "C" void __threadTlsFreeAll(void)
{
    for (auto t : *RtlDataSet)
    {
        DataAllocator.destroy(t);
        DataAllocator.deallocate(t,1);
    }
    RtlDataSet->clear();
}
extern "C" struct __rtl_data* __getRtlData(void)
{
    struct __rtl_data* rv = (struct __rtl_data*)TlsGetValue(__rtlTlsIndex);
    if (!rv)
        rv = __threadTlsAlloc(TRUE);
    return rv;
}

