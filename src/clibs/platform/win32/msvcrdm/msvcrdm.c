/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

extern void _import exit(int);
extern int _import raise(int);
#include <windows.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

extern void _import __getmainargs(void*, void*, void*, int, int*);
extern void _import _assert(const char*, const char*, int);
extern char _import _iob;
extern int _import* _errno();
extern int _import* __doserrno();
#undef _daylight;
#undef _timezone;
#undef _tzname
extern int _import _daylight;
extern int _import _timezone;
extern char* _import _tzname;

static int* _xceptblkptr;
int _RTL_DATA _argc;
char _RTL_DATA** _argv;
char _RTL_DATA** _environ;

FILE* __stdin;
FILE* __stdout;
FILE* __stderr;
FILE* __stdaux;
FILE* __stdprn;

void __thrdRegisterModule(HANDLE module, void* tlsStart, void* tlsEnd) {}
void __thrdUnregisterModule(HANDLE module) {}

void __crtexit(int n) { exit(n); }
LONG ___xceptionhandle(PEXCEPTION_RECORD er, void* frame, PCONTEXT context, void* dispatchercontext)
{

    int signum = -1, rv = 0;
    if (er->ExceptionFlags == 2)  // unwinding
        return 1;
    switch (er->ExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
        case EXCEPTION_DATATYPE_MISALIGNMENT:
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            signum = SIGSEGV;
            break;
        case EXCEPTION_PRIV_INSTRUCTION:
            signum = SIGILL;
            break;
        case EXCEPTION_BREAKPOINT:
            // IF they really want to put a breakpoint in, let them but mmove on
            return EXCEPTION_CONTINUABLE;
        case EXCEPTION_SINGLE_STEP:
            break;
        case EXCEPTION_FLT_DENORMAL_OPERAND:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_FLT_INEXACT_RESULT:
        case EXCEPTION_FLT_INVALID_OPERATION:
        case EXCEPTION_FLT_OVERFLOW:
        case EXCEPTION_FLT_STACK_CHECK:
        case EXCEPTION_FLT_UNDERFLOW:
            signum = SIGFPE;
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            break;
        case EXCEPTION_INT_OVERFLOW:
            break;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        case EXCEPTION_INVALID_DISPOSITION:
            break;
        case EXCEPTION_STACK_OVERFLOW:
            break;
    }
    if (signum != -1)
        rv = !raise(signum);

    if (rv)
    {
        EXCEPTION_POINTERS xx;
        xx.ExceptionRecord = er;
        xx.ContextRecord = context;
        return UnhandledExceptionFilter(&xx);
    }
    return EXCEPTION_CONTINUABLE;
}
void PASCAL __xceptinit(int* block)
{
    int newmode = 0;
    _xceptblkptr = block;
    __asm mov eax, [block];
    __asm mov[eax + 4], offset ___xceptionhandle;
    __asm mov ecx, fs : [0];
    __asm mov[eax], ecx;
    __asm mov fs : [0], eax;

    // msvcrdm startup
    __stdin = __getStream(0);
    __stdout = __getStream(1);
    __stderr = __getStream(2);
    __stdaux = __getStream(3);
    __stdprn = __getStream(4);
    __getmainargs(&_argc, &_argv, &_environ, 0, &newmode);
}
void PASCAL __xceptrundown(void)
{
    __asm mov eax, [_xceptblkptr];
    __asm cmp eax, fs : [0];
    __asm jnz nounset;
    __asm mov eax, [eax];
    __asm mov fs : [0], eax;
nounset:
    return;
}
void PASCAL __llfpinit(void) {}

void _RTL_FUNC __assertfail(const char* __who, const char* __file, int __line, const char* __func, const char* __msg)
{
    _assert(__who, __file, __line);
}

FILE* __getStream(int stream)
{
    if (stream < 5)
        return &_iob + 32 * stream;
    return 0;
}

void __threadinit(void) {}

int* _RTL_FUNC __GetErrno(void) { return _errno(); }
int* _RTL_FUNC __GetDosErrno(void) { return __doserrno(); }
int* _RTL_FUNC __getDaylight(void) { return &_daylight; }
long* _RTL_FUNC __getTimezone(void) { return &_timezone; }
char** _RTL_FUNC __getTzName(void) { return &_tzname; }
