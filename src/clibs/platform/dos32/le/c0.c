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
