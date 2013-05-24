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
#include "i86.h"
#include "dpmi.h"

void _RTL_FUNC _intr(int intr, union REGPACK *r)
{
   DPMI_REGS dr ;
   dr.d.edi = r->x.edi ;
   dr.d.esi = r->x.esi ;
   dr.d.ebp = r->x.ebp ;
   dr.d.eax = r->x.eax ;
   dr.d.ecx = r->x.ecx ;
   dr.d.edx = r->x.edx ;
   dr.d.ebx = r->x.ebx ;
   dr.h.ds = r->x.ds ;
   dr.h.es = r->x.es ;
   dr.h.fs = r->x.fs ;
   dr.h.gs = r->x.gs ;
   dr.h.flags = r->x.flags | 0x42;
   dr.h.ss = 0 ;
   dr.h.sp = 0 ;
   dpmi_simulate_real_interrupt(intr,&dr);
   r->x.edi = dr.d.edi ;
   r->x.esi = dr.d.esi ;
   r->x.ebp = dr.d.ebp ;
   r->x.eax = dr.d.eax ;
   r->x.ecx = dr.d.ecx ;
   r->x.edx = dr.d.edx ;
   r->x.ebx = dr.d.ebx ;
   r->x.ds = dr.h.ds ;
   r->x.es = dr.h.es ;
   r->x.fs = dr.h.fs ;
   r->x.gs = dr.h.gs ;
   r->x.flags = dr.h.flags ;
}