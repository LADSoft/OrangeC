/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <windows.h>
#include <errno.h>
#include <process.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

struct __threads {
    void *start;
    void *arglist;
    void *handle;
    int startmode;
    HLOCAL memhand;
} ;
void _RTL_FUNC _endthread( void )
{
    HANDLE handle = __getRtlData()->threadhand;
    if (handle)
        CloseHandle(handle);
    __threadTlsFree(TRUE);
    ExitThread(0);
}
void _RTL_FUNC _endthreadex(unsigned retval)
{
    HANDLE handle = __getRtlData()->threadhand;
    if (handle)
        CloseHandle(handle);
    __threadTlsFree(TRUE);
    ExitThread(retval);
}

static int WINAPI __threadstart(struct __threads *h)
{
    struct __threads th;
    struct __rtl_data *r = __getRtlData(); // allocate the local storage
    __ll_enter_critical();
    __ll_exit_critical();
    th = *h;
    LocalUnlock(h->memhand);
    LocalFree(th.memhand);
    if (th.startmode) {
        int rv = ((unsigned (__stdcall *)(void *))th.start)(th.arglist);
        _endthreadex(rv);
    } else {
        r->threadhand = th.handle; // so we can close it automagically
        ((void (__cdecl *)(void *))th.start)(th.arglist);
        _endthread();
    }
    return 0;
}

uintptr_t _RTL_FUNC _beginthread(void( __cdecl *start_address )( void * ),
   unsigned stack_size,
   void *arglist )
{
    DWORD rv; 
    struct __threads *mem ;
    HLOCAL mhand = LocalAlloc(LPTR,sizeof(struct __threads));
    if (!mhand) {
        errno = EAGAIN;
        return -1;
    }
    mem = LocalLock(mhand);
    mem->memhand = mhand;
    mem->start = start_address ;
    mem->arglist = arglist;
    mem->startmode = 0;
    __ll_enter_critical();
    mem->handle = CreateThread(0,stack_size,(LPTHREAD_START_ROUTINE)__threadstart,mem,0,&rv);
    __ll_exit_critical();
    if (mem->handle == NULL) {
        int err = GetLastError();
        errno = EAGAIN;
        return -1;
    }
    return mem->handle;
}
uintptr_t _RTL_FUNC _beginthreadex(void *security, unsigned stack_size,
   unsigned ( __stdcall *start_address )( void * ),
   void *arglist, unsigned initflag, unsigned *thrdaddr)
{
    DWORD rv; 
    struct __threads *mem ;
    HLOCAL mhand = LocalAlloc(LPTR,sizeof(struct __threads));
    if (!mhand) {
        errno = EAGAIN;
        return 0;
    }
    mem = LocalLock(mhand);
    mem->memhand = mhand;
    mem->start = start_address ;
    mem->arglist = arglist;
    mem->startmode = 1;
    __ll_enter_critical();
    mem->handle = CreateThread(security,stack_size,(LPTHREAD_START_ROUTINE)__threadstart,mem,initflag, thrdaddr);
    __ll_exit_critical();
    if (mem->handle == NULL) {
        int err = GetLastError();
        errno = EAGAIN;
        return 0;
    }
    return mem->handle;
}
