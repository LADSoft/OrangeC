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

/*
 * DOS routines to read the number of ticks of the timer used for the int
 * 8 interrupt since the program started (more or less)
 *
 * note: ASSUMES the clock rate is 18.2Hz
 */
#include <dpmi.h>
#include <conio.h>

static unsigned int ticks=0;
static unsigned lastread;

unsigned int _elapsed_time(void)
{
    int thisread;

    __asm sub eax,eax

    __asm mov al,0x36
    __asm out 0x43,al

  __asm in al,0x40
    __asm xchg al,ah
    __asm in al,0x40
  __asm xchg al,ah
    __asm not ax
}