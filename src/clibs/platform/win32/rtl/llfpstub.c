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
#include <signal.h>
#include <fenv.h>

int _fltused;

static fenv_t dfltenv;

fenv_t _RTL_DATA * const __fe_default_env = &dfltenv;

unsigned int _RTL_DATA _default87 = 0x133F;

unsigned _RTL_FUNC _status87(void)
{
   unsigned short work ;
   asm fnstsw [work] ;
   asm fwait ;
   return work ;
}
unsigned _RTL_FUNC _clear87(void)
{
	unsigned short work ;
	asm fnstsw	[work ];
   asm fwait ;
	asm fnclex
	return work ;
}
unsigned _RTL_FUNC _control87(unsigned mask1, unsigned mask2)
{
	unsigned short work ;
	asm fstcw [work];
	mask1 &= mask2;
	mask2 = ~mask2;
	asm fwait
	work &= mask2;
	work |= mask1;
	asm fldcw [work]
	return work ;
}

void _RTL_FUNC _fpreset(void)
{
   asm finit ;
   _control87(_default87,0x1fff) ;
}
void __stdcall __llfpinit(void)
{
   _fpreset() ;
   asm fnstenv [dfltenv]
   _fpreset() ;
}
