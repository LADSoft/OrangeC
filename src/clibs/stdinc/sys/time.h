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

#pragma GCC system_header

#ifndef __SYS_TIME_H
#define __SYS_TIME_H

#    include <_defs.h>

#ifndef RC_INVOKED
#ifndef _TIMEVAL_DEFINED
#    define _TIMEVAL_DEFINED
struct timeval
{
    long tv_sec;  /* seconds */
    long tv_usec; /* microseconds */
};

struct timezone
{
    int tz_minuteswest; /* minutes west of Greenwich */
    int tz_dsttime;     /* type of dst correction */
};
#endif /* _TIMEVAL_DEFINED */

#ifdef __cplusplus
extern "C"
{
#endif

    int _RTL_FUNC _IMPORT gettimeofday(struct timeval* tv, struct timezone* tz);
    int _RTL_FUNC utimes(const char *filename, const struct timeval times[2]);

#ifdef __cplusplus
}
#endif
#endif
#endif
