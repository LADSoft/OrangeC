/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include <bios.h>
#include <dpmi.h>

unsigned _RTL_FUNC _bios_timeofday(unsigned __cmd, long *__timeval)
{
   union REGS regs ;
   regs.h.ah = __cmd ;
   regs.w.cx = *__timeval >> 16 ;
   regs.w.dx = *__timeval ;
   _int386(0x12,&regs,&regs) ;
   *__timeval = (regs.w.cx << 16) + regs.w.dx ;
   return regs.w.ax ;
   
}
long _RTL_FUNC     biostime(int __cmd, long __newtime)
{
   _bios_timeofday(__cmd, &__newtime) ;
   return __newtime ;
}