/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <setjmp.h>
#include <string.h>
extern char CODESTART[], INITSTART[], INITEND[], EXITSTART[], EXITEND[], BSSSTART[], BSSEND[];
extern int _argc;
extern char **_argv;
extern char **_environ;

jmp_buf __exitbranch, __abortbranch;
unsigned __isDLL = 0;	// not a dll
unsigned __pm308;
unsigned _linear = 0;

extern int main();
extern void __stdcall monitor_init();
static void Lock()
{
    asm mov ecx, CODESTART
    asm mov esi, BSSEND
    asm sub esi, ecx
    asm {
        mov cx,bx
        shr ebx,16
        mov di,si
        shr edi,16
        mov ax,0x600
        int 0x31
    }	
}
static void UnLock()
{
    asm mov ecx, CODESTART
    asm mov esi, BSSEND
    asm sub esi, ecx
    asm {
        mov cx,bx
        shr ebx,16
        mov di,si
        shr esi,16
        mov ax,0x601
        int 0x31
    }
}
int __startup()
{
    int rv;
    asm	mov	[__pm308],edx
    Lock();
    memset(BSSSTART, 0, BSSEND-BSSSTART);
    __srproc(INITSTART, INITEND);
    if (!(rv = setjmp(__abortbranch)))
    {
        if (!(rv = setjmp(__exitbranch)))
        {
            monitor_init();
            rv = main(_argc, _argv, _environ) + 1;
        }
        __srproc(EXITSTART, EXITEND);
    }
    UnLock();
    return rv - 1;
}
