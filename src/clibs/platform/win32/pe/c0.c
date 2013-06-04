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
extern PASCAL struct defstr {
    int flags;
    DWORD (*func)();
} startupStruct ;

extern int _argc;
extern char **_argv;
extern char **_environ;
char *_oscmd;
char *_osenv;
HINSTANCE 	__hInstance;
unsigned	_win32;
jmp_buf __exitbranch, __abortbranch;
static unsigned	dllexists = 0;
unsigned _isDLL;
void PASCAL __xceptinit(int *block);
void PASCAL __xceptrundown(void);
// in the follow, the args are ONLY valid for DLLs
int __stdcall ___startup(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
    int rv;
    int exceptBlock[2];
    char quote;
    char *cmd;
    if (startupStruct.flags & GUI)
        _win32 = 1;
    __xceptinit(&exceptBlock);
    if (!(startupStruct.flags & DLL) || fdwReason == DLL_PROCESS_ATTACH)
    {
        if (startupStruct.flags & DLL)
        {
            _isDLL = 1;
            if (!dllexists)
                dllexists++;
            else
            {
                MessageBox(0,"NonShared data segment required","Error",0);
                return 1;
            }
        }
        _osenv = GetEnvironmentStringsA();
        _oscmd = GetCommandLineA();
        __hInstance = GetModuleHandleA(0);
        _llfpinit();

        __threadinit();
        __srproc(INITSTART, INITEND);
    }
    if (!(rv = setjmp(__abortbranch)))
    {
        if (!(rv = setjmp(__exitbranch)))
        {
            if (startupStruct.flags & DLL)
            {
                rv = (*startupStruct.func)(hInst, fdwReason, lpvReserved) + 1;
            }
            else if (startupStruct.flags & GUI)
            {
                cmd = _oscmd;
                quote = *cmd;
                while ((!quote || *cmd != ' ') && *cmd)
                {
                    if (*cmd == '"')
                        quote ^= *cmd;
                    cmd++;
                }
                while (*cmd && *cmd == ' ') cmd++;
                rv = (*startupStruct.func)(__hInstance, 0, cmd, 10) + 1;
            }
            else
            {
                rv = (*startupStruct.func)(_argc, _argv, _environ) + 1;
            }
        }
        if (!(startupStruct.flags & DLL) || fdwReason == DLL_PROCESS_DETACH)
        {
            __srproc(EXITSTART, EXITEND);
        }
    }
    rv--;
    __xceptrundown();
    if (!(startupStruct.flags & DLL))
    {
        __crtexit(rv);	// never returns if linked to CRTDLL
        ExitProcess(rv);
    }
    return rv;
}