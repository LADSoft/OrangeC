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

#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <stdlib.h>
#include <errno.h>
static __int64 startTime;

#pragma startup initClock 32

static void initClock() { startTime = __ll_ticks(); }
clock_t _RTL_FUNC clock(void) { return (int)(__ll_ticks() - startTime); }

int _RTL_FUNC clock_getres(clockid_t clk_id, struct timespec *res)
{
    if (!res)
    {
        errno = EFAULT;
        return -1;
    }
    switch(clk_id)
    {
	case CLOCK_REALTIME:
            {
                res->tv_sec = 0;
                res->tv_nsec = 1000000000/1000;
            }
            return 0;
	case CLOCK_MONOTONIC:
            {
                if (!__ll_xtimehp(res, 1))
                    return 0;
            }
            break;
	case CLOCK_PROCESS_CPUTIME_ID:
            {
                if (!__ll_xtimeprocess(res, 1))
                    return 0;
            }
            break;
        case CLOCK_THREAD_CPUTIME_ID:
            {
                if (!__ll_xtimethread(res, 1))
                    return 0;
            }
            break;
    }
    errno = EINVAL;
    return -1;
}
int _RTL_FUNC clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    if (!tp)
    {
        errno = EFAULT;
        return -1;
    }
    switch(clk_id)
    {
	case CLOCK_REALTIME:
            {
                struct _timeb t;
                __ll_xtime(&t);
                tp->tv_sec = t.time;
                tp->tv_nsec = t.millitm * 1000000;
            }
            return 0;
	case CLOCK_MONOTONIC:
            {
                if (!__ll_xtimehp(tp, 0))
                    return 0;
            }
            break;
	case CLOCK_PROCESS_CPUTIME_ID:
            {
                if (!__ll_xtimeprocess(tp, 0))
                    return 0;
            }
            break;
        case CLOCK_THREAD_CPUTIME_ID:
            {
                if (!__ll_xtimethread(tp, 0))
                    return 0;
            }
            break;
        default:
            break;
    }
    errno = EINVAL;
    return -1;
}
int _RTL_FUNC clock_settime(clockid_t clk_id, const struct timespec *tp)
{
    if (!tp)
    {
        errno = EFAULT;
        return -1;
    }
    switch(clk_id)
    {
	case CLOCK_REALTIME:
            {
               time_t aa = tp->tv_sec;
               if (!stime(&aa))
                   return 0;
            }
            errno = EPERM;
            break;
	case CLOCK_MONOTONIC:
	case CLOCK_PROCESS_CPUTIME_ID:
        case CLOCK_THREAD_CPUTIME_ID:
            errno = EPERM;
        default:
            errno = EINVAL;
            break;
    }
    return -1;
}