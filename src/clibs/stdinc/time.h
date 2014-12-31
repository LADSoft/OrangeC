/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
