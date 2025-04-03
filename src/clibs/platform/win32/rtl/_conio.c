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

#include <stdio.h>
#include <stdarg.h>
#include <dpmi.h>

#include "conio.h"

char* _RTL_FUNC _cgets(char* p) { return cgets(p); }
int _RTL_FUNC _cputs(const char* p) { return cputs(p); }
int _RTL_FUNC _getch(void) { return getch(); }
int _RTL_FUNC _getche(void) { return getche(); }
int _RTL_FUNC _inp(unsigned short port) { return inp(port); }
unsigned short _RTL_FUNC _inpw(unsigned short port) { return inpw(port); }
unsigned long _RTL_FUNC _inpd(unsigned short port)
{
    __asm mov dx, [port];
    __asm in eax, dx;
    return _EAX;
}
int _RTL_FUNC _kbhit(void) { return kbhit(); }
int _RTL_FUNC _outp(unsigned short port, int val) { return outp(port, val); }
unsigned short _RTL_FUNC _outpw(unsigned short port, unsigned short val) { return outpw(port, val); }
unsigned long _RTL_FUNC _outpd(unsigned short port, unsigned long val)
{
    __asm mov dx, [port];
    __asm mov eax, dword ptr[val];
    __asm out dx, eax;
    return val;
}
int _RTL_FUNC _putch(int ch) { return putch(ch); }
int _RTL_FUNC _ungetch(int ch) { return ungetch(ch); }
int _RTL_FUNC _cprintf(const char* __format, ...)
{
    char buf[4096];
    va_list argptr;

    va_start(argptr, __format);
    vsprintf(buf, __format, argptr);
    va_end(argptr);
    cputs(buf);
    return 0;
}
#ifndef VSDLL
int _RTL_FUNC _cscanf(const char* __format, ...)
{
    va_list argptr;
    char buf[1024];
    cgets(buf);
    va_start(argptr, __format);
    vsscanf(buf, __format, argptr);
    va_end(argptr);
    return 0;
}
#endif