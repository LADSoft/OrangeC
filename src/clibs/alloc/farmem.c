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
*/#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include "libp.h"
#include <dpmi.h>
#include <dos.h>

void far *_RTL_FUNC farmalloc(size_t size)
{
   SELECTOR seg=0 ;
   void *s = malloc(size) ;
   if (s) {
      if (!dpmi_alloc_descriptors(&seg,1)) {
         dpmi_set_sel_access_rights(seg,0xc092) ;
         dpmi_set_sel_limit(seg, size-1) ;
         dpmi_set_sel_base(seg, (ULONG)s) ;
      } else {
         seg = 0 ;
         free(s) ;
      }
   }
   return MK_FP(seg,0) ;
}
void _RTL_FUNC farfree(void far *ptr)
{
   SELECTOR seg = FP_SEG(ptr) ;
   ULONG base ;
   if (seg == _CS || seg == _DS || seg == _SS)
      return ;
   if (!dpmi_get_sel_base(&base,seg)) {
      free((void *)base) ;
      dpmi_free_selector(seg) ;
   }
}