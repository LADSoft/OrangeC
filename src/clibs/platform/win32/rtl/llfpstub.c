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
#include <stdlib.h>
#include <signal.h>
#include <fenv.h>

int _fltused;

static fenv_t dfltenv;

fenv_t _RTL_DATA * const __fe_default_env = &dfltenv;

unsigned int _RTL_DATA _default87 = 0x133F;

unsigned _RTL_FUNC _status87(void)
{
   unsigned short work ;
   asm fnstsw [work] ;
   asm fwait ;
   return work ;
}
unsigned _RTL_FUNC _clear87(void)
{
	unsigned short work ;
	asm fnstsw	[work ];
   asm fwait ;
	asm fnclex
	return work ;
}
unsigned _RTL_FUNC _control87(unsigned mask1, unsigned mask2)
{
	unsigned short work ;
	asm fstcw [work];
	mask1 &= mask2;
	mask2 = ~mask2;
	asm fwait
	work &= mask2;
	work |= mask1;
	asm fldcw [work]
	return work ;
}

void _RTL_FUNC _fpreset(void)
{
   asm finit ;
   _control87(_default87,0x1fff) ;
}
void __stdcall __llfpinit(void)
{
   _fpreset() ;
   asm fnstenv [dfltenv]
   _fpreset() ;
}
