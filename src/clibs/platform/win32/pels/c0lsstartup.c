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

extern void _import _exit(int);
#include <windows.h>
#include <setjmp.h>
#include <string.h>

extern char INITSTART[], INITEND[], EXITSTART[], EXITEND[], BSSSTART[], BSSEND[];
extern __import int _argc;
extern __import char **_argv;
extern __import char **_environ;
extern __import char *_oscmd;
extern __import char *_osenv;
static unsigned	dllexists = 0;
static void _dorundown(void);
// in the follow, the args are ONLY valid for DLLs
int __stdcall DllEntryPoint(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved);
void __stdcall __import ___lsdllinit(DWORD flags, void (*rundown)(void), unsigned *exceptBlock);
void __srproc(char *, char *);
int __stdcall ___lscrtl_startup(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
    int flags;
    int rv;
    unsigned exceptBlock[2];
    char quote;
    char *cmd;
    asm mov [flags],edx;
    quote = 0;
    ___lsdllinit(flags, _dorundown, exceptBlock);
    if (!(flags & DLL) || fdwReason == DLL_PROCESS_ATTACH)
    {   
        if (flags & DLL)
        {
            if (!dllexists)
                dllexists++;
            else
            {
                MessageBox(0,"NonShared data segment required","Error",0);
                return 1;
            }
        }
    }
    __srproc(INITSTART, INITEND);
    if (flags & DLL)
    {
        rv = DllEntryPoint(hInst, fdwReason, lpvReserved) + 1;
    }
    else if (flags & GUI)
    {
        cmd = _oscmd;
        while ((!quote || *cmd != ' ') && *cmd)
        {
            if (*cmd == '"')
                quote ^= *cmd;
            cmd++;
        }
        while (*cmd && *cmd == ' ') cmd++;
        rv = WinMain(hInst, 0, cmd, 10) + 1;
    }
    else
    {
        rv = main(_argc, _argv, _environ) + 1;
    }
    if ((flags & DLL) && fdwReason == DLL_PROCESS_DETACH)
    {
        __srproc(EXITSTART, EXITEND);
    }
    rv--;
    if (!(flags & DLL))
    {
        _exit(rv);
        ExitProcess(rv); // needed to satisfy the debugger
    }
    return rv;
}
static void _dorundown(void)
{
    __srproc(EXITSTART, EXITEND);
}
