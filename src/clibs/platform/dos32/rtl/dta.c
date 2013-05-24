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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <dpmi.h>
#include <errno.h>
 #include "llp.h"

extern int __dtabuflen ;

UWORD __dtaseg;
static SELECTOR __dtasel;

#pragma startup initdta 253
#pragma rundown freedta 3

static void initdta(void)
{
   DPMI_REGS r ;
   DESCRIPTOR desc ;
   int val ;
   dpmi_alloc_real_memory(&__dtasel,&__dtaseg,(__dtabuflen + 15)/16); /* 2048 bytes */
}
static void freedta(void)
{
   dpmi_dealloc_real_memory(__dtasel);
}

int __nametodta(char *name, int index)
{
    int len = strlen(name) + 1;
   dpmi_copy_from_ds(__dtasel,index,name,len);
    return index;
}
int __buftodta(unsigned char *buf, int len)
{
   dpmi_copy_from_ds(__dtasel,0,buf,len);
    return 0;
}
int __dtatobuf(unsigned char *buf, int len)
{
   dpmi_copy_to_ds(buf,__dtasel,0,len);
    return 0;
}
