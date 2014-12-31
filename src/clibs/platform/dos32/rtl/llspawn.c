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
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <dpmi.h>

#pragma pack(1)
struct sb {
    short envseg;
    short ocomtail;
    short scomtail;
    short ofcb1,sfcb1;
    short ofcb2,sfcb2;
    short reserved[2 + 2];
};
struct sb32 {
   int oenv;
   short senv;
   int ocomtail;
    short scomtail;
   int ofcb1;
   short sfcb1;
   int ofcb2;
   short sfcb2;
   short reserved[2 + 4];
};
extern UWORD __dtaseg;
extern int __pm308;

extern char _RTL_DATA **_environ ;

static char *fcbstring = "                ";

static char *createenviron(char **env, int *elen, char *file)
{
   int len = 0 ;
   char **dummy,*rv=0 ;
   if (!env)
      env = _environ ;
   dummy = env ;
   while (*dummy) {
      len += strlen(*dummy) + 1 ;
      dummy++ ;
   }
   len += strlen(file) +4;
   if (len) {
      len += 1 ;
      len += 15;
      len &= ~15;
      rv = malloc (len) ;
      if (rv) {
         char *p = rv ;
         if (!*env)
             *p++ = 0;
         else while(*env) {
            strcpy(p,*env) ;
            p+= strlen(p)+ 1 ;
            env++ ;
         }
         *p++ = 0 ;
         *p++ = 1;
         *p++ = 0;
         strcpy(p, file);
      }
   }
   *elen = len ;
   return rv ;
}
int __ll_spawn(char *file, char *parms, char **env, int mode)
{
    DPMI_REGS regs;
    struct sb spawnblock;
   struct sb32 spawnblock32 ;
    UWORD envseg;
    SELECTOR envsel;
   short _DSS ;
    char parms1[1024];
   int elen ;
   int rv = 0 ;
   char *block = createenviron(env, &elen, file) ;
   if (!block)
      return -1 ;
   strcpy(parms1+1,parms);
   strcat(parms1+1,"\r");
   parms1[0] = strlen(parms1+1);
   if (__pm308) {
      if (dpmi_alloc_real_memory(&envsel,&envseg,(elen+15)/16)) {
         free(block) ;
         return -1 ;
      }
      dpmi_copy_from_ds(envsel,0,block,elen) ;

      regs.h.dx = __nametodta(file,256);
      spawnblock.ocomtail =  __nametodta(parms1,512);
      spawnblock.scomtail =  __dtaseg;
      spawnblock.ofcb1 = __nametodta(fcbstring,768);
      spawnblock.ofcb2 = __nametodta(fcbstring,768+32);
      spawnblock.sfcb1 = spawnblock.sfcb2 = __dtaseg;
      spawnblock.envseg = envseg;
      regs.h.bx = __buftodta(&spawnblock,sizeof(struct sb));
      regs.b.al = 0;
      __doscall(0x4b,&regs);

      free(block) ;
      dpmi_dealloc_real_memory(envsel) ;

      if (regs.h.flags & 1)
         return -1;

      __doscall(0x4d,&regs);
      return regs.b.al;
   } else {
      asm mov word [_DSS],ds
      spawnblock32.oenv = block ;
      spawnblock32.senv = _DSS ;
      spawnblock32.ocomtail = parms1 ;
      spawnblock32.scomtail = _DSS ;
      spawnblock32.ofcb1 = fcbstring ;
      spawnblock32.sfcb1 = _DSS ;
      spawnblock32.ofcb2 = fcbstring ;
      spawnblock32.sfcb2 = _DSS ;
      asm {
         mov ax,0x4b00
         push ebx
         lea ebx,dword [spawnblock32]
         mov edx,[file]
         int 0x21
         pop ebx
         jnc  spawnok
         mov [rv],-1
      spawnok:
      }
      free(block) ;
      if (rv != -1) {
          asm {
              mov ax,0x4d00
            int 0x21
            mov byte [rv], al
          }
      }
      return rv ;
   }
}
