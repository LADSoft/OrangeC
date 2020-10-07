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
#include <errno.h>
#include <process.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <threads.h>
#include "libp.h"
#include "LocalAlloc.h"
#include <map>

extern "C"
{
void __tss_run_dtors(thrd_t thrd);
void __mtx_remove_thrd(thrd_t thrd);
void __cnd_remove_thrd(thrd_t thrd);
void rpmalloc_thread_initialize();
void rpmalloc_thread_finalize();
BOOL __stdcall GetModuleHandleExW(DWORD dwFlags, LPCTSTR lpModuleName, HMODULE* phModule);
#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 4
extern void *__hInstance;
}

extern "C" void __load_local_data(void);

typedef std::map<HANDLE, std::pair<void*, void*>, std::less<HANDLE>, LocalAllocAllocator<std::pair<HANDLE, std::pair<void*, void*>>>> Registered;

typedef std::map<HANDLE, void *, std::less<HANDLE>, LocalAllocAllocator<std::pair<HANDLE, void *>>> HandleMap;

typedef std::map<int, HandleMap, std::less<int>, LocalAllocAllocator<std::pair<int, HandleMap>>> Handles;

static Registered *registered;
static Handles *handles;

#pragma startup thrd_init 254
#pragma rundown thrd_end 2

static PASCAL unsigned char * __getTlsData(int eip, int thread)
{
    struct __rtl_data* r = __getRtlData();
    if (!r->thread_local_data)
    {
        __ll_enter_critical();
        __load_local_data();
        __ll_exit_critical();
    }
    HANDLE hModule;
    HandleMap *map = (HandleMap *)r->thread_local_data;
    if (map->size() == 1)
    {
         // optimize the non-dll use case
         return map->begin()->second;
    }
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)eip, &hModule))
    {

         auto it = map->find(hModule);
         if (it != map->end())
         {
              return (*it).second;
         }
    }
    // this will just crash it
    // but we should never get here...
    return 0;
}
// this is a thunk to get around the compiler not being able to assemble a direct call statement
static void *GetTlsData = __getTlsData;
// this func is a compiler helper... it is not visible to the optimizer so it
// cannot muddle with any registers.  The return value is stacked back where
// the arg came from, because, the compiler has to put the return value into some
// arbitrary register based on how the register allocator decided to do it
// limiting the functionality beyond that will put a major crimp in optimizations...
#if !defined(__MSIL__)
extern "C" void _RTL_FUNC __tlsaddr(int n)
{
    asm push eax;
    asm push ecx;
    asm push edx;
    asm push fs:[0x18]
    asm push [esp + 16] // eip
    asm call [GetTlsData]; // eax has pointer to thread data now
    asm add [esp + 16], eax // stacked parameter which is the offset in the tls region
    asm pop edx;
    asm pop ecx;
    asm pop eax;
}
#endif

static unsigned char * Load(unsigned char *start, unsigned char *end)
{
    int n = end - start;
    if (n == 0) n = 4;
    unsigned char *p = (unsigned char *)LocalAlloc(LMEM_FIXED, n);
    if (!p)
        abort();
    memcpy(p, start, n);
    return p;
}
static void AddLocalData(int thread)
{
    if (handles->find(thread) == handles->end())
    {
        HandleMap aa;
        (*handles)[thread] = aa;
        for (auto d : *registered)
        {
            unsigned char *p = Load((unsigned char *)d.second.first, (unsigned char *)d.second.second);
            (*handles)[thread][d.first] = p;
        }
        struct __rtl_data* r = __getRtlData();
        r->thread_local_data = (unsigned char *)&(*handles)[thread];
    }
}
static void RemoveLocalData(int thread)
{
    auto it = handles->find(thread);
    if (it != handles->end())
    {
        for (auto && h : it->second)
        {
             LocalFree(h.second);
        }
        handles->erase(thread);
    }
}
extern "C" void __load_local_data(void)
{
     int tid;
     asm mov eax,fs:[0x18] // currentteb
     asm mov [tid], eax
     AddLocalData(tid);
}
extern "C" void __unload_local_data(void)
{
     int tid;
     asm mov eax,fs:[0x18] // currentteb
     asm mov [tid], eax
     RemoveLocalData(tid);
}
static void thrd_init(void)
{
    LocalAllocAllocator<Registered> RegisteredAllocator;
    LocalAllocAllocator<Handles> HandlesAllocator;

    registered = RegisteredAllocator.allocate(1);
    RegisteredAllocator.construct(registered);
    handles = HandlesAllocator.allocate(1);
    HandlesAllocator.construct(handles);
    __threadTlsAlloc(false);	
    __load_local_data();  // main thread
}

static void thrd_end(void)
{
    LocalAllocAllocator<Registered> RegisteredAllocator;
    LocalAllocAllocator<Handles> HandlesAllocator;

    RegisteredAllocator.destroy(registered);
    RegisteredAllocator.deallocate(registered, 1);
    HandlesAllocator.destroy(handles);
    HandlesAllocator.deallocate(handles, 1);
}
extern "C" void _RTL_FUNC __thrdRegisterModule(HANDLE module, void *tlsStart, void *tlsEnd)
{
    __ll_enter_critical();
    if (registered->find(module) == registered->end())
    {
        (*registered)[module] = std::pair<void*, void*>(tlsStart, tlsEnd);
         for (auto&& h : *handles)
         {
              h.second[module] = (void *)Load((unsigned char *)tlsStart, (unsigned char *)tlsEnd);
         }
    }
    __ll_exit_critical();
}
extern "C" void _RTL_FUNC __thrdUnregisterModule(HANDLE module)
{
    __ll_enter_critical();
    auto it = registered->find(module);
    if (it != registered->end())
    {
         registered->erase(module);
         for (auto&& h : *handles)
         {
              auto it1 = h.second.find(module);
              if (it1 != h.second.end())
              {
                  // this is buggy, doesn't run dtors...
                  auto data = (*it1).second;
                  h.second.erase(module);
                  LocalFree(data);
              }
         }
    }
    __ll_exit_critical();
}
extern "C" void __ll_thrdexit(unsigned retval)
{
    struct __rtl_data* r = __getRtlData();  // allocate the local storage
    struct ithrd* p = (struct ithrd*)r->thrd_id;
    __tss_run_dtors((thrd_t)p);  // do this immediately on exit in case the thread
                                 // languishes waiting for a detatch.
                                 // it also allows us to free the local storage immediately
                                 // which is good because once the thread exits we don't have access to that
    __ll_enter_critical();
    if (p && p->detach)
    {
        __mtx_remove_thrd((thrd_t)p);
        __cnd_remove_thrd((thrd_t)p);
        CloseHandle(p->handle);
        p->sig = 0;
        free(p);
    }
    __unload_local_data();
    __ll_exit_critical();
    __threadTlsFree(TRUE);
    ExitThread(retval);
}

static int WINAPI thrdstart(struct ithrd* h)
{
    int rv;
    struct __rtl_data* r = __getRtlData();  // allocate the local storage
    r->thrd_id = h;
    SetEvent((HANDLE)h->stevent);
    rv = ((unsigned (*)(void*))h->start)(h->arglist);
    __ll_thrdexit(rv);
    return 0;
}


extern "C" int __ll_thrdstart(struct ithrd** thr, thrd_start_t* func, void* arglist)
{
    DWORD id;
    struct ithrd* mem = (struct ithrd*)calloc(1, sizeof(struct ithrd));

    if (mem)
    {
        mem->start = func;
        mem->arglist = arglist;
        mem->stevent = (void*)CreateEvent(0, 0, 0, 0);
        mem->handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thrdstart, mem, 0, &id);
        if (mem->handle != NULL)
        {
            WaitForSingleObject((HANDLE)mem->stevent, INFINITE);
            CloseHandle((HANDLE)mem->stevent);
            *thr = mem;
            return thrd_success;
        }
        else
        {
            CloseHandle((HANDLE)mem->stevent);
            free(mem);
            return thrd_error;
        }
    }
    return thrd_nomem;
}

extern "C" int __ll_thrdwait(struct ithrd* thrd)
{
    if (WaitForSingleObject(thrd->handle, INFINITE) == WAIT_OBJECT_0)
        return thrd_success;
    return thrd_error;
}

extern "C" struct ithrd* __ll_thrdcurrent(void)
{
    struct __rtl_data* r = __getRtlData();  // allocate the local storage
    return r->thrd_id;
}

extern "C" int __ll_thrdexitcode(struct ithrd* thrd, int* rv)
{
    DWORD val;
    if (GetExitCodeThread(thrd->handle, &val))
    {
        *rv = val;
        return thrd_success;
    }
    return thrd_error;
}

extern "C" int __ll_thrd_detach(struct ithrd* thrd)
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

extern "C" void __ll_thrdsleep(unsigned ms) { Sleep(ms); }
