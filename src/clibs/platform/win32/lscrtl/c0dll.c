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
#define GUI 1
#define DLL 2

#include <windows.h>
#include <setjmp.h>
#include <string.h>

extern char INITSTART[], INITEND[], EXITSTART[], EXITEND[], BSSSTART[], BSSEND[];
extern int _argc;
extern char **_argv;
extern char **_environ;
char __export *_oscmd;
char __export *_osenv;
HINSTANCE 	__export __hInstance;
unsigned	_win32 = 0;
jmp_buf __exitbranch, __abortbranch;
static unsigned	dllexists = 0;
unsigned _isDLL = 1;
void (*userRundown)();
void PASCAL __xceptinit(int *block);
void PASCAL __xceptrundown(void);
// in the follow, the args are ONLY valid for DLLs
int __stdcall ___startup(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
    __hInstance = hInst;
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            __threadinit();
        case DLL_THREAD_ATTACH:
            __threadTlsAlloc(FALSE);
            break;
        case DLL_PROCESS_DETACH:
            __threadTlsFreeAll();
            __threadrundown();
            break;
        case DLL_THREAD_DETACH:
            __threadTlsFree(FALSE);
            break;
    }
    return TRUE;
}
void __export __stdcall ___lsdllinit(DWORD flags, void (*rundown)(), int *exceptBlock)
{
    static int Flags ;
    static int rv;
    Flags = flags;
    if (flags & GUI)
        _win32 = 1;
    userRundown = rundown;
    if (!(flags & DLL))
        __xceptinit(exceptBlock);

    if ((rv = setjmp(__abortbranch)) || (rv = setjmp(__exitbranch)))
    {
        // this is only safe because we aren't using the stack any more...
        rv--;
        if (!(Flags & DLL))
        {
            userRundown();
            __xceptrundown();
        }
        __srproc(EXITSTART, EXITEND);
        ExitProcess(rv);
    }
    _osenv = GetEnvironmentStrings();
    _oscmd = GetCommandLine();		
    __hInstance = GetModuleHandle(0);
    _llfpinit();
    __srproc(INITSTART, INITEND);
}
