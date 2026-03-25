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
#include <libp.h>


extern void _import _exit(int);

extern char INITSTART[], INITEND[], EXITSTART[], EXITEND[], BSSSTART[], BSSEND[];
extern char _TLSINITSTART[], _TLSINITEND[];
extern int __isdll;

extern HINSTANCE __hInstance;

extern int _win32;
extern int _isDLL;

extern int _import __argc;
extern char _import** __argv;
extern char _import** _environ;

extern int __rtl_argc;
extern char** __rtl_argv;
extern char** __rtl_environ;

#pragma startup init 3
#pragma rundown destroy 3

void __thrdRegisterModule(HANDLE, void*, void*);
void __thrdUnRegisterModule(HANDLE);
static void init(void)
{
    HANDLE handle;
    int eip;
    __asm call label1;
label1:
    __asm pop [eip]
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)eip, &handle);
    __thrdRegisterModule(handle, _TLSINITSTART, _TLSINITEND);
}
static void destroy(void)
{
    HANDLE handle;
    int eip;
    __asm call label1;
label1:
    __asm pop [eip]
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)eip, &handle);
    __thrdUnregisterModule(handle);
}

static void _dorundown(void);
void __stdcall ___lsdllinit(void* tlsStart, void* tlsEnd, DWORD flags, void (*rundown)(), int* exceptBlock, HMODULE mainInst);
void __rtlinit(int *exceptBlock)
{
    int flags = 0;
    if (_win32)
        flags |= GUI;
    if (_isDLL)
        flags |= DLL;
    __hInstance = GetModuleHandleA(0);
    ___lsdllinit(_TLSINITSTART, _TLSINITEND, flags, _dorundown, exceptBlock, _isDLL ? NULL : __hInstance);
    __rtl_argc = __argc;
    __rtl_argv = __argv;
    __rtl_environ = _environ;
}
void __rtlshutdown(int jumped, int rv)
{
}
static void _dorundown(void) { __srproc(EXITSTART, EXITEND, 0); }
