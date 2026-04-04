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

void PASCAL __xceptinit(int* block);
void PASCAL __xceptrundown(void);

extern HINSTANCE __hInstance;

extern int __rtl_argc;
extern char** __rtl_argv;
extern char** __rtl_environ;

#pragma startup init 3
#pragma rundown destroy 3
#pragma startup do_args 31
static void init(void) { __thrdRegisterModule(__hInstance, _TLSINITSTART, _TLSINITEND); }
static void destroy(void) { __thrdUnregisterModule(__hInstance); }

extern void __main_argset();
extern void __main_envset();

static void do_args(void)
{
    __main_argset();
    __main_envset();
    __rtl_argc = __argc;
    __rtl_argv = __argv;
    __rtl_environ = _environ;
}
// in the follow, the args are ONLY valid for DLLs
void __rtlinit(int *exceptBlock)
{
    __hInstance = GetModuleHandleA(0);
    __xceptinit(exceptBlock);
    _llfpinit();
    __threadinit();
}
void __rtlshutdown(int jumped, int rv)
{
    if (!jumped)
        __xceptrundown();
}
