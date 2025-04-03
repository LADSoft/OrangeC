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

#include <dos.h>
#include <errno.h>
#include <dpmi.h>
#include <time.h>

long _RTL_FUNC dostounix(struct date *__datep, struct time *__timep)
{
   struct tm tmx ;
   tmx.tm_mday = __datep->da_day ;
   tmx.tm_mon = __datep->da_mon-1 ;
   tmx.tm_year = __datep->da_year - 1900 ;      
   tmx.tm_hour = __timep->ti_hour ;
   tmx.tm_min = __timep->ti_min ;
   tmx.tm_sec = __timep->ti_sec ;
   return (long) mktime(&tmx) ;

}
