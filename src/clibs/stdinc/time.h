/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
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
#    define _TIME_T
    typedef long time_t;
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
#if __STDC_VERSION__ >= 201112L || defined(__cplusplus)
#    define TIME_UTC 1
    struct timespec
    {
        time_t sec;
        long nsec;
    };
    int timespec_get(struct timespec* ts, int base);
#endif
    char* _RTL_FUNC _IMPORT asctime(const struct tm* __tblock);
    char* _RTL_FUNC _IMPORT ctime(const time_t* __time);
    double _RTL_FUNC _IMPORT difftime(time_t __time2, time_t __time1);
    struct tm* _RTL_FUNC _IMPORT gmtime(const time_t* __timer);
    struct tm* _RTL_FUNC _IMPORT localtime(const time_t* __timer);
    time_t _RTL_FUNC _IMPORT time(time_t* __timer);
    time_t _RTL_FUNC _IMPORT mktime(struct tm* __timeptr);
    clock_t _RTL_FUNC _IMPORT clock(void);
    size_t _RTL_FUNC _IMPORT strftime(char *ZSTR restrict __s, size_t __maxsize, const char *ZSTR restrict __fmt,
                                      const struct tm* restrict __t);
    size_t _RTL_FUNC _IMPORT _lstrftime(char *ZSTR  __s, size_t __maxsize, const char *ZSTR __fmt, const struct tm* __t);

    int* _RTL_FUNC _IMPORT __getDaylight(void);
    long* _RTL_FUNC _IMPORT __getTimezone(void);
    char** _RTL_FUNC _IMPORT __getTzName(void);
    int _RTL_FUNC _IMPORT stime(time_t* __tp);
    void _RTL_FUNC _IMPORT tzset(void);
    char* _RTL_FUNC _IMPORT _strdate(char *ZSTR __datestr);
    char* _RTL_FUNC _IMPORT _strtime(char *ZSTR __timestr);

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
