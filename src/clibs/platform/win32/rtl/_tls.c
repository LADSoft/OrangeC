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
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#undef errno
#include <wchar.h>
#include <locale.h>
#include "libp.h"

static int __rtlTlsIndex ;
static struct __rtl_data *rd;

void __threadinit(void)
{
    __rtlTlsIndex = TlsAlloc();
}
void __threadrundown(void)
{
    TlsFree(__rtlTlsIndex);
}
struct __rtl_data *__threadTlsAlloc(int cs)
{
    HLOCAL data = LocalAlloc(LPTR,sizeof(struct __rtl_data));
    struct __rtl_data *rv ;
    if (!data) {
        printf(stderr,"out of memory");
        abort();
    }
    rv = LocalLock(data);
    rv->handle = (void *)data;
    TlsSetValue(__rtlTlsIndex,(void *)rv);
    if (cs)
        __ll_enter_critical();
    rv->link = rd ;
    rd = rv ;
    if (cs)
        __ll_exit_critical();
    return rv;
}
void __threadTlsFree(int cs)
{
    struct __rtl_data *rv = TlsGetValue(__rtlTlsIndex) , **list = &rd;
    if (rv) {
        HLOCAL handle ;
        handle = rv->handle;
        if (cs)
            __ll_enter_critical();
        while (*list) {
            if (*list == rv) {
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
    TlsSetValue(__rtlTlsIndex,0);
}
void __threadTlsFreeAll(void)
{
    while (rd) {
        struct __rtl_data *next = rd->link;
        HLOCAL handle = rd->handle;
        LocalUnlock(handle);
        LocalFree(handle);
        rd = next;
    }
}
struct __rtl_data *__getRtlData(void)
{
    struct __rtl_data *rv = (struct __rtl_data *)TlsGetValue(__rtlTlsIndex);
    if (!rv)
        rv = __threadTlsAlloc(TRUE);
    return rv;
}

