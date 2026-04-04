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

extern HINSTANCE __hInstance			;

extern HMODULE __mainHInst;

extern unsigned _win32;
extern unsigned _isDLL;

extern jmp_buf __exitbranch, __abortbranch;

extern DWORD __unaligned_stacktop;
int PASCAL DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);

void __rtlInit(int *exceptBlock);
void __rtlShutdown(int jumped, int rv);

#pragma entrypoint __C0DllMain

int __stdcall ___C0DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
    int rv;
    int exceptBlock[2];
    char quote;
    char *cmd;
    int jumped = 0;
    _isDLL = 1;
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        static int dllexists = 0;
        if (!dllexists)
            dllexists++;
        else
        {
            MessageBox(0, "NonShared data segment required", "Error", 0);
            return 1;
        }
    	__rtlinit(&exceptBlock);
	__srproc(INITSTART, INITEND, 1);
    }
    if (!(rv = setjmp(__abortbranch)))
    {
        if (!(rv = setjmp(__exitbranch)))
        {
            rv = DllMain(hInst, fdwReason, lpvReserved) + 1;
        }
        else
            jumped++;
        if (fdwReason == DLL_PROCESS_DETACH)
        {
            __srproc(EXITSTART, EXITEND, 0);
        }
    }
    else
        jumped++;
    rv--;
     __rtlshutdown(jumped, rv);
    return rv;
}
