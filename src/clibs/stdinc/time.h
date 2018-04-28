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
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

/*  time.h                               

    Struct and function declarations for dealing with time.

*/

#ifndef __TIME_H
#define __TIME_H

#pragma pack(1)

#ifndef __STDDEF_H
#include <stddef.h>
#endif

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

#ifndef  _TIME_T
typedef long time_t;
#ifdef __cplusplus
#  define _TIME_T __STD_NS_QUALIFIER time_t
#else
#  define _TIME_T time_t
#endif /* __cplusplus */
#endif

#ifndef  _CLOCK_T
#define  _CLOCK_T
typedef long clock_t;
#endif

#define CLOCKS_PER_SEC 1000
#define CLK_TCK        1000

#define TZNAME "Eastern Standard Time"
#define GMT_OFFS 7


struct tm
{
  int   tm_sec;
  int   tm_min;
  int   tm_hour;
  int   tm_mday;
  int   tm_mon;
  int   tm_year;
  int   tm_wday;
  int   tm_yday;
  int   tm_isdst;
};

char *       _RTL_FUNC asctime(const struct tm *__tblock);
char *       _RTL_FUNC ctime(const time_t *__time);
double       _RTL_FUNC difftime(time_t __time2, time_t __time1);
struct tm *  _RTL_FUNC gmtime(const time_t *__timer);
struct tm *  _RTL_FUNC localtime(const time_t *__timer);
time_t       _RTL_FUNC time(time_t *__timer);
time_t       _RTL_FUNC mktime(struct tm *__timeptr);
clock_t      _RTL_FUNC clock(void);
size_t       _RTL_FUNC strftime(char *restrict __s, size_t __maxsize,
                        const char *restrict __fmt, const struct tm *restrict __t);
size_t       _RTL_FUNC _lstrftime(char *__s, size_t __maxsize,
                        const char *__fmt, const struct tm *__t);

int  *       _RTL_FUNC __getDaylight(void);
long *       _RTL_FUNC __getTimezone(void);
char * *     _RTL_FUNC  __getTzName(void);
int          _RTL_FUNC stime(time_t *__tp);
void         _RTL_FUNC tzset(void);
char *       _RTL_FUNC _strdate(char *__datestr);
char *       _RTL_FUNC _strtime(char *__timestr);

unsigned _RTL_FUNC _getsystime(struct tm *);
unsigned _RTL_FUNC _setsystime(struct tm *, unsigned);

#ifdef __cplusplus
} ;
};
#endif

#define _daylight (*__getDaylight())
#define _tzname   ( __getTzName())
#define _timezone (*__getTimezone())

#pragma pack()

#endif  /* __TIME_H */
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__TIME_H_USING_LIST)
#define __TIME_H_USING_LIST
    using __STD_NS_QUALIFIER _getsystime;
    using __STD_NS_QUALIFIER _setsystime;
    using __STD_NS_QUALIFIER asctime;
    using __STD_NS_QUALIFIER ctime;
    using __STD_NS_QUALIFIER difftime;
    using __STD_NS_QUALIFIER gmtime;
    using __STD_NS_QUALIFIER localtime;
    using __STD_NS_QUALIFIER time;
    using __STD_NS_QUALIFIER mktime;
    using __STD_NS_QUALIFIER clock;
    using __STD_NS_QUALIFIER strftime;
    using __STD_NS_QUALIFIER _lstrftime;
    using __STD_NS_QUALIFIER __getDaylight;
    using __STD_NS_QUALIFIER __getTimezone;
    using __STD_NS_QUALIFIER __getTzName;
    using __STD_NS_QUALIFIER stime;
    using __STD_NS_QUALIFIER tzset;
    using __STD_NS_QUALIFIER _strdate;
    using __STD_NS_QUALIFIER _strtime;
    using __STD_NS_QUALIFIER time_t;
    using __STD_NS_QUALIFIER clock_t;
    using __STD_NS_QUALIFIER tm;
#endif
