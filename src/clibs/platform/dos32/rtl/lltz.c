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

#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#undef _daylight
#undef _timezone
#undef _tzname

static int _daylight = 1;
static long _timezone=5 * 60 * 60 ;

static char tz1[] = "EST", tz2[]= "EDT" ;
static char * _tzname[2] = { tz1, tz2 } ;

static char deftz[] = "EST5EDT" ;

int  *_RTL_FUNC        __getDaylight(void)
{
   return &_daylight ;
}
long *_RTL_FUNC        __getTimezone(void)
{
   return &_timezone ;
}
char * *_RTL_FUNC      __getTzName(void)
{
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
   if (str[0] && strlen(str) != 3)
      return 0 ;
   if (str[0]) {
      memcpy(tz2,str,3) ;
      _daylight = 1 ;
      _tzname[1] = tz2 ;
   } else {
      _daylight = 0 ;
      _tzname[1] = 0 ;
   }
   _timezone = - temp * 60 * 60 ;
   return 1 ;
}
void __ll_tzset(void)
{
   char *s = getenv("TZ") ;
   if (!s)
      s = deftz ;
   if (!parsetz(s))
      parsetz(deftz) ;
}