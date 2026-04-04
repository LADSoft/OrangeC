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

extern HINSTANCE __hInstance;

extern HMODULE __mainHInst;

extern unsigned _win32;
extern unsigned _isDLL;

extern jmp_buf __exitbranch, __abortbranch;

extern DWORD __unaligned_stacktop;

void __rtlInit(int *exceptBlock);
void __rtlShutdown(int jumped, int rv);
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

#pragma entrypoint __C0WinMain

void __C0WinMain()
{
static int rv;
static int *exceptBlock;
static int jumped = 0;
static char quote;
static char *cmd;

    __asm mov [__unaligned_stacktop],esp
    __asm push 0
    __asm push 0
    __asm push ebp
    __asm mov ebp, esp
    __asm sub esp,8
    __asm mov [exceptBlock],esp
    _win32 = TRUE;
    __rtlinit(exceptBlock);
    __srproc(INITSTART, INITEND, 1);
    if (!(rv = setjmp(__abortbranch)))
    {
        if (!(rv = setjmp(__exitbranch)))
        {
            cmd = GetCommandLineA();
            quote = *cmd;
            while ((!quote || *cmd != ' ') && *cmd)
            {
                if (*cmd == '"')
                    quote ^= *cmd;
                 cmd++;
            }
            while (*cmd && *cmd == ' ')
            cmd++;
            rv = WinMain(__hInstance, 0, cmd, 10) + 1;
        }
        else
            jumped++;
        __srproc(EXITSTART, EXITEND, 0);
    }
    else
        jumped++;
    rv--;
    __rtlshutdown(jumped, rv);
    ExitProcess(rv);
}