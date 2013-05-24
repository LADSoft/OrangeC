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
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dpmi.h>
#include "llp.h"

extern short __pspseg, __envseg;
int _RTL_DATA _argc, _RTL_DATA __argc;
char _RTL_DATA **_argv, _RTL_DATA **__argv;

#pragma startup argset 225

static void get_progname(char *buf)
{
    char * pos;
    SELECTOR sel;
    DPMI_REGS regs;
    int len;
    int temp;
    
    __doscall(0x19,&regs);
    buf[0] = regs.b.al + 'A';
    buf[1] = ':';
    /* get path */

    /* find file name, assumes DOS 3... */
    asm push edi
    asm push es
    asm mov ecx,-1
    asm sub edi,edi
   asm mov es,[__envseg]
    asm sub al,al
lp:
    asm repnz scasb
    asm cmp es:[edi],al
    asm jnz lp
    asm add edi,3
    asm movzx edx,byte ptr es:[edi+1]
  asm mov [pos],edi
  asm mov [sel],es
    asm mov ecx,-1
    asm repnz scasb
    asm not ecx
    asm mov [len],ecx
    asm pop es
    asm pop edi
    asm mov [temp],edx
    if (temp == ':')
        dpmi_copy_to_ds(buf,sel,(ULONG) pos,len);
    else
        dpmi_copy_to_ds(buf+2,sel,(ULONG) pos,len);
    

}
static void argset(void)
{
    char buf[260];
    char *bufp[300], *ocl, *_cmdline;
    int inquote = 0;
    ocl = _cmdline = malloc(128);
    dpmi_copy_to_ds(_cmdline,__pspseg,0x80,128);
  _cmdline[_cmdline[0]+1] = 0;
    _cmdline++;
    _argc = 1;
    while (*_cmdline) {
        while (isspace(*_cmdline)) _cmdline++;
        if (*_cmdline) {
            int i = 0;
            while ((inquote || !isspace(*_cmdline)) && *_cmdline)  {
            if (*_cmdline == '"' && (_cmdline == ocl+1 || *(_cmdline-1) != '\\')) {
                    inquote = !inquote ;
                    _cmdline++ ;
                    continue ;
                }
                buf[i++] = *_cmdline++;
            }
            buf[i++] = 0;
            bufp[_argc++] = strdup(buf) ;
        }
    }
   _argv = calloc((_argc+1),sizeof(char *));
   memcpy(_argv,bufp,_argc*sizeof(char *));
    _cmdline = ocl;
    get_progname(_cmdline);
   _argv[0] = _cmdline;
   __argv = _argv ;
   __argc = _argc ;
}