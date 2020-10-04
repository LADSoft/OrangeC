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

#include <errno.h>
#include <windows.h>
#include <process.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <threads.h>
#include "libp.h"

void __load_local_data();
void __unload_local_data();
void rpmalloc_thread_initialize();
void rpmalloc_thread_finalize();
extern HANDLE
 PASCAL WINBASEAPI CreateThreadExternal(
	     LPSECURITY_ATTRIBUTES lpThreadAttributes,
	     DWORD dwStackSize,
	     LPTHREAD_START_ROUTINE lpStartAddress,
	     LPVOID lpParameter,
	     DWORD dwCreationFlags,
	     LPDWORD lpThreadId
	     );

struct __threads
{
    void* start;
    void* arglist;
    void* handle;
    int startmode;
    HLOCAL memhand;
};
void _RTL_FUNC _endthread(void)
{
    HANDLE handle = __getRtlData()->threadhand;
    if (handle)
        CloseHandle(handle);
    __ll_enter_critical();
    rpmalloc_thread_finalize();
    __unload_local_data();
    __ll_exit_critical();
    __threadTlsFree(TRUE);
    ExitThread(0);
}
void _RTL_FUNC _endthreadex(unsigned retval)
{
    HANDLE handle = __getRtlData()->threadhand;
    if (handle)
        CloseHandle(handle);
    __ll_enter_critical();
    rpmalloc_thread_finalize();
    __unload_local_data();
    __ll_exit_critical();
    __threadTlsFree(TRUE);
    ExitThread(retval);
}

static int WINAPI __threadstart(struct __threads* h)
{
    struct __threads th;
    struct __rtl_data* r = __getRtlData();  // allocate the local storage
    r->thrd_id = 0;
    __ll_enter_critical();
    __load_local_data();
    __ll_exit_critical();
    rpmalloc_thread_initialize();
    th = *h;
    LocalUnlock(h->memhand);
    LocalFree(th.memhand);
    if (th.startmode)
    {
        int rv = ((unsigned(__stdcall*)(void*))th.start)(th.arglist);
        _endthreadex(rv);
    }
    else
    {
        r->threadhand = th.handle;  // so we can close it automagically
        ((void(__cdecl*)(void*))th.start)(th.arglist);
        _endthread();
    }
    return 0;
}

uintptr_t _RTL_FUNC _beginthread(void(__cdecl* start_address)(void*), unsigned stack_size, void* arglist)
{
    DWORD rv;
    struct __threads* mem;
    HLOCAL mhand = LocalAlloc(LPTR, sizeof(struct __threads));
    if (!mhand)
    {
        errno = EAGAIN;
        return -1;
    }
    mem = LocalLock(mhand);
    mem->memhand = mhand;
    mem->start = start_address;
    mem->arglist = arglist;
    mem->startmode = 0;
    __ll_enter_critical();
    mem->handle = CreateThreadExternal(0, stack_size, (LPTHREAD_START_ROUTINE)__threadstart, mem, 0, &rv);
    __ll_exit_critical();
    if (mem->handle == NULL)
    {
        int err = GetLastError();
        errno = EAGAIN;
        return -1;
    }
    return mem->handle;
}
uintptr_t _RTL_FUNC _beginthreadex(void* security, unsigned stack_size, unsigned(__stdcall* start_address)(void*), void* arglist,
                                   unsigned initflag, unsigned* thrdaddr)
{
    DWORD rv;
    struct __threads* mem;
    HLOCAL mhand = LocalAlloc(LPTR, sizeof(struct __threads));
    if (!mhand)
    {
        errno = EAGAIN;
        return 0;
    }
    mem = LocalLock(mhand);
    mem->memhand = mhand;
    mem->start = start_address;
    mem->arglist = arglist;
    mem->startmode = 1;
    __ll_enter_critical();
    mem->handle = CreateThreadExternal(security, stack_size, (LPTHREAD_START_ROUTINE)__threadstart, mem, initflag, thrdaddr);
    __ll_exit_critical();
    if (mem->handle == NULL)
    {
        errno = EAGAIN;
        return 0;
    }
    return mem->handle;
}
HANDLE _RTL_FUNC CreateThread(
	     LPSECURITY_ATTRIBUTES lpThreadAttributes,
	     DWORD dwStackSize,
	     LPTHREAD_START_ROUTINE lpStartAddress,
	     LPVOID lpParameter,
	     DWORD dwCreationFlags,
	     LPDWORD lpThreadId
	     )
{
    DWORD rv;
    struct __threads* mem;
    HLOCAL mhand = LocalAlloc(LPTR, sizeof(struct __threads));
    if (!mhand)
    {
        errno = EAGAIN;
        return 0;
    }
    mem = LocalLock(mhand);
    mem->memhand = mhand;
    mem->start = lpStartAddress;
    mem->arglist = lpParameter;
    mem->startmode = 1;
    __ll_enter_critical();
    mem->handle = CreateThreadExternal(lpThreadAttributes, dwStackSize, (LPTHREAD_START_ROUTINE)__threadstart, mem, dwCreationFlags, lpThreadId);
    __ll_exit_critical();
    if (mem->handle == NULL)
    {
        errno = EAGAIN;
        return 0;
    }
    return mem->handle;
}

