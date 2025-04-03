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
#include <errno.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <dpmi.h>
#include "llp.h"

extern unsigned _linear;
void *__ll_malloc(size_t size)
{
    DPMI_MEM_HANDLE handle;
    ULONG address;
    if (dpmi_get_memory(&address, &handle, (size+sizeof(DPMI_MEM_HANDLE) + 4095) & -4096)) {
        errno = _dos_errno = ENOMEM;
        return 0;
    }
    address -= _linear;
    *(DPMI_MEM_HANDLE *)address = handle;
    return (void *) (address + sizeof(DPMI_MEM_HANDLE));
}

void __ll_free(void *block)
{
    DPMI_MEM_HANDLE handle;
    handle = *(((DPMI_MEM_HANDLE *) block) -1);
    dpmi_free_memory(handle);
}