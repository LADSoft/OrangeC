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


#ifndef RC_INVOKED
#ifndef _TIME_T
#    define _TIME_T
typedef long __time_t_32;
typedef long long __time_t_64;

#if defined(__MSVCRT_DLL) || defined(__CRTDLL_DLL) || defined (_USE_32BIT_TIME_T) || defined( _DEFINING_TIME_T) || defined(__MSIL__)
    typedef __time_t_32 time_t;
#else
    typedef __time_t_64 time_t;
#endif
#endif

#ifndef _CLOCK_T
#    define _CLOCK_T
    typedef long clock_t;
#endif

#endif

#define CLOCKS_PER_SEC 1000
#define CLK_TCK 1000

#define TZNAME "Eastern Standard Time"
#define GMT_OFFS 7

#ifndef RC_INVOKED
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
    #define CLOCK_REALTIME 1
    #define CLOCK_MONOTONIC 2
    #define CLOCK_PROCESS_CPUTIME_ID 3
    #define CLOCK_THREAD_CPUTIME_ID 4

    typedef unsigned clockid_t;

#if __STDC_VERSION__ >= 201112L || defined(__cplusplus)
#    define TIME_UTC 1
    struct timespec
    {
        time_t tv_sec;
        long tv_nsec;
    };
    int timespec_get(struct timespec* ts, int base);
#endif
    char* _RTL_FUNC _IMPORT asctime(const struct tm* __tblock);
    clock_t _RTL_FUNC _IMPORT clock(void);
    size_t _RTL_FUNC _IMPORT strftime(char* ZSTR restrict __s, size_t __maxsize, const char* ZSTR restrict __fmt,
                                      const struct tm* restrict __t);
    size_t _RTL_FUNC _IMPORT _lstrftime(char* ZSTR __s, size_t __maxsize, const char* ZSTR __fmt, const struct tm* __t);

    int* _RTL_FUNC _IMPORT __getDaylight(void);
    long* _RTL_FUNC _IMPORT __getTimezone(void);
    char** _RTL_FUNC _IMPORT __getTzName(void);
    void _RTL_FUNC _IMPORT tzset(void);
    char* _RTL_FUNC _IMPORT _strdate(char* ZSTR __datestr);
    char* _RTL_FUNC _IMPORT _strtime(char* ZSTR __timestr);

    unsigned _RTL_FUNC _IMPORT _getsystime(struct tm*);
    unsigned _RTL_FUNC _IMPORT _setsystime(struct tm*, unsigned);

    void _RTL_FUNC _IMPORT _sleep(unsigned long);
    void _RTL_FUNC _IMPORT _nanosleep(const struct _timespec *, struct _timespec *);

    char* _RTL_FUNC _IMPORT _ctime32(const __time_t_32* __time);
    int _RTL_FUNC _IMPORT _stime32(__time_t_32* __tp);
    struct tm* _RTL_FUNC _IMPORT _gmtime32(const __time_t_32* __timer);
    struct tm* _RTL_FUNC _IMPORT _localtime32(const __time_t_32* __timer);
    __time_t_32 _RTL_FUNC _IMPORT _time32(__time_t_32* __timer);
    __time_t_32 _RTL_FUNC _IMPORT _mktime32(struct tm* __timeptr);

    char* _RTL_FUNC _IMPORT _ctime64(const __time_t_64* __time);
    int _RTL_FUNC _IMPORT _stime64(__time_t_64* __tp);
    struct tm* _RTL_FUNC _IMPORT _gmtime64(const __time_t_64* __timer);
    struct tm* _RTL_FUNC _IMPORT _localtime64(const __time_t_64* __timer);
    __time_t_64 _RTL_FUNC _IMPORT _time64(__time_t_64* __timer);
    __time_t_64 _RTL_FUNC _IMPORT _mktime64(struct tm* __timeptr);

    int _RTL_FUNC clock_getres(clockid_t clk_id, struct timespec *res);
    int _RTL_FUNC clock_gettime(clockid_t clk_id, struct timespec *tp);
    int _RTL_FUNC clock_settime(clockid_t clk_id, const struct timespec *tp);

#if defined(__MSVCRT_DLL) || defined(__CRTDLL_DLL) || defined(_DEFINING_TIME_T) || defined(__MSIL__)
    char* _RTL_FUNC _IMPORT ctime(const time_t* __time);
    int _RTL_FUNC _IMPORT stime(time_t* __tp);
    struct tm* _RTL_FUNC _IMPORT gmtime(const time_t* __timer);
    struct tm* _RTL_FUNC _IMPORT localtime(const time_t* __timer);
    time_t _RTL_FUNC _IMPORT time(time_t* __timer);
    time_t _RTL_FUNC _IMPORT mktime(struct tm* __timeptr);
    double _RTL_FUNC _IMPORT difftime(time_t __time2, time_t __time1);
#else
#if defined(_USE_32BIT_TIME_T)
    inline char* ctime(const time_t* __time)
    {
        return _ctime32(__time);
    }
    inline int stime(time_t* __tp)
    {
        return _stime32(__tp);
    }
    inline struct tm* gmtime(const time_t* __timer)
    {
        return _gmtime32(__timer);
    }
    inline struct tm* localtime(const time_t* __timer)
    {
        return _localtime32(__timer);
    }
    inline time_t time(time_t* __timer)
    {
        return _time32(__timer);
    }
    inline time_t mktime(struct tm* __timeptr)
    {
        return _mktime32(__timeptr);
    }
#else
    inline char* ctime(const time_t* __time)
    {
        return _ctime64(__time);
    }
    inline int stime(time_t* __tp)
    {
        return _stime64(__tp);
    }
    inline struct tm* gmtime(const time_t* __timer)
    {
        return _gmtime64(__timer);
    }
    inline struct tm* localtime(const time_t* __timer)
    {
        return _localtime64(__timer);
    }
    inline time_t _IMPORT time(time_t* __timer)
    {
        return _time64(__timer);
    }
    inline time_t mktime(struct tm* __timeptr)
    {
        return _mktime64(__timeptr);
    }
#endif
    inline double difftime(time_t __time2, time_t __time1)
    {
        return((double)(__time2 - __time1));
    }
#endif

#endif
#if 0
extern _CRTIMP int __cdecl _nanosleep(const struct _timespec *, struct _timespec *);
#endif
#ifdef __cplusplus
};
#endif

#define _daylight (*__getDaylight())
#define _tzname (__getTzName())
#define _timezone (*__getTimezone())

#pragma pack()

#endif /* __TIME_H */
