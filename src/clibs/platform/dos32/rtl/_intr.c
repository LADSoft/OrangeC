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

#include "i86.h"
#include "dpmi.h"

void _RTL_FUNC _intr(int intr, union REGPACK *r)
{
   DPMI_REGS dr ;
   dr.d.edi = r->x.edi ;
   dr.d.esi = r->x.esi ;
   dr.d.ebp = r->x.ebp ;
   dr.d.eax = r->x.eax ;
   dr.d.ecx = r->x.ecx ;
   dr.d.edx = r->x.edx ;
   dr.d.ebx = r->x.ebx ;
   dr.h.ds = r->x.ds ;
   dr.h.es = r->x.es ;
   dr.h.fs = r->x.fs ;
   dr.h.gs = r->x.gs ;
   dr.h.flags = r->x.flags | 0x42;
   dr.h.ss = 0 ;
   dr.h.sp = 0 ;
   dpmi_simulate_real_interrupt(intr,&dr);
   r->x.edi = dr.d.edi ;
   r->x.esi = dr.d.esi ;
   r->x.ebp = dr.d.ebp ;
   r->x.eax = dr.d.eax ;
   r->x.ecx = dr.d.ecx ;
   r->x.edx = dr.d.edx ;
   r->x.ebx = dr.d.ebx ;
   r->x.ds = dr.h.ds ;
   r->x.es = dr.h.es ;
   r->x.fs = dr.h.fs ;
   r->x.gs = dr.h.gs ;
   r->x.flags = dr.h.flags ;
}