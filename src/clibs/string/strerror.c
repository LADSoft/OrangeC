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

#include <stdio.h>
#include <errno.h>
#include <string.h>

static char *thunk(const char *string, int error)
{
   static char strbuf[256] ;
   char *str ;
   if (error >= _sys_nerr)
      str = "Unknown error" ;
   else
      str = _sys_errlist[error] ;
   strbuf[0] = 0 ;
   if (string && *string) {
      strcat(strbuf,string) ;
      strcat(strbuf,": ") ;
   }
   strcat(strbuf,str) ;
   strcat(strbuf,"\n") ;
   return strbuf ;
}
char *_RTL_FUNC strerror(int error)
{
   return thunk(0,error) ;
}
char *_RTL_FUNC _strerror(const char *str)
{
   return thunk(str,errno) ;
}