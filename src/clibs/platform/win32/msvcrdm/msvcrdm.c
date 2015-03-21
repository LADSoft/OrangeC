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
extern void _import exit(int);
extern int _import raise(int);
#include <windows.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

extern void _import __getmainargs(void *,void *,void*,int, int *) ;
extern void _import _assert(const char *, const char *, int);
extern char _import _iob ;
extern int _import *_errno();
extern int _import *__doserrno();
extern int _import _daylight;
extern int _import _timezone;
extern char * _import _tzname;

static int *_xceptblkptr;
int _RTL_DATA _argc;
char _RTL_DATA **_argv;
char _RTL_DATA **_environ;

FILE *__stdin ;
FILE *__stdout ;
FILE *__stderr ;
FILE *__stdaux ;
FILE *__stdprn ;

void __crtexit(int n)
{
    exit(n);
}
LONG ___xceptionhandle(PEXCEPTION_RECORD er, void *frame,
            PCONTEXT context, void *dispatchercontext)
{

    int signum = -1,rv = 0;
    if (er->ExceptionFlags == 2) // unwinding
   		return 1 ;
    switch(er->ExceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:
        case EXCEPTION_DATATYPE_MISALIGNMENT:
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            signum = SIGSEGV;
            break ;
        case EXCEPTION_PRIV_INSTRUCTION:
            signum = SIGILL;
            break ;
      case EXCEPTION_BREAKPOINT:
         // IF they really want to put a breakpoint in, let them but mmove on
         return EXCEPTION_CONTINUABLE ;
        case EXCEPTION_SINGLE_STEP:
            break ;
        case EXCEPTION_FLT_DENORMAL_OPERAND:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_FLT_INEXACT_RESULT:
        case EXCEPTION_FLT_INVALID_OPERATION:
        case EXCEPTION_FLT_OVERFLOW:
        case EXCEPTION_FLT_STACK_CHECK:
        case EXCEPTION_FLT_UNDERFLOW:
            signum = SIGFPE ;
            break ;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            break ;
        case EXCEPTION_INT_OVERFLOW:
            break ;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        case EXCEPTION_INVALID_DISPOSITION:
            break ;
        case EXCEPTION_STACK_OVERFLOW:
            break ;
    }
    if (signum != -1)
        rv = !raise(signum);

   if (rv) {
        EXCEPTION_POINTERS xx;
        xx.ExceptionRecord = er ;
        xx.ContextRecord = context ;
        return UnhandledExceptionFilter(&xx) ;
    } else
        return EXCEPTION_CONTINUABLE ;
}
void PASCAL __xceptinit(int *block)
{
    int newmode = 0;
    _xceptblkptr = block ;
    asm mov eax,[block]
   asm mov [eax+4],offset ___xceptionhandle
    asm mov	ecx,fs:[0]
    asm mov [eax],ecx
    asm mov fs:[0],eax
    
    // msvcrdm startup
   __stdin = __getStream(0) ;
   __stdout = __getStream(1) ;
   __stderr = __getStream(2) ;
   __stdaux = __getStream(3) ;
   __stdprn = __getStream(4) ;
   __getmainargs(&_argc,&_argv,&_environ,0, &newmode) ;
}
void PASCAL __xceptrundown(void)
{
    asm mov eax,[_xceptblkptr]
   asm cmp eax,fs:[0]
  asm jnz nounset
    asm mov eax,[eax]
    asm mov fs:[0],eax
nounset:
    return ;
}
void PASCAL __llfpinit(void)
{
}

void _RTL_FUNC __assertfail( const char *__who, const char *__file, 
                            int __line, const char *__func, const char *__msg )
{
    _assert(__who, __file, __line);
}

FILE *__getStream(int stream)
{
   if (stream < 5)
      return &_iob + 32 * stream ;
   else
      return 0 ;
}

void __threadinit(void)
{
}

int *_RTL_FUNC __GetErrno(void)
{
    return _errno();
}
int *_RTL_FUNC __GetDosErrno(void)
{
    return __doserrno();
}
int *_RTL_FUNC __getDaylight(void)
{
    return &_daylight;
}
int *_RTL_FUNC __getTimezone(void)
{
    return &_timezone;
}
char **_RTL_FUNC __getTzName(void)
{
    return &_tzname;
}
