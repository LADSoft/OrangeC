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
    struct __rtl_data* r = __getRtlData();  // allocate the local storage
    struct ithrd* p = (struct ithrd*)r->thrd_id;
    __ll_enter_critical();
    if (p && p->detach)
    {
        __mtx_remove_thrd((thrd_t)p);
        __cnd_remove_thrd((thrd_t)p);
        p->sig = 0;
        free(p);
    }
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
    struct __rtl_data* r = __getRtlData();  // allocate the local storage
    struct ithrd* p = (struct ithrd*)r->thrd_id;
    __ll_enter_critical();
    if (p && p->detach)
    {
        __mtx_remove_thrd((thrd_t)p);
        __cnd_remove_thrd((thrd_t)p);
        p->sig = 0;
        free(p);
    }
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
    mem->handle = CreateThread(0, stack_size, (LPTHREAD_START_ROUTINE)__threadstart, mem, 0, &rv);
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
    mem->handle = CreateThread(security, stack_size, (LPTHREAD_START_ROUTINE)__threadstart, mem, initflag, thrdaddr);
    __ll_exit_critical();
    if (mem->handle == NULL)
    {
        errno = EAGAIN;
        return 0;
    }
    return mem->handle;
}
