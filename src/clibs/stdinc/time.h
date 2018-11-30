/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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

/*  time.h

    Struct and function declarations for dealing with time.

*/

#ifndef __TIME_H
#define __TIME_H

#pragma pack(1)

#ifndef __STDDEF_H
#    include <stddef.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _TIME_T
    typedef long time_t;
#    define _TIME_T
#endif

#ifndef _CLOCK_T
#    define _CLOCK_T
    typedef long clock_t;
#endif

#define CLOCKS_PER_SEC 1000
#define CLK_TCK 1000

#define TZNAME "Eastern Standard Time"
#define GMT_OFFS 7

    struct tm
    {
        int tm_sec;
        int tm_min;
        int tm_hour;
        int tm_mday;
        int tm_mon;
        int tm_year;
        int tm_wday;
        int tm_yday;
        int tm_isdst;
    };

#ifndef _TIMESPEC_DEFINED
#    define _TIMESPEC_DEFINED
    typedef struct
    {
        time_t sec;
        long nsec;
    } timespec;
#    define TIME_UTC 1
#endif

    char* _RTL_FUNC _IMPORT asctime(const struct tm* __tblock);
    char* _RTL_FUNC _IMPORT ctime(const time_t* __time);
    double _RTL_FUNC _IMPORT difftime(time_t __time2, time_t __time1);
    struct tm* _RTL_FUNC _IMPORT gmtime(const time_t* __timer);
    struct tm* _RTL_FUNC _IMPORT localtime(const time_t* __timer);
    time_t _RTL_FUNC _IMPORT time(time_t* __timer);
    time_t _RTL_FUNC _IMPORT mktime(struct tm* __timeptr);
    clock_t _RTL_FUNC _IMPORT clock(void);
    size_t _RTL_FUNC _IMPORT strftime(char* restrict __s, size_t __maxsize, const char* restrict __fmt,
                                      const struct tm* restrict __t);
    size_t _RTL_FUNC _IMPORT _lstrftime(char* __s, size_t __maxsize, const char* __fmt, const struct tm* __t);

    int* _RTL_FUNC _IMPORT __getDaylight(void);
    long* _RTL_FUNC _IMPORT __getTimezone(void);
    char** _RTL_FUNC _IMPORT __getTzName(void);
    int _RTL_FUNC _IMPORT stime(time_t* __tp);
    void _RTL_FUNC _IMPORT tzset(void);
    char* _RTL_FUNC _IMPORT _strdate(char* __datestr);
    char* _RTL_FUNC _IMPORT _strtime(char* __timestr);

    unsigned _RTL_FUNC _IMPORT _getsystime(struct tm*);
    unsigned _RTL_FUNC _IMPORT _setsystime(struct tm*, unsigned);

#ifdef __cplusplus
};
#endif

#define _daylight (*__getDaylight())
#define _tzname (__getTzName())
#define _timezone (*__getTimezone())

#pragma pack()

#endif /* __TIME_H */
