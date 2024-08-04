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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
#include <threads.h>
#include <sys\timeb.h>
#include <threads.h>
#include "libp.h"

#define THRD_SIG 0x44524854  // 'THRD'

int __thrd_rel_delay(const struct timespec* xt)
{
    struct timespec xt2, xt3;
    xt3 = *xt;
    timespec_get(&xt2, TIME_UTC);
    xt3.tv_sec -= xt2.tv_sec;
    xt3.tv_nsec -= xt2.tv_nsec;
    while (xt3.tv_nsec < 0)
    {
        xt3.tv_sec--;
        xt3.tv_nsec += 1000000000;
    }
    int t = xt3.tv_sec * 1000 + xt3.tv_nsec / 1000000;
    return t;
}
int _RTL_FUNC thrd_create(thrd_t* thr, thrd_start_t func, void* arg)
{
    int rv = __ll_thrdstart((struct ithrd**)thr, func, arg);
    if (rv == thrd_success)
    {
        (*(struct ithrd**)thr)->sig = THRD_SIG;
    }
    return rv;
}
thrd_t _RTL_FUNC thrd_current(void) { return (thrd_t)__ll_thrdcurrent(); }
int _RTL_FUNC thrd_detach(thrd_t thr)
{
    struct ithrd* p = (struct ithrd*)thr;
    if (p->sig == THRD_SIG && !p->join && !p->detach)
    {
        return __ll_thrd_detach(p);
    }
    return thrd_error;
}
int _RTL_FUNC thrd_equal(thrd_t thr0, thrd_t thr1)
{
    struct ithrd* cur0 = (struct ithrd*)thr0;
    struct ithrd* cur1 = (struct ithrd*)thr1;
    int rv = 0;
    __ll_enter_critical();
    if (cur0->sig == THRD_SIG && cur1->sig == THRD_SIG)
    {
        if (cur0->handle == cur1->handle)
            rv = 1;
    }
    __ll_exit_critical();
    return rv;
}
void _RTL_FUNC thrd_exit(int res)
{
    __ll_thrdexit(res);
    // never gets here
}
int _RTL_FUNC thrd_join(thrd_t thr, int* res)
{
    struct ithrd* p = (struct ithrd*)thr;
    if (p->sig == THRD_SIG && !p->join && !p->detach)
    {
        struct ithrd* current = (struct ithrd*)thrd_current();
        p->join = 1;
        int rv = __ll_thrdwait(p);
        if (res != NULL && __ll_thrdexitcode(p, res) != thrd_success)
            rv = thrd_error;
        if (__ll_thrd_detach(p) != thrd_success)
            rv = thrd_error;
        return rv;
    }
    return thrd_error;
}
void _RTL_FUNC thrd_sleep(const struct timespec* xt, struct timespec* rv)
{
    int t = __thrd_rel_delay(xt);
    if (t >= 0)
    {
        __ll_thrdsleep(t);
    }
    if (rv)
    {
        int t = __thrd_rel_delay(xt);
        if (t < 0)
        {
            rv->tv_sec = 0;
            rv->tv_nsec = 0;
        }
        else
        {
            rv->tv_sec = t / 1000;
            rv->tv_nsec = (t % 1000) * 1000000;
        }
    }
}
void _RTL_FUNC thrd_yield(void) { __ll_thrdsleep(0); }
