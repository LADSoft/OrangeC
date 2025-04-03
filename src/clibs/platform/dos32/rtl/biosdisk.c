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

#include <bios.h>
#include <dpmi.h>

extern UWORD __dtaseg;

int _RTL_FUNC      bdll(int __cmd, int __drive, int __head, int __track,
                            int __sector, int __nsects, void *__buffer)
{
      DPMI_REGS regs ;
      regs.b.ah = __cmd ;
      regs.b.al = __drive ;
      regs.b.ch = __track ;
      regs.b.cl = __sector ;
      regs.b.dh = __head ;
      regs.b.dl = __drive ;
      regs.h.ds = __dtaseg;
      regs.h.es = __dtaseg;
      regs.h.ss = 0;
      regs.h.sp = 0;
      regs.h.flags = 0x42;
      __buftodta(__buffer, __nsects * 512) ;
      dpmi_simulate_real_interrupt(0x13,&regs);
      __dtatobuf(__buffer, __nsects * 512) ;
      return regs.h.ax ;
}
int _RTL_FUNC      biosdisk(int __cmd, int __drive, int __head, int __track,
                            int __sector, int __nsects, void *__buffer)
{
   return bdll(__cmd,__drive,__head,__track,__sector,__nsects,__buffer) ;
}
unsigned _RTL_FUNC _bios_disk(unsigned __cmd, struct diskinfo_t *__dinfo)
{
   return bdll(__cmd,__dinfo->drive, __dinfo->head, __dinfo->track,
         __dinfo->sector, __dinfo->nsectors,__dinfo->buffer) ;
}

