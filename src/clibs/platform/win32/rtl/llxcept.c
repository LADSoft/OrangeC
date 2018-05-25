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

#include <windows.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#undef errno
#include <wchar.h>
#include <locale.h>
#include <libp.h>
extern int _win32 ;
extern char **_argv;

static int *_xceptblkptr;
/*static*/ PCONTEXT xxctxt;
/*static*/ void regdump(char *text, PCONTEXT p)
{
   char buf[1024] ;
      sprintf(buf,"\n%s:(%s)\n",text, _argv[0]);
      sprintf(buf+strlen(buf),"CS:EIP %04X:%08X  SS:ESP %04X:%08X\n",p->SegCs,p->Eip,p->SegSs,p->Esp);
      sprintf(buf+strlen(buf),"EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X  flags: %08X\n",
            p->Eax, p->Ebx, p->Ecx, p->Edx, p->EFlags);
      sprintf(buf+strlen(buf),"EBP: %08X  ESI: %08X  EDI: %08X\n",p->Ebp,p->Esi,p->Edi);
      sprintf(buf+strlen(buf)," DS:     %04X   ES:     %04X   FS:     %04X   GS:     %04X\n",
         p->SegDs,p->SegEs,p->SegFs,p->SegGs);
   if (!_win32) {
      fprintf(stderr,buf) ;
      fflush(stderr);
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
   // if we get a C++ exception here, it is a 'loose' throw that needs an abort...
   if (p->ExceptionCode == OUR_CPP_EXC_CODE) 
       __call_terminate();
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
