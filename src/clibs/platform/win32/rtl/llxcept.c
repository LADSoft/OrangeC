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
#include <windows.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
extern int _win32 ;
static int *_xceptblkptr;
/*static*/ PCONTEXT xxctxt;
LONG (* ___cppxcept)(PEXCEPTION_RECORD p, void *record , CONTEXT *context, void *param ) ;

LONG ___cppxceptionhandle(PEXCEPTION_RECORD p, void *record, CONTEXT *context, void *param) ;

/*static*/ void regdump(char *text, PCONTEXT p)
{
   char buf[1024] ;
      sprintf(buf,"\n%s\n\n",text);
      sprintf(buf+strlen(buf),"CS:EIP %04X:%08X  SS:ESP %04X:%08X\n",p->SegCs,p->Eip,p->SegSs,p->Esp);
      sprintf(buf+strlen(buf),"EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X  flags: %08X\n",
            p->Eax, p->Ebx, p->Ecx, p->Edx, p->EFlags);
      sprintf(buf+strlen(buf),"EBP: %08X  ESI: %08X  EDI: %08X\n",p->Ebp,p->Esi,p->Edi);
      sprintf(buf+strlen(buf)," DS:     %04X   ES:     %04X   FS:     %04X   GS:     %04X\n",
         p->SegDs,p->SegEs,p->SegFs,p->SegGs);
   if (!_win32) {
      fprintf(stderr,buf) ;
   } else {
      MessageBox(0,buf,text,0) ;
   }
}
void __ll_sigsegv(int aa)
{
   regdump("Access Violation",xxctxt) ;
   _exit(1) ;
}
LONG ___xceptionhandle(PEXCEPTION_RECORD p, void *record, PCONTEXT context, void *param) 
{

   int signum = -1,rv = 1;
   if (p->ExceptionFlags == 2) // unwinding
   		return 1 ;
   if (___cppxcept)
   	(*___cppxcept)(p,record,context,param) ; // won't return if it was a throw raise
   switch(p->ExceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION:
		case EXCEPTION_DATATYPE_MISALIGNMENT:
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		case EXCEPTION_GUARD_PAGE:
			signum = SIGSEGV;
			break ;
//		case EXCEPTION_INVALID_INSTRUCTION:
		case EXCEPTION_PRIV_INSTRUCTION:
			signum = SIGILL;
			break ;
		case EXCEPTION_SINGLE_STEP:
	         return EXCEPTION_CONTINUE_EXECUTION ;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		case EXCEPTION_FLT_INEXACT_RESULT:
		case EXCEPTION_FLT_INVALID_OPERATION:
		case EXCEPTION_FLT_OVERFLOW:
		case EXCEPTION_FLT_STACK_CHECK:
		case EXCEPTION_FLT_UNDERFLOW:
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			signum = SIGFPE ;
			break ;
		case EXCEPTION_INT_OVERFLOW:
			break ;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		case EXCEPTION_INVALID_DISPOSITION:
			break ;
		case EXCEPTION_STACK_OVERFLOW:
			break ;
	}
	xxctxt = context ;
	if (signum != -1)
      if (!raise(signum))
         return 0 ; // continue execution

   return 1 ; // continue search
}
void PASCAL __xceptinit(int *block)
{
	asm {
		mov		eax,[block]
		mov		[eax+4],___xceptionhandle
		mov		ecx,fs:[0]
		mov		[eax],ecx
		mov		fs:[0],eax
	} ;
}
void PASCAL __xceptrundown(void)
{
	asm {
		mov	eax,fs:[0]
		cmp	eax,___xceptionhandle // C++ frames should be unloaded by now
		jnz	none				  // this is a sanity check
		mov	eax,[eax]
		mov	fs:[0],eax
none:
	} ;
}
