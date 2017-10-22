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

#define THRD_SIG 0x44524854 // 'THRD'

int __thrd_rel_delay(const timespec *xt)
{
    timespec xt2, xt3;
    xt3 = *xt;
    timespec_get(&xt2, TIME_UTC);
    xt3.sec -= xt2.sec;
    xt3.nsec -= xt2.nsec;
    while (xt3.nsec < 0)
    {
        xt3.sec--;
        xt3.nsec += 1000000000;
    }
    int t = xt3.sec * 1000 + xt3.nsec / 1000000;
    return t;
}
int     _RTL_FUNC thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
    int rv = __ll_thrdstart((struct ithrd **)thr, func, arg);
    if (rv == thrd_success)
    {
        (*(struct ithrd **)thr)->sig = THRD_SIG;
    }
    return rv;
}
thrd_t  _RTL_FUNC thrd_current(void)
{
    return (thrd_t)__ll_thrdcurrent();
}
int     _RTL_FUNC thrd_detach(thrd_t thr)
{
    struct ithrd *p = (struct ithrd *)thr;
    if (p->sig == THRD_SIG && !p->join && !p->detach)
    {
        return __ll_thrd_detach(p);
    }
    return thrd_error;
}
int     _RTL_FUNC thrd_equal(thrd_t thr0, thrd_t thr1)
{
    struct ithrd *cur0 = (struct ithrd *)thr0;
    struct ithrd *cur1 = (struct ithrd *)thr1;
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
void    _RTL_FUNC thrd_exit(int res)
{
    __ll_thrdexit(res);
    // never gets here
}
int     _RTL_FUNC thrd_join(thrd_t thr, int *res)
{
    struct ithrd *p = (struct ithrd *)thr;
    if (p->sig == THRD_SIG && !p->join && !p->detach)
    {
        struct ithrd *current = (struct ithrd *)thrd_current();
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
void    _RTL_FUNC thrd_sleep(const timespec *xt, timespec *rv)
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
            rv->sec = 0;
            rv->nsec = 0;
        }
        else
        {
            rv->sec = t / 1000;
            rv->nsec = (t % 1000) * 1000000;
        }
    }
}
void    _RTL_FUNC thrd_yield(void)
{
    __ll_thrdsleep(0);
}
