/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <windows.h>
#include <errno.h>
#include <process.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <threads.h>
#include "libp.h"

void __tss_run_dtors(thrd_t thrd);
void __mtx_remove_thrd(thrd_t thrd);
void __cnd_remove_thrd(thrd_t thrd);
void rpmalloc_thread_initilaize(void);
void rpmalloc_thread_finalize(void);
extern char _TLSINITSTART[], _TLSINITEND[];

#pragma startup thrd_init 16

// this func is a compiler helper... it is not visible to the optimizer so it
// cannot muddle with any registers.  The return value is stacked back where
// the arg came from, because, the compiler has to put the return value into some
// arbitrary register based on how the register allocator decided to do it
// limiting the functionality beyond that will put a major crimp in optimizations...
#if !defined(__MSIL__)
void _RTL_FUNC __tlsaddr(int n)
{
    __asm push eax
    __asm push ecx
    __asm push edx
    struct __rtl_data *r = __getRtlData();
    n = (int) r->thread_local_data + n; // return value is on stack for this one...
    __asm pop edx
    __asm pop ecx
    __asm pop eax
}
#endif
static void load_local_data(void)
{
    struct __rtl_data *r = __getRtlData();
    int n = _TLSINITEND - _TLSINITSTART;
    if (!n)
    {
        n = 4;
    }
    char *p = malloc(n);
    if (!p)
        abort();
    memcpy(p, _TLSINITSTART, n);
    r->thread_local_data = p;
}
static void thrd_init(void)
{
    load_local_data(); // main thread
}

void __ll_thrdexit(unsigned retval)
{
    struct __rtl_data *r = __getRtlData(); // allocate the local storage
    struct ithrd *p = (struct ithrd *)r->thrd_id;
    __tss_run_dtors((thrd_t)p); // do this immediately on exit in case the thread
                                // languishes waiting for a detatch.
                                // it also allows us to free the local storage immediately
                                // which is good because once the thread exits we don't have access to that
    __ll_enter_critical();
    rpmalloc_thread_finalize();
    free(r->thread_local_data);
    if (p && p->detach)
    {
        __mtx_remove_thrd((thrd_t)p);
        __cnd_remove_thrd((thrd_t)p);
        CloseHandle(p->handle);
        p->sig = 0;
        free(p);
    }
    __ll_exit_critical();
    __threadTlsFree(TRUE);
    ExitThread(retval);
}

static int WINAPI thrdstart(struct ithrd *h)
{
    int rv;
    struct __rtl_data *r = __getRtlData(); // allocate the local storage
    r->thrd_id = h;
    load_local_data();
    rpmalloc_thread_initialize();
    rv = ((unsigned (*)(void *))h->start)(h->arglist);
    __ll_thrdexit(rv);
    return 0;
}

int __ll_thrdstart(struct ithrd **thr, thrd_start_t *func, void *arglist )
{
    DWORD id;
    struct ithrd *mem = calloc(1, sizeof(struct ithrd));
    
    if (mem) {
        mem->start = func ;
        mem->arglist = arglist;
        __ll_enter_critical();
        mem->handle = CreateThreadExternal(0,0,(LPTHREAD_START_ROUTINE)thrdstart,mem,0,&id);
        __ll_exit_critical();
        if (mem->handle != NULL) {
            *thr = mem;
            return thrd_success;
        }
        else
        {
            free(mem);
            return thrd_error;
        }
    }
    return thrd_nomem;
}
int __ll_thrdwait(struct ithrd *thrd)
{
    if (WaitForSingleObject(thrd->handle, INFINITE) == WAIT_OBJECT_0)
        return thrd_success;
    return thrd_error;
}
struct ithrd *__ll_thrdcurrent(void)
{
    struct __rtl_data *r = __getRtlData(); // allocate the local storage
    return r->thrd_id;
}
int __ll_thrdexitcode(struct ithrd *thrd, int *rv)
{
    DWORD val;
    if (GetExitCodeThread(thrd->handle, &val))
    {
        *rv = val;
        return thrd_success;
    }
    return thrd_error;
        
}
int __ll_thrd_detach(struct ithrd *thrd)
{
    __ll_enter_critical();
    switch (WaitForSingleObject(thrd->handle, 0))
    {
        case WAIT_OBJECT_0:
            __mtx_remove_thrd((thrd_t)thrd);
            __cnd_remove_thrd((thrd_t)thrd);
            CloseHandle(thrd->handle);
            thrd->sig = 0;
            free(thrd);
            __ll_exit_critical();
            return thrd_success;
        case WAIT_TIMEOUT:
            thrd->detach = 1;
            __ll_exit_critical();
            return thrd_success;
        default:
            return thrd_error;
    }
}
void __ll_thrdsleep(unsigned ms)
{
    Sleep(ms);
}
