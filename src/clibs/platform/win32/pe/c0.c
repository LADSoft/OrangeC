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

#define GUI 1
#define DLL 2

#include <windows.h>
#include <setjmp.h>
#include <string.h>
#include <errno.h>
#include <windows.h>
#include <process.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern char INITSTART[], INITEND[], EXITSTART[], EXITEND[], BSSSTART[], BSSEND[];
extern char _TLSINITSTART[], _TLSINITEND[];

extern PASCAL struct defstr
{
    int flags;
    DWORD (*func)();
} startupStruct;

extern int _argc;
extern char** _argv;
extern char** _environ;
char* _oscmd;
char* _osenv;
HINSTANCE __hInstance;
unsigned _win32;
jmp_buf __exitbranch, __abortbranch;
static unsigned dllexists = 0;
unsigned _isDLL;
static int jumped;
void PASCAL __xceptinit(int* block);
void PASCAL __xceptrundown(void);

#pragma startup init 253
#pragma rundown destroy 3

static void init(void) { __thrdRegisterModule(__hInstance, _TLSINITSTART, _TLSINITEND); }
static void destroy(void) { __thrdUnregisterModule(__hInstance); }

// in the follow, the args are ONLY valid for DLLs
int __stdcall ___startup(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
    int rv;
    int exceptBlock[2];
    char quote;
    char* cmd;
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
                MessageBox(0, "NonShared data segment required", "Error", 0);
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
                int (*PASCAL dllMain)() = startupStruct.func;
                rv = (*dllMain)(hInst, fdwReason, lpvReserved) + 1;
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
                while (*cmd && *cmd == ' ')
                    cmd++;
                rv = (*startupStruct.func)(__hInstance, 0, cmd, 10) + 1;
            }
            else
            {
                rv = (*startupStruct.func)(_argc, _argv, _environ) + 1;
            }
        }
        else
            jumped++;
        if (!(startupStruct.flags & DLL) || fdwReason == DLL_PROCESS_DETACH)
        {
            __srproc(EXITSTART, EXITEND);
        }
    }
    else
        jumped++;
    rv--;
    if (!jumped)
        __xceptrundown();
    if (jumped || !(startupStruct.flags & DLL))
    {
        __crtexit(rv);  // never returns if linked to CRTDLL
        ExitProcess(rv);
    }
    return rv;
}