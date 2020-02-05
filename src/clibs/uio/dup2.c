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

#include <io.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern int __uihandles[HANDLE_MAX], __uimodes[HANDLE_MAX], __uiflags[HANDLE_MAX] ;
extern char __uinames[HANDLE_MAX][256] ;

int _RTL_FUNC dup2(int __handle, int __newhandle)
{
   int h = __newhandle ;
   int i,ohand = __handle;
   if (h < 0 || h >= HANDLE_MAX) {
      errno = EBADF ;
      return -1 ;
   }
   __ll_enter_critical() ;
   __handle = __uiohandle(__handle) ;
   if (__handle == -1) {
      __ll_exit_critical() ;
      return -1 ;
   }
   i = __ll_dup(__handle) ;
   if (i == -1) {
      __ll_exit_critical() ;
      return -1 ;
   }
   if (__uihandles[h] != -1)
      close(h) ;
   __uimodes[h] = __uimodes[ohand] ;
   __uihandles[h] = i ;
   __uiflags[h] = __uiflags[ohand] ;
   strcpy(__uinames[h],__uinames[ohand]) ;
   __ll_exit_critical() ;
   return 0 ;
}
int _RTL_FUNC _dup2(int __handle, int __newhandle)
{
    return dup2(__handle, __newhandle);
}
