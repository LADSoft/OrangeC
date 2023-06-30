/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
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

#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <signal.h>
#include <fenv.h>
#include <dos.h>
#include <windows.h>
#include "libp.h"
#include <stdbool.h>
extern void __ll_sigsegv(int);
extern PCONTEXT xxctxt;
extern void regdump(char* text, PCONTEXT p);
extern unsigned _isDLL;

int _abterm = 0;

void _RTL_FUNC _abort()
{
    __aborthook();
    fprintf(stderr, "\nAbnormal program termination");
    fflush(stderr);
    _abterm = 1;
    _exit(3);
}

static void sigterm(int aa) { _exit(1); }
static void sigint(int aa) { _exit(1); }
static void sigill(int aa)
{
    //   fprintf(stderr,"\nIllegal instruction") ;
    regdump("Illegal instruction", xxctxt);
    fflush(stderr);
    _exit(1);
}
static void sigfp(int aa)
{
    //   unsigned short bb;
    //   __asm fnstsw word [bb];
    fenv_t env;
    __asm fnstenv[env];
    //   fprintf(stderr,"\nfloating point or divide by zero exception") ;
    //   fprintf(stderr," %04X",bb);
    if (env.statusWord)
        fprintf(stderr,
                "\nFloating point exception\n\n"
                "FPU SW: %04X  CS:EIP %04X:%08X\n",
                env.statusWord, env.fCS1, env.fIP);
    else
        regdump("Divide by zero exception", xxctxt);
    fflush(stderr);
    _exit(1);
}
static void sigabort(int aa) { _abort(); }
struct sigaction __defsigtab[NSIG] = {{SIG_ERR}, {SIG_ERR}, {sigint},  {SIG_ERR},      {sigill},  {SIG_ERR}, {SIG_ERR}, {SIG_ERR},
                                      {sigfp},   {SIG_ERR}, {SIG_ERR}, {__ll_sigsegv}, {SIG_ERR}, {SIG_ERR}, {SIG_ERR}, {sigterm},
                                      {SIG_IGN}, {SIG_IGN}, {SIG_ERR}, {SIG_ERR},      {SIG_IGN}, {sigint},  {sigabort},
{SIG_IGN}};

struct sigaction __sigtab[NSIG];
static char insignal[NSIG];

#pragma startup siginit 29

static void siginit(void)
{
    int i;
    for (i = 0; i < NSIG; i++)
    {
        __defsigtab[i].sa_flags |= SA_RESETHAND;
        __sigtab[i] = __defsigtab[i];
    }
}
static int raiseinternal(int sig, int source, int code, void* address)
{
    struct sigaction temp;
    if (sig >= NSIG || sig < 1)
    {
        errno = EINVAL;
        return 1;
    }
    temp = __sigtab[sig];
    if (_isDLL && temp.sa_handler == __defsigtab[sig].sa_handler)
    {
        // if this is a DLL and we have the default handler
        // try to send the signal to the main program in the process
        ULONG_PTR params[3];
        params[0] = (ULONG_PTR)sig;
        params[1] = (ULONG_PTR)code;
        params[2] = (ULONG_PTR)address;
        // note that if the main program isn't compiled with orange c, it will trigger the unfiltered exception handler
        // and probably abort
        RaiseException(OUR_C_EXC_CODE, EXCEPTION_CONTINUABLE, 3, (DWORD*)&params[0]);
    }
    if (!(temp.sa_flags & SA_SIGINFO))
    {
        if (temp.sa_handler == SIG_ERR)
            return 1;
        if (temp.sa_handler == SIG_IGN)
            return 0;
    }
    bool cont = true;
    while (cont)
    {
        cont = false;
        if (!(temp.sa_flags & SA_NODEFER))
            if (insignal[sig])
                cont = true;
        if (temp.sa_mask)
            for (int i = 1; i < NSIG; i++)
                if (insignal[i] && ((1 << i) & temp.sa_mask))
                    cont = true;
        if (cont)
            sleep(1);
    }
    insignal[sig]++;
    if (temp.sa_flags & SA_RESETHAND)
        __sigtab[sig] = __defsigtab[sig];
    if (!(temp.sa_flags & SA_SIGINFO))
    {
        (*temp.sa_handler)(sig);
    }
    else
    {
        siginfo_t info = {0};
        info.si_signo = sig;
        info.si_code = code;
        info.si_addr = address;
        (*temp.sa_sigaction)(sig, &info, NULL);
    }
    insignal[sig]--;
    __ll_cancelsleep();
    return 0;
}
// library internal func
int __raise(int sig, int code, void* addr) { return raiseinternal(sig, SI_KERNEL, code, addr); }
int _RTL_FUNC raise(int sig) { return raiseinternal(sig, SI_USER, -1, NULL); }
