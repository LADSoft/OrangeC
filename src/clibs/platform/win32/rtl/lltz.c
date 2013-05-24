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
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

#undef _daylight
#undef _timezone
#undef _tzname

static int _daylight = 1;
static long _timezone=5 * 60 * 60 ;

static char tz1[36] = "EST", tz2[36]= "EDT" ;
static char * _tzname[2] = { tz1, tz2 } ;

int  *_RTL_FUNC        __getDaylight(void)
{
   __ll_tzset() ;
   return &_daylight ;
}
long *_RTL_FUNC        __getTimezone(void)
{
   __ll_tzset() ;
   return &_timezone ;
}
char * *_RTL_FUNC      __getTzName(void)
{
   __ll_tzset() ;
   return &_tzname[0] ;
}
static int parsetz(char *str)
{
   int temp ;
   if (strlen(str) < 4 || (str[3] != '+' && str[3] != '-' && !isdigit(str[3])))
      return 0 ;
   memcpy(tz1,str,3) ;
   str += 3 ;
   temp = strtol(str,&str,10) ;
   if (str[0] && strlen(str) < 3)
      return 0 ;
   if (str[0]) {
      memcpy(tz2,str,3) ;
      _daylight = 1 ;
      _tzname[1] = tz2 ;
   } else {
      _daylight = 0 ;
      _tzname[1] = 0 ;
   }
   _timezone = temp * 60 * 60 ;
   return 1 ;
}
void __ll_tzset(void)
{
   TIME_ZONE_INFORMATION tzinfo ;
   int temp ;
   int i ;
   __ll_enter_critical() ;

   char * s = getenv ("TZ");
   if (s)
     if (parsetz (s)) {
       __ll_exit_critical ();
       return;
     }

   temp = GetTimeZoneInformation(&tzinfo) ;
   if (temp != TIME_ZONE_ID_INVALID) {
      _daylight = temp == TIME_ZONE_ID_DAYLIGHT ;
      _timezone = tzinfo.Bias * 60 ;
//      if (_daylight) 
//        _timezone -=tzinfo.DaylightBias ;
//      else
//        _timezone -=tzinfo.StandardBias ;

      if (_daylight) {
         _tzname[1] = tz2 ;
      } else {
         _tzname[1] = 0 ;
      }
      for (i=0; i < 32; i++) {
         tz1[i] = tzinfo.StandardName[i] ;
         tz2[i] = tzinfo.DaylightName[i] ;
      }
   }
   __ll_exit_critical();
}
