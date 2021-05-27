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
#include "libp.h"

void _RTL_FUNC call_once(once_flag* flag, void (*func)(void))
{
    __ll_enter_critical();
    if (!flag->called)
    {
        flag->called = 1;
        __ll_exit_critical();
        (*func)();
    }
    else
    {
        __ll_exit_critical();
    }
}
int _RTL_FUNC timespec_get(struct timespec* xt, int base)
{
    if (base == TIME_UTC)
    {
        struct _timeb tb;
        __ll_xtime(&tb);
        xt->tv_nsec = tb.millitm * 1000000;
        xt->tv_sec = tb.time;
        return base;
    }
    return 0;  // error
}
