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

extern int __rtl_argc;
extern char** __rtl_argv;
extern char** __rtl_environ;
extern HMODULE __mainHInst;

extern unsigned _win32;
extern unsigned _isDLL;

extern jmp_buf __exitbranch, __abortbranch;

extern DWORD __unaligned_stacktop;

void __rtlInit(int *exceptBlock);
void __rtlShutdown(int jumped, int rv);

int main(int argc, char* argv[], char* environ[]);


#pragma entrypoint __C0Main

void __C0Main()
{
static int rv;
static int *exceptBlock;
static int jumped = 0;

    __asm mov [__unaligned_stacktop],esp
    __asm push 0
    __asm push 0
    __asm push ebp
    __asm mov ebp, esp
    __asm sub esp,8
    __asm mov [exceptBlock],esp
    __rtlinit(exceptBlock);
    __srproc(INITSTART, INITEND, 1);

    if (!(rv = setjmp(__abortbranch)))
    {
        if (!(rv = setjmp(__exitbranch)))
        {
            rv = main(__rtl_argc, __rtl_argv, __rtl_environ) + 1;
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