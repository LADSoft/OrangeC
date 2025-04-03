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
#include <limits.h>

void __ll_cndFree(long long handle) { CloseHandle((HANDLE)handle); }
long long __ll_cndAlloc(void) { return (long long)CreateSemaphore(NULL, 0, INT_MAX, NULL); }
int __ll_cndWait(long long handle, unsigned ms)
{
    switch (WaitForSingleObject((HANDLE)handle, ms == (unsigned)-1 ? INFINITE : ms))
    {
        case WAIT_OBJECT_0:
            return 1;
        case WAIT_TIMEOUT:
            return 0;
        case WAIT_ABANDONED:  // undefined behavior if a thread exits without releasing a mutex...
            return -1;
        default:
            return -1;
    }
}
int __ll_cndSignal(long long handle, int cnt)
{
    if (ReleaseSemaphore((HANDLE)handle, cnt, NULL))
        return 0;
    return -1;
}
