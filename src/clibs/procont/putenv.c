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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"


extern char _RTL_DATA **_environ;
int _RTL_FUNC putenv(const char *name)
{
   char **q = _environ, **p ;
   int len = 0, has = 0,count = 0 ;
   __ll_enter_critical() ;
   if (!strchr(name,'=')) {
      __ll_exit_critical() ;
      return -1 ;
   }
   while (name[len] && name[len] != '=') {
      if ((*q)[len] == '=') {
         if ((*q)[len] != 0)
            has = 1 ;
      }   
      len++ ;
   }
    while (*q) {
      count++ ;
      if (!strnicmp(name,*q,len)) {
         if (has) {
            *q = name ;
            __ll_exit_critical() ;
            return 0 ;
         } else {
            while (*q) {
               *q = *(q+1) ;
               q++ ;
            }
            __ll_exit_critical() ;
            return 0 ;
         }
      }
        q++;
    }
   p = (char **)realloc(_environ,(count+2)*sizeof(char **)) ;
   if (!p) {
      __ll_exit_critical() ;
      return -1 ;
   }
   p[count-1] = NULL ;
   p[count-2] = name ;
   _environ = p ;
   __ll_exit_critical() ;
    return 0;
}
int _RTL_FUNC _putenv(const char *name)
{
    return putenv(name);
}