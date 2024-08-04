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

#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <signal.h>
#include "libp.h"
extern struct sigaction __sigtab[NSIG];
extern const struct sigaction __defsigtab[NSIG];

sighandler_t _RTL_FUNC signal(int signum, sighandler_t func)
{
    sighandler_t temp;
    if (signum >= NSIG || signum < 1 || (__sigtab[signum].sa_flags & SA_SIGINFO))
    {
        errno = EINVAL;
        return SIG_ERR;
    }
    temp = __sigtab[signum].sa_handler;
    __sigtab[signum] = __defsigtab[signum];
    if (func == SIG_DFL)
        __sigtab[signum] = __defsigtab[signum];
    else /* SIG_IGN && SIG_ERR gets used as-is,
                otherwise func is a valid func */
        __sigtab[signum].sa_handler = func;
    __ll_signal(signum, func);
    return temp;
}

int _RTL_FUNC sigaction(int signum, const struct sigaction* act, struct sigaction* oldact)
{
    if (signum >= NSIG || signum < 1)
    {
        errno = EINVAL;
        return -1;
    }
    if (oldact)
        *oldact = __sigtab[signum];
    if (act)
    {
        __sigtab[signum] = *act;
        if (!(__sigtab[signum].sa_flags & SA_SIGINFO) && __sigtab[signum].sa_handler == SIG_DFL)
        {
            __sigtab[signum].sa_handler = __defsigtab[signum].sa_handler;
        }
        __ll_signal(signum, __sigtab[signum].sa_handler);
    }
    return 0;
}

int _RTL_FUNC sigaddset(sigset_t* set, int v)
{
    *set |= 1 << v;
    return 0;
}
int _RTL_FUNC sigdelset(sigset_t* set, int v)
{
    *set &= ~(1 << v);
    return 0;
}
int _RTL_FUNC sigemptyset(sigset_t* set)
{
    *set = 0;
    return 0;
}
int _RTL_FUNC sigfillset(sigset_t* set)
{
    *set |= (1 << SIGINT) | (1 << SIGUSR1) | (1 << SIGUSR2) | (1 << SIGUSR3);
    *set |= (1 << SIGBREAK);
    return 0;
}
int _RTL_FUNC sigismember(const sigset_t* set, int v) { return !!(*set & (1 << v)); }
