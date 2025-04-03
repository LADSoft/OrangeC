/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include <windows.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#undef errno
#include <wchar.h>
#include <locale.h>
#include <libp.h>
#include <stdbool.h>

int __raise(int signum, int code, void* addr);
extern int _win32;
extern char** _argv;
extern DWORD __unaligned_stacktop;
extern HINSTANCE __hInstance;
extern unsigned _isDLL;
extern unsigned char CODESTART[], CODEEND[];
extern bool ___rtl_initted;

typedef void CALLBACK trace_func(char* text, char* filename, PCONTEXT p, void* hInstance, void* stacktop, void *codestart, void *codeend);

void __ShowMessageBox(const char *text, const char *title)
{
    typedef void WINAPI Func(HWND, LPCSTR, LPCSTR, UINT);
    HMODULE aa = LoadLibrary("User32.dll");
    if (aa)
    {
        Func* ptr = (Func*)GetProcAddress(aa, "MessageBoxA");
        if (ptr)
        {
            ptr(0, text, title, MB_TASKMODAL);
            FreeLibrary(aa);
        }
        else
            _abort();
    }
    else
    {
        _abort();
    }
}

static int* _xceptblkptr;
/*static*/ PCONTEXT xxctxt;
/*static*/ void regdump(char* text, PCONTEXT p)
{
    __excepthook();
    HMODULE hmod = LoadLibrary("lsdbghelper");
    if (hmod)
    {
        trace_func* f = (trace_func*)GetProcAddress(hmod, "StackTraceWithCode");
        if (f)
            f(text, _argv[0], p, (void*)__hInstance, (void*)__unaligned_stacktop, CODESTART, CODEEND);
        FreeLibrary(hmod);
        if (f)
            return;
    }
    char buf[2048];
    sprintf(buf, "\n%s:(%s)\n", text, _argv[0]);
    sprintf(buf + strlen(buf), "CS:EIP %04X:%08X  SS:ESP %04X:%08X\n", p->SegCs, p->Eip, p->SegSs, p->Esp);
    sprintf(buf + strlen(buf), "EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X  flags: %08X\n", p->Eax, p->Ebx, p->Ecx, p->Edx,
            p->EFlags);
    sprintf(buf + strlen(buf), "EBP: %08X  ESI: %08X  EDI: %08X\n", p->Ebp, p->Esi, p->Edi);
    sprintf(buf + strlen(buf), " DS:     %04X   ES:     %04X   FS:     %04X   GS:     %04X\n", p->SegDs, p->SegEs, p->SegFs,
            p->SegGs);
    if (p->Eip < 0x80000000)
    {
        sprintf(buf + strlen(buf), "\n\nCS:EIP  ");
        for (int i = 0; i < 16; i++)
            sprintf(buf + strlen(buf), "%02X ", ((unsigned char*)p->Eip)[i]);
    }
    if (!_win32)
    {
        fprintf(stderr, buf);
        fflush(stderr);
    }
    else
    {
        __ShowMessageBox(buf, text);
    }
}
void __ll_sigsegv(int aa)
{
    regdump("Access Violation", xxctxt);
    _exit(1);
}
LONG ___xceptionhandle(PEXCEPTION_RECORD p, void* record, PCONTEXT context, void* param)
{

    int signum = -1, rv = 1;
    int code = -1;
    void* addr = 0;
    if (p->ExceptionFlags == 2)  // unwinding
        return 1;
        // if we get a C++ exception here, it is a 'loose' throw that needs an abort...
#ifndef __BUILDING_LSCRTL_DLL
    if (p->ExceptionCode == OUR_CPP_EXC_CODE)
        __call_terminate();
#endif
    addr = (void*)context->Eip;
    switch (p->ExceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            signum = SIGSEGV;
            code = SEGV_ACCERR;
            break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            signum = SIGBUS;
            code = BUS_ADRALN;
            break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        case EXCEPTION_GUARD_PAGE:
            signum = SIGSEGV;
            code = SEGV_MAPERR;
            break;
            //		case EXCEPTION_INVALID_INSTRUCTION:
        case EXCEPTION_PRIV_INSTRUCTION:
            signum = SIGILL;
            code = ILL_PRVOPC;
            break;
        case EXCEPTION_SINGLE_STEP:
            return EXCEPTION_CONTINUE_EXECUTION;
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            signum = SIGFPE;
            code = FPE_FLTSUB;
            break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            signum = SIGFPE;
            code = FPE_FLTDIV;
            break;
        case EXCEPTION_FLT_INEXACT_RESULT:
            signum = SIGFPE;
            code = FPE_FLTRES;
            break;
        case EXCEPTION_FLT_INVALID_OPERATION:
            signum = SIGFPE;
            code = FPE_FLTINV;
            break;
        case EXCEPTION_FLT_OVERFLOW:
            signum = SIGFPE;
            code = FPE_FLTOVF;
            break;
        case EXCEPTION_FLT_STACK_CHECK:
            signum = SIGFPE;
            code = -1;
            break;
        case EXCEPTION_FLT_UNDERFLOW:
            signum = SIGFPE;
            code = FPE_FLTUND;
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            signum = SIGFPE;
            code = FPE_INTDIV;
            break;
        case EXCEPTION_INT_OVERFLOW:
            signum = SIGFPE;
            code = FPE_INTOVF;
            break;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        case EXCEPTION_INVALID_DISPOSITION:
            break;
        case EXCEPTION_STACK_OVERFLOW:
            break;
        case OUR_C_EXC_CODE:
        {
            signum = p->ExceptionInformation[0];
            code = p->ExceptionInformation[1];
            addr = p->ExceptionInformation[2];
        }
    }
    xxctxt = context;
    if (signum != -1)
    {
        if (!___rtl_initted)
            ExitProcess(3);
        if (!_isDLL && !__raise(signum, code, addr))
            return 0;  // continue execution
    }
    return 1;  // continue search
}
void PASCAL __xceptinit(int* block)
{
    __asm {
		mov		eax,[block]
		mov		[eax+4],___xceptionhandle
		mov		ecx,fs:[0]
		mov		[eax],ecx
		mov		fs:[0],eax
    }
    ;
}
void PASCAL __xceptrundown(void)
{
    __asm {
		mov	eax,fs:[0]
		cmp	eax,___xceptionhandle  // C++ frames should be unloaded by now
		jnz	none  // this is a sanity check
		mov	eax,[eax]
		mov	fs:[0],eax
none:
    }
    ;
}
